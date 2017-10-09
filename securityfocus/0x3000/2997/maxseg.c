/*
 * (C)Copyright 2001 Darren Reed.
 *
 * maxseg.c
 */
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#if BSD >= 199306
#include <sys/sysctl.h>
#endif

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>
#include <netinet/tcp.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>

#include <time.h>
#include <fcntl.h>
#include <errno.h>

void prepare_icmp(struct sockaddr_in *);
void primedefaultmss(int, int);
u_short in_cksum(u_short *, int);
int icmp_unreach(struct sockaddr_in *, struct sockaddr_in *);


#define	NEW_MSS	512
#define	NEW_MTU	1500
static int start_mtu = NEW_MTU;

void primedefaultmss(fd, mss)
int fd, mss;
{
#ifdef __NetBSD__
	static int defaultmss = 0;
	int mib[4], msso, mssn;
	size_t olen;

	if (mss == 0)
		mss = defaultmss;
	mssn = mss;
	olen = sizeof(msso);

	mib[0] = CTL_NET;
	mib[1] = AF_INET;
	mib[2] = IPPROTO_TCP;
	mib[3] = TCPCTL_MSSDFLT;
	if (sysctl(mib, 4, &msso, &olen, NULL, 0))
		err(1, "sysctl");
	if (defaultmss == 0)
		defaultmss = msso;

	if (sysctl(mib, 4, 0, NULL, &mssn, sizeof(mssn)))
		err(1, "sysctl");

	if (sysctl(mib, 4, &mssn, &olen, NULL, 0))
		err(1, "sysctl");

	printf("Default MSS: old %d new %d\n", msso, mssn);
#endif

#if HACKED_KERNEL
	int opt;

	if (mss)
		op = mss;
	else
		op = 512;
	if (setsockopt(fd, IPPROTO_TCP, TCP_MAXSEG+1, (char *)&op, sizeof(op)))
		err(1, "setsockopt");
#endif
}


int
main(int argc, char *argv[])
{
	struct sockaddr_in me, them;
	int fd, op, olen, mss;
	char prebuf[16374];
	time_t now1, now2;
	struct timeval tv;

	mss = NEW_MSS;

	primedefaultmss(-1, mss);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		err(1, "socket");

	memset((char *)&them, 0, sizeof(me));
	them.sin_family = AF_INET;
	them.sin_port = ntohs(atoi(argv[2]));
	them.sin_addr.s_addr = inet_addr(argv[1]);

	primedefaultmss(fd, mss);

	op = fcntl(fd, F_GETFL, 0);
	if (op != -1) {
		op |= O_NONBLOCK;
		fcntl(fd, F_SETFL, op);
	}

	op = 1;
	(void) setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &op, sizeof(op));

	if (connect(fd, (struct sockaddr *)&them, sizeof(them)) &&
	    (errno != EINPROGRESS))
		err(1, "connect");

	olen = sizeof(op);
	if (!getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, (char *)&op, &olen))
		printf("Remote mss %d\n", op);
	else
		err(1, "getsockopt");

#if HACKED_KERNEL
	olen = sizeof(op);
	if (!getsockopt(fd, IPPROTO_TCP, TCP_MAXSEG+1, (char *)&op, &olen))
		printf("Our mss %d\n", op);
	else
		err(1, "getsockopt(+1)");
#endif

	olen = sizeof(me);
	if (getsockname(fd, (struct sockaddr *)&me, &olen))
		err(1, "getsockname");

	(void) read(fd, prebuf, sizeof(prebuf));

	now1 = time(NULL);
	for (op = 2; op; op--) {
		icmp_unreach(&me, &them);
		olen = read(fd, prebuf, sizeof(prebuf));
		if (olen == -1) {
			if (errno == ENOBUFS || errno == EAGAIN ||
			    errno == EWOULDBLOCK) {
				tv.tv_sec = 0;
				tv.tv_usec = 10000;
				select(3, NULL, NULL, NULL, &tv);
				continue;
			}
			warn("read");
			break;
		}
	}
	now2 = time(NULL);
	printf("Elapsed time %d\n", now2 - now1);

	primedefaultmss(fd, 0);
	close(fd);
	return 0;
}


