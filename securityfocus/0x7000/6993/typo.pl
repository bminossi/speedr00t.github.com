#!/usr/bin/perl
use strict;
use Getopt::Std;
use LWP::UserAgent;
use HTTP::Request;
use HTTP::Response;
use HTTP::Headers;
use HTML::Form;
use Digest::MD5 qw(md5_hex);

my ($thehost,$account) = @ARGV;
my ($uid,$pwd) = split(/:/,$account,2);
my $pass = $pwd;
print "\nchecking $thehost | $uid | $pwd\n";
$pwd = md5_hex("$pwd");

my $content = "";
my $userident = "";

my $hds = HTTP::Headers->new;
my $ua = new LWP::UserAgent();
push @{ $ua->requests_redirectable }, 'POST';
$ua->agent("Opera 6.0");

my $uri = "http://".$thehost."/typo3/typo3/index.php";
my $req = HTTP::Request->new("GET", $uri, $hds, $content);
my $res = $ua->request($req);
my $res_heads = $res->headers;

my $cookie = $res_heads->header("Set-Cookie");
my $form = HTML::Form->parse($res->content, "$uri");
my $challenge = $form->value("challenge");
$userident = md5_hex("$uid:$pwd:$challenge");

$hds->header('Cookie' => "$cookie");
$hds->header('Content-Type' =>  "application/x-www-form-urlencoded");
$content =  "username=$uid&p_field=&userident=$userident&challenge=$challenge&redirect_url=alt_main.php";
$content .= "&loginRefresh=&login_status=login&interface=alternative";

my $req = HTTP::Request->new("POST", $uri, $hds, $content);
my $res = $ua->request($req);

$res_heads = $res->headers;
$cookie = $res_heads->header("Set-Cookie");

print "\nRescode:".$res->code()."\n".$res_heads->as_string()."\n\n";
#print "\n".$res_heads->as_string()."\n\n".$res->content()."\n\n";
