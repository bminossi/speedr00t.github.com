#!/usr/bin/perl
# perl  asterisk-Invite.pl 192.168.1.104 5060 userX  192.168.1.2 5060 userY

use IO::Socket::INET;

die "Usage $0 <dst> <dport> <dusername> <src> <sport> <susername>" unless ($ARGV[5]);

 

$socket=new IO::Socket::INET->new(PeerPort=>$ARGV[1],

        Proto=>'udp',

        PeerAddr=>$ARGV[0]);


$msg="INVITE sip:$ARGV[2]\@$ARGV[0]:$ARGV[1] SIP/2.0\r\nVia: SIP/2.0/UDP $ARGV[3]:$ARGV[4];branch=01;rport\r\nTo: <sip:$ARGV[2]\@$ARGV[0]:$ARGV[1]>\r\nFrom:  <sip:$ARGV[3]:$ARGV[4]>;tag=01\r\nCall-ID: 01\@$ARGV[3]\r\nContent-Type: application/sdp\r\nCSeq: 01 INVITE\r\nContent-Length: 187\r\n\r\nv=0\r\no=root 25903 25903 IN IP4 $ARGV[3]\r\ns=session\r\nc=IN IP4 $ARGV[3]\r\nc=IN IP4 910.188.8.2\r\nt=0 0\r\nm=audio 13956 RTP/AVP 0 4 3 8 111 5 10 7 18 110 97 101\r\na=rtpmap:98 speex/16000\r\n\r\n";

$socket->send($msg);