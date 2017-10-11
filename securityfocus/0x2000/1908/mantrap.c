/*
 *  ManTrap detection/testing program by wilson / f8labs - www.f8labs.org
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

void check_proc_vs_kill(int listpids)
{ 
  struct stat st;
  int i, counter;
  char buf[520];
  
  printf("proc-vs-kill() test: \n");
  fflush(0);
  
  if (geteuid() == 0)
  {
    printf("  Error: Running as root. NOT performing /proc-vs-kill() test.\n");
    return;
  }

  if (listpids == 1)
  {
    printf("Listing mismatching PIDs:\n");
  }

  counter = 0;
  for (i = 1; i < 65535; i ++)
  {
    if ((kill(i, SIGCONT) != 0) && (errno == EPERM)) /* send SIGCONT (which hopefully won't matter) to the process */
    {
      snprintf(buf, 511, "/proc/%d", i);
      if (stat(buf, &st) != 0)
      {
        counter ++;
        if (listpids == 1)
        {
          printf("%.5d ", i);
          if (counter%8 == 0)
          {
            printf("\n");
          } 
        }
      }
    }
  }
  if (listpids == 1)
  {
    printf("\n");
  }
  if (counter == 0)
  {
    printf("  Normal: No mismatches found.\n");
  } else
  {
    printf("  ManTrap? %d mismatching PIDs found.\n", counter);
  }
}

void check_proc_dotdot()
{
  DIR *procDIR;
  struct dirent *procdirent;
  int found;
  
  printf("dotdot test:\n");
  procDIR = opendir("/proc");
  if (procDIR == NULL)
  {
    printf("  Error: Couldn't open /proc while performing dotdot test.\n");
    return;
  }
  found = 0;
  procdirent = readdir(procDIR);
  while (procdirent != NULL)
  {
    if (strcmp(procdirent->d_name, "..") == 0)
    {
      found = 1;
      break;
    }
    procdirent = readdir(procDIR);
  }
  closedir(procDIR);
  if (found == 0)
  {
    printf("  ManTrap? /proc/.. not found in directory listing!\n");
  } else {
    printf("  Normal: /proc/.. found in directory listing.\n");
  }

}

void check_proc_cwdwalk()
{
  char savedpwd[2048], newpwd[2048];
  
  printf("cwdwalk test:\n");
  if (getwd(savedpwd) == NULL)
  {
    printf("  Error: Couldn't get working directory while performing cwdwalk test.\n");
    return;
  }
  
  if (chdir("/proc/self") != 0)
  {
    printf("  Error: Couldn't chdir to /proc/self while performing cwdwalk test.\n");
    return;
  }
  if (chdir("cwd") != 0)
  {
    printf(" Error: Couldn't chdir to /proc/self/cwd while performing cwdwalk test.\n");
    return;
  }
  if (getwd(newpwd) == NULL)
  {
    printf("  ManTrap? getwd() failed after chdir to /proc/self/cwd.\n");
  } else {
    printf("  Normal: getwd() succeeded after chdir to /proc/self/cwd.\n");
  }
  chdir(savedpwd);
  return;
}

void usage(char *myname)
{
  printf("Usage: %s <-a|-p|-l|-d|-c|-h>\n", myname);
  printf(" -a performs ALL tests\n");
  printf(" -p performs /proc-vs-kill() test\n");
  printf(" -l performs /proc-vs-kill() test and lists mismatching PIDs\n");
  printf(" -d performs /proc/.. test\n");
  printf(" -c performs /proc/self/cwd test\n");
  printf(" -h shows this help\n");
}

int main(int argc, char *argv[])
{
  printf("ManTrap detection/testing program by wilson@f8labs.org - www.f8labs.org\n");
  if (argc != 2)
  {
    usage(argv[0]);
    exit(1);
  }
  if (strlen(argv[1]) != 2)
  {
    usage(argv[0]);
    exit(1);
  }
  switch(argv[1][1])
  {
    case 'a':
      check_proc_vs_kill(0);
      check_proc_dotdot();
      check_proc_cwdwalk();
      break;
    case 'p':
      check_proc_vs_kill(0);
      break;
    case 'l':
      check_proc_vs_kill(1);
      break;
    case 'd':
      check_proc_dotdot();
      break;
    case 'c':
      check_proc_cwdwalk();
      break;
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
      break;    
  }
  printf("Finished.\n");
}

