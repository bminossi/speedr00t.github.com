#include <sys/types.h>
    #include <sys/ioctl.h>
    #include <dirent.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <fcntl.h>
    struct kernel_dirent {
             long            d_ino;
             long               d_off;
             unsigned short  d_reclen;
             char            d_name[256]; /* We must not include limits.h! */
    };
    #define VFAT_IOCTL_READDIR_BOTH  _IOR('r', 1, struct kernel_dirent [2])
    #define VFAT_IOCTL_READDIR_SHORT  _IOR('r', 2, struct kernel_dirent [2])

    int main(void)
    {
             int fd = open(".", O_RDONLY);
             struct kernel_dirent de[2];

             while (1) {
                     int i = ioctl(fd, VFAT_IOCTL_READDIR_BOTH, (long)de);
                     if (i == -1) break;
                     if (de[0].d_reclen == 0) break;
                     printf("SFN: reclen=%2d off=%d ino=%d, %-12s",
                       de[0].d_reclen, de[0].d_off, de[0].d_ino, de[0].d_name);
                if (de[1].d_reclen)
                  printf("\tLFN: reclen=%2d off=%d ino=%d, %s",
                    de[1].d_reclen, de[1].d_off, de[1].d_ino, de[1].d_name);
                printf("\n");
             }
             return 0;
    }