/*
 * in_cksum() & icmp_unreach() ripped from nuke.c prior to modifying
 */
static char icmpbuf[256];
static int icmpsock = -1;
static struct sockaddr_in destsock;

void
prepare_icmp(dst)
	 struct sockaddr_in *dst;
{
	struct tcphdr *tcp;
	struct icmp *icmp;

	icmp = (struct icmp *)icmpbuf;

	if (icmpsock == -1) {

		memset((char *)&destsock, 0, sizeof(destsock));
		destsock.sin_family = AF_INET;
		destsock.sin_addr = dst->sin_addr;

		srand(getpid());

		icmpsock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (icmpsock == -1)
			err(1, "socket");

		/* the following messy stuff from Adam Glass (icmpsquish.c) */
		memset(icmp, 0, sizeof(struct icmp) + 8);
		icmp->icmp_type = ICMP_UNREACH;
		icmp->icmp_code = ICMP_UNREACH_NEEDFRAG;
		icmp->icmp_pmvoid = 0;

		icmp->icmp_ip.ip_v = IPVERSION;
		icmp->icmp_ip.ip_hl = 5;
		icmp->icmp_ip.ip_len = htons(NEW_MSS);
		icmp->icmp_ip.ip_p = IPPROTO_TCP;
		icmp->icmp_ip.ip_off = htons(IP_DF);
		icmp->icmp_ip.ip_ttl = 11 + (rand() % 50);
		icmp->icmp_ip.ip_id = rand() & 0xffff;

		icmp->icmp_ip.ip_src = dst->sin_addr;

		tcp = (struct tcphdr *)(&icmp->icmp_ip + 1);
		tcp->th_sport = dst->sin_port;
	}
	icmp->icmp_nextmtu = htons(start_mtu);
	icmp->icmp_cksum = 0;
}


u_short
in_cksum(addr, len)
u_short *addr;
int len;
{
	    register int nleft = len;
	    register u_short *w = addr;
	    register int sum = 0;
	    u_short answer = 0;

	    /*
	     *  Our algorithm is simple, using a 32 bit accumulator (sum),
	     *  we add sequential 16 bit words to it, and at the end, fold
	     *  back all the carry bits from the top 16 bits into the lower
	     *  16 bits.
	     */
	    while( nleft > 1 )  {
	            sum += *w++;
	            nleft -= 2;
	    }

	    /* mop up an odd byte, if necessary */
	    if( nleft == 1 ) {
	            *(u_char *)(&answer) = *(u_char *)w ;
	            sum += answer;
	    }

	    /*
	     * add back carry outs from top 16 bits to low 16 bits
	     */
	    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
	    sum += (sum >> 16);                     /* add carry */
	    answer = ~sum;                          /* truncate to 16 bits */
	    return (answer);
}

int icmp_unreach(src, dst)
	 struct sockaddr_in *src, *dst;
{
	static int donecksum = 0;
	struct sockaddr_in dest;
	struct tcphdr *tcp;
	struct icmp *icmp;
	int i, rc;
	u_short sum;

	icmp = (struct icmp *)icmpbuf;

	prepare_icmp(dst);

	icmp->icmp_ip.ip_dst = src->sin_addr;

	sum = in_cksum((u_short *)&icmp->icmp_ip, sizeof(struct ip));
	icmp->icmp_ip.ip_sum = sum;

	tcp = (struct tcphdr *)(&icmp->icmp_ip + 1);
	tcp->th_dport = src->sin_port;

	sum = in_cksum((u_short *)icmp, sizeof(struct icmp) + 8);
	icmp->icmp_cksum = sum;
	start_mtu /= 2;
	if (start_mtu < 69)
		start_mtu = 69;

	i = sendto(icmpsock, icmpbuf, sizeof(struct icmp) + 8, 0,
		   (struct sockaddr *)&destsock, sizeof(destsock));
	if (i == -1 && errno != ENOBUFS && errno != EAGAIN &&
	    errno != EWOULDBLOCK)
		err(1, "sendto");
	return(0);
}
